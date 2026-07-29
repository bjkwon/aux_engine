# `.left` / `.right` channel selectors on stereo audio

## Purpose

Lets a statement read or write a single channel of a stereo audio variable with the same
indexing forms already supported for whole-object access (numeric range, time range).
Before this, `x(range) = RHS` on a stereo `x` either applied to both channels (time range) or
only ever touched the primary channel regardless of intent (numeric range) — there was no way
to target just one channel from the LHS.

The policy is intentionally narrow: `.left` and `.right` are declared channel selectors. On the
RHS they produce a mono channel value/view that can be indexed by the next suffix; on the LHS
they preserve assignability only when followed directly by an index or time range.

**Implementation:** `src/engine/xcope.cpp` (`AuxScope::read_node`) and
`src/engine/xcope_process.cpp` (`AuxScope::get_available_struct_item`, `AuxScope::eval_lhs`,
`AuxScope::mod_sig`, `AuxScope::insertreplace`, `AuxScope::adjust_buf`).

---

## Syntax

```text
x.left(t1~t2)
x.right(id1:id2)
x.left(t1~t2) = RHS
x.right(id1:id2) = RHS
```

| Operand | Role |
|---------|------|
| `x` | An existing stereo audio variable (must already have two channels). |
| `.left` / `.right` | Selects which channel of `x` the next index applies to. |
| `(t1~t2)` / `(id1:id2)` | Same time-range or numeric-range index forms already valid for whole-object indexing. |
| `RHS` | For a time-range index, must be audio (spliced in, like whole-object time-range assignment). For a numeric-range index, may be a scalar fill or a same-length numeric/audio value, same as whole-object numeric indexing. |

An index is required: bare `x.left = RHS` (no parentheses) is rejected rather than silently
misbehaving — use `x.left(:) = RHS` if a whole-channel replace is ever added.

`.left`/`.right` are recognized only when the name isn't already a real struct member of `x` and
`x` is audio-typed; this does not change plain struct field access (`x.someField = ...`).

---

## Semantics

- `.left` writes only into `x`'s primary channel; `.right` writes only into `x`'s second
  (`next`) channel. The other channel's data is left completely untouched — this does not go
  through the read-side `left()`/`right()` builtins (which mutate a computed copy), it writes
  directly into `x`'s own stored channel data.
- On the RHS, `x.left(range)` is interpreted compositionally as `(x.left)(range)`: first select
  the mono left channel, then apply the numeric/time index to that result. This makes it symmetric
  with the older `x(range).left` form.
- Requires `x` to already be stereo (mirrors the existing `IsSTEREOG` qualifier already required
  by the read-side `left()`/`right()` builtins) — a mono `x` raises an error rather than being
  silently treated as its own left/right channel.
- The LHS/RHS type-compatibility check treats a channel-scoped write as inherently mono: it masks
  out the stereo bit before comparing, since `x` itself is still genuinely stereo even though only
  one channel is being written.
- Binary operators and computed/read-only suffixes break assignability. For example,
  `x.left.rms = RHS` and `(x.left + x.right) = RHS` are invalid assignment targets even though
  `x.left(...) = RHS` is valid.

---

## Errors

- `.left`/`.right` used without an index (e.g. `x.left = RHS`): *"`.left` must be used with an
  index on the LHS, e.g. `x.left(t1~t2) = ...`"*.
- `.left`/`.right` followed by a computed suffix on the LHS (e.g. `x.left.rms = RHS`): rejected
  because the next suffix is not an assignable index.
- `.left`/`.right` on a mono `x`: *"`.left` requires a stereo signal."*
- Combined with the replicator (`x.left(t1~t2) ++= ...`): rejected as not yet supported, rather
  than risking a channel-crossed write.

---

## Relationship to other builtins

| Form | Read | Write (assignment target) |
|------|------|----------------------------|
| `left(x)` / `x.left` / `x.left()` | Returns a mono copy of the left channel. | Bare `x.left = ...` remains rejected. |
| `x.left(range)` | Reads the indexed region of `x`'s primary channel. | Writes into `x`'s primary channel in place, leaving `x`'s other channel untouched. |
| `x.left.rms` | Computes the RMS of the left channel. | Not an lvalue. |

---

## Example

```text
x = [silence(1000); silence(1000)]
x.left(0~.01) = noise(10)     % first 10ms of the left channel only
x.right(1:5) = 0.5            % first 5 samples of the right channel only
b = x.left(0~.01)             % read the first 10ms of the left channel
```
