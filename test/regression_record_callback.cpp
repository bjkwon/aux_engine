#include <auxe/auxe.h>

#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <system_error>
#include <unistd.h>
#include <vector>

namespace fs = std::filesystem;

struct Session {
  auxContext* ctx = nullptr;
  auxConfig cfg{};
  fs::path dir;
  std::string err;

  explicit Session(const std::string& name) {
    dir = fs::temp_directory_path() / "auxe_regression_record_callback" / name;
    std::error_code ec;
    fs::remove_all(dir, ec);
    fs::create_directories(dir, ec);
    if (ec) {
      err = "Could not create temp dir: " + dir.string();
      return;
    }

    cfg.sample_rate = 22050;
    cfg.display_precision = 6;
    cfg.display_limit_x = 64;
    cfg.display_limit_y = 64;
    cfg.display_limit_bytes = 512;
    cfg.display_limit_str = 512;
    cfg.search_paths = {dir.string()};
    cfg.debug_hook = nullptr;

    ctx = aux_init(&cfg);
    if (!ctx) {
      err = "aux_init returned null.";
    }
  }

  ~Session() {
    if (ctx) {
      aux_close(ctx);
      ctx = nullptr;
    }
  }

  bool ok() const { return err.empty() && ctx != nullptr; }
};

static bool write_file(const fs::path& path, const std::string& body, std::string& err) {
  std::ofstream out(path);
  if (!out) {
    err = "Failed to write file: " + path.string();
    return false;
  }
  out << body;
  return true;
}

static bool define_register(Session& s, const std::string& udfName) {
  if (!s.ok()) return false;
  std::string err;
  if (aux_define_udf(s.ctx, udfName, s.dir.string(), err) != 0) {
    s.err = "aux_define_udf(" + udfName + ") failed: " + err;
    return false;
  }
  if (aux_register_udf(s.ctx, udfName) != 0) {
    s.err = "aux_register_udf(" + udfName + ") failed.";
    return false;
  }
  return true;
}

static std::string capture_stdout(const std::function<int()>& fn, int& rc, std::string& err) {
  int pipefd[2];
  if (pipe(pipefd) != 0) {
    err = std::string("pipe() failed: ") + std::strerror(errno);
    rc = -1;
    return {};
  }

  fflush(stdout);
  const int saved = dup(STDOUT_FILENO);
  if (saved < 0) {
    err = std::string("dup() failed: ") + std::strerror(errno);
    close(pipefd[0]);
    close(pipefd[1]);
    rc = -1;
    return {};
  }

  if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
    err = std::string("dup2() failed: ") + std::strerror(errno);
    close(saved);
    close(pipefd[0]);
    close(pipefd[1]);
    rc = -1;
    return {};
  }
  close(pipefd[1]);

  rc = fn();

  fflush(stdout);
  if (dup2(saved, STDOUT_FILENO) < 0) {
    err = std::string("dup2(restore) failed: ") + std::strerror(errno);
  }
  close(saved);

  std::string out;
  char buffer[512];
  ssize_t nread = 0;
  while ((nread = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
    out.append(buffer, static_cast<size_t>(nread));
  }
  if (nread < 0 && err.empty()) {
    err = std::string("read() failed: ") + std::strerror(errno);
  }
  close(pipefd[0]);
  return out;
}

static bool invoke_and_expect(Session& s,
                              uint64_t sessionId,
                              const std::string& callbackName,
                              const auxRecordCallbackPayload& payload,
                              const std::string& expectedStdout,
                              std::string& err) {
  std::string preview;
  int rc = 0;
  const std::string captured = capture_stdout([&]() {
    return aux_invoke_record_callback(&s.ctx, sessionId, callbackName, payload, s.cfg, preview);
  }, rc, err);
  if (!err.empty()) {
    return false;
  }
  if (rc != 0) {
    err = "aux_invoke_record_callback failed rc=" + std::to_string(rc) + " msg=" + preview;
    return false;
  }
  if (captured != expectedStdout) {
    err = "Unexpected callback stdout. expected=[" + expectedStdout + "] actual=[" + captured + "]";
    return false;
  }
  return true;
}

static auxRecordCallbackPayload payload_for(int callbackIndex, int sampleRate, int channels, std::vector<double> interleaved) {
  auxRecordCallbackPayload payload;
  payload.sample_rate = sampleRate;
  payload.num_channels = channels;
  payload.callback_index = callbackIndex;
  payload.interleaved = std::move(interleaved);
  return payload;
}

static bool case_persistent_scalar_out_and_reset(std::string& err) {
  Session s("case_persistent_scalar_out_and_reset");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "recpersist.aux",
                  "function [out] = recpersist(in)\n"
                  "if in.?index==0\n"
                  "    out = 0\n"
                  "else\n"
                  "    out += 1\n"
                  "end\n"
                  "printf(\"%d %d\\n\", in.?index, out)\n",
                  s.err) ||
      !define_register(s, "recpersist")) { err = s.err; return false; }

  if (!invoke_and_expect(s, 101, "recpersist", payload_for(0, 8000, 1, {}), "0 0\n", err)) return false;
  if (!invoke_and_expect(s, 101, "recpersist", payload_for(1, 8000, 1, {0.1, 0.2, 0.3}), "1 1\n", err)) return false;
  if (!invoke_and_expect(s, 101, "recpersist", payload_for(2, 8000, 1, {0.4, 0.5}), "2 2\n", err)) return false;

  if (!invoke_and_expect(s, 202, "recpersist", payload_for(0, 8000, 1, {}), "0 0\n", err)) return false;
  if (!invoke_and_expect(s, 202, "recpersist", payload_for(1, 8000, 1, {0.9}), "1 1\n", err)) return false;
  return true;
}

static bool case_data_length_accumulates_per_session(std::string& err) {
  Session s("case_data_length_accumulates_per_session");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "recdata.aux",
                  "function [out] = recdata(in)\n"
                  "if in.?index==0\n"
                  "    out = 0\n"
                  "else\n"
                  "    out += in.?data.length\n"
                  "end\n"
                  "printf(\"%d %d %d\\n\", in.?index, in.?fs, out)\n",
                  s.err) ||
      !define_register(s, "recdata")) { err = s.err; return false; }

  if (!invoke_and_expect(s, 303, "recdata", payload_for(0, 12000, 1, {}), "0 12000 0\n", err)) return false;
  if (!invoke_and_expect(s, 303, "recdata", payload_for(1, 12000, 1, {0.1, 0.2, 0.3}), "1 12000 3\n", err)) return false;
  if (!invoke_and_expect(s, 303, "recdata", payload_for(2, 12000, 1, {0.4, 0.5, 0.6, 0.7}), "2 12000 7\n", err)) return false;

  if (!invoke_and_expect(s, 404, "recdata", payload_for(0, 12000, 1, {}), "0 12000 0\n", err)) return false;
  if (!invoke_and_expect(s, 404, "recdata", payload_for(1, 12000, 1, {0.8, 0.9}), "1 12000 2\n", err)) return false;
  return true;
}

static bool case_user_style_callback_syntax(std::string& err) {
  Session s("case_user_style_callback_syntax");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "cb1.aux",
                  "function out=cb1(in)\n"
                  "if in.?index==0\n"
                  "    out = 0\n"
                  "    printf(\"%d:\\n\",in.?index)\n"
                  "elseif in.?index==1\n"
                  "    out = in.?data\n"
                  "    val=rms(in.?data)\n"
                  "    printf(\"%d:%.2f\\n\",in.?index, val)\n"
                  "else\n"
                  "    out ++= in.?data\n"
                  "    val=rms(in.?data)\n"
                  "    printf(\"%d:%.2f\\n\",in.?index, val)\n"
                  "end\n",
                  s.err) ||
      !define_register(s, "cb1")) { err = s.err; return false; }

  if (!invoke_and_expect(s, 505, "cb1", payload_for(0, 8000, 1, {}), "0:\n", err)) return false;
  if (!invoke_and_expect(s, 505, "cb1", payload_for(1, 8000, 1, {0.5, -0.5, 0.5, -0.5}), "1:-3.01\n", err)) return false;
  if (!invoke_and_expect(s, 505, "cb1", payload_for(2, 8000, 1, {0.25, -0.25, 0.25, -0.25}), "2:-9.03\n", err)) return false;
  return true;
}

static bool case_outputs_attach_to_handle(std::string& err) {
  Session s("case_outputs_attach_to_handle");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "multiout.aux",
                  "function [out,level] = multiout(in)\n"
                  "if in.?index==0\n"
                  "    out = []\n"
                  "    level = 0\n"
                  "else\n"
                  "    out ++= in.?data\n"
                  "    level = in.?index\n"
                  "end\n",
                  s.err) ||
      !define_register(s, "multiout")) { err = s.err; return false; }

  if (!invoke_and_expect(s, 606, "multiout", payload_for(0, 8000, 1, {}), "", err)) return false;
  if (!invoke_and_expect(s, 606, "multiout", payload_for(1, 8000, 1, {0.1, 0.2, 0.3}), "", err)) return false;
  if (aux_set_handle_values(s.ctx, "h", {606}) != 0) {
    err = "aux_set_handle_values failed.";
    return false;
  }
  if (aux_attach_record_callback_outputs_to_handle(s.ctx, 606, 606) != 0) {
    err = "aux_attach_record_callback_outputs_to_handle failed.";
    return false;
  }

  auto members = aux_get_struct(s.ctx, "h");
  if (members.find("out") == members.end() || members.find("level") == members.end()) {
    err = "Expected callback outputs to be attached as handle members.";
    return false;
  }
  if (!aux_is_audio(members["out"]) || aux_flatten_channel_length(members["out"], 0) != 3) {
    err = "Unexpected attached audio length for h.out.";
    return false;
  }
  return true;
}

static bool case_reserved_output_name_rejected(std::string& err) {
  Session s("case_reserved_output_name_rejected");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "badcb.aux",
                  "function [prog] = badcb(in)\n"
                  "prog = 1\n",
                  s.err) ||
      !define_register(s, "badcb")) { err = s.err; return false; }

  std::string preview;
  const int rc = aux_invoke_record_callback(&s.ctx, 707, "badcb", payload_for(0, 8000, 1, {}), s.cfg, preview);
  if (rc == 0) {
    err = "Reserved callback output name unexpectedly succeeded.";
    return false;
  }
  if (preview.find("reserved") == std::string::npos) {
    err = "Reserved-name rejection message was not surfaced.";
    return false;
  }
  return true;
}

int main() {
  std::string err;

  if (!case_persistent_scalar_out_and_reset(err)) {
    std::cerr << "FAIL: case_persistent_scalar_out_and_reset: " << err << "\n";
    return 1;
  }

  if (!case_data_length_accumulates_per_session(err)) {
    std::cerr << "FAIL: case_data_length_accumulates_per_session: " << err << "\n";
    return 1;
  }

  if (!case_user_style_callback_syntax(err)) {
    std::cerr << "FAIL: case_user_style_callback_syntax: " << err << "\n";
    return 1;
  }

  if (!case_outputs_attach_to_handle(err)) {
    std::cerr << "FAIL: case_outputs_attach_to_handle: " << err << "\n";
    return 1;
  }

  if (!case_reserved_output_name_rejected(err)) {
    std::cerr << "FAIL: case_reserved_output_name_rejected: " << err << "\n";
    return 1;
  }

  std::cout << "PASS: regression_record_callback (all cases)\n";
  return 0;
}
