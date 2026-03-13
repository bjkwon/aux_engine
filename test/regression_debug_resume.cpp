#include <auxe/auxe.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct Session {
  auxContext* ctx = nullptr;
  auxConfig cfg{};
  fs::path dir;
  std::string err;

  explicit Session(const std::string& name) {
    dir = fs::temp_directory_path() / "auxe_regression_debug_resume" / name;
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

static int eval(Session& s, const std::string& cmd) {
  std::string preview;
  return aux_eval(&s.ctx, cmd, s.cfg, preview);
}

static bool add_bp(Session& s, const std::string& udf, int line) {
  const int rc = aux_debug_add_breakpoints(s.ctx, udf, std::vector<int>{line});
  if (rc != 0) {
    s.err = "aux_debug_add_breakpoints(" + udf + ", " + std::to_string(line) + ") failed rc=" + std::to_string(rc);
    return false;
  }
  return true;
}

static bool del_bp(Session& s, const std::string& udf, int line) {
  const int rc = aux_debug_del_breakpoints(s.ctx, udf, std::vector<int>{-line});
  if (rc != 0) {
    s.err = "aux_debug_del_breakpoints(" + udf + ", " + std::to_string(line) + ") failed rc=" + std::to_string(rc);
    return false;
  }
  return true;
}

static bool paused_line(Session& s, int line) {
  auxDebugInfo info{};
  const int rc = aux_debug_get_pause_info(s.ctx, info);
  if (rc != 0) {
    s.err = "Expected paused state at line " + std::to_string(line) + ", but pause info was unavailable.";
    return false;
  }
  if (info.line != line) {
    s.err = "Expected pause line " + std::to_string(line) + ", got " + std::to_string(info.line) + ".";
    return false;
  }
  return true;
}

static bool not_paused(Session& s) {
  auxDebugInfo info{};
  if (aux_debug_get_pause_info(s.ctx, info) == 0) {
    s.err = "Expected no paused state, but still paused at line " + std::to_string(info.line) + ".";
    return false;
  }
  return true;
}

static void abort_if_paused(Session& s) {
  auxDebugInfo info{};
  if (aux_debug_get_pause_info(s.ctx, info) == 0) {
    aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_ABORT_BASE);
  }
}

static bool preview_has_number(const std::string& preview, double expected, double tol = 1e-8) {
  static const std::regex numRe(R"([-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)");
  for (std::sregex_iterator it(preview.begin(), preview.end(), numRe), end; it != end; ++it) {
    const double v = std::stod((*it).str());
    if (std::fabs(v - expected) <= tol) {
      return true;
    }
  }
  return false;
}

static bool expect_scalar(Session& s, const std::string& varName, double expected) {
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
  if (!preview_has_number(preview, expected)) {
    s.err = "Unexpected value for " + varName + ". preview=" + preview;
    return false;
  }
  return true;
}

static bool case1_continue_no_child_pause(std::string& err) {
  Session s("case1_continue_no_child_pause");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "c1.aux",
                  "function out = c1(x)\n"
                  "a = x + 1\n"
                  "out = a + 1\n", s.err) ||
      !define_register(s, "c1")) { err = s.err; return false; }
  if (!add_bp(s, "c1", 2)) { err = s.err; return false; }
  if (eval(s, "r1 = c1(1)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 2)) {
    err = s.err.empty() ? "Expected pause at c1 line 2." : s.err;
    return false;
  }
  if (!del_bp(s, "c1", 2)) { err = s.err; return false; }
  aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_CONTINUE);
  if (!not_paused(s) || !expect_scalar(s, "r1", 3.0)) { err = s.err; return false; }
  return true;
}

static bool case2_continue_multi_nested(std::string& err) {
  Session s("case2_continue_multi_nested");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "c2a.aux",
                  "function out = c2a(x)\n"
                  "y = c2b(x)\n"
                  "out = y + 1\n"
                  "\n"
                  "function out = c2b(v)\n"
                  "w = c2c(v)\n"
                  "out = w + 2\n"
                  "\n"
                  "function out = c2c(q)\n"
                  "p = q + 3\n"
                  "out = p * 2\n", s.err) ||
      !define_register(s, "c2a")) { err = s.err; return false; }
  if (!add_bp(s, "c2a", 10)) { err = s.err; return false; }
  if (eval(s, "r2 = c2a(1)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 10)) {
    err = s.err.empty() ? "Expected pause at c2a local line 10." : s.err;
    return false;
  }
  if (!del_bp(s, "c2a", 10)) { err = s.err; return false; }
  aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_CONTINUE);
  if (!not_paused(s) || !expect_scalar(s, "r2", 11.0)) { err = s.err; return false; }
  return true;
}

static bool case3_continue_hits_next_breakpoint(std::string& err) {
  Session s("case3_continue_hits_next_breakpoint");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "c3.aux",
                  "function out = c3(x)\n"
                  "a = x + 1\n"
                  "out = a + 1\n", s.err) ||
      !define_register(s, "c3")) { err = s.err; return false; }
  if (!add_bp(s, "c3", 2) || !add_bp(s, "c3", 3)) { err = s.err; return false; }
  if (eval(s, "r3 = c3(2)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 2)) {
    err = s.err.empty() ? "Expected first pause at c3 line 2." : s.err;
    return false;
  }
  if (!del_bp(s, "c3", 2)) { err = s.err; return false; }
  aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_CONTINUE);
  if (!paused_line(s, 3)) { err = s.err; return false; }
  abort_if_paused(s);
  return true;
}

static bool case4_step_then_continue_assignment(std::string& err) {
  Session s("case4_step_then_continue_assignment");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "c4f.aux",
                  "function out = c4f(a)\n"
                  "b = a + 4\n"
                  "out = b * 2\n", s.err) ||
      !define_register(s, "c4f")) { err = s.err; return false; }
  if (!add_bp(s, "c4f", 2)) { err = s.err; return false; }
  if (eval(s, "x4 = c4f(3)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 2)) {
    err = s.err.empty() ? "Expected pause at c4f line 2." : s.err;
    return false;
  }
  aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_STEP);
  if (!paused_line(s, 3)) { err = s.err; return false; }
  aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_CONTINUE);
  if (!not_paused(s) || !expect_scalar(s, "x4", 14.0)) { err = s.err; return false; }
  return true;
}

static bool case5_vector_lhs_pending_assignment(std::string& err) {
  Session s("case5_vector_lhs_pending_assignment");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "c5f.aux",
                  "function [o1,o2] = c5f(a)\n"
                  "t = a + 2\n"
                  "o1 = t\n"
                  "o2 = t * 10\n", s.err) ||
      !define_register(s, "c5f")) { err = s.err; return false; }
  if (!add_bp(s, "c5f", 2)) { err = s.err; return false; }
  if (eval(s, "[a5,b5] = c5f(5)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 2)) {
    err = s.err.empty() ? "Expected pause at c5f line 2." : s.err;
    return false;
  }
  aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_CONTINUE);
  if (!not_paused(s) || !expect_scalar(s, "a5", 7.0) || !expect_scalar(s, "b5", 70.0)) { err = s.err; return false; }
  return true;
}

static bool case6_pause_without_breakpoint(std::string& err) {
  Session s("case6_pause_without_breakpoint");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "c6f.aux",
                  "function out = c6f(a)\n"
                  "b = a + 1\n"
                  "c = b + 2\n"
                  "out = c\n", s.err) ||
      !define_register(s, "c6f")) { err = s.err; return false; }
  if (!add_bp(s, "c6f", 2)) { err = s.err; return false; }
  if (eval(s, "r6 = c6f(5)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 2)) {
    err = s.err.empty() ? "Expected pause at c6f line 2." : s.err;
    return false;
  }
  if (!del_bp(s, "c6f", 2)) { err = s.err; return false; }
  aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_STEP);
  // This pause is step-generated, not breakpoint-generated.
  if (!paused_line(s, 3)) { err = s.err; return false; }
  aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_CONTINUE);
  if (!not_paused(s) || !expect_scalar(s, "r6", 8.0)) { err = s.err; return false; }
  return true;
}

static bool case7_breakpoint_readd_same_line(std::string& err) {
  Session s("case7_breakpoint_readd_same_line");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "c7f.aux",
                  "function out = c7f(a)\n"
                  "b = a + 1\n"
                  "out = b + 2\n", s.err) ||
      !define_register(s, "c7f")) { err = s.err; return false; }
  if (!add_bp(s, "c7f", 2)) { err = s.err; return false; }
  if (eval(s, "r7a = c7f(1)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 2)) {
    err = s.err.empty() ? "Expected first pause at c7f line 2." : s.err;
    return false;
  }
  if (!del_bp(s, "c7f", 2)) { err = s.err; return false; }
  aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_CONTINUE);
  if (!not_paused(s) || !expect_scalar(s, "r7a", 4.0)) { err = s.err; return false; }
  if (!add_bp(s, "c7f", 2)) { err = s.err; return false; }
  if (eval(s, "r7b = c7f(2)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 2)) {
    err = s.err.empty() ? "Expected second pause at c7f line 2 after re-adding breakpoint." : s.err;
    return false;
  }
  abort_if_paused(s);
  return true;
}

static bool case8_local_and_cross_breakpoint_resolution(std::string& err) {
  Session s("case8_local_and_cross_resolution");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "c8local.aux",
                  "function out = c8local(x)\n"
                  "y = c8inner(x)\n"
                  "out = y + 1\n"
                  "\n"
                  "function out = c8inner(v)\n"
                  "t = v + 2\n"
                  "out = t * 3\n", s.err) ||
      !write_file(s.dir / "c8main.aux",
                  "function out = c8main(x)\n"
                  "y = c8ext(x)\n"
                  "out = y + 1\n", s.err) ||
      !write_file(s.dir / "c8ext.aux",
                  "function out = c8ext(v)\n"
                  "t = v + 4\n"
                  "out = t * 2\n", s.err) ||
      !define_register(s, "c8local") ||
      !define_register(s, "c8main") ||
      !define_register(s, "c8ext")) { err = s.err; return false; }

  // Local function in same file, addressed via owning UDF.
  if (!add_bp(s, "c8local", 6)) { err = s.err; return false; }
  if (eval(s, "l8 = c8local(1)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 6)) {
    err = s.err.empty() ? "Expected pause in local function at line 6." : s.err;
    return false;
  }
  abort_if_paused(s);
  if (!del_bp(s, "c8local", 6)) { err = s.err; return false; }

  // Cross-file callee addressed by its own UDF name.
  if (!add_bp(s, "c8ext", 2)) { err = s.err; return false; }
  if (eval(s, "x8 = c8main(1)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 2)) {
    err = s.err.empty() ? "Expected pause in cross-file callee c8ext line 2." : s.err;
    return false;
  }
  abort_if_paused(s);
  return true;
}

static bool case9_loop_continue_after_pause(std::string& err) {
  Session s("case9_loop_continue_after_pause");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "c9.aux",
                  "function out = c9(n)\n"
                  "s = 0\n"
                  "for k = 1:n\n"
                  "s += k\n"
                  "end\n"
                  "out = s\n", s.err) ||
      !define_register(s, "c9")) { err = s.err; return false; }
  if (!add_bp(s, "c9", 4)) { err = s.err; return false; }
  if (eval(s, "r9 = c9(3)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 4)) {
    err = s.err.empty() ? "Expected pause inside loop at line 4." : s.err;
    return false;
  }
  if (!del_bp(s, "c9", 4)) { err = s.err; return false; }
  aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_CONTINUE);
  if (!not_paused(s) || !expect_scalar(s, "r9", 6.0)) { err = s.err; return false; }
  return true;
}

static bool case10_switch_continue_after_pause(std::string& err) {
  Session s("case10_switch_continue_after_pause");
  if (!s.ok()) { err = s.err; return false; }
  if (!write_file(s.dir / "c10.aux",
                  "function out = c10(x)\n"
                  "switch x\n"
                  "case 1\n"
                  "y = 10\n"
                  "otherwise\n"
                  "y = 20\n"
                  "end\n"
                  "out = y + 1\n", s.err) ||
      !define_register(s, "c10")) { err = s.err; return false; }
  if (!add_bp(s, "c10", 4)) { err = s.err; return false; }
  if (eval(s, "r10 = c10(1)") != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED) || !paused_line(s, 4)) {
    err = s.err.empty() ? "Expected pause inside switch at line 4." : s.err;
    return false;
  }
  if (!del_bp(s, "c10", 4)) { err = s.err; return false; }
  aux_debug_resume(&s.ctx, auxDebugAction::AUX_DEBUG_CONTINUE);
  if (!not_paused(s) || !expect_scalar(s, "r10", 11.0)) { err = s.err; return false; }
  return true;
}

struct CaseSpec {
  const char* name;
  bool (*fn)(std::string&);
};

int main() {
  const std::vector<CaseSpec> cases = {
      {"continue_no_child_pause", case1_continue_no_child_pause},
      {"continue_multi_nested", case2_continue_multi_nested},
      {"continue_hits_next_breakpoint", case3_continue_hits_next_breakpoint},
      {"step_then_continue_assignment", case4_step_then_continue_assignment},
      {"vector_lhs_pending_assignment", case5_vector_lhs_pending_assignment},
      {"pause_without_breakpoint", case6_pause_without_breakpoint},
      {"breakpoint_readd_same_line", case7_breakpoint_readd_same_line},
      {"local_function_breakpoint_resolution", case8_local_and_cross_breakpoint_resolution},
      {"loop_continue_after_pause", case9_loop_continue_after_pause},
      {"switch_case_continue_after_pause", case10_switch_continue_after_pause},
  };

  for (const auto& tc : cases) {
    std::string err;
    if (!tc.fn(err)) {
      std::cerr << "FAIL [" << tc.name << "]: " << err << '\n';
      return 1;
    }
    std::cout << "PASS [" << tc.name << "]\n";
  }

  std::cout << "PASS: regression_debug_resume (all cases)\n";
  return 0;
}
