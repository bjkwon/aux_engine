# AUX syntax conventions: zero-arg calls and assignable selectors

## Zero-argument calls

AUX keeps its historical shorthand for zero-argument functions:

```text
getfs
toc
myudf
```

For compatibility with users coming from other languages, explicit empty parentheses are also
accepted for zero-argument builtins and UDFs:

```text
getfs()
toc()
myudf()
```

The two forms are equivalent only when the function has no arguments. Non-empty parentheses still
go through the normal argument checker, so `getfs(1)` remains an error.

## Assignable selectors

A function call result is not an lvalue. AUX only allows assignment through a narrow class of
declared selector suffixes that preserve a writable path into an existing object.

Currently declared assignable selectors:

| Selector | LHS meaning |
|----------|-------------|
| `.left(index)` | Write into the selected region of the primary channel of a stereo audio variable. |
| `.right(index)` | Write into the selected region of the secondary channel of a stereo audio variable. |

Valid examples:

```text
x.left(1:5) = 0.5
x.right(0~10s) = y
```

Invalid examples:

```text
getfs() = 1
x.left.rms = 1
(x.left + x.right) = y
```

The rule is direct-chain only: a root variable followed by declared assignable selectors may be an
assignment target. Ordinary function calls, binary operators, and computed suffixes produce values,
not writable locations.
