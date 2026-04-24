#include <auxe/auxe.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct Session {
  auxContext* ctx = nullptr;
  auxConfig cfg{};
  fs::path dir;
  std::string err;

  explicit Session(const std::string& name) {
    dir = fs::temp_directory_path() / "auxe_regression_graphics_nogui" / name;
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

struct EvalOutcome {
  int rc = -1;
  std::string preview;
};

static EvalOutcome eval(Session& s, const std::string& cmd) {
  EvalOutcome out;
  out.rc = aux_eval(&s.ctx, cmd, s.cfg, out.preview);
  return out;
}

static bool expect_error_contains(Session& s,
                                  const std::string& cmd,
                                  const std::string& needle,
                                  std::string& err) {
  const EvalOutcome out = eval(s, cmd);
  if (out.rc != static_cast<int>(auxEvalStatus::AUX_EVAL_ERROR)) {
    err = "Expected AUX_EVAL_ERROR for [" + cmd + "], rc=" + std::to_string(out.rc) +
          ", preview=[" + out.preview + "]";
    return false;
  }
  if (out.preview.find(needle) == std::string::npos) {
    err = "Expected error containing [" + needle + "] for [" + cmd + "], preview=[" + out.preview + "]";
    return false;
  }
  return true;
}

static bool expect_missing_var(Session& s, const std::string& varName, std::string& err) {
  if (aux_get_var(s.ctx, varName) != nullptr) {
    err = "Variable should not exist after failed eval: " + varName;
    return false;
  }
  return true;
}

static bool case_graphics_builtins_fail_gracefully(std::string& err) {
  Session s("case_graphics_builtins_fail_gracefully");
  if (!s.ok()) {
    err = s.err;
    return false;
  }

  const std::vector<std::string> setup = {
      "v=[1 3 2 5 4]",
      "x=[0 1 2 3 4]",
      "y=[10 20 15 25 22]",
  };
  for (const std::string& cmd : setup) {
    const EvalOutcome out = eval(s, cmd);
    if (out.rc != static_cast<int>(auxEvalStatus::AUX_EVAL_OK)) {
      err = "Setup eval failed for [" + cmd + "], preview=[" + out.preview + "]";
      return false;
    }
  }

  const std::vector<std::pair<std::string, std::string>> cases = {
      {"h=figure([100 100 640 480])", "Graphics backend not available in this frontend."},
      {"hf=figure(\"v\")", "Graphics backend not available in this frontend."},
      {"ax=axes([.13 .11 .775 .815])", "Graphics backend not available in this frontend."},
      {"p=plot(v)", "Graphics backend not available in this frontend."},
      {"ln=line(x,y)", "Graphics backend not available in this frontend."},
      {"tx=text(.2,.8,\"hello\")", "Graphics backend not available in this frontend."},
      {"delete(1)", "Graphics backend not available in this frontend."},
      {"repaint(1)", "Graphics backend not available in this frontend."},
      {"gcf", "Graphics backend not available in this frontend."},
      {"gca", "Graphics backend not available in this frontend."},
  };

  for (const auto& [cmd, needle] : cases) {
    if (!expect_error_contains(s, cmd, needle, err)) {
      return false;
    }
  }

  const std::vector<std::string> failedVars = {"h", "hf", "ax", "p", "ln", "tx"};
  for (const std::string& varName : failedVars) {
    if (!expect_missing_var(s, varName, err)) {
      return false;
    }
  }

  return true;
}

int main() {
  std::string err;

  if (!case_graphics_builtins_fail_gracefully(err)) {
    std::cerr << "FAIL: regression_graphics_nogui: " << err << "\n";
    return 1;
  }

  std::cout << "PASS: regression_graphics_nogui\n";
  return 0;
}
