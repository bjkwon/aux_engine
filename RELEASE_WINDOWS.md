# Generating the auxe Windows Release Package

This is the step-by-step runbook for producing a redistributable **auxe SDK**
archive on Windows: the `auxe.dll` runtime, its import library, the public
header, the CMake package files, and the third-party runtime DLLs the engine
links against.

The artifact is a developer package. A UI layer (`auxlab2`, or any other
embedder) points `CMAKE_PREFIX_PATH` at the unpacked tree and consumes it with
`find_package(auxe CONFIG REQUIRED)`. `auxlab2` normally builds the engine from
source via `add_subdirectory(../aux_engine ...)`; this package exists for
consumers that want a prebuilt engine.

For the GUI application's own Windows release, see
`../auxlab2/RELEASE.md` and `../auxlab2/BUILDING.md`.

## Contents

- [What the package contains](#what-the-package-contains)
- [Prerequisites](#prerequisites)
- [The release procedure](#the-release-procedure)
- [Verifying the artifact](#verifying-the-artifact)
- [Consuming the package](#consuming-the-package)
- [Signing](#signing)
- [Release checklist](#release-checklist)
- [Troubleshooting](#troubleshooting)
- [Packaging options reference](#packaging-options-reference)

---

## What the package contains

`auxe-<version>-win64.zip` unpacks to:

```
auxe-<version>-win64/
  bin/
    auxe.dll                      engine runtime
    fftw3.dll                     bundled vcpkg runtime
    samplerate.dll                bundled vcpkg runtime
  lib/
    auxe.lib                      import library
    cmake/auxe/
      auxeConfig.cmake            find_package(auxe CONFIG) entry point
      auxeConfigVersion.cmake
      auxeTargets.cmake           defines the auxe::auxe imported target
      auxeTargets-release.cmake
  include/
    auxe/auxe.h                   the public API; the only header consumers need
    thirdparty/fftw3.h
  share/auxe/
    README.md  LICENSE  VERSION
    docs/                         engine documentation
```

Two things this package deliberately does **not** contain:

- **The engine's internal headers.** `AuxScope.h`, `csignals.h`, `utils.h`, and
  the rest of `src/` are private. Only symbols marked `AUXE_API` in
  `include/auxe/auxe.h` are exported from the DLL, so linking against anything
  else fails by design.
- **The `aux2` console app.** `example_apps/aux2` is a sample embedder and is
  not part of the SDK target set.

Exact DLL filenames for FFTW3 and libsamplerate depend on the vcpkg port
version (`fftw3.dll` vs `libfftw3-3.dll`, `samplerate.dll` vs
`samplerate-0.dll`). Whatever the linker actually recorded is what gets
bundled — the deploy step resolves them from `auxe.dll`'s import table rather
than from a hardcoded list.

### The ABI is C++, not C — read this before you distribute

Despite the `aux_*` naming, `include/auxe/auxe.h` is **not** a C ABI. The
exported signatures pass `std::string`, `std::vector`, and `std::map` by value
and by reference across the DLL boundary, e.g.:

```cpp
AUXE_API string aux_version(auxContext* ctx);
AUXE_API int    aux_eval(auxContext** ctx, const string& script,
                         const auxConfig& cfg, string& preview);
```

That makes the package **toolchain- and configuration-locked**. A consumer must
be built with:

- the same MSVC major toolset (the `v143` ABI, i.e. VS 2022) as the DLL,
- the same CRT flavor — a `Release` (`/MD`) DLL cannot be safely used from a
  `Debug` (`/MDd`) consumer, because the two get different `std::string`
  layouts and separate heaps,
- the same C++ standard library, which on Windows means MSVC's, not MinGW's.

Consequences for the release:

- Ship the toolset and configuration in the archive name or release notes.
- If downstream consumers need a Debug build, publish a **second** package
  built `--config Debug` rather than telling them to mix.
- Cross-compiler consumption (MinGW, Clang with libc++) is not supported. Only
  `auxlab2`-style MSVC consumers are.

This is fine for the in-house `auxlab2` use case, which is why the header looks
the way it does. It is the main reason this SDK is not a general-purpose
redistributable.

---

## Prerequisites

On the release machine:

1. **Visual Studio 2022** (or the 2022 Build Tools) with the *Desktop
   development with C++* workload.
2. **CMake 3.21 or newer** — `cmake --version`.
3. **Git for Windows**.
4. **vcpkg**, bootstrapped, with `VCPKG_ROOT` set as a real environment
   variable (the packaging step reads it to locate runtime DLLs):

   ```powershell
   git clone https://github.com/microsoft/vcpkg C:\dev\vcpkg
   C:\dev\vcpkg\bootstrap-vcpkg.bat
   [Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\dev\vcpkg", "User")
   ```

   Open a fresh PowerShell afterwards so `$env:VCPKG_ROOT` is populated.

5. The engine's dependencies for the `x64-windows` triplet:

   ```powershell
   & "$env:VCPKG_ROOT\vcpkg.exe" install fftw3:x64-windows libsamplerate:x64-windows nlohmann-json:x64-windows
   ```

Optional:

- **NSIS** on `PATH`, if you also want an installer (`-DAUXE_ENABLE_WINDOWS_NSIS=ON`).
- A **code-signing certificate** and `signtool.exe` (ships with the Windows SDK).

Qt is *not* a dependency of the engine. Only `auxlab2` needs Qt.

---

## The release procedure

Run everything from a plain PowerShell prompt. Paths below assume
`C:\dev\aux_engine`; substitute your checkout.

### 1. Start from a clean, tagged tree

```powershell
cd C:\dev\aux_engine
git fetch --all --tags
git status            # must be clean; the version is read from VERSION
Get-Content VERSION
```

`VERSION` drives `CPACK_PACKAGE_VERSION`, the `project()` version, and
`AuxeVersion.h`. Bump and commit it *before* packaging — a package built from a
dirty tree cannot be reproduced.

### 2. Remove stale build state

```powershell
Remove-Item -Recurse -Force C:\dev\aux_engine\build-release -ErrorAction SilentlyContinue
Remove-Item -Recurse -Force C:\dev\aux_engine\stage -ErrorAction SilentlyContinue
```

The source list uses `file(GLOB CONFIGURE_DEPENDS)`; a stale build directory
from a different generator or vcpkg state is the most common source of
confusing configure errors. Release builds should always start clean.

### 3. Configure

```powershell
cmake -S C:\dev\aux_engine -B C:\dev\aux_engine\build-release `
  -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake" `
  -DCMAKE_CONFIGURATION_TYPES="Debug;Release" `
  -DAUXE_BUILD_SHARED=ON `
  -DAUXE_BUILD_TESTS=ON `
  -DAUXE_ENABLE_CPACK=ON `
  -DAUXE_BUNDLE_RUNTIME_DEPS=ON
```

Notes on the flags:

- `AUXE_BUILD_SHARED=ON` is required for a redistributable package. It is what
  defines `AUXE_BUILD_DLL`, which flips `AUXE_API` from `__declspec(dllimport)`
  to `__declspec(dllexport)`. A static build produces `auxe.lib` with no DLL and
  no export table.
- `CMAKE_CONFIGURATION_TYPES="Debug;Release"` avoids vcpkg's `MinSizeRel` /
  `RelWithDebInfo` lookups failing on ports that only built those two.
- `AUXE_ENABLE_CPACK` and `AUXE_BUNDLE_RUNTIME_DEPS` are `ON` by default but are
  spelled out here so the release command is self-documenting. They only take
  effect when `aux_engine` is the top-level CMake project, which it is here.
- Ninja works too, from a *Developer PowerShell for VS 2022* prompt: swap
  `-G "Visual Studio 17 2022" -A x64` for `-G Ninja -DCMAKE_BUILD_TYPE=Release`
  and drop `--config Release` from the commands below.

### 4. Build

```powershell
cmake --build C:\dev\aux_engine\build-release --config Release -j
```

Visual Studio is a multi-config generator, so `--config Release` is required on
every build, test, install, and package invocation. Omitting it silently builds
`Debug` and you end up shipping a debug DLL that depends on the debug CRT.

### 5. Run the regression tests

```powershell
cd C:\dev\aux_engine\build-release
ctest -C Release --output-on-failure
cd C:\dev\aux_engine
```

All five tests must pass:

```
auxe_regression_debug_resume
auxe_regression_record_callback
auxe_regression_graphics_nogui
auxe_regression_eval_errors
auxe_regression_external_module
```

`auxe_regression_external_module` is the one that catches Windows-specific
packaging regressions: it loads a DLL and resolves `auxe_module_init` with
`GetProcAddress`, so it fails if module symbol export breaks.

### 6. Stage the install tree and inspect it

Do this before packaging. It is the same content the archive will hold, but in a
form you can read and diff.

```powershell
cmake --install C:\dev\aux_engine\build-release --config Release --prefix C:\dev\aux_engine\stage
Get-ChildItem -Recurse C:\dev\aux_engine\stage | Select-Object FullName
```

The install step runs `cmake/AuxeRuntimeDeploy.cmake.in`, which resolves
`auxe.dll`'s runtime dependencies with `file(GET_RUNTIME_DEPENDENCIES)` and
copies the non-system ones into `bin/`. Watch its output:

```
-- Collecting auxe runtime dependencies...
-- Installing: C:/dev/aux_engine/stage/bin/fftw3.dll
-- Installing: C:/dev/aux_engine/stage/bin/samplerate.dll
```

If it instead prints `auxe unresolved runtime deps (not bundled): ...`, the
listed DLLs were not found in any search directory. Check `VCPKG_ROOT` is set
and points at the vcpkg root, not at `installed\`.

### 7. Generate the package

```powershell
cmake --build C:\dev\aux_engine\build-release --target package --config Release
```

This writes `C:\dev\aux_engine\build-release\auxe-<version>-win64.zip`.

For an NSIS installer alongside the ZIP, reconfigure with
`-DAUXE_ENABLE_WINDOWS_NSIS=ON` and re-run the `package` target.

### 8. Publish checksums

```powershell
cd C:\dev\aux_engine\build-release
Get-FileHash -Algorithm SHA256 auxe-*.zip | Format-List
```

Ship the SHA-256 alongside the archive.

---

## Verifying the artifact

Do this on a **clean** Windows machine or VM — one without Visual Studio,
vcpkg, or the source tree. A package that only works on the build machine is
the failure this step exists to catch.

### 1. Contents

```powershell
Expand-Archive .\auxe-<version>-win64.zip -DestinationPath C:\check
Get-ChildItem -Recurse C:\check
```

Confirm `bin\auxe.dll`, `lib\auxe.lib`, `lib\cmake\auxe\auxeConfig.cmake`,
`include\auxe\auxe.h`, and the FFTW3/libsamplerate DLLs are all present.

### 2. The DLL's imports resolve

From a Developer prompt on a machine that has the SDK (or using
[Dependencies](https://github.com/lucasg/Dependencies) on a clean one):

```powershell
dumpbin /DEPENDENTS C:\check\bin\auxe.dll
```

Every non-`api-ms-win-*`, non-`KERNEL32`/`USER32`-style entry must be a file
that sits next to `auxe.dll` in `bin\`. `VCRUNTIME140.dll` and `MSVCP140.dll`
resolving to the system is expected if the target machine has the Visual C++
Redistributable; if you cannot assume that, ship the redistributable or document
it as a prerequisite.

### 3. The export table is populated

```powershell
dumpbin /EXPORTS C:\check\bin\auxe.dll | Select-String "aux"
```

An empty or near-empty export list means the DLL was built without
`AUXE_BUILD_DLL`, i.e. `AUXE_BUILD_SHARED` was `OFF` or a stale build directory
was reused.

### 4. A consumer actually builds and runs against it

This is the real test. On the clean machine, with only CMake and the MSVC
toolchain:

```powershell
mkdir C:\check\consumer; cd C:\check\consumer
```

`CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.21)
project(consumer LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
find_package(auxe CONFIG REQUIRED)
add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE auxe::auxe)
```

`main.cpp`:

```cpp
#include <auxe/auxe.h>
#include <cstdio>

int main() {
  auxConfig cfg{};
  auxContext* ctx = aux_init(&cfg);
  if (!ctx) { printf("aux_init failed\n"); return 1; }
  printf("auxe %s\n", aux_version(ctx).c_str());
  aux_close(ctx);
  return 0;
}
```

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:/check"
cmake --build build --config Release
Copy-Item C:\check\bin\*.dll .\build\Release\
.\build\Release\consumer.exe
```

The DLL copy is needed because Windows has no rpath: the loader looks next to
the `.exe`. Downstream projects should either copy the DLLs in a `POST_BUILD`
step (`$<TARGET_RUNTIME_DLLS:...>`) or put `bin\` on `PATH`.

---

## Consuming the package

```cmake
find_package(auxe CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE auxe::auxe)
```

configured with:

```powershell
cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/path/to/auxe-<version>-win64"
```

`auxe::auxe` carries its include directory, so `#include <auxe/auxe.h>` works
with no further wiring. FFTW3, libsamplerate, and nlohmann-json are linked
`PRIVATE` inside the engine, so consumers do **not** need those packages at
configure time — only the runtime DLLs, which the archive provides.

Version constraints work as expected (`COMPATIBILITY AnyNewerVersion`):

```cmake
find_package(auxe 2.5 CONFIG REQUIRED)
```

---

## Signing

Sign the DLL before packaging, then sign the resulting installer:

```powershell
$ts = "http://timestamp.digicert.com"

signtool sign /fd SHA256 /tr $ts /td SHA256 /a `
  C:\dev\aux_engine\stage\bin\auxe.dll

signtool verify /pa /v C:\dev\aux_engine\stage\bin\auxe.dll
```

ZIP archives cannot themselves be Authenticode-signed, so for a signed ZIP the
order matters: install to `stage`, sign the binaries in `stage`, then build the
`package` target — CPack re-runs the install into its own staging area, so
**re-run the install/sign step against CPack's tree or use the NSIS generator
and sign the installer instead**. The reliable sequences are:

- **ZIP**: sign `bin\*.dll` in a staged prefix, then `Compress-Archive` that
  prefix yourself, and publish the SHA-256.
- **NSIS**: build the installer with `-DAUXE_ENABLE_WINDOWS_NSIS=ON`, then
  `signtool sign` the produced `.exe`.

Do not ship third-party DLLs re-signed with your certificate; leave vcpkg's
FFTW3 and libsamplerate binaries as they are.

---

## Release checklist

1. `VERSION` bumped and committed; working tree clean.
2. `build-release\` and `stage\` deleted.
3. Configured with the vcpkg toolchain, `AUXE_BUILD_SHARED=ON`, `x64`.
4. Built `--config Release`.
5. `ctest -C Release` — all five tests pass.
6. Installed to `stage\` and the file list inspected.
7. `dumpbin /EXPORTS` shows the `aux*` API surface.
8. `package` target run; ZIP produced.
9. Consumer smoke test passes on a clean machine.
10. Binaries signed and verified.
11. SHA-256 published with the archive.
12. Git tag pushed for the released `VERSION`.

---

## Troubleshooting

| Symptom | Cause / fix |
| --- | --- |
| `Could not find a package configuration file provided by "nlohmann_json"` | vcpkg toolchain file not passed, or the port is missing for `x64-windows`. The engine requires it in CONFIG mode. |
| `Could not find any of: targets=[fftw3::fftw3 FFTW3::fftw3] or libraries=[fftw3]` | Same cause, for FFTW3. The `auxe_link_first_found` helper tried vcpkg targets then `find_library` and found neither. |
| `auxe unresolved runtime deps (not bundled)` at install | `VCPKG_ROOT` unset or wrong. The deploy script's search dirs are built from `CMAKE_PREFIX_PATH` and `VCPKG_ROOT`. |
| `dumpbin /EXPORTS` shows no `aux*` symbols | Built with `AUXE_BUILD_SHARED=OFF`, or a stale build dir carried the old setting. Delete `build-release\` and reconfigure. |
| Consumer gets `LNK2019 __imp_aux...` | Consumer is linking `auxe.lib` from a *static* build, or defining `AUXE_BUILD_DLL` itself. Consumers must not define it. |
| Consumer runs but exits `0xc0000135` / "auxe.dll was not found" | DLLs not next to the `.exe`. Copy `bin\*.dll` or add `bin\` to `PATH`. |
| `error C2872: 'byte': ambiguous symbol` | `_HAS_STD_BYTE=0` is normally set for MSVC by the engine's CMake; this appears when compiling engine sources outside this build system. |
| `fatal error C1083: Cannot open include file: 'unistd.h'` | Stale checkout. `test/regression_record_callback.cpp` was POSIX-only and is fixed. Workaround: `-DAUXE_BUILD_TESTS=OFF`. |
| `MinSizeRel`/`RelWithDebInfo` vcpkg lookup failures | Add `-DCMAKE_CONFIGURATION_TYPES="Debug;Release"`. |
| `auxe_regression_external_module` fails only on Windows | The test module's `auxe_module_init` is not in the DLL export table. The fixture target sets `WINDOWS_EXPORT_ALL_SYMBOLS`; third-party modules need `__declspec(dllexport)`. See `docs/external_modules.md`. |

---

## Packaging options reference

These options only take effect when `aux_engine` is the top-level CMake project.
When it is pulled in with `add_subdirectory` (as `auxlab2` does), the consuming
project owns packaging and none of this is active.

| Option | Default | Effect |
| --- | --- | --- |
| `AUXE_BUILD_SHARED` | `ON` | Build `auxe` as a DLL. Required for a redistributable package. |
| `AUXE_BUILD_TESTS` | `ON` | Build the five regression tests. Not installed; keep `ON` so step 5 can run. |
| `AUXE_ENABLE_CPACK` | `ON` | Register the CPack generator (`ZIP` on Windows, `TGZ` elsewhere). |
| `AUXE_BUNDLE_RUNTIME_DEPS` | `ON` | Copy FFTW3/libsamplerate runtimes into the install tree. |
| `AUXE_ENABLE_WINDOWS_NSIS` | `OFF` | Add an NSIS installer alongside the ZIP. Requires NSIS on `PATH`. |
| `AUXE_ENABLE_PORTAUDIO` | `OFF` | Enable PortAudio-dependent built-ins. Off for releases; the UI layer owns audio I/O. |
