#include <auxe/auxe.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// Covers aux_list_udfs / aux_forget_udf: a UDF resolved from the working directory stays cached
// after the directory changes until it is forgotten, and is then searched for again.

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

static bool preview_has_number(const std::string& preview, double expected, double tol = 1e-8) {
  static const std::regex numRe(R"([-+]?\d*\.?\d+(?:[eE][-+]?\d+)?)");
  for (std::sregex_iterator it(preview.begin(), preview.end(), numRe), end; it != end; ++it) {
    if (std::fabs(std::stod((*it).str()) - expected) <= tol) {
      return true;
    }
  }
  return false;
}

static bool eval_expect(Session& s, const std::string& cmd, const std::string& varName, double expected) {
  std::string preview;
  if (aux_eval(&s.ctx, cmd, s.cfg, preview) != static_cast<int>(auxEvalStatus::AUX_EVAL_OK)) {
    s.err = "Eval failed: " + cmd + " -> " + preview;
    return false;
  }
  AuxObj obj = aux_get_var(s.ctx, varName);
  if (!obj) {
    s.err = "Variable not found: " + varName;
    return false;
  }
  uint16_t type = 0;
  std::string size;
  if (aux_describe_var(s.ctx, obj, s.cfg, type, size, preview) != 0 || !preview_has_number(preview, expected)) {
    s.err = "After '" + cmd + "', expected " + varName + " = " + std::to_string(expected) + ", preview=" + preview;
    return false;
  }
  return true;
}

static bool run(std::string& err) {
  const fs::path root = fs::temp_directory_path() / "auxe_regression_udf_cache";
  const fs::path dirA = root / "a";
  const fs::path dirB = root / "b";
  std::error_code ec;
  fs::remove_all(root, ec);
  fs::create_directories(dirA, ec);
  fs::create_directories(dirB, ec);
  if (ec) {
    err = "Could not create temp dirs under " + root.string();
    return false;
  }
  if (!write_file(dirA / "cachefn.aux", "function out = cachefn\nout = 1\n", err) ||
      !write_file(dirB / "cachefn.aux", "function out = cachefn\nout = 2\n", err)) {
    return false;
  }

  const fs::path original = fs::current_path();
  fs::current_path(dirA);

  bool ok = false;
  {
    Session s;
    if (!s.err.empty()) {
      err = s.err;
    } else if (!eval_expect(s, "r = cachefn()", "r", 1.0)) {
      err = s.err;
    } else {
      const auto listed = aux_list_udfs(s.ctx);
      const auto it = listed.find("cachefn");
      if (it == listed.end()) {
        err = "aux_list_udfs did not report cachefn.";
      } else if (fs::path(it->second).has_parent_path()) {
        err = "Expected cwd-relative path for cachefn, got " + it->second;
      } else {
        fs::current_path(dirB);
        if (!eval_expect(s, "r = cachefn()", "r", 1.0)) {
          err = "Cached UDF should survive a directory change until forgotten: " + s.err;
        } else if (aux_forget_udf(s.ctx, "CacheFn") != 0) {
          err = "aux_forget_udf(CacheFn) should succeed (case-insensitive).";
        } else if (aux_list_udfs(s.ctx).count("cachefn") != 0) {
          err = "cachefn still listed after aux_forget_udf.";
        } else if (aux_forget_udf(s.ctx, "cachefn") != 1) {
          err = "aux_forget_udf on an uncached name should return 1.";
        } else if (!eval_expect(s, "r = cachefn()", "r", 2.0)) {
          err = "Forgotten UDF should be re-resolved from the new directory: " + s.err;
        } else if (aux_forget_udf(nullptr, "cachefn") != -1) {
          err = "aux_forget_udf(nullptr) should return -1.";
        } else {
          ok = true;
        }
      }
    }
  }

  fs::current_path(original);
  fs::remove_all(root, ec);
  return ok;
}

int main() {
  std::string err;
  if (!run(err)) {
    std::cerr << "[FAIL] udf_cache: " << err << "\n";
    return 1;
  }
  std::cout << "[PASS] udf_cache\n";
  return 0;
}
