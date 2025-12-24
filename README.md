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
- No file I/O (no MP3/WAV/AIFF handling)
- No codec libraries (no libsndfile, mp3lame, mpg123)
- No UI logic (no readline, no console state)

File decoding/encoding and persistence are the responsibility of the **UI layer**, which injects data into auxe using engine APIs.

---

## Data Flow Model

UI applications interact with auxe using **data blocks + metadata**, not filenames.

Typical flow:

1. UI decodes a file (e.g., MP3/WAV) using any library it chooses
2. UI injects the decoded data into auxe:
   - audio buffers (de-interleaved PCM)
   - sample rate, channel count, timing metadata
3. AUX scripts operate on the data using engine semantics
4. UI retrieves results from auxe and decides how to present or save them

This keeps auxe independent of storage formats and preserves the meaning of AUX abstractions (segments, time shifts, chained signals).

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
sudo apt install libfftw3-dev libsamplerate0-dev
```

Build:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DAUXE_BUILD_SHARED=ON

cmake --build build -j
cmake --install build --prefix ./install
```

---

## Build (Windows)

Recommended: **vcpkg** for third-party libraries.

Example (PowerShell):

```powershell
git clone https://github.com/microsoft/vcpkg
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg install fftw3 libsamplerate
```

Configure and build:

```powershell
cmake -S . -B build -A x64 `
  -DCMAKE_TOOLCHAIN_FILE={repo_cloned_dir}\scripts\buildsystems\vcpkg.cmake `
  -DAUXE_BUILD_SHARED=ON

cmake --build build --config Release
cmake --install build --config Release --prefix .\install
```

---

## Using auxe from a UI Application

A UI / front-end application should:

- depend on auxe as:
  - an installed package, or
  - a git submodule
- include the public header:
  ```cpp
  #include <auxe/aux2_core.h>
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

(Debian/Ubuntu names shown):
```bash
sudo apt install libreadline-dev portaudio19-dev
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
  -DCMAKE_TOOLCHAIN_FILE={aux_engine_dir}\scripts\buildsystems\vcpkg.cmake `
  -DAUXE_BUILD_SHARED=ON `
  -DCMAKE_PREFIX_PATH="{aux_engine_dir}\install"

cmake --build build --config Release
cmake --install build --config Release --prefix .\install
```

## Status

auxe is under active development as a reusable engine.
The public API is evolving as data-injection and extraction interfaces are formalized.

