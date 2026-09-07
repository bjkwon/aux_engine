# External Native Modules

This document describes the V1 external native module system for `auxe`. External native modules are trusted in-process C/C++ libraries that are installed on the user's machine and imported explicitly from AUX scripts.

## AUX Syntax

Import a module before using its functions:

```aux
import("wsola")
y = wsola::wsola_timestretch(x, 1.5)
```

An imported module may also be bound to an alias:

```aux
import("wsola", "ws")
y = ws::wsola_timestretch(x, 1.5)
```

For functions that allow receiver syntax, the first argument may be written before the module-qualified function name:

```aux
y = x.wsola::wsola_timestretch(1.5)
y = x.ws::wsola_timestretch(1.5)
```

These are equivalent at the AUX level:

```aux
y = wsola::wsola_timestretch(x, 1.5)
y = x.wsola::wsola_timestretch(1.5)
```

The older dotted module spelling remains accepted for compatibility:

```aux
y = wsola.wsola_timestretch(x, 1.5)
```

Prefer `module::function(...)` and `x.module::function(...)` in new scripts because `::` makes the module boundary explicit.

## Static vs Receiver-Callable Functions

External module functions use the same AUX convention as builtins: a function may opt out of dot notation when it is AUX-static.

In C/C++ module code, this is controlled by `auxNativeFunctionDesc::allow_dot_call`:

```cpp
static const auxNativeFunctionDesc kFunctions[] = {
    { "metadata", 1, 1, 0, &metadata },     // AUX-static: x.module::metadata() is rejected
    { "process", 2, 2, 1, &process },       // receiver-callable: x.module::process(factor) is allowed
};
```

`allow_dot_call = 0` means the function can still be called normally:

```aux
info = mod::metadata(x)
```

but receiver syntax is rejected:

```aux
info = x.mod::metadata()
```

`allow_dot_call = 1` allows both spellings.

## Callback Argument Convention

The native callback ABI separates the first AUX argument from the remaining arguments:

```cpp
using auxNativeFunctionCallback = int(*)(auxContext* ctx,
                                         auxNativeValue receiver,
                                         const auxNativeValue* args,
                                         size_t nargs,
                                         auxNativeMutableValue result,
                                         char* err,
                                         size_t err_cap);
```

For a normal qualified call with at least one AUX argument:

```aux
y = mod::process(x, 1.5)
```

the callback receives:

```text
receiver = x
args[0]  = 1.5
nargs    = 1
```

For the equivalent receiver call:

```aux
y = x.mod::process(1.5)
```

the callback receives the same values:

```text
receiver = x
args[0]  = 1.5
nargs    = 1
```

For a zero-argument call:

```aux
y = mod::version()
```

the callback receives an empty `receiver` and `nargs = 0`.

The engine checks `min_args` and `max_args` against the total AUX argument count, where `receiver` counts as one argument when present. `max_args = -1` means variadic.

The `allow_dot_call` flag controls whether AUX receiver syntax is legal. It does not change the callback ABI layout for ordinary `module::function(x, ...)` calls.

## Public C ABI

Every native module library exports this entrypoint:

```cpp
extern "C" int auxe_module_init(const auxNativeModuleHost* host,
                                auxNativeModuleInfo* info,
                                char* err,
                                size_t err_cap);
```

The module must:

- Check `host`, `info`, and `host->abi_version`.
- Store the `host` pointer if callbacks are needed later.
- Fill `info->abi_version`, `info->name`, `info->functions`, and `info->function_count`.
- Return `0` on success.
- Return nonzero on failure and write a short diagnostic to `err` when possible.

### Exporting the entrypoint on Windows

The loader resolves the symbol with `dlsym` on POSIX and `GetProcAddress` on
Windows. `extern "C"` alone gives the symbol external linkage but does **not**
put it in a DLL's export table, so on Windows the module must additionally be
exported. Any of these works:

```cpp
#ifdef _WIN32
#define AUXE_MODULE_EXPORT __declspec(dllexport)
#else
#define AUXE_MODULE_EXPORT
#endif

extern "C" AUXE_MODULE_EXPORT int auxe_module_init(/* ... */);
```

or, from CMake, without touching the source:

```cmake
set_target_properties(my_module PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
```

or a `.def` file listing `auxe_module_init`. Without one of these, `import`
fails on Windows with *"Module library does not export auxe_module_init"* even
though the DLL loaded successfully. The in-tree fixture
`test/native_module_fixture.cpp` relies on the `WINDOWS_EXPORT_ALL_SYMBOLS`
form.

Minimal shape:

```cpp
#include <auxe/auxe.h>

static const auxNativeModuleHost* g_host = nullptr;

static int add1(auxContext*,
                auxNativeValue receiver,
                const auxNativeValue*,
                size_t,
                auxNativeMutableValue result,
                char* err,
                size_t err_cap)
{
    double value = 0.0;
    if (g_host->value_get_scalar(receiver, &value) != 0) {
        if (err && err_cap > 0) {
            snprintf(err, err_cap, "add1 expects a scalar.");
        }
        return 1;
    }
    return g_host->result_set_scalar(result, value + 1.0);
}

static const auxNativeFunctionDesc kFunctions[] = {
    { "add1", 1, 1, 1, &add1 },
};

extern "C" int auxe_module_init(const auxNativeModuleHost* host,
                                auxNativeModuleInfo* info,
                                char* err,
                                size_t err_cap)
{
    if (!host || !info || host->abi_version != AUXE_NATIVE_MODULE_ABI_VERSION) {
        if (err && err_cap > 0) {
            snprintf(err, err_cap, "Unsupported auxe native module ABI.");
        }
        return 1;
    }
    g_host = host;
    info->abi_version = AUXE_NATIVE_MODULE_ABI_VERSION;
    info->name = "mymodule";
    info->functions = kFunctions;
    info->function_count = sizeof(kFunctions) / sizeof(kFunctions[0]);
    return 0;
}
```

## Function Descriptors

`auxNativeFunctionDesc` describes each exported AUX function:

```cpp
struct auxNativeFunctionDesc {
    const char* name;
    int min_args;
    int max_args;
    int allow_dot_call;
    auxNativeFunctionCallback callback;
};
```

Fields:

- `name`: Function name as used after the module qualifier. For `mymodule::add1(x)`, this is `"add1"`.
- `min_args`: Minimum total AUX argument count.
- `max_args`: Maximum total AUX argument count, or `-1` for variadic.
- `allow_dot_call`: `1` permits `x.module::function(...)`; `0` marks the function AUX-static.
- `callback`: Native function implementation.

Function names are registered under the loaded module's real name. Aliases are resolved by the engine at call time.

## Host Value Readers

Input values are opaque `auxNativeValue` handles. Use `auxNativeModuleHost` callbacks to inspect them:

```cpp
uint16_t (*value_type)(auxNativeValue value);
int (*value_get_scalar)(auxNativeValue value, auxtype* out);
size_t (*value_vector_length)(auxNativeValue value);
size_t (*value_copy_vector)(auxNativeValue value, auxtype* out, size_t max_len);
size_t (*value_string_length)(auxNativeValue value);
size_t (*value_copy_string)(auxNativeValue value, char* out, size_t max_len);
int (*value_num_channels)(auxNativeValue value);
int (*value_sample_rate)(auxNativeValue value);
size_t (*value_flatten_channel_length)(auxNativeValue value, int channel_index);
size_t (*value_flatten_channel)(auxNativeValue value, int channel_index, auxtype* out, size_t max_len);
```

Scalar/vector behavior:

- `value_get_scalar` succeeds only for scalar-compatible AUX values.
- `value_vector_length` returns `1` for scalars and the vector length for non-audio, non-string vectors.
- `value_copy_vector` copies scalar-compatible values as a one-item vector.

String behavior:

- `value_string_length` returns the byte length of an AUX string.
- `value_copy_string` writes a null-terminated string up to `max_len`.

Audio behavior:

- `value_num_channels` walks the AUX channel chain and returns the channel count.
- `value_sample_rate` returns the value's sample rate, or `0` for an empty value.
- `value_flatten_channel_length` returns the total sample count for a channel, including chained temporal segments.
- `value_flatten_channel` copies a flattened channel into the caller-provided buffer.

## Host Result Writers

Callbacks write exactly one output value through `auxNativeMutableValue`:

```cpp
int (*result_set_null)(auxNativeMutableValue result);
int (*result_set_scalar)(auxNativeMutableValue result, auxtype value);
int (*result_set_vector)(auxNativeMutableValue result, const auxtype* values, size_t len);
int (*result_set_string)(auxNativeMutableValue result, const char* value);
int (*result_set_audio_mono)(auxNativeMutableValue result, const auxtype* values, size_t frames, int sample_rate);
int (*result_set_audio_stereo)(auxNativeMutableValue result, const auxtype* left, const auxtype* right, size_t frames, int sample_rate);
int (*result_set_bytes)(auxNativeMutableValue result, const unsigned char* values, size_t len);
int (*result_set_file)(auxNativeMutableValue result, const char* path, int preferred_sample_rate);
```

Result behavior:

- `result_set_null` resets the output.
- `result_set_scalar`, `result_set_vector`, and `result_set_string` create ordinary AUX values.
- `result_set_audio_mono` and `result_set_audio_stereo` create audio values with the supplied sample rate.
- `result_set_bytes` creates an AUX byte object.
- `result_set_file` loads a local file path into an AUX value. WAV and MP3 are decoded to audio; text-like files become strings; other files become byte objects.

## Registry and Manifest

`import("module")` looks for a module directory named `module` in the module registry roots.

Registry roots:

- `AUXE_MODULE_PATH`, split with the platform path-list delimiter.
- macOS/Linux fallback: `~/.auxe/modules`
- Windows fallback: `%LOCALAPPDATA%\auxe\modules`

Module layout:

```text
module_name/
  auxe-module.json
  lib/
    macos-arm64/libmodule.dylib
    macos-x64/libmodule.dylib
    linux-arm64/libmodule.so
    linux-x64/libmodule.so
  bin/
    windows-arm64/module.dll
    windows-x64/module.dll
```

Minimal manifest:

```json
{
  "name": "module_name",
  "abi_version": 1,
  "library": "lib/macos-arm64/libmodule.dylib",
  "functions": [
    { "name": "module_function" }
  ]
}
```

Platform-specific manifest form:

```json
{
  "name": "module_name",
  "abi_version": 1,
  "platforms": {
    "macos-arm64": "lib/macos-arm64/libmodule.dylib",
    "macos-x64": "lib/macos-x64/libmodule.dylib",
    "linux-arm64": "lib/linux-arm64/libmodule.so",
    "linux-x64": "lib/linux-x64/libmodule.so",
    "windows-arm64": "bin/windows-arm64/module.dll",
    "windows-x64": "bin/windows-x64/module.dll"
  },
  "functions": [
    { "name": "module_function" }
  ]
}
```

Manifest rules:

- `name` must match the imported module name.
- `abi_version` must match `AUXE_NATIVE_MODULE_ABI_VERSION`.
- `platforms[platform_id]` is preferred when present.
- `library` is used when there is no matching `platforms` entry.
- Relative library paths are resolved from the module directory.
- If `functions` is present, every descriptor exported by the library must be listed there by name.

The manifest does not define `allow_dot_call`; that is owned by the compiled module descriptor so the engine validates the behavior actually exported by the library.

## Import and Resolution

`import("module")` binds the module name as the AUX module qualifier. `import("module", "alias")` binds `alias` instead.

Repeated imports of the same module and same binding in one `auxContext` are idempotent.

Imports fail if:

- The module name or alias is empty.
- The alias already refers to a different module.
- The alias collides with an existing builtin, pseudo variable, workspace variable, or runtime handle variable.
- The module cannot be found.
- The manifest is missing, malformed, or mismatched.
- The library cannot be loaded.
- The library does not export `auxe_module_init`.
- The module init callback fails.
- The module info name or ABI does not match.
- The module exports no functions.
- A function descriptor is invalid.
- A function descriptor is not listed in the manifest when the manifest has a `functions` list.

Module functions are not registered as global builtins. This fails even after import:

```aux
import("mymodule")
y = add1(4)
```

Use a module qualifier:

```aux
y = mymodule::add1(4)
```

## Callback Errors

A callback returns `0` on success. On failure, it returns nonzero and should write a readable message into `err`.

```cpp
if (bad_input) {
    snprintf(err, err_cap, "process expects mono or stereo audio.");
    return 1;
}
```

If a callback fails without writing an error string, auxe reports a generic native module callback failure for that function.

## Lifetime and Safety

Native modules are trusted in-process code. The engine loads libraries with normal OS dynamic loading and keeps them open for the lifetime of the `auxContext`'s runtime. Module libraries must not retain `auxNativeValue` or `auxNativeMutableValue` handles after a callback returns; copy any data that must outlive the callback.

The public ABI intentionally exposes opaque values and host callbacks instead of internal engine types. Do not include or depend on `AuxScope`, `AstNode`, `CVar`, or other private engine headers from external module projects.

## Current Limits

V1 supports scalar, vector, string, bytes, and simple mono/stereo audio exchange. It does not expose native inspection APIs for AUX structs, cells, runtime handles, or arbitrary object graphs.

External modules are local and trusted. V1 does not include online package discovery, download, uninstall, dependency solving, sandboxing, hot reload, or AUX-source module packaging.

## Verification

The auxe regression target `auxe_regression_external_module` builds a small native fixture and verifies:

- Import success and idempotence.
- Missing module, bad manifest, bad ABI, missing library, and missing entrypoint failures.
- Module-qualified calls.
- Alias-qualified calls.
- Receiver syntax.
- `allow_dot_call = 0` static rejection.
- Scalar, vector, string, audio, no-arg, and callback-failure paths.

