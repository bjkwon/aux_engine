# AUX syntax conventions: zero-arg calls and assignable selectors

## Zero-argument calls

Starting in AUX Engine 2.5.0, zero-argument function and class-constructor calls must use
explicit empty parentheses:

```text
getfs()
toc()
myudf()
myclass()
```

Bare identifiers are variable lookups, not function calls:

```text
u = myclass()  // instantiate a class object
v = myclass    // read an existing variable named myclass
```

Non-empty parentheses still go through the normal argument checker, so `getfs(1)` remains an error.

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
