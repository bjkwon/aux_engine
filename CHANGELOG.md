# Changelog

This file records user-visible changes in each auxe release.

## 2.7.0

2026-11-23

### Nonblocking assignment

- Use `<-` with a plain top-level variable. The outermost call determines when the engine publishes a result. Direct calls to `filt`, `progblocks`, and audio reads with `wave` or `file` publish incrementally. Other expressions, including `r <- sum(filt(x,b))`, use the workspace values captured at launch and publish one final value or error. Reassigning or clearing the destination discards late results from the old job.
- Replace the experimental `<~` spelling with `<-` in progressive scripts. An ordinary assignment such as `snapshot = y` copies the samples available at that moment.

### Progressive audio and block callbacks

- Direct `x <- wave(path)` and `x <- file(path)` calls now publish decoded audio in blocks at the workspace sample rate. `wave()` accepts an optional time range in milliseconds and can decode a downloaded HTTP(S) WAV. `file()` recognizes local WAV and MP3 audio. [Audio file decoding](docs/audio_file_decoding.md#progressive-reads)
- `y <- filt(x,b)` and `y <- x.filt(b)` retain completed samples from real vectors and contiguous mono or aligned stereo audio. Each channel starts with the supplied initial state and carries its own state across blocks. Grouped stereo keeps its row boundaries. Inspect `y.h.block` for the latest block and `y.h.completed` for the sample count. Use `stop(y.h)` to cancel.
- `r <- progblocks(x,window_ms,"callback",...)` passes each window, its start time in milliseconds, the previous result, and a first-window flag to a named UDF. Both channels of a stereo window arrive in one callback. The engine publishes the returned accumulator after each window. A callback can set `out.h.stream = 1` and publish growing mono or stereo audio in `out.h.completed`, with a fixed channel count and one output frame per processed input frame. The input frame count is available in `r.h.samples_completed`.

### Playback and embedding

- Added `streamplay(path_or_url)` for frontends that install the stream-start hook. It returns a playback handle and delegates media reading and decoding to the frontend. AUX retains no sample array for this operation.
- Embedders can read published frames through `aux_progressive_audio`, read the right channel through `aux_progressive_right_samples`, and access nested output through `aux_get_struct_member`. Both channels share the count reported by `ready()`. Call `aux_poll_async` on the evaluator thread to publish results and advance block callbacks. The `aux2` console polls while waiting for input.

## 2.6.1 - 2026-10-01

auxe API revised to handle UDF search roots affected by app's system calls such as `cd`

## 2.6.0 - 2026-09-27

Changes since 2.5.1.

### Added

- Added read-only temporal block selection with one-based brace syntax, such as `x{2}`.
  Selection composes with cell access and later suffixes, including `a{1}{2}` and
  `a{1}{2}(1s~1.4s)`.
- Added the receiver-style `blockat(time)` builtin. It selects the unique temporal block
  containing a timepoint using half-open intervals (`start <= time < end`).
- Temporal block selections preserve the selected block's original start time, sample rate,
  sample type, and data. Stereo values require selecting `.left` or `.right` first.

### Changed

- Logical indexing of audio now preserves discontiguous matches as separate, correctly timed
  chain segments instead of collapsing them onto one continuous timeline.
- A single logical-mask read from a grouped two-dimensional value now returns a flattened
  one-dimensional vector. Logical indexed assignment continues to preserve the grouped shape,
  and explicit two-dimensional row/column indexing is unchanged.

### Tests

- Added regression coverage for segmented audio logical indexing, grouped logical reads and
  writes, ordinal and timepoint temporal block selection, suffix chaining, channel selection,
  interval boundaries, invalid selectors, overlap errors, and read-only enforcement.
