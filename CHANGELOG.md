# Changelog

This file records user-visible changes in each auxe release.

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
