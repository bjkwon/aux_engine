# Temporal block selection

A mono temporal object may contain multiple separated chain blocks. AUX supports selecting one
block without flattening the object or losing its position on the original timeline.

## Ordinal selection

Use braces with a one-based positive integer:

```aux
x = noise(.5s) + noise(.5s)>>1s + noise(.5s)>>2s
second = x{2}
```

`second` is a detached copy of only the second block. It retains its original start time, so in
this example it is equivalent to `noise(.5s)>>1s`. Subsequent suffixes operate on that copy:

```aux
portion = x{2}(1s~1.25s)
a = {x}
same_portion = a{1}{2}(1s~1.25s)
```

If the receiver is a cell object, braces keep their existing cell-member meaning. Therefore
`a{1}{2}` first selects cell item 1 and then selects block 2 from that item. A cell with a temporal
face also uses braces for its cell members; obtain the temporal value first when block selection is
intended.

## Selection by timepoint

Use the receiver-style `blockat` builtin to select the block containing a timepoint:

```aux
second = x.blockat(1.1s)
t = 1.1s
same = x.blockat(t)
```

Block intervals are half-open: the start time is included and the end time is excluded. A lookup
fails when the timepoint is in a gap, outside the object, or contained by more than one overlapping
block.

Use `begint()` to inspect the selected block's original position on the timeline:

```aux
second_start = x{2}.begint()
same_start = x.blockat(1.1s).begint()
```

The result's value is the block's start time in the normal AUX time unit (milliseconds internally,
so it compares directly with literals such as `1s`). Like other temporal queries, `begint()` retains
the temporal organization of its receiver; selecting one block first produces one result. The
internal implementation field is named `tmark`, but that name is not part of the user-facing syntax.

## Mono-only and read-only behavior

Block identity is intentionally defined only for mono temporal objects. Select a stereo channel
before selecting its block:

```aux
left_second = stereo.left{2}
right_at_t = stereo.right.blockat(1.1s)
```

Calling `{n}` or `.blockat(t)` directly on a stereo object is an error. Temporal block selections
are read-only in this version: they may be copied, inspected, or sliced on the right-hand side, but
cannot be assignment targets. Existing cell assignments such as `a{1}=value` are unchanged.
