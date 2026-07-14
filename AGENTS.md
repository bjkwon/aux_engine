# AGENTS.md

## Purpose

This repository contains `auxe`, the reusable C++17 AUX Engine library. It parses and evaluates AUX scripts, manages scopes and runtime values, exposes an embedding API, and provides built-in math, DSP, file, graphics, playback, and recording entry points.

Keep `aux_engine` focused on portable runtime semantics. Applications such as `auxlab2` and the console `example_apps/aux2` may install front-end backends, own UI state, decode/persist app-specific data, and decide how users interact with results.

## Repository Map

- `include/auxe/auxe.h`: public C++ embedding API. Include it as `<auxe/auxe.h>`.
- `src/engine/`: parser/runtime, scope management, type system, operators, flow/debug machinery, generated parser sources (`psycon.tab.*`, `psycon.yy.c`).
- `src/api/`: public API implementation and preview/echo formatting.
- `src/func/`: AUX builtins. Add builtin gates here and register them in `src/engine/AuxFunc.cpp` / `src/engine/builtin_functions.h`.
- `src/iir/`: legacy C IIR/filter math support.
- `test/`: CTest regression executables and AUX fixtures.
- `docs/`: detailed feature notes, currently including `fget`.
- `example_apps/aux2/`: console frontend example that embeds `auxe`; keep app-only behavior here rather than in the library.

## Build And Test

Default local verification:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DAUXE_BUILD_TESTS=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Core build options:

- `AUXE_BUILD_SHARED=ON|OFF`: build shared or static library.
- `AUXE_BUILD_TESTS=ON|OFF`: build regression test executables.
- `AUXE_ENABLE_PORTAUDIO` exists but current audio device I/O is backend/API driven; do not assume this option wires PortAudio into `auxe`.

Dependencies verified from CMake:

- CMake 3.20+
- C++17 compiler
- `nlohmann_json`
- FFTW3
- libsamplerate

For the console app:

```sh
cmake -S example_apps/aux2 -B example_apps/aux2/build \
  -DAUX2_USE_INSTALLED_AUXE=OFF
cmake --build example_apps/aux2/build -j
```

`example_apps/aux2` optionally uses Readline on non-Windows and PortAudio for playback if found.

There is no repository-wide formatter or lint target. Match surrounding style and run the build/tests above for library changes.

## Architecture Invariants

- `auxe` owns AUX language/runtime semantics: parsing, evaluation, value/type behavior, scopes, builtins, debug flow, runtime handles, and backend-facing semantic events.
- GUI rendering belongs outside `auxe`. Do not add Qt, windows, paint events, widget lifetime, device contexts, or GUI invalidation logic to this repo.
- Graphics builtins are registered in `auxe`, but rendering is performed by an installed graphics backend such as `auxlab2`. In no-GUI frontends, graphics operations must fail with clear runtime errors instead of crashing or becoming unregistered functions.
- Playback and recording builtins use frontend-installed playback/record backends. Keep device APIs and async device lifetime in apps/backends, not in core runtime code.
- Runtime handle values use `TYPEBIT_HANDLE` and `ISHANDLE`. Older internal names such as `struts`, `GOvars`, and `IsGO()` still exist; do not mechanically rename them without checking handle/reference semantics.
- `TYPEBIT_STRUT` is value-like struct data. `TYPEBIT_HANDLE` is reference-like identity/alias data. Preserve that distinction in assignment, property access, deletion, and preview paths.
- `fget` returns raw byte objects from local paths or HTTP(S) sources. See `docs/fget.md` before changing file/URL byte-fetch behavior.

## Coding Conventions

- Use C++17. Existing code uses a mix of tabs and legacy style; keep edits local and consistent with the file being changed.
- Builtins usually have a `set_builtin_function_*` descriptor and an `_*` gate function. Register new names in `EngineRuntime::InitBuiltInFunctions`.
- Keep public API changes in `include/auxe/auxe.h` and implement them in `src/api/interface.cpp`.
- Keep app-facing behavior behind explicit backend structs/hooks where possible.
- Avoid broad cleanup in legacy parser, IIR, or signal code unless it is required for the task and covered by verification.

## Platform Notes

- Windows builds use vcpkg-friendly `find_package` paths and set `_HAS_STD_BYTE=0` for MSVC byte-name compatibility.
- Unix/Linux links `pthread`, `m`, and `dl`.
- Remote `fget` uses `curl` or `wget` on non-Windows and PowerShell on Windows.
- `auxlab2` is a sibling repo that builds `aux_engine` with `add_subdirectory(../aux_engine ...)`; changes to public API or backend contracts may require coordinated edits and verification there.

## Do Not Edit Casually

- Build output directories such as `build*/`, `example_apps/aux2/build*/`, generated `auxe_build/`, CMake package files, compiled binaries, `.dylib`, and packaged artifacts.
- Generated parser files `src/engine/psycon.tab.c`, `src/engine/psycon.tab.h`, `src/engine/psycon.yy.c`, and `src/engine/psycon.yacc.h` unless intentionally regenerating/updating parser output.
- Third-party compatibility headers/sources such as `include/thirdparty/fftw3.h` and legacy `src/iir/*` unless the task is specifically about those dependencies.
- Local history/environment files such as `aux2.history` and `auxenv.json`.
- Backup or scratch artifacts such as `*.bak`.

## Verification Expectations

- For runtime, API, builtin, handle, graphics-no-GUI, playback/record callback, or parser behavior changes, run the full CTest suite.
- For graphics changes, also verify `auxlab2` behavior when possible because `auxe` owns semantic builtins while `auxlab2` owns actual rendering and focus/current-handle behavior.
- For public API changes, build at least one embedding target (`example_apps/aux2` or `auxlab2`) in addition to `auxe`.
- Treat compiler warnings from legacy code as existing noise unless the changed area introduces new warnings or touches that code path.

## Common Pitfalls

- Do not copy app-only command interception or Qt rendering logic from `auxlab2` into `auxe`.
- Do not assume roadmap docs describe the current state; verify against code first.
- Do not reintroduce `TYPEBIT_STRUTS`; code has moved to `TYPEBIT_HANDLE`.
- Do not treat all structs as handles or all handles as value structs.
- Do not add codec/UI dependencies to the core library just to support an app workflow.
- Do not edit generated/build artifacts in place to fix source problems.

## Documents To Consult

- `README.md`: high-level project goals, dependency notes, and platform build examples. Check against code before copying details.
- `HANDLE_RENAME_PLAN.md`: semantic background for handle/reference naming; note that the safe `TYPEBIT_HANDLE` rename is already reflected in code.
- `GRAPHICS_RUNTIME_BACKEND_SPLIT.md`: authoritative boundary between runtime graphics semantics and GUI rendering.
- `GRAPHICS_MIGRATION_ROADMAP.md`: graphics migration direction and success criteria; verify current implementation before treating items as pending.
- `docs/fget.md`: `fget` source forms, return type, prerequisites, and error behavior.
- `/Users/bkwon/dev/auxlab2/GRAPHICS_HANDLE_IMPLEMENTATION_PLAN.md`: app-side graphics semantics and manual expectations for GUI behavior.
- `/Users/bkwon/dev/auxlab2/README.md`, `TEST_PLAN_GRAPHICS_PLAY_RECORD.md`, and `MANUAL_CHECKLIST_AUXLAB2_GRAPHICS_PLAY_RECORD.md`: consult for app-specific graphics/play/record verification. A separate `auxlab2/AGENTS.md` should eventually hold this guidance.
