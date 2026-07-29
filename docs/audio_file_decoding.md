# Audio file decoding — WAV and MP3 (codec allowlist)

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

## Verification status

There are no regression tests covering MP3 decoding (`grep mp3 test/` is empty) — it has only been
exercised manually through `file()`. Adding coverage requires committing a small MP3 fixture, which
is why it was not done alongside the initial decode support.

Encoding remains WAV-only: `wavwrite` is the only audio writer; there is no MP3 encoder and adding
one would reintroduce a licensing question that decoding does not have.
