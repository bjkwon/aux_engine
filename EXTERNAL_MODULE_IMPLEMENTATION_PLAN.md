# External Native Module Implementation Plan

See `docs/external_modules.md` for the maintained external native module reference. This file records the original implementation plan and should stay shorter than the reference document.

## Summary

Add a first-version external module system for `auxe` that supports private C/C++ native modules installed once on a user machine and imported explicitly by AUX scripts.

V1 uses:

- `import("module_name")` from AUX code.
- A local module registry discovered from `AUXE_MODULE_PATH` and a per-user default directory.
- A stable exported C ABI at the module boundary.
- In-process trusted native code.

Future AUX-source modules should build on the same import concept, but they are not part of this native-module pass.

## V1 Behavior

An AUX script imports a native module before using its functions:

```aux
import("testmodule")
y = testmodule::test_add1(x)
y = x.testmodule::test_add1()

import("testmodule", "tm")
z = tm::test_add1(x)
z = x.tm::test_add1()
```

After import, module functions are resolved through the imported module namespace or alias with `module::function(...)` syntax. They are not registered as global builtins. For non-static functions, `x.module::function(...)` is equivalent to `module::function(x, ...)`. Repeated imports of the same module and alias in the same `auxContext` are idempotent.

Errors must be readable when a module is missing, a manifest is malformed, a library cannot be loaded, the module ABI is incompatible, the entrypoint is absent, an alias collides, or a module callback fails.

## ABI Boundary

Native modules export:

```cpp
extern "C" int auxe_module_init(const auxNativeModuleHost* host,
                                auxNativeModuleInfo* info,
                                char* err,
                                size_t err_cap);
```

The module receives host callbacks for reading opaque input values and writing an opaque output value. The ABI deliberately avoids exposing internal engine types such as `AuxScope`, `AstNode`, and `CVar`.

Each `auxNativeFunctionDesc` includes an `allow_dot_call` flag. Module C/C++ code marks an AUX-static function by setting `allow_dot_call` to `0`; set it to `1` for functions that may use receiver syntax such as `x.module::function(...)`.

V1 supports scalar, vector, string, and simple mono/stereo audio value exchange. Struct and cell inspection can be expanded later without changing the import model.

## Registry Layout

Registry roots:

- `AUXE_MODULE_PATH`, using the platform path-list delimiter.
- macOS/Linux default: `~/.auxe/modules`
- Windows default: `%LOCALAPPDATA%\auxe\modules`

Module layout:

```text
module_name/
  auxe-module.json
  lib/
    macos-arm64/libmodule.dylib
    macos-x64/libmodule.dylib
    linux-x64/libmodule.so
  bin/
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

## Non-Goals

V1 does not include online package discovery, download, uninstall, dependency solving, sandboxing, hot reload, or AUX-source module packaging.

## Verification

Regression coverage should build a small native test module during CTest and verify successful import, function dispatch, value conversion, idempotent import, and clear failure modes.
