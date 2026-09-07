#include <auxe/auxe.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <cmath>
#include <regex>
#include <string>

namespace fs = std::filesystem;

struct Session {
  auxContext* ctx = nullptr;
  auxConfig cfg{};
  fs::path dir;
  std::string err;

  explicit Session(const std::string& name) {
    dir = fs::temp_directory_path() / "auxe_regression_eval_errors" / name;
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

static bool expect_eval_error(Session& s, const std::string& cmd, const std::string& label) {
  std::string preview;
  const int rc = aux_eval(&s.ctx, cmd, s.cfg, preview);
  if (rc != static_cast<int>(auxEvalStatus::AUX_EVAL_ERROR)) {
    s.err = label + " should have returned AUX_EVAL_ERROR, rc=" + std::to_string(rc) + ", preview=" + preview;
    return false;
  }
  if (preview.empty()) {
    s.err = label + " returned empty error preview.";
    return false;
  }
  return true;
}

static bool expect_eval_ok(Session& s, const std::string& cmd, const std::string& label) {
  std::string preview;
  const int rc = aux_eval(&s.ctx, cmd, s.cfg, preview);
  if (rc != static_cast<int>(auxEvalStatus::AUX_EVAL_OK)) {
    s.err = label + " should have returned AUX_EVAL_OK, rc=" + std::to_string(rc) + ", preview=" + preview;
    return false;
  }
  return true;
}

static bool expect_var_null(Session& s, const std::string& varName) {
  AuxObj obj = aux_get_var(s.ctx, varName);
  if (!obj) {
    s.err = "Variable not found: " + varName;
    return false;
  }
  uint16_t type = 0;
  std::string size;
  std::string preview;
  if (aux_describe_var(s.ctx, obj, s.cfg, type, size, preview) != 0) {
    s.err = "aux_describe_var failed for " + varName;
    return false;
  }
  if (type != 0) {
    s.err = "Expected null type for " + varName + ", got type=" + std::to_string(type) + ", preview=" + preview;
    return false;
  }
  return true;
}

static bool expect_vector_values(Session& s, const std::string& varName, const std::vector<double>& expected, double tol = 1e-9) {
  AuxObj obj = aux_get_var(s.ctx, varName);
  if (!obj) {
    s.err = "Variable not found: " + varName;
    return false;
  }
  if (aux_vector_length(obj) != expected.size()) {
    s.err = "Unexpected length for " + varName + ": got " + std::to_string(aux_vector_length(obj)) +
            ", expected " + std::to_string(expected.size());
    return false;
  }
  std::vector<double> vals(expected.size(), 0.0);
  if (aux_copy_vector(obj, vals.data(), vals.size()) != vals.size()) {
    s.err = "aux_copy_vector failed for " + varName;
    return false;
  }
  for (size_t i = 0; i < expected.size(); ++i) {
    if (std::fabs(vals[i] - expected[i]) > tol) {
      s.err = "Unexpected value for " + varName + " at index " + std::to_string(i) +
              ": got " + std::to_string(vals[i]) + ", expected " + std::to_string(expected[i]);
      return false;
    }
  }
  return true;
}

static bool expect_scalar_value(Session& s, const std::string& varName, double expected, double tol = 1e-9) {
  AuxObj obj = aux_get_var(s.ctx, varName);
  if (!obj) {
    s.err = "Variable not found: " + varName;
    return false;
  }
  if (aux_vector_length(obj) != 1) {
    s.err = "Unexpected scalar length for " + varName + ": got " + std::to_string(aux_vector_length(obj));
    return false;
  }
  double val = 0.0;
  if (aux_copy_vector(obj, &val, 1) != 1) {
    s.err = "aux_copy_vector failed for scalar " + varName;
    return false;
  }
  if (std::fabs(val - expected) > tol) {
    s.err = "Unexpected scalar value for " + varName + ": got " + std::to_string(val) +
            ", expected " + std::to_string(expected);
    return false;
  }
  return true;
}

static bool expect_matrix_preview_values(Session& s, const std::string& varName, const std::string& expectedSize, const std::vector<double>& expected, double tol = 1e-9) {
  AuxObj obj = aux_get_var(s.ctx, varName);
  if (!obj) {
    s.err = "Variable not found: " + varName;
    return false;
  }
  uint16_t type = 0;
  std::string size;
  std::string preview;
  if (aux_describe_var(s.ctx, obj, s.cfg, type, size, preview) != 0) {
    s.err = "aux_describe_var failed for " + varName;
    return false;
  }
  if (size != expectedSize) {
    s.err = "Unexpected size for " + varName + ": got " + size + ", expected " + expectedSize + ", preview=" + preview;
    return false;
  }

  static const std::regex numRe(R"([-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)");
  std::vector<double> vals;
  for (std::sregex_iterator it(preview.begin(), preview.end(), numRe), end; it != end; ++it) {
    vals.push_back(std::stod((*it).str()));
  }
  if (vals.size() != expected.size()) {
    s.err = "Unexpected preview value count for " + varName + ": got " + std::to_string(vals.size()) +
            ", expected " + std::to_string(expected.size()) + ", preview=" + preview;
    return false;
  }
  for (size_t i = 0; i < expected.size(); ++i) {
    if (std::fabs(vals[i] - expected[i]) > tol) {
      s.err = "Unexpected matrix preview value for " + varName + " at index " + std::to_string(i) +
              ": got " + std::to_string(vals[i]) + ", expected " + std::to_string(expected[i]) +
              ", preview=" + preview;
      return false;
    }
  }
  return true;
}

static bool case_empty_index_read_returns_null(std::string& err) {
  Session s("case_empty_index_read_returns_null");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "tt=1:4", "tt=1:4") ||
      !expect_eval_ok(s, "v=[]", "v=[]") ||
      !expect_eval_ok(s, "r=tt(v)", "r=tt(v)") ||
      !expect_var_null(s, "r")) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_zero_arg_parentheses_builtin_and_udf(std::string& err) {
  Session s("case_zero_arg_parentheses_builtin_and_udf");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "zeroparen.aux",
                  "function out = zeroparen\n"
                  "out = 7\n", s.err) ||
      !define_register(s, "zeroparen")) { err = s.err; return false; }

  if (!expect_eval_error(s, "fs1 = getfs", "fs1 = getfs") ||
      !expect_eval_ok(s, "fs2 = getfs()", "fs2 = getfs()") ||
      !expect_eval_error(s, "u1 = zeroparen", "u1 = zeroparen") ||
      !expect_eval_ok(s, "u2 = zeroparen()", "u2 = zeroparen()") ||
      !expect_scalar_value(s, "fs2", 22050.0) ||
      !expect_scalar_value(s, "u2", 7.0)) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_group_overlap_uses_second_method_arg(std::string& err) {
  Session s("case_group_overlap_uses_second_method_arg");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "g = (1:7).group(3,1)", "(1:7).group(3,1)") ||
      !expect_matrix_preview_values(s, "g", "3x3", {1.0, 2.0, 3.0, 3.0, 4.0, 5.0, 5.0, 6.0, 7.0})) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_group_overlap_pads_partial_final_group(std::string& err) {
  Session s("case_group_overlap_pads_partial_final_group");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "g = (1:8).group(3,1)", "(1:8).group(3,1)") ||
      !expect_matrix_preview_values(s, "g", "3x4", {1.0, 2.0, 3.0, 4.0, 4.0, 5.0, 6.0, 7.0, 7.0, 8.0, 0.0, 0.0})) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_ungroup_overlap_reverses_group_overlap(std::string& err) {
  Session s("case_ungroup_overlap_reverses_group_overlap");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "u = (1:7).group(3,1).ungroup(1)", "(1:7).group(3,1).ungroup(1)") ||
      !expect_vector_values(s, "u", {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0})) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_empty_index_write_null_is_noop(std::string& err) {
  Session s("case_empty_index_write_null_is_noop");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "tt=1:4", "tt=1:4") ||
      !expect_eval_ok(s, "v=[]", "v=[]") ||
      !expect_eval_ok(s, "tt(v)=[]", "tt(v)=[]") ||
      !expect_eval_ok(s, "r=tt", "r=tt")) {
    err = s.err;
    return false;
  }

  AuxObj obj = aux_get_var(s.ctx, "r");
  if (!obj) {
    err = "Variable not found: r";
    return false;
  }
  if (aux_vector_length(obj) != 4) {
    err = "Expected tt to remain length 4 after tt(v)=[]";
    return false;
  }
  return true;
}

static bool case_empty_index_write_nonnull_is_error(std::string& err) {
  Session s("case_empty_index_write_nonnull_is_error");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "tt=1:4", "tt=1:4") ||
      !expect_eval_ok(s, "v=[]", "v=[]") ||
      !expect_eval_error(s, "tt(v)=3", "tt(v)=3")) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_end_index_on_empty_lhs_is_error(std::string& err) {
  Session s("case_end_index_on_empty_lhs_is_error");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "badend.aux",
                  "function out = badend()\n"
                  "x = [1 2]\n"
                  "x([1 2]) = []\n"
                  "x(end) = 3\n"
                  "out = x\n", s.err) ||
      !define_register(s, "badend")) { err = s.err; return false; }

  if (!expect_eval_error(s, "r = badend()", "badend()")) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_empty_builtin_max_is_error(std::string& err) {
  Session s("case_empty_builtin_max_is_error");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_error(s, "m = _max([])", "_max([])")) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_indexed_scalar_write_matches_numeric_container(std::string& err) {
  Session s("case_indexed_scalar_write_matches_numeric_container");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=zeros(8)", "x=zeros(8)") ||
      !expect_eval_ok(s, "w=[1 2 3 4 5 6 7 8]", "w=[1 2 3 4 5 6 7 8]") ||
      !expect_eval_ok(s, "n=3", "n=3") ||
      !expect_eval_ok(s, "x(n+1)=w(n+1)*0.5", "x(n+1)=w(n+1)*0.5")) {
    err = s.err;
    return false;
  }

  AuxObj obj = aux_get_var(s.ctx, "x");
  if (!obj) {
    err = "Variable not found: x";
    return false;
  }
  std::vector<double> vals(aux_vector_length(obj), 0.0);
  if (aux_copy_vector(obj, vals.data(), vals.size()) != vals.size()) {
    err = "aux_copy_vector failed for x";
    return false;
  }
  if (vals.size() < 4 || vals[3] != 2.0) {
    err = "Expected x(4) to be 2 after indexed scalar write.";
    return false;
  }
  return true;
}

static bool case_complex_fft_index_extract_preserves_bins(std::string& err) {
  Session s("case_complex_fft_index_extract_preserves_bins");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "f=fft([1 2 3 4])", "f=fft([1 2 3 4])") ||
      !expect_eval_ok(s, "s=f(2)", "s=f(2)") ||
      !expect_eval_ok(s, "sr=real(s)", "sr=real(s)") ||
      !expect_eval_ok(s, "si=imag(s)", "si=imag(s)") ||
      !expect_eval_ok(s, "vr=real(f(1:3))", "vr=real(f(1:3))") ||
      !expect_eval_ok(s, "vi=imag(f(1:3))", "vi=imag(f(1:3))") ||
      !expect_vector_values(s, "sr", {-2.0}) ||
      !expect_vector_values(s, "si", {2.0}) ||
      !expect_vector_values(s, "vr", {10.0, -2.0, -2.0}) ||
      !expect_vector_values(s, "vi", {0.0, 2.0, 0.0})) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_complex_indexed_write_preserves_real_and_imag(std::string& err) {
  Session s("case_complex_indexed_write_preserves_real_and_imag");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "j=sqrt(-1)", "j=sqrt(-1)") ||
      !expect_eval_ok(s, "c=zeros(4)+0*j", "c=zeros(4)+0*j") ||
      !expect_eval_ok(s, "c(2)=3+4*j", "c(2)=3+4*j") ||
      !expect_eval_ok(s, "cr=real(c)", "cr=real(c)") ||
      !expect_eval_ok(s, "ci=imag(c)", "ci=imag(c)") ||
      !expect_vector_values(s, "cr", {0.0, 3.0, 0.0, 0.0}) ||
      !expect_vector_values(s, "ci", {0.0, 4.0, 0.0, 0.0})) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_continue_skips_remaining_loop_body(std::string& err) {
  Session s("case_continue_skips_remaining_loop_body");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "forcontinue.aux",
                  "function out = forcontinue\n"
                  "out = []\n"
                  "for k = 1:5\n"
                  "  if k == 3\n"
                  "    continue\n"
                  "  end\n"
                  "  out = [out k]\n"
                  "end\n", s.err) ||
      !write_file(s.dir / "whilecontinue.aux",
                  "function out = whilecontinue\n"
                  "out = []\n"
                  "k = 0\n"
                  "while k < 5\n"
                  "  k = k + 1\n"
                  "  if k == 3\n"
                  "    continue\n"
                  "  end\n"
                  "  out = [out k]\n"
                  "end\n", s.err) ||
      !define_register(s, "forcontinue") ||
      !define_register(s, "whilecontinue")) {
    err = s.err;
    return false;
  }
  if (!expect_eval_ok(s, "x = forcontinue()", "forcontinue()") ||
      !expect_vector_values(s, "x", {1.0, 2.0, 4.0, 5.0}) ||
      !expect_eval_ok(s, "w = whilecontinue()", "whilecontinue()") ||
      !expect_vector_values(s, "w", {1.0, 2.0, 4.0, 5.0})) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_long_stereo_plus_short_shifted_stereo(std::string& err) {
  Session s("case_long_stereo_plus_short_shifted_stereo");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=[silence(2000); silence(2000)]", "long stereo setup") ||
      !expect_eval_ok(s, "noi=silence(600)", "short noise setup") ||
      !expect_eval_ok(s, "noi2=silence(600)", "short noise2 setup") ||
      !expect_eval_ok(s, "x2=x+([(noi+noi2)@-15;]>>600)", "long stereo plus shifted short stereo")) {
    err = s.err;
    return false;
  }
  AuxObj x2 = aux_get_var(s.ctx, "x2");
  if (!x2 || !aux_is_audio(x2)) {
    err = "x2 should be stereo audio.";
    return false;
  }
  const size_t leftLen = aux_flatten_channel_length(x2, 0);
  const size_t rightLen = aux_flatten_channel_length(x2, 1);
  const size_t expectedLen = static_cast<size_t>(std::lround(2000.0 / 1000.0 * s.cfg.sample_rate));
  if (leftLen != expectedLen || rightLen != expectedLen) {
    err = "Unexpected x2 channel lengths: left=" + std::to_string(leftLen) +
          ", right=" + std::to_string(rightLen) +
          ", expected=" + std::to_string(expectedLen);
    return false;
  }
  return true;
}

static bool expect_channel_values(Session& s, const std::string& varName, int channel_index,
                                   const std::vector<double>& expected, double tol, std::string& err) {
  AuxObj obj = aux_get_var(s.ctx, varName);
  if (!obj) { err = "Variable not found: " + varName; return false; }
  const size_t len = aux_flatten_channel_length(obj, channel_index);
  if (len < expected.size()) {
    err = "Channel " + std::to_string(channel_index) + " of " + varName + " too short: " + std::to_string(len);
    return false;
  }
  std::vector<double> vals(len, 0.0);
  if (aux_flatten_channel(obj, channel_index, vals.data(), vals.size()) != len) {
    err = "aux_flatten_channel failed for " + varName + " channel " + std::to_string(channel_index);
    return false;
  }
  for (size_t i = 0; i < expected.size(); ++i) {
    if (std::fabs(vals[i] - expected[i]) > tol) {
      err = varName + " channel " + std::to_string(channel_index) + " sample " + std::to_string(i) +
            ": got " + std::to_string(vals[i]) + ", expected " + std::to_string(expected[i]);
      return false;
    }
  }
  return true;
}

static bool case_channel_left_numeric_range_write_scoped(std::string& err) {
  Session s("case_channel_left_numeric_range_write_scoped");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=[silence(1000); silence(1000)]", "stereo silence setup") ||
      !expect_eval_ok(s, "x.left(1:5)=0.5", "x.left(1:5)=0.5")) {
    err = s.err;
    return false;
  }
  if (!expect_channel_values(s, "x", 0, {0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0}, 1e-9, err))
    return false;
  if (!expect_channel_values(s, "x", 1, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, 1e-9, err))
    return false;
  return true;
}

static bool case_channel_right_numeric_range_write_scoped(std::string& err) {
  Session s("case_channel_right_numeric_range_write_scoped");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=[silence(1000); silence(1000)]", "stereo silence setup") ||
      !expect_eval_ok(s, "x.right(1:5)=0.5", "x.right(1:5)=0.5")) {
    err = s.err;
    return false;
  }
  if (!expect_channel_values(s, "x", 1, {0.5, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0}, 1e-9, err))
    return false;
  if (!expect_channel_values(s, "x", 0, {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, 1e-9, err))
    return false;
  return true;
}

static bool case_channel_left_time_range_write_scoped(std::string& err) {
  Session s("case_channel_left_time_range_write_scoped");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=[silence(1000); silence(1000)]", "stereo silence setup") ||
      !expect_eval_ok(s, "x.left(0~.01)=noise(10)", "x.left(0~.01)=noise(10)")) {
    err = s.err;
    return false;
  }
  AuxObj obj = aux_get_var(s.ctx, "x");
  if (!obj) { err = "Variable not found: x"; return false; }
  const size_t leftLen = aux_flatten_channel_length(obj, 0);
  std::vector<double> left(leftLen, 0.0);
  aux_flatten_channel(obj, 0, left.data(), left.size());
  bool anyNonzero = false;
  for (double v : left) if (std::fabs(v) > 1e-9) { anyNonzero = true; break; }
  if (!anyNonzero) {
    err = "Expected some left-channel samples to be nonzero after x.left(0~.01)=noise(10)";
    return false;
  }
  const size_t rightLen = aux_flatten_channel_length(obj, 1);
  std::vector<double> right(rightLen, 0.0);
  aux_flatten_channel(obj, 1, right.data(), right.size());
  for (double v : right) {
    if (std::fabs(v) > 1e-9) {
      err = "Right channel should remain untouched (silence) after x.left(...)=... write.";
      return false;
    }
  }
  return true;
}

static bool case_channel_right_compound_gain_write_scoped(std::string& err) {
  Session s("case_channel_right_compound_gain_write_scoped");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=[tone(500,100)@-12; tone(1000,100)@-18]", "stereo tone setup") ||
      !expect_eval_ok(s, "x.right(25~75)@@=-35", "x.right(25~75)@@=-35") ||
      !expect_eval_ok(s, "l=x.left(25~75).rms", "left rms after right scoped @@=") ||
      !expect_eval_ok(s, "r=x.right(25~75).rms", "right rms after right scoped @@=") ||
      !expect_scalar_value(s, "l", -12.0, 0.5) ||
      !expect_scalar_value(s, "r", -53.0, 0.5)) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_channel_selector_read_then_index(std::string& err) {
  Session s("case_channel_selector_read_then_index");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=[silence(1000); silence(1000)]", "stereo silence setup") ||
      !expect_eval_ok(s, "x.left(1:5)=0.5", "x.left(1:5)=0.5") ||
      !expect_eval_ok(s, "b=x.left(1:5)", "b=x.left(1:5)") ||
      !expect_vector_values(s, "b", {0.5, 0.5, 0.5, 0.5, 0.5})) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_channel_lhs_rejects_computed_suffix(std::string& err) {
  Session s("case_channel_lhs_rejects_computed_suffix");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=[silence(500); silence(500)]", "stereo silence setup") ||
      !expect_eval_error(s, "x.left.rms=0.5", "x.left.rms=0.5")) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_channel_write_requires_stereo(std::string& err) {
  Session s("case_channel_write_requires_stereo");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=silence(500)", "mono silence setup") ||
      !expect_eval_error(s, "x.left(1:5)=0.5", "x.left(1:5)=0.5 on mono x")) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_channel_write_requires_index(std::string& err) {
  Session s("case_channel_write_requires_index");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=[silence(500); silence(500)]", "stereo silence setup") ||
      !expect_eval_error(s, "x.left=0.5", "bare x.left=0.5")) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_long_stereo_plus_equals_short_left_only_stereo(std::string& err) {
  Session s("case_long_stereo_plus_equals_short_left_only_stereo");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=[silence(2000); silence(2000)]", "long stereo setup") ||
      !expect_eval_ok(s, "short=silence(500)", "short mono setup") ||
      !expect_eval_ok(s, "x += [short;]", "long stereo plus-equals short left-only stereo")) {
    err = s.err;
    return false;
  }
  AuxObj x = aux_get_var(s.ctx, "x");
  if (!x || !aux_is_audio(x)) {
    err = "x should remain stereo audio.";
    return false;
  }
  const size_t leftLen = aux_flatten_channel_length(x, 0);
  const size_t rightLen = aux_flatten_channel_length(x, 1);
  const size_t expectedLen = static_cast<size_t>(std::lround(2000.0 / 1000.0 * s.cfg.sample_rate));
  if (leftLen != expectedLen || rightLen != expectedLen) {
    err = "Unexpected x channel lengths: left=" + std::to_string(leftLen) +
          ", right=" + std::to_string(rightLen) +
          ", expected=" + std::to_string(expectedLen);
    return false;
  }
  return true;
}

static bool case_randperm_shuffles_full_range(std::string& err) {
  Session s("case_randperm_shuffles_full_range");
  if (!s.ok()) { err = s.err; return false; }
  if (!expect_eval_ok(s, "a=randperm(1200).group(20)", "grouped randperm") ||
      !expect_eval_ok(s, "p=a.ungroup", "flatten grouped randperm")) {
    err = s.err;
    return false;
  }
  AuxObj obj = aux_get_var(s.ctx, "p");
  std::vector<double> values(1200);
  if (!obj || aux_vector_length(obj) != values.size() ||
      aux_copy_vector(obj, values.data(), values.size()) != values.size()) {
    err = "Expected 1200 permutation entries.";
    return false;
  }
  std::vector<bool> seen(1200, false);
  size_t fixed = 0;
  for (size_t i = 0; i < values.size(); ++i) {
    double v = values[i];
    if (!std::isfinite(v) || v < 1 || v > 1200 || v != std::floor(v) || seen[static_cast<size_t>(v) - 1]) {
      err = "Output must contain each integer from 1 to 1200 exactly once.";
      return false;
    }
    seen[static_cast<size_t>(v) - 1] = true;
    if (v == i + 1) ++fixed;
  }
  // The old 346-swap implementation leaves at least 508 fixed points.
  // A uniform permutation has one on average; this generous bound avoids
  // requiring a derangement or any particular random ordering.
  if (fixed >= 100) {
    err = "Too many untouched entries: " + std::to_string(fixed);
    return false;
  }
  if (!expect_eval_ok(s, "single=randperm(1)", "singleton randperm") ||
      !expect_vector_values(s, "single", {1.0}) ||
      !expect_eval_error(s, "randperm(0)", "zero randperm") ||
      !expect_eval_error(s, "randperm(-1)", "negative randperm")) {
    err = s.err;
    return false;
  }
  return true;
}

static bool case_class_internal_tag_hidden_from_preview_and_members(std::string& err) {
  Session s("case_class_internal_tag_hidden_from_preview_and_members");
  if (!s.ok()) { err = s.err; return false; }

  const fs::path classFile = s.dir / "exclass.aux";
  if (!write_file(classFile,
                  "class exclass\n"
                  "id=0\n"
                  "name=\"\"\n"
                  "grade\n"
                  "method exclass(i,n)\n"
                  "id=i\n"
                  "name=n\n"
                  "method out=myfunc(arg)\n"
                  "out=name++sprintf(\"%d\",id)\n",
                  s.err)) {
    err = s.err;
    return false;
  }
  if (aux_add_udfpath(s.ctx, s.dir.string() + "/") != 0) {
    err = "aux_add_udfpath failed for " + s.dir.string();
    return false;
  }

  std::string preview;
  if (aux_eval(&s.ctx, "u=exclass(1,\"z\")", s.cfg, preview) != static_cast<int>(auxEvalStatus::AUX_EVAL_OK)) {
    err = "Class instantiation failed: " + preview;
    return false;
  }
  if (preview.find("__class") != std::string::npos) {
    err = "Class preview should hide __class, got: " + preview;
    return false;
  }
  if (preview.find(".id") == std::string::npos || preview.find(".name") == std::string::npos ||
      preview.find(".grade") == std::string::npos) {
    err = "Class preview should still show public members, got: " + preview;
    return false;
  }

  AuxObj obj = aux_get_var(s.ctx, "u");
  if (!obj) {
    err = "Variable not found: u";
    return false;
  }
  uint16_t type = 0;
  std::string size;
  if (aux_describe_var(s.ctx, obj, s.cfg, type, size, preview) != 0) {
    err = "aux_describe_var failed for u";
    return false;
  }
  if (size != "3") {
    err = "Class object size should count visible members only, got size=" + size + ", preview=" + preview;
    return false;
  }
  if (preview.find("__class") != std::string::npos) {
    err = "Class variable preview should hide __class, got: " + preview;
    return false;
  }

  const auto members = aux_get_struct(s.ctx, "u");
  if (members.find("__class") != members.end() || members.find("id") == members.end() ||
      members.find("name") == members.end() || members.find("grade") == members.end()) {
    err = "aux_get_struct should return only visible class members.";
    return false;
  }

  if (!expect_eval_ok(s, "u.grade=\"A\"", "write declared class member") ||
      !expect_eval_error(s, "u.address=\"bogus\"", "write undeclared class member") ||
      !expect_eval_error(s, "u.__class", "read reserved class metadata") ||
      !expect_eval_error(s, "u.__class=\"bogus\"", "write reserved class metadata")) {
    err = s.err;
    return false;
  }
  if (aux_eval(&s.ctx, "out=u.myfunc(0)", s.cfg, preview) != static_cast<int>(auxEvalStatus::AUX_EVAL_OK) ||
      preview.find("\"z1\"") == std::string::npos) {
    err = "Class method call should return \"z1\", got: " + preview;
    return false;
  }

  const fs::path badClassFile = s.dir / "badclass.aux";
  if (!write_file(badClassFile,
                  "class badclass\n"
                  "__class=\"bogus\"\n",
                  s.err)) {
    err = s.err;
    return false;
  }
  if (!expect_eval_error(s, "bad=badclass()", "declare reserved class metadata member")) {
    err = s.err;
    return false;
  }

  return true;
}

static bool case_class_without_constructor_requires_parentheses(std::string& err) {
  Session s("case_class_without_constructor_requires_parentheses");
  if (!s.ok()) { err = s.err; return false; }

  const fs::path classFile = s.dir / "emptyctor.aux";
  if (!write_file(classFile,
                  "class emptyctor\n"
                  "id=3\n"
                  "name=\"default\"\n",
                  s.err)) {
    err = s.err;
    return false;
  }
  if (aux_add_udfpath(s.ctx, s.dir.string() + "/") != 0) {
    err = "aux_add_udfpath failed for " + s.dir.string();
    return false;
  }

  std::string preview;
  if (aux_eval(&s.ctx, "u=emptyctor()", s.cfg, preview) != static_cast<int>(auxEvalStatus::AUX_EVAL_OK)) {
    err = "Class without constructor should instantiate with empty parentheses: " + preview;
    return false;
  }
  if (preview.find(".id") == std::string::npos || preview.find(".name") == std::string::npos) {
    err = "Class without constructor should expose declared defaults, got: " + preview;
    return false;
  }
  if (!expect_eval_error(s, "v=emptyctor", "class constructor without parentheses")) {
    err = s.err;
    return false;
  }

  return true;
}

static bool case_class_method_not_shadowed_by_caller_local(std::string& err) {
  Session s("case_class_method_not_shadowed_by_caller_local");
  if (!s.ok()) { err = s.err; return false; }

  if (!write_file(s.dir / "tagged.aux",
                  "class tagged\n"
                  "id=0\n"
                  "\n"
                  "method tagged(i)\n"
                  "id=i\n"
                  "\n"
                  "method out=myfunc(arg)\n"
                  "out=\"CLASS\"\n",
                  s.err)) {
    err = s.err;
    return false;
  }
  // A UDF with a local subfunction whose name collides with the class method.
  if (!write_file(s.dir / "caller.aux",
                  "function out=caller(obj)\n"
                  "out=obj.myfunc(0)\n"
                  "\n"
                  "function out=myfunc(o,a)\n"
                  "out=\"LOCAL\"\n",
                  s.err)) {
    err = s.err;
    return false;
  }
  if (aux_add_udfpath(s.ctx, s.dir.string() + "/") != 0) {
    err = "aux_add_udfpath failed for " + s.dir.string();
    return false;
  }

  std::string preview;
  if (aux_eval(&s.ctx, "u=tagged(1)", s.cfg, preview) != static_cast<int>(auxEvalStatus::AUX_EVAL_OK)) {
    err = "Class instantiation failed: " + preview;
    return false;
  }
  // A receiver-qualified call must dispatch through the receiver's class, even when the
  // calling UDF has a local subfunction of the same name.
  if (aux_eval(&s.ctx, "out=caller(u)", s.cfg, preview) != static_cast<int>(auxEvalStatus::AUX_EVAL_OK)) {
    err = "Qualified method call inside a UDF failed: " + preview;
    return false;
  }
  if (preview.find("CLASS") == std::string::npos) {
    err = "Caller's local subfunction shadowed the class method, got: " + preview;
    return false;
  }

  return true;
}

static bool case_member_function_text_in_string_literal_preserved(std::string& err) {
  Session s("case_member_function_text_in_string_literal_preserved");
  if (!s.ok()) { err = s.err; return false; }

  // The "member function" rewrite must not edit text inside a string literal.
  std::string preview;
  if (aux_eval(&s.ctx, "a=\"member function\"", s.cfg, preview) != static_cast<int>(auxEvalStatus::AUX_EVAL_OK)) {
    err = "String literal assignment failed: " + preview;
    return false;
  }
  if (preview.find("member function") == std::string::npos) {
    err = "String literal was rewritten by the member-function preprocessor, got: " + preview;
    return false;
  }
  if (aux_eval(&s.ctx, "b=\"say \"\"member function\"\" ok\"", s.cfg, preview) != static_cast<int>(auxEvalStatus::AUX_EVAL_OK)) {
    err = "Escaped-quote string literal failed: " + preview;
    return false;
  }
  if (preview.find("member function") == std::string::npos) {
    err = "Escaped-quote string literal was rewritten, got: " + preview;
    return false;
  }

  return true;
}

static bool case_class_method_parameter_colliding_with_member_is_rejected(std::string& err) {
  Session s("case_class_method_parameter_colliding_with_member_is_rejected");
  if (!s.ok()) { err = s.err; return false; }

  // A formal parameter sharing a member name would shadow the member for the whole call,
  // leaving it unreachable from the body. The class must fail to load.
  if (!write_file(s.dir / "collide.aux",
                  "class collide\n"
                  "id=7\n"
                  "\n"
                  "method show(id)\n"
                  "tmp=id\n",
                  s.err)) {
    err = s.err;
    return false;
  }
  // The constructor is checked the same way as any other method.
  if (!write_file(s.dir / "collidector.aux",
                  "class collidector\n"
                  "id=7\n"
                  "\n"
                  "method collidector(id)\n"
                  "tmp=id\n",
                  s.err)) {
    err = s.err;
    return false;
  }
  if (aux_add_udfpath(s.ctx, s.dir.string() + "/") != 0) {
    err = "aux_add_udfpath failed for " + s.dir.string();
    return false;
  }

  std::string preview;
  if (aux_eval(&s.ctx, "u=collide()", s.cfg, preview) != static_cast<int>(auxEvalStatus::AUX_EVAL_ERROR)) {
    err = "Colliding method parameter should be rejected at class load, got: " + preview;
    return false;
  }
  if (preview.find("collides with member") == std::string::npos) {
    err = "Unexpected collision error: " + preview;
    return false;
  }
  if (aux_eval(&s.ctx, "v=collidector()", s.cfg, preview) != static_cast<int>(auxEvalStatus::AUX_EVAL_ERROR)) {
    err = "Colliding constructor parameter should be rejected, got: " + preview;
    return false;
  }
  if (preview.find("collides with member") == std::string::npos) {
    err = "Unexpected collision error for constructor: " + preview;
    return false;
  }

  return true;
}

static bool case_class_member_writes_persist_from_method(std::string& err) {
  Session s("case_class_member_writes_persist_from_method");
  if (!s.ok()) { err = s.err; return false; }

  if (!write_file(s.dir / "persist.aux",
                  "class persist\n"
                  "id=7\n"
                  "v=[1 2 3]\n"
                  "\n"
                  // Plain member write.
                  "method setto(n)\n"
                  "id=n\n"
                  "\n"
                  // In-place member write must persist too.
                  "method poke(n)\n"
                  "v(2)=n\n"
                  "\n"
                  // A local that is not a declared member must not become one.
                  "method scratch(n)\n"
                  "tmp=n\n"
                  "\n"
                  "method out=readv()\n"
                  "out=v\n",
                  s.err)) {
    err = s.err;
    return false;
  }
  if (aux_add_udfpath(s.ctx, s.dir.string() + "/") != 0) {
    err = "aux_add_udfpath failed for " + s.dir.string();
    return false;
  }

  if (!expect_eval_ok(s, "u=persist()", "instantiate persist")) { err = s.err; return false; }
  if (!expect_eval_ok(s, "u.setto(42)", "plain member write")) { err = s.err; return false; }
  if (!expect_eval_ok(s, "x=u.id", "read member after plain write")) { err = s.err; return false; }
  if (!expect_scalar_value(s, "x", 42.0)) { err = s.err; return false; }

  if (!expect_eval_ok(s, "u.poke(99)", "in-place member write")) { err = s.err; return false; }
  if (!expect_eval_ok(s, "y=u.readv()", "read vector member")) { err = s.err; return false; }
  if (!expect_vector_values(s, "y", {1.0, 99.0, 3.0})) { err = s.err; return false; }

  if (!expect_eval_ok(s, "u.scratch(5)", "method-local scratch variable")) { err = s.err; return false; }
  if (!expect_eval_error(s, "z=u.tmp", "method local must not become a member")) { err = s.err; return false; }

  return true;
}

static bool case_standalone_method_udf_is_rejected(std::string& err) {
  Session s("case_standalone_method_udf_is_rejected");
  if (!s.ok()) { err = s.err; return false; }

  const fs::path udfFile = s.dir / "myfunc.aux";
  if (!write_file(udfFile,
                  "method out=myfunc(arg)\n"
                  "out=arg\n"
                  "end\n",
                  s.err)) {
    err = s.err;
    return false;
  }

  std::string defineErr;
  if (aux_define_udf(s.ctx, "myfunc", s.dir.string(), defineErr) == 0) {
    err = "aux_define_udf should reject a standalone method definition.";
    return false;
  }
  if (defineErr.find("Standalone UDF files cannot start with \"method\" or \"member function\"") == std::string::npos) {
    err = "Unexpected aux_define_udf error: " + defineErr;
    return false;
  }

  if (aux_add_udfpath(s.ctx, s.dir.string() + "/") != 0) {
    err = "aux_add_udfpath failed for " + s.dir.string();
    return false;
  }
  if (!expect_eval_error(s, "x=myfunc(1)", "lazy standalone method load")) {
    err = s.err;
    return false;
  }

  return true;
}

static bool case_standalone_member_function_udf_is_rejected(std::string& err) {
  Session s("case_standalone_member_function_udf_is_rejected");
  if (!s.ok()) { err = s.err; return false; }

  const fs::path udfFile = s.dir / "myfunc.aux";
  if (!write_file(udfFile,
                  "member function out=myfunc(arg)\n"
                  "out=arg\n"
                  "end\n",
                  s.err)) {
    err = s.err;
    return false;
  }

  std::string defineErr;
  if (aux_define_udf(s.ctx, "myfunc", s.dir.string(), defineErr) == 0) {
    err = "aux_define_udf should reject a standalone member function definition.";
    return false;
  }
  if (defineErr.find("Standalone UDF files cannot start with \"method\" or \"member function\"") == std::string::npos) {
    err = "Unexpected aux_define_udf error: " + defineErr;
    return false;
  }

  if (aux_add_udfpath(s.ctx, s.dir.string() + "/") != 0) {
    err = "aux_add_udfpath failed for " + s.dir.string();
    return false;
  }
  if (!expect_eval_error(s, "x=myfunc(1)", "lazy standalone member function load")) {
    err = s.err;
    return false;
  }

  return true;
}

static bool case_class_member_function_definition_is_rejected(std::string& err) {
  Session s("case_class_member_function_definition_is_rejected");
  if (!s.ok()) { err = s.err; return false; }

  const fs::path classFile = s.dir / "badmethod.aux";
  if (!write_file(classFile,
                  "class badmethod\n"
                  "id=0\n"
                  "member function out=myfunc(arg)\n"
                  "out=arg\n",
                  s.err)) {
    err = s.err;
    return false;
  }
  if (aux_add_udfpath(s.ctx, s.dir.string() + "/") != 0) {
    err = "aux_add_udfpath failed for " + s.dir.string();
    return false;
  }

  std::string preview;
  if (aux_eval(&s.ctx, "x=badmethod(1)", s.cfg, preview) != static_cast<int>(auxEvalStatus::AUX_EVAL_ERROR)) {
    err = "Class member function definition should have returned AUX_EVAL_ERROR, preview=" + preview;
    return false;
  }
  if (preview.find("Class methods must use \"method\"") == std::string::npos) {
    err = "Unexpected class member function error: " + preview;
    return false;
  }

  return true;
}

int main() {
  struct TestCase {
    const char* name;
    bool (*fn)(std::string&);
  };

  const TestCase tests[] = {
    {"case_class_internal_tag_hidden_from_preview_and_members", case_class_internal_tag_hidden_from_preview_and_members},
    {"case_class_without_constructor_requires_parentheses", case_class_without_constructor_requires_parentheses},
    {"case_class_method_not_shadowed_by_caller_local", case_class_method_not_shadowed_by_caller_local},
    {"case_member_function_text_in_string_literal_preserved", case_member_function_text_in_string_literal_preserved},
    {"case_class_method_parameter_colliding_with_member_is_rejected", case_class_method_parameter_colliding_with_member_is_rejected},
    {"case_class_member_writes_persist_from_method", case_class_member_writes_persist_from_method},
    {"case_standalone_method_udf_is_rejected", case_standalone_method_udf_is_rejected},
    {"case_standalone_member_function_udf_is_rejected", case_standalone_member_function_udf_is_rejected},
    {"case_class_member_function_definition_is_rejected", case_class_member_function_definition_is_rejected},
    {"case_randperm_shuffles_full_range", case_randperm_shuffles_full_range},
    {"case_empty_index_read_returns_null", case_empty_index_read_returns_null},
    {"case_zero_arg_parentheses_builtin_and_udf", case_zero_arg_parentheses_builtin_and_udf},
    {"case_group_overlap_uses_second_method_arg", case_group_overlap_uses_second_method_arg},
    {"case_group_overlap_pads_partial_final_group", case_group_overlap_pads_partial_final_group},
    {"case_ungroup_overlap_reverses_group_overlap", case_ungroup_overlap_reverses_group_overlap},
    {"case_empty_index_write_null_is_noop", case_empty_index_write_null_is_noop},
    {"case_empty_index_write_nonnull_is_error", case_empty_index_write_nonnull_is_error},
    {"case_end_index_on_empty_lhs_is_error", case_end_index_on_empty_lhs_is_error},
    {"case_empty_builtin_max_is_error", case_empty_builtin_max_is_error},
    {"case_indexed_scalar_write_matches_numeric_container", case_indexed_scalar_write_matches_numeric_container},
    {"case_complex_fft_index_extract_preserves_bins", case_complex_fft_index_extract_preserves_bins},
    {"case_complex_indexed_write_preserves_real_and_imag", case_complex_indexed_write_preserves_real_and_imag},
    {"case_continue_skips_remaining_loop_body", case_continue_skips_remaining_loop_body},
    {"case_long_stereo_plus_short_shifted_stereo", case_long_stereo_plus_short_shifted_stereo},
    {"case_channel_left_numeric_range_write_scoped", case_channel_left_numeric_range_write_scoped},
    {"case_channel_right_numeric_range_write_scoped", case_channel_right_numeric_range_write_scoped},
    {"case_channel_left_time_range_write_scoped", case_channel_left_time_range_write_scoped},
    {"case_channel_right_compound_gain_write_scoped", case_channel_right_compound_gain_write_scoped},
    {"case_channel_selector_read_then_index", case_channel_selector_read_then_index},
    {"case_channel_lhs_rejects_computed_suffix", case_channel_lhs_rejects_computed_suffix},
    {"case_channel_write_requires_stereo", case_channel_write_requires_stereo},
    {"case_channel_write_requires_index", case_channel_write_requires_index},
    {"case_long_stereo_plus_equals_short_left_only_stereo", case_long_stereo_plus_equals_short_left_only_stereo},
  };

  bool ok = true;
  for (const auto& tc : tests) {
    std::string err;
    if (tc.fn(err)) {
      std::cout << "[PASS] " << tc.name << "\n";
    } else {
      ok = false;
      std::cerr << "[FAIL] " << tc.name << ": " << err << "\n";
    }
  }
  return ok ? 0 : 1;
}
