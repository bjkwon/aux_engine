# Audio file decoding and WAV writing — codec allowlist

## Purpose

`auxe` decodes audio files in-engine for a **fixed, deliberately small allowlist** of formats, so a
frontend does not need to link a codec library just to load common audio. Everything outside that
allowlist stays the UI layer's job: the UI decodes externally and injects raw PCM + metadata through
the engine API.

Currently allowlisted:

| Format | Decoder | Location | License |
|--------|---------|----------|---------|
| WAV (PCM / IEEE float) | hand-rolled parser | `src/func/_file_wav.*` | in-tree, ours |
| MP3 | vendored `dr_mp3` single header | `src/third_party/dr_mp3.h`, wrapped by `src/func/_file_mp3.*` | public domain / MIT-0 |

No system or external codec library (`libsndfile`, `mp3lame`, `libmpg123`) is linked, and no
patent-encumbered or royalty-bearing codec is used — MP3's core patents expired in 2017.

**This list is intentionally not meant to grow.** AIFF, FLAC, AAC, Opus, etc. are still rejected by
the engine. If a new in-tree decoder is ever added, it must be permissively licensed (public domain,
MIT, BSD, or similar) and dependency-free, so that `auxe`'s build stays codec-dependency-free
regardless of `auxe`'s own license.

---

## How a file reaches a decoder

`file(filename)` sniffs the first 16 bytes (`filetype()` in `src/func/_file.cpp`) and dispatches:

| Sniff result | Header signature | Dispatch |
|--------------|------------------|----------|
| 1 — WAV | `RIFF` + `WAVE` | `_wave()` |
| 2 — MP3 | `ID3`, or a frame sync byte pair (`0xFF`, `0xE0`–`0xFF`) | `_mp3()` |
| 3 — AIFF | `FORM` | runtime error: *aiff files are not supported currently in auxe* |
| 4 — text/unknown | anything else | read as text content |
| 0 — error | — | runtime error: file not found / cannot be opened |

`wave(filename [, start_time, end_time])` bypasses sniffing and always runs the WAV parser.

**`file()` is the only entry point to MP3 decoding, by design.** There is deliberately no standalone
`mp3()` builtin and `_mp3()` is intentionally not registered in `src/engine/AuxFunc.cpp`; `file()`
supplies fixed arguments `(0, -1)`, i.e. decode the whole file from the start. Treat this as the
intended surface, not an oversight — do not "complete" the API by registering an `mp3()` builtin.
Format-specific loading stays WAV-only (`wave()`) because MP3 is reached by content sniffing rather
than by the caller naming the codec.

Both paths behave identically after decoding:

- Mono input produces a single-channel `CSignals`; stereo input allocates `Sig.next` as a plain
  `CSignals` and de-interleaves into the two channels (see `docs/channel_indexing.md` — never
  reinterpret-cast `next` to `CVar*`).
- `bufType` is set to `'R'`.
- If the file's sample rate differs from the current environment `fs`, the signal is resampled to
  the environment rate via `__resample`, and a resampler error is raised as a runtime error.
- Remote `http(s)` sources are downloaded to a `ScopedTempFile` first, then decoded from disk.

---

## `mp3_read_float32` contract

```cpp
uint64_t mp3_read_float32(const std::string& fname, double beginMs, double durMs,
        Mp3Info& info, std::vector<float>& out, std::string& estr);
```

- Returns the number of PCM **frames** decoded; appends interleaved float32 samples to `out`
  (`frames * info.num_channels` values).
- `durMs < 0` means "read to end of file".
- On failure it returns 0 **and** sets `estr` to a non-empty message. Callers must check `estr`,
  not just the return value — a legitimately empty read also returns 0.
- Fills `info.sample_rate` / `info.num_channels` from the stream header before reading.
- Decodes in 4096-frame chunks; a short read is treated as EOF.

### The second time argument does not match `wave()`

`_wave()` interprets its second time argument as an **end time** (`frames2read = end/1000*fs - id1`),
while `mp3_read_float32` interprets `durMs` as a **duration** (`frames2read = dur/1000*fs`).

Nothing exercises this today: `file()` is the only MP3 caller and it always passes `(0, -1)`, so the
`beginMs`/`durMs` parameters only ever take their defaults, and `wave()` never reaches the MP3 path.
Since MP3 access through `file()` is the intended design, the divergence is harmless and left as-is —
the parameters exist for internal reuse, not as a user-facing signature. It only becomes a real bug
if someone plumbs caller-supplied times into `_mp3()`, so match the semantics to `_wave()` at that
point rather than assuming they already agree.

---

## Writing: `wavwrite(x, filename [, option])`

Encoding is WAV-only: `wavwrite` is the only audio writer; there is no MP3 encoder and adding one
would reintroduce a licensing question that decoding does not have. `write(x, "f.wav", option)`
forwards to `wavwrite` and accepts the same option string.

The optional third argument is a whitespace/comma-separated token list, so future options can be
added without another positional argument. The only tokens today select the sample format:

| token | written as |
| --- | --- |
| (omitted) | 16-bit signed LE — the historical default |
| `8`, `int8`, `uint8` | 8-bit unsigned (what the WAV spec requires at that depth) |
| `16`, `int16` | 16-bit signed LE |
| `24`, `int24` | 24-bit signed LE, three bytes packed |
| `32`, `int32` | 32-bit signed LE |
| `float`, `float32` | IEEE float32 (`audio_format` 3) |

Tokens are case-insensitive. An unrecognized token, or two format tokens in one string, is an error
rather than a silent fallback. The scale factors match the divisors in `read_pcm_int()`, so a file
read at a given depth writes back to the same bytes.

Integer formats **clamp** samples outside [-1, +1] (and map NaN to silence) instead of wrapping
around, which the old `(int16_t)(x * 32768)` cast did — a sample at +1.0 used to come back as full
negative scale. float32 stores out-of-range values as they are. Normalizing is deliberately not an
option token: `wavwrite(x / max(abs(x)), f, "24")` says the same thing visibly in the script.

### Header layout

`make_wav_header()` returns the header size rather than assuming 44 bytes, because non-PCM output
is not a canonical 44-byte header:

| format | fmt chunk | extra | header size |
| --- | --- | --- | --- |
| PCM (1) — 8/16/24/32-bit int | 16 bytes | — | 44 |
| IEEE float (3) — float32 | 18 bytes (`cbSize` = 0) | `fact` chunk, 4 bytes | 58 |

The spec calls for `WAVEFORMATEX` (a `cbSize` field) and a `fact` chunk holding the per-channel
sample count on any non-PCM format, so a reader can get the frame count without dividing the data
size by a block align that may not apply. `wav_read_header()` skips chunks it does not recognize, so
these files round-trip through `file()`; macOS CoreAudio (`afinfo`) reads them as `Float32`.

`WAVE_FORMAT_EXTENSIBLE` (0xFFFE) is still never written. It is only *recommended* above 16 bits,
and plain format-1 24-bit is read correctly everywhere in practice. The one case where it would buy
something is 32-bit int: some readers infer float from the 32-bit width instead of honoring the
format code. The reader side already parses extensible ([`parse_fmt_chunk`](../src/func/_file_wav.cpp)),
so adding it for that case later would be additive.

Resampling is likewise not an option token. `wave()` resamples on read to reach the environment rate
(see above), but on write the environment rate is what gets stamped into the header; use
`resample()` explicitly to change it.

---

## Verification status

There are no regression tests covering MP3 decoding (`grep mp3 test/` is empty) — it has only been
exercised manually through `file()`. Adding coverage requires committing a small MP3 fixture, which
is why it was not done alongside the initial decode support.

WAV decoding and encoding are covered by `test/regression_wav_header.cpp`. On the write side each
format round-trips through `wavwrite` → `file()` and is checked against the expected header fields
(format code, bit depth, block align, fmt/fact/data chunk layout, data and RIFF sizes), along with
clamping, case-insensitive tokens, and rejection of unknown or conflicting tokens. Output was also
checked against macOS CoreAudio (`afinfo`), which is an independent parser; there is no automated
cross-reader check in CI.
