# auxe (AUX Engine)

This repo contains **auxe**, the C++17 core engine that parses and executes **AUX** scripts.

* **AUX** (AUdio syntaX) is a DSL for audio processing. It roughly resembles the syntax of MATLAB, there are plenty of language features suited for creating, manipulating and processing audio signals.
* The goal of **AUX** is to motivate users to write scripts based on the conceptual abstraction of audio rather than engineering implementation of audio signals (i.e., digital samples)
* **auxe** = the AUX Engine library

# **auxe**
* Parses and evaluates AUX expressions
* Manages variables, scopes, segments, and time-shifted signals
* Operates on abstract data: audio signals with metadata (sample rate, channels, timing)


## Architecture Overview

### What auxe does
- Parses and evaluates AUX expressions
- Manages variables, scopes, segments, and time-shifted signals
- Operates on abstract data:
  - numeric arrays
  - audio signals with metadata (sample rate, channels, timing)
- Provides a stable C/C++ API for embedding in UIs

### What auxe does *not* do
- No general-purpose file I/O beyond the small, fixed allowlist below (no AIFF, FLAC, AAC, Opus, etc.)
- No system/external codec libraries (no libsndfile, mp3lame, libmpg123) and no patent-encumbered or royalty-bearing codecs
- No UI logic (no readline, no console state)

### Codec policy
auxe vendors small, permissively-licensed (public domain/MIT-0), dependency-free decoders in-tree for a **fixed, deliberately small allowlist** of common, patent-free formats:
- **WAV** - hand-rolled PCM/IEEE-float parser (`src/func/_file_wav.*`)
- **MP3** - vendored `dr_mp3` (`src/third_party/dr_mp3.h`, public domain/MIT-0). MP3's core patents expired in 2017, so there is no royalty/licensing risk in decoding it.

This list is intentionally not meant to grow. Any other format (AIFF, FLAC, AAC, Opus, ...) remains the responsibility of the **UI layer**, which decodes it externally and injects raw PCM + metadata into auxe using engine APIs. Vendored in-tree decoders must stay permissively licensed (public domain, MIT, BSD, or similar) - no GPL/LGPL system codec libraries are linked, keeping auxe's build free of external codec dependencies regardless of auxe's own license.

---

## Data Flow Model

UI applications interact with auxe using **data blocks + metadata**, not filenames.

Typical flow:

1. For WAV/MP3, the UI can just call `wave()`/`file()` and auxe decodes the file in-engine. For any other format, the UI decodes the file using any library it chooses.
2. UI injects the decoded data into auxe (directly, or already decoded via `wave()`/`file()`):
   - audio buffers (de-interleaved PCM)
   - sample rate, channel count, timing metadata
3. AUX scripts operate on the data using engine semantics
4. UI retrieves results from auxe and decides how to present or save them

This keeps auxe independent of storage formats outside its small codec allowlist, and preserves the meaning of AUX abstractions (segments, time shifts, chained signals).

---

## Repository Layout

- `include/auxe/`
  Public headers (engine API)

- `src/engine/`
  Core runtime, parser, evaluator, scope management

- `src/api/`
  Public API implementation (`aux_init`, `aux_eval`, data injection/extraction)

- `src/func/`
  Built-in math, DSP, and utility functions

- `docs/external_modules.md`
  External native module syntax, registry layout, manifest format, C ABI, value conversion, and static/dot-call rules

- `example_apps/aux/`
  Console app (this is included for illustration purposes. Ideally this should be in a separate repo.

---

## Build Dependencies

### Required
- **C++17**
- **FFTW3** (FFT and spectral operations)
- **libsamplerate** (resampling)

### Optional
- **PortAudio** (real-time audio device I/O)
  - Disabled by default
  - Enables device playback/recording, not file I/O

---

## Build (Linux)

Install dependencies (Debian/Ubuntu names shown):

```bash
sudo apt install libfftw3-dev libsamplerate0-dev nlohmann-json3-dev
```

`nlohmann-json` is required in CMake **config mode**
(`find_package(nlohmann_json CONFIG REQUIRED)`), so a bare header drop-in is not
enough — install the distro `-dev` package or the vcpkg port.

Build:
(WSL)
```bash
cmake -S . -B build-wsl \
  -DCMAKE_BUILD_TYPE=Release \
  -DAUXE_BUILD_SHARED=ON \
  -DCMAKE_INSTALL_PREFIX=$PWD/install-wsl

cmake --build build-wsl -j
cmake --install build-wsl
```

Package (`TGZ`):

```bash
cmake --build build-wsl --target package
```

---

## Build (Windows)

Recommended: **vcpkg** for third-party libraries.

Example (PowerShell):

```powershell
git clone https://github.com/microsoft/vcpkg C:\dev\vcpkg
C:\dev\vcpkg\bootstrap-vcpkg.bat
$env:VCPKG_ROOT = "C:\dev\vcpkg"
& "$env:VCPKG_ROOT\vcpkg.exe" install fftw3:x64-windows libsamplerate:x64-windows nlohmann-json:x64-windows
```

Configure and build:

```powershell
$TYPE="Release"  # or $TYPE="Debug"
cmake -S . -B build -A x64 `
  -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake" `
  -DCMAKE_CONFIGURATION_TYPES="Debug;Release" `
  -DAUXE_BUILD_SHARED=ON

cmake --build build --config $TYPE
cmake --install build --config $TYPE --prefix .\install
```

For a redistributable SDK archive — including the runtime DLL bundling, test
gate, artifact verification, signing, and the ABI constraints that come with
this header — follow [`RELEASE_WINDOWS.md`](RELEASE_WINDOWS.md).

---

## Using auxe from a UI Application

A UI / front-end application should:

- depend on auxe as:
  - an installed package (`find_package(auxe CONFIG REQUIRED)` +
    `target_link_libraries(app PRIVATE auxe::auxe)`), or
  - a sibling source tree pulled in with `add_subdirectory` (what `auxlab2` does)
- include the public header:
  ```cpp
  #include <auxe/auxe.h>
  ```
- inject data explicitly (arrays, audio buffers, metadata)
- retrieve results and handle presentation / persistence

auxe does not care *where* data came from or *how* it will be saved.

---
## Example: aux2 (console-based app using auxe)


## Build Dependencies

### Required
- **Readline**
- **PortAudio** (real-time audio device I/O)
  - Device playback/recording

Configure and build:

Install dependencies

(Debian/Ubuntu/WSL):
```bash
sudo apt install libreadline-dev portaudio19-dev
TYPE=Debug # or Release
cmake -S . -B build-wsl \
  -DCMAKE_BUILD_TYPE=$TYPE \
  -DCMAKE_PREFIX_PATH=../../install-wsl
cmake --build build --config $TYPE
cmake --install build --config $TYPE --prefix ./install

```
MacOS
```bash
brew install portaudio readline
```
Windows
```powershell
# aux2 supports audio play with portaudio.
vcpkg install portaudio

cmake -S . -B build -A x64 `
  -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT\scripts\buildsystems\vcpkg.cmake" `
  -DAUX2_USE_INSTALLED_AUXE=ON `
  -DCMAKE_CONFIGURATION_TYPES="Debug;Release" `
  -DCMAKE_PREFIX_PATH=..\..\install"

cmake --build build --config $TYPE
cmake --install build --config $TYPE --prefix .\install
```

## Status

auxe is under active development as a reusable engine.
The public API is evolving as data-injection and extraction interfaces are formalized.
