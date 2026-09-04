#include <auxe/auxe.h>

#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

#ifndef TEST_NATIVE_MODULE_PATH
#error TEST_NATIVE_MODULE_PATH must be defined
#endif

#ifndef TEST_NATIVE_NO_ENTRY_MODULE_PATH
#error TEST_NATIVE_NO_ENTRY_MODULE_PATH must be defined
#endif

struct Session {
  auxContext* ctx = nullptr;
  auxConfig cfg{};
  std::string err;

  Session() {
    cfg.sample_rate = 22050;
    cfg.display_precision = 6;
    cfg.display_limit_x = 64;
    cfg.display_limit_y = 64;
    cfg.display_limit_bytes = 512;
    cfg.display_limit_str = 512;
    cfg.debug_hook = nullptr;
    ctx = aux_init(&cfg);
    if (!ctx) err = "aux_init returned null.";
  }

  ~Session() {
    if (ctx) aux_close(ctx);
  }

  bool ok() const { return ctx && err.empty(); }
};

struct EvalOutcome {
  int rc = -1;
  std::string preview;
};

static EvalOutcome eval(Session& s, const std::string& script) {
  EvalOutcome out;
  out.rc = aux_eval(&s.ctx, script, s.cfg, out.preview);
  return out;
}

static bool write_file(const fs::path& path, const std::string& body, std::string& err) {
  std::ofstream out(path);
  if (!out) {
    err = "Failed to write " + path.string();
    return false;
  }
  out << body;
  return true;
}

static bool set_module_path(const fs::path& root, std::string& err) {
#ifdef _WIN32
  if (_putenv_s("AUXE_MODULE_PATH", root.string().c_str()) != 0) {
    err = "Failed to set AUXE_MODULE_PATH.";
    return false;
  }
#else
  if (setenv("AUXE_MODULE_PATH", root.string().c_str(), 1) != 0) {
    err = "Failed to set AUXE_MODULE_PATH.";
    return false;
  }
#endif
  return true;
}

static bool make_module(const fs::path& root,
                        const std::string& name,
                        const std::string& manifest,
                        std::string& err) {
  std::error_code ec;
  fs::create_directories(root / name, ec);
  if (ec) {
    err = "Failed to create module directory: " + (root / name).string();
    return false;
  }
  return write_file(root / name / "auxe-module.json", manifest, err);
}

static std::string manifest_for(const std::string& name, const std::string& library, int abi = 1) {
  return std::string("{\n") +
      "  \"name\": \"" + name + "\",\n" +
      "  \"abi_version\": " + std::to_string(abi) + ",\n" +
      "  \"library\": \"" + library + "\",\n" +
      "  \"functions\": [\n" +
      "    {\"name\": \"test_add1\"},\n" +
      "    {\"name\": \"test_vec_add1\"},\n" +
      "    {\"name\": \"test_string\"},\n" +
      "    {\"name\": \"test_audio\"},\n" +
      "    {\"name\": \"test_fail\"},\n" +
      "    {\"name\": \"test_noarg\"}\n" +
      "  ]\n" +
      "}\n";
}

static bool expect_error_contains(Session& s,
                                  const std::string& script,
                                  const std::string& needle,
                                  std::string& err) {
  const EvalOutcome out = eval(s, script);
  if (out.rc != static_cast<int>(auxEvalStatus::AUX_EVAL_ERROR)) {
    err = "Expected error for [" + script + "], rc=" + std::to_string(out.rc) +
          ", preview=[" + out.preview + "]";
    return false;
  }
  if (out.preview.find(needle) == std::string::npos) {
    err = "Expected error containing [" + needle + "] for [" + script + "], preview=[" + out.preview + "]";
    return false;
  }
  return true;
}

static bool expect_ok(Session& s, const std::string& script, std::string& err) {
  const EvalOutcome out = eval(s, script);
  if (out.rc != static_cast<int>(auxEvalStatus::AUX_EVAL_OK)) {
    err = "Expected ok for [" + script + "], rc=" + std::to_string(out.rc) +
          ", preview=[" + out.preview + "]";
    return false;
  }
  return true;
}

static bool expect_scalar(Session& s, const std::string& varname, double expected, std::string& err) {
  AuxObj obj = aux_get_var(s.ctx, varname);
  if (!obj) {
    err = "Missing variable: " + varname;
    return false;
  }
  double value = 0.0;
  if (aux_copy_vector(obj, &value, 1) != 1 || std::fabs(value - expected) > 1e-9) {
    err = "Unexpected scalar for " + varname + ": " + std::to_string(value);
    return false;
  }
  return true;
}

static bool expect_vector(Session& s, const std::string& varname, const std::vector<double>& expected, std::string& err) {
  AuxObj obj = aux_get_var(s.ctx, varname);
  if (!obj) {
    err = "Missing variable: " + varname;
    return false;
  }
  if (aux_vector_length(obj) != expected.size()) {
    err = "Unexpected vector length for " + varname;
    return false;
  }
  std::vector<double> values(expected.size());
  if (aux_copy_vector(obj, values.data(), values.size()) != values.size()) {
    err = "Could not copy vector " + varname;
    return false;
  }
  for (size_t i = 0; i < expected.size(); ++i) {
    if (std::fabs(values[i] - expected[i]) > 1e-9) {
      err = "Unexpected vector value for " + varname;
      return false;
    }
  }
  return true;
}

static bool expect_preview_contains(Session& s, const std::string& varname, const std::string& needle, std::string& err) {
  AuxObj obj = aux_get_var(s.ctx, varname);
  if (!obj) {
    err = "Missing variable: " + varname;
    return false;
  }
  uint16_t type = 0;
  std::string size;
  std::string preview;
  if (aux_describe_var(s.ctx, obj, s.cfg, type, size, preview) != 0) {
    err = "Could not describe " + varname;
    return false;
  }
  if (preview.find(needle) == std::string::npos) {
    err = "Expected preview containing [" + needle + "], got [" + preview + "]";
    return false;
  }
  return true;
}

static bool expect_audio(Session& s, const std::string& varname, std::string& err) {
  AuxObj obj = aux_get_var(s.ctx, varname);
  if (!obj) {
    err = "Missing variable: " + varname;
    return false;
  }
  if (!aux_is_audio(obj) || aux_num_channels(obj) != 2) {
    err = "Expected stereo audio for " + varname;
    return false;
  }
  if (aux_flatten_channel_length(obj, 0) != 3 || aux_flatten_channel_length(obj, 1) != 3) {
    err = "Unexpected audio channel length.";
    return false;
  }
  double left[3] = {};
  double right[3] = {};
  if (aux_flatten_channel(obj, 0, left, 3) != 3 || aux_flatten_channel(obj, 1, right, 3) != 3) {
    err = "Could not flatten audio.";
    return false;
  }
  if (std::fabs(left[0] - 0.1) > 1e-9 || std::fabs(right[2] + 0.3) > 1e-9) {
    err = "Unexpected audio samples.";
    return false;
  }
  return true;
}

static bool case_good_module(const fs::path& root, std::string& err) {
  if (!make_module(root, "testmodule", manifest_for("testmodule", TEST_NATIVE_MODULE_PATH), err))
    return false;
  Session s;
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_error_contains(s, "x=test_add1(1)", "test_add1", err)) return false;
  if (!expect_ok(s, "import(\"testmodule\")", err)) return false;
  if (!expect_ok(s, "import(\"testmodule\")", err)) return false;
  if (!expect_error_contains(s, "g=test_add1(4)", "test_add1", err)) return false;
  if (!expect_ok(s, "x=testmodule.test_add1(4)", err) || !expect_scalar(s, "x", 5.0, err)) return false;
  if (!expect_ok(s, "v=testmodule.test_vec_add1([1 2 3])", err) || !expect_vector(s, "v", {2, 3, 4}, err)) return false;
  if (!expect_ok(s, "s=testmodule.test_string(\"abc\")", err) || !expect_preview_contains(s, "s", "native:abc", err)) return false;
  if (!expect_ok(s, "a=testmodule.test_audio(0)", err) || !expect_audio(s, "a", err)) return false;
  if (!expect_ok(s, "n=testmodule.test_noarg()", err) || !expect_scalar(s, "n", 77.0, err)) return false;
  if (!expect_error_contains(s, "z=testmodule.test_fail(0)", "intentional native failure", err)) return false;

  Session aliasSession;
  if (!aliasSession.ok()) { err = aliasSession.err; return false; }
  if (!expect_ok(aliasSession, "import(\"testmodule\",\"tm\")", err)) return false;
  if (!expect_ok(aliasSession, "x=tm.test_add1(8)", err) || !expect_scalar(aliasSession, "x", 9.0, err)) return false;
  if (!expect_error_contains(aliasSession, "x=testmodule.test_add1(8)", "testmodule", err)) return false;
  return true;
}

static bool case_error_modules(const fs::path& root, std::string& err) {
  if (!make_module(root, "badjson", "{", err)) return false;
  if (!make_module(root, "badabi", manifest_for("badabi", TEST_NATIVE_MODULE_PATH, 999), err)) return false;
  if (!make_module(root, "missinglib", manifest_for("missinglib", (root / "missing" / "none.so").string()), err)) return false;
  if (!make_module(root, "noentry", manifest_for("noentry", TEST_NATIVE_NO_ENTRY_MODULE_PATH), err)) return false;

  Session s;
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_error_contains(s, "import(\"not_here\")", "Module not found", err)) return false;
  if (!expect_error_contains(s, "import(\"badjson\")", "Invalid module manifest", err)) return false;
  if (!expect_error_contains(s, "import(\"badabi\")", "Module ABI mismatch", err)) return false;
  if (!expect_error_contains(s, "import(\"missinglib\")", "Failed to load module library", err)) return false;
  if (!expect_error_contains(s, "import(\"noentry\")", "does not export auxe_module_init", err)) return false;
  return true;
}

int main() {
  std::string err;
  const fs::path root = fs::temp_directory_path() / "auxe_regression_external_module";
  std::error_code ec;
  fs::remove_all(root, ec);
  fs::create_directories(root, ec);
  if (ec) {
    std::cerr << "Failed to create registry root: " << root << "\n";
    return 1;
  }
  if (!set_module_path(root, err) ||
      !case_good_module(root, err) ||
      !case_error_modules(root, err)) {
    std::cerr << err << "\n";
    return 1;
  }
  return 0;
}
