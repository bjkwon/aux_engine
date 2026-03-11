#include <auxe/auxe.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

static int fail(const std::string& msg) {
  std::cerr << "FAIL: " << msg << '\n';
  return 1;
}

int main() {
  const fs::path testDir = fs::temp_directory_path() / "auxe_regression_debug_resume";
  std::error_code ec;
  fs::create_directories(testDir, ec);
  if (ec) {
    return fail("Could not create temp test directory: " + testDir.string());
  }

  const fs::path udfPath = testDir / "dbg_parent.aux";
  {
    std::ofstream out(udfPath);
    if (!out) {
      return fail("Could not write test UDF file: " + udfPath.string());
    }
    out <<
      "function out = dbg_parent(x)\n"
      "y = dbg_child(x)\n"
      "out = y + 1\n"
      "\n"
      "function z = dbg_child(a)\n"
      "b = a + 2\n"
      "z = b * 3\n";
  }

  auxConfig cfg{};
  cfg.sample_rate = 22050;
  cfg.display_precision = 6;
  cfg.display_limit_x = 32;
  cfg.display_limit_y = 32;
  cfg.display_limit_bytes = 256;
  cfg.display_limit_str = 256;
  cfg.search_paths = {testDir.string()};
  cfg.debug_hook = nullptr;

  auxContext* ctx = aux_init(&cfg);
  if (!ctx) {
    return fail("aux_init returned null.");
  }

  int rc = 0;
  std::string preview;
  std::string err;
  rc = aux_define_udf(ctx, "dbg_parent", testDir.string(), err);
  if (rc != 0) {
    aux_close(ctx);
    return fail("aux_define_udf failed: " + err);
  }
  rc = aux_register_udf(ctx, "dbg_parent");
  if (rc != 0) {
    aux_close(ctx);
    return fail("aux_register_udf(dbg_parent) failed.");
  }

  // Break at child line "b = a + 2" via owning file UDF.
  // For local functions in same file, UI/debugger uses the base UDF entry.
  rc = aux_debug_add_breakpoints(ctx, "dbg_parent", std::vector<int>{6});
  if (rc != 0) {
    aux_close(ctx);
    return fail("aux_debug_add_breakpoints(dbg_parent, 6) failed.");
  }

  int st = aux_eval(&ctx, "out2 = dbg_parent(1)", cfg, preview);
  if (st != static_cast<int>(auxEvalStatus::AUX_EVAL_PAUSED)) {
    aux_close(ctx);
    return fail("Expected first eval to pause at dbg_child line 6.");
  }

  auxDebugInfo info{};
  if (aux_debug_get_pause_info(ctx, info) != 0 || info.line != 6) {
    aux_close(ctx);
    return fail("Expected pause at line 6.");
  }

  auto act = aux_debug_resume(&ctx, auxDebugAction::AUX_DEBUG_STEP);
  (void)act;
  if (aux_debug_get_pause_info(ctx, info) != 0 || info.line != 7) {
    aux_close(ctx);
    return fail("Expected step to pause at line 7.");
  }

  rc = aux_debug_del_breakpoints(ctx, "dbg_parent", std::vector<int>{-6});
  if (rc != 0) {
    aux_close(ctx);
    return fail("Clearing dbg_parent breakpoint at line 6 failed.");
  }

  act = aux_debug_resume(&ctx, auxDebugAction::AUX_DEBUG_CONTINUE);
  (void)act;

  if (aux_debug_get_pause_info(ctx, info) == 0) {
    aux_close(ctx);
    return fail("Engine is still paused after continue with breakpoint cleared.");
  }

  AuxObj out2 = aux_get_var(ctx, "out2");
  if (!out2) {
    aux_close(ctx);
    return fail("out2 was not created after continue.");
  }

  uint16_t type = 0;
  std::string size;
  std::string value;
  if (aux_describe_var(ctx, out2, cfg, type, size, value) != 0) {
    aux_close(ctx);
    return fail("aux_describe_var(out2) failed.");
  }

  // dbg_parent(1): child=(1+2)*3=9, out=10.
  if (value.find("10") == std::string::npos) {
    aux_close(ctx);
    return fail("Unexpected out2 value preview: " + value);
  }

  aux_close(ctx);
  std::cout << "PASS: regression_debug_resume\n";
  return 0;
}
