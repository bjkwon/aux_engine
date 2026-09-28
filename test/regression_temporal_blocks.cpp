#include <auxe/auxe.h>
#include "AuxScope_exception.h"

#include <cmath>
#include <iostream>
#include <string>
#include <vector>

struct Session {
  auxContext* ctx = nullptr;
  auxConfig cfg{};

  Session() {
    cfg.sample_rate = 1000;
    cfg.display_precision = 6;
    cfg.display_limit_x = 64;
    cfg.display_limit_y = 64;
    cfg.display_limit_bytes = 512;
    cfg.display_limit_str = 512;
    ctx = aux_init(&cfg);
  }

  ~Session() {
    if (ctx) aux_close(ctx);
  }
};

static bool eval_ok(Session& s, const std::string& command, std::string& err) {
  std::string preview;
  const int rc = aux_eval(&s.ctx, command, s.cfg, preview);
  if (rc != static_cast<int>(auxEvalStatus::AUX_EVAL_OK)) {
    err = command + " failed: " + preview;
    return false;
  }
  return true;
}

static bool eval_error(Session& s, const std::string& command, const std::string& expected, std::string& err) {
  std::string preview;
  const int rc = aux_eval(&s.ctx, command, s.cfg, preview);
  if (rc != static_cast<int>(auxEvalStatus::AUX_EVAL_ERROR)) {
    err = command + " should fail, rc=" + std::to_string(rc) + ", preview=" + preview;
    return false;
  }
  if (preview.find(expected) == std::string::npos) {
    err = command + " returned an unexpected error: " + preview;
    return false;
  }
  return true;
}

static bool mono_length_equals(Session& s, const std::string& name, size_t expected, std::string& err) {
  AuxObj obj = aux_get_var(s.ctx, name);
  if (!obj || !aux_is_audio(obj)) {
    err = name + " is not audio";
    return false;
  }
  const size_t actual = aux_flatten_channel_length(obj, 0);
  if (actual != expected) {
    err = name + " length=" + std::to_string(actual) + ", expected " + std::to_string(expected);
    return false;
  }
  return true;
}

static bool mono_segment_equals(Session& s, const std::string& name, size_t expected_length,
                                double expected_tmark, int expected_fs, std::string& err) {
  AuxObj obj = aux_get_var(s.ctx, name);
  if (!obj || aux_num_channels(obj) != 1 || aux_num_segments(obj, 0) != 1) {
    err = name + " is not a single-segment mono object";
    return false;
  }
  AuxSignal segment{};
  if (!aux_get_segment(obj, 0, 0, segment)) {
    err = "could not read " + name;
    return false;
  }
  if (segment.nSamples != expected_length || segment.tmark != expected_tmark || segment.fs != expected_fs) {
    err = name + " has length=" + std::to_string(segment.nSamples) +
          ", tmark=" + std::to_string(segment.tmark) +
          ", fs=" + std::to_string(segment.fs);
    return false;
  }
  return true;
}

static bool temporal_value_equals(Session& s, const std::string& name, double expected,
                                  double expected_tmark, std::string& err) {
  AuxObj obj = aux_get_var(s.ctx, name);
  if (!obj || aux_num_channels(obj) != 1) {
    err = name + " is not a one-value temporal result";
    return false;
  }
  int values = 0;
  AuxSignal value_segment{};
  const int segment_count = aux_num_segments(obj, 0);
  for (int i = 0; i < segment_count; ++i) {
    AuxSignal segment{};
    if (!aux_get_segment(obj, 0, i, segment)) continue;
    if (segment.nSamples == 0) continue;
    if (segment.nSamples != 1 || !segment.buf) {
      err = name + " contains a non-scalar segment";
      return false;
    }
    value_segment = segment;
    ++values;
  }
  if (values != 1) {
    err = name + " does not contain exactly one value";
    return false;
  }
  if (value_segment.buf[0] != expected || value_segment.tmark != expected_tmark) {
    err = name + " has value=" + std::to_string(value_segment.buf[0]) +
          ", tmark=" + std::to_string(value_segment.tmark);
    return false;
  }
  return true;
}

static bool case_ordinal_and_suffix_composition(std::string& err) {
  Session s;
  if (!s.ctx) { err = "aux_init failed"; return false; }
  if (!eval_ok(s,
      "x=dc(.1s)+dc(.08s)>>.2s+dc(.06s)>>.4s,"
      "b1=x{1},b2=x{2},b3=x{3},"
      "a={x},face(a,dc(.03s)),nested=a{1}{2},slice=a{1}{2}(.2s~.25s),"
      "block_start=x{2}.begint(),at_start=x.blockat(.45s).begint(),"
      "static_start=begint(x{3})", err)) return false;

  return mono_segment_equals(s, "b1", 100, 0., 1000, err) &&
         mono_segment_equals(s, "b2", 80, 200., 1000, err) &&
         mono_segment_equals(s, "b3", 60, 400., 1000, err) &&
         mono_segment_equals(s, "nested", 80, 200., 1000, err) &&
         mono_segment_equals(s, "slice", 50, 200., 1000, err) &&
         temporal_value_equals(s, "block_start", 200., 200., err) &&
         temporal_value_equals(s, "at_start", 400., 400., err) &&
         temporal_value_equals(s, "static_start", 400., 400., err);
}

static bool case_blockat_and_channels(std::string& err) {
  Session s;
  if (!s.ctx) { err = "aux_init failed"; return false; }
  if (!eval_ok(s,
      "x=dc(.1s)+dc(.08s)>>.2s+dc(.06s)>>.4s,"
      "t=.25s,p=x.blockat(t),start=x.blockat(.2s),"
      "st=[x;x],l=st.left{2},r=st.right.blockat(.45s),"
      "tail=r(.4s~.46s)", err)) return false;

  return mono_length_equals(s, "p", 80, err) &&
         mono_length_equals(s, "start", 80, err) &&
         mono_length_equals(s, "l", 80, err) &&
         mono_length_equals(s, "r", 60, err) &&
         mono_length_equals(s, "tail", 60, err);
}

static bool case_errors_and_read_only(std::string& err) {
  Session s;
  if (!s.ctx) { err = "aux_init failed"; return false; }
  if (!eval_ok(s, "x=dc(.1s)+dc(.08s)>>.2s,s=[x;x],a={x},face(a,dc(.03s))", err)) return false;

  return eval_error(s, "bad=x{0}", "positive integer", err) &&
         eval_error(s, "bad=x{1.5}", "positive integer", err) &&
         eval_error(s, "bad=x{3}", "Temporal block index", err) &&
         eval_error(s, "bad=s{1}", "use .left or .right", err) &&
         eval_error(s, "bad=s.blockat(.05s)", "use .left or .right", err) &&
         eval_error(s, "bad=x.blockat(-.01s)", "does not occur", err) &&
         eval_error(s, "bad=x.blockat(.15s)", "does not occur", err) &&
         eval_error(s, "bad=x.blockat(.28s)", "does not occur", err) &&
         eval_error(s, "bad=x.blockat(.3s)", "does not occur", err) &&
         eval_error(s, "bad=a.blockat(.01s)", "mono temporal receiver", err) &&
         eval_error(s, "x{2}=dc(.05s)", "read-only", err) &&
         eval_error(s, "a{1}{2}=dc(.05s)", "read-only", err) &&
         eval_error(s, "s.left{2}=dc(.05s)", "read-only", err) &&
         eval_error(s, "x.blockat(.25s)=dc(.05s)", "read-only", err);
}

static bool case_adjacent_chains_dissolve(std::string& err) {
  Session s;
  if (!s.ctx) { err = "aux_init failed"; return false; }
  // x(x>0) and x(x<=0) tile the timeline with interleaved, gap-free blocks; x(x<=0)
  // begins with a one-sample block at t=0.
  if (!eval_ok(s,
      "x=tone(20,1s).ramp(50),"
      "y=x(x>0)*x(x>0)-x(x<=0)*x(x<=0),"
      "z=x(x<=0)*x(x<=0)-x(x>0)*x(x>0),"
      "w=x(x>0)+x(x<=0),"
      "x1=x+1,d=x+x1(x<=0)-x", err)) return false;
  if (!(mono_segment_equals(s, "y", 1000, 0., 1000, err) &&
        mono_segment_equals(s, "z", 1000, 0., 1000, err) &&
        mono_segment_equals(s, "w", 1000, 0., 1000, err) &&
        mono_segment_equals(s, "d", 1000, 0., 1000, err))) return false;

  // The one-sample block at t=0 applies only at t=0, not across the overlapping block.
  AuxSignal segment{};
  if (!aux_get_segment(aux_get_var(s.ctx, "d"), 0, 0, segment) || !segment.buf) {
    err = "could not read d";
    return false;
  }
  if (std::fabs(segment.buf[0] - 1.) > 1e-12 || std::fabs(segment.buf[1]) > 1e-12) {
    err = "d(1)=" + std::to_string(segment.buf[0]) + ", d(2)=" + std::to_string(segment.buf[1]) +
          "; expected 1 and 0";
    return false;
  }
  return true;
}

static bool case_overlapping_blocks(std::string& err) {
  EngineRuntime runtime(1000);
  AuxScope scope(&runtime);
  AstNode location{};
  location.line = 1;
  location.col = 1;

  CVar contiguous;
  contiguous.SetFs(1000);
  contiguous.UpdateBuffer(100);
  contiguous.tmark = 0.;
  contiguous.chain = new CTimeSeries(1000);
  contiguous.chain->UpdateBuffer(100);
  contiguous.chain->tmark = 100.;
  CVar boundary = scope.temporal_block_at(contiguous, 100., &location);
  if (boundary.tmark != 100. || boundary.nSamples != 100 || boundary.chain) {
    err = "a contiguous boundary did not select only the following block";
    return false;
  }

  CVar source;
  source.SetFs(1000);
  source.UpdateBuffer(200);
  source.tmark = 0.;
  source.chain = new CTimeSeries(1000);
  source.chain->UpdateBuffer(200);
  source.chain->tmark = 100.;

  try {
    scope.temporal_block_at(source, 150., &location);
  } catch (const AuxScope_exception& e) {
    if (e.getErrMsg().find("more than one block") != std::string::npos) return true;
    err = "unexpected overlap error: " + e.getErrMsg();
    return false;
  }
  err = "overlapping block lookup should fail";
  return false;
}

int main() {
  struct TestCase { const char* name; bool (*run)(std::string&); };
  const TestCase tests[] = {
    {"ordinal_and_suffix_composition", case_ordinal_and_suffix_composition},
    {"blockat_and_channels", case_blockat_and_channels},
    {"errors_and_read_only", case_errors_and_read_only},
    {"overlapping_blocks", case_overlapping_blocks},
    {"adjacent_chains_dissolve", case_adjacent_chains_dissolve},
  };

  bool ok = true;
  for (const auto& test : tests) {
    std::string err;
    if (test.run(err)) std::cout << "[PASS] " << test.name << "\n";
    else { ok = false; std::cerr << "[FAIL] " << test.name << ": " << err << "\n"; }
  }
  return ok ? 0 : 1;
}
