#include <auxe/auxe.h>

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

static const auxNativeModuleHost* g_host = nullptr;

static void set_err(char* err, size_t err_cap, const char* msg)
{
  if (!err || err_cap == 0) return;
  const size_t n = std::min(err_cap - 1, std::strlen(msg));
  std::memcpy(err, msg, n);
  err[n] = '\0';
}

static int test_add1(auxContext*,
                     auxNativeValue receiver,
                     const auxNativeValue*,
                     size_t,
                     auxNativeMutableValue result,
                     char* err,
                     size_t err_cap)
{
  double value = 0.0;
  if (!g_host->value_get_scalar(receiver, &value)) {
    return g_host->result_set_scalar(result, value + 1.0);
  }
  set_err(err, err_cap, "test_add1 expects a scalar.");
  return 1;
}

static int test_vec_add1(auxContext*,
                         auxNativeValue receiver,
                         const auxNativeValue*,
                         size_t,
                         auxNativeMutableValue result,
                         char* err,
                         size_t err_cap)
{
  const size_t len = g_host->value_vector_length(receiver);
  if (len == 0) {
    set_err(err, err_cap, "test_vec_add1 expects a vector.");
    return 1;
  }
  std::vector<double> values(len);
  if (g_host->value_copy_vector(receiver, values.data(), values.size()) != len) {
    set_err(err, err_cap, "test_vec_add1 could not read vector.");
    return 1;
  }
  for (double& v : values) v += 1.0;
  return g_host->result_set_vector(result, values.data(), values.size());
}

static int test_string(auxContext*,
                       auxNativeValue receiver,
                       const auxNativeValue*,
                       size_t,
                       auxNativeMutableValue result,
                       char* err,
                       size_t err_cap)
{
  const size_t len = g_host->value_string_length(receiver);
  if (len == 0) {
    set_err(err, err_cap, "test_string expects a non-empty string.");
    return 1;
  }
  std::vector<char> text(len + 1);
  g_host->value_copy_string(receiver, text.data(), text.size());
  std::string out = std::string("native:") + text.data();
  return g_host->result_set_string(result, out.c_str());
}

static int test_audio(auxContext*,
                      auxNativeValue,
                      const auxNativeValue*,
                      size_t,
                      auxNativeMutableValue result,
                      char*,
                      size_t)
{
  const double left[] = {0.1, 0.2, 0.3};
  const double right[] = {-0.1, -0.2, -0.3};
  return g_host->result_set_audio_stereo(result, left, right, 3, 8000);
}

static int test_fail(auxContext*,
                     auxNativeValue,
                     const auxNativeValue*,
                     size_t,
                     auxNativeMutableValue,
                     char* err,
                     size_t err_cap)
{
  set_err(err, err_cap, "intentional native failure");
  return 1;
}

static int test_noarg(auxContext*,
                      auxNativeValue,
                      const auxNativeValue*,
                      size_t,
                      auxNativeMutableValue result,
                      char*,
                      size_t)
{
  return g_host->result_set_scalar(result, 77.0);
}

static int test_static(auxContext*,
                       auxNativeValue receiver,
                       const auxNativeValue*,
                       size_t,
                       auxNativeMutableValue result,
                       char* err,
                       size_t err_cap)
{
  double value = 0.0;
  if (!g_host->value_get_scalar(receiver, &value)) {
    return g_host->result_set_scalar(result, value * 2.0);
  }
  set_err(err, err_cap, "test_static expects a scalar.");
  return 1;
}

static auxNativeFunctionDesc kFunctions[] = {
    {"test_add1", 1, 1, 1, &test_add1},
    {"test_vec_add1", 1, 1, 1, &test_vec_add1},
    {"test_string", 1, 1, 1, &test_string},
    {"test_audio", 1, 1, 1, &test_audio},
    {"test_fail", 1, 1, 1, &test_fail},
    {"test_noarg", 0, 0, 0, &test_noarg},
    {"test_static", 1, 1, 0, &test_static},
};

extern "C" int auxe_module_init(const auxNativeModuleHost* host,
                                auxNativeModuleInfo* info,
                                char* err,
                                size_t err_cap)
{
  if (!host || !info || host->abi_version != AUXE_NATIVE_MODULE_ABI_VERSION) {
    set_err(err, err_cap, "bad host ABI");
    return 1;
  }
  g_host = host;
  info->abi_version = AUXE_NATIVE_MODULE_ABI_VERSION;
  info->name = "testmodule";
  info->functions = kFunctions;
  info->function_count = sizeof(kFunctions) / sizeof(kFunctions[0]);
  return 0;
}
