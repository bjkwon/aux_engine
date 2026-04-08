# Graphics Migration Roadmap

## Goal

Move the current graphics implementation from an `auxlab2`-heavy command bridge toward a cleaner architecture where:

- `auxe` owns graphics language/runtime semantics
- `auxlab2` owns actual GUI rendering
- `aux2` fails gracefully when graphics is requested without a GUI backend

This roadmap focuses on the items we explicitly want to work on next:

- harden semantics
- true handle/reference behavior in runtime
- deletion/current-handle consistency
- named-plot update semantics
- builtin registration in `auxe`
- backend abstraction
- `auxlab2` install / `aux2` graceful error path

## Current State

Today, much of the user-facing behavior already works in `auxlab2`:

- graphics commands:
  - `figure`
  - `axes`
  - `plot`
  - `line`
  - `text`
  - `delete`
  - `gcf`
  - `gca`
- method-style calls:
  - `h.plot(...)`
  - `h.line(...)`
  - `h.text(...)`
  - `h.axes()`
  - `h.delete()`
- property get/set through the `auxlab2` bridge
- figure/axes/line/text graphics model
- partial named-plot behavior
- partial current-handle semantics

However, the architecture is still transitional:

- graphics commands are mostly intercepted in `auxlab2`
- handle semantics are not yet first-class in `auxe`
- `gcf` and `gca` are not true runtime pseudo-vars
- deletion/current-object rules are only partially enforced at runtime level
- there is no formal backend interface between `auxe` and `auxlab2`
- `aux2` does not yet have a graceful graphics-unavailable path through shared runtime builtins

## Desired End State

### `auxe` owns

- graphics builtin registration
- graphics-handle runtime type semantics
- property access and mutation semantics
- parent/child identity rules
- current-handle semantics:
  - `gcf`
  - `gca`
- deletion/invalidation rules
- named-plot semantics
- repaint-worthy semantic change detection
- backend-facing change notifications

### `auxlab2` owns

- figure windows and widgets
- Qt lifetime management
- all drawing
- repaint scheduling and coalescing
- focus events from the GUI
- translating semantic notifications into UI refreshes

### `aux2` owns

- no graphics backend installation
- graceful runtime errors when graphics functions are used

## Guiding Principles

### 1. Rendering stays out of `auxe`

`auxe` should never know about:

- Qt classes
- paint events
- pixel drawing
- GUI invalidation APIs

### 2. Graphics semantics belong in `auxe`

`auxe` should know:

- what a graphics handle is
- what `figure`, `axes`, `plot`, `line`, `text`, and `delete` mean
- what `gcf` and `gca` mean
- when a semantic graphics change happened

### 3. Handle objects need alias/reference semantics

Graphics handles must behave like references, not copied value structs.

Example:

- `h = plot(x); h2 = h;`
  - `h` and `h2` refer to the same underlying object
- changing through `h2` must be visible through `h`

This is the semantic reason to adopt `TYPEBIT_HANDLE` as the runtime-facing concept.

### 4. No-GUI frontends should degrade gracefully

If graphics is used in `aux2`, it should produce a clear runtime error, not an unregistered-function failure and not a crash.

Example error shape:

- `Graphics backend not available in this frontend.`

## Workstreams

## Workstream 1: Harden Existing Semantics

This work can continue in the current bridge while we prepare the deeper move into `auxe`.

### Goals

- make current behavior match the agreed graphics design more closely
- reduce semantic surprises before migration

### Remaining tasks

- verify true alias behavior for handle variables in all supported command forms
- tighten deletion semantics:
  - deleting a non-current figure must not change `gcf`
  - deleting a non-current axes must not change `gca`
  - deleting the current object clears only that current object
- tighten stale-handle behavior:
  - property access on deleted handles must error
  - method calls on deleted handles must error
- tighten parent/children consistency after delete
- finish named-plot refresh rules:
  - linked data may update
  - user-overridden style/layout should persist
  - still-auto axes properties may recompute
- verify stereo invariants:
  - left-channel axes defines `gca`
  - `F2` changes layout only, not handle identity

### Output of this phase

- a more stable behavioral reference
- fewer semantic changes later when builtins move into `auxe`

## Workstream 2: Introduce Runtime Handle Terminology

This is the terminology and type-cleanup track.

### Goals

- rename `TYPEBIT_STRUTS` to `TYPEBIT_HANDLE`
- make the runtime type meaning explicit

### Scope

- perform the safe type-bit rename described in:
  - [`HANDLE_RENAME_PLAN.md`](/Users/bkwon/dev/aux_engine/HANDLE_RENAME_PLAN.md)
- add helper naming such as:
  - `ISHANDLE(...)`
  - `IsHandle()` compatibility helpers where useful

### Non-goals for first pass

- do not immediately rename `struts`
- do not immediately rename `GOvars`
- do not immediately rename every `IsGO()` path

### Output of this phase

- clearer runtime terminology
- better foundation for graphics-handle work in `auxe`

## Workstream 3: Define Graphics Backend Interface

This is the architectural boundary between `auxe` and GUI backends.

### Goals

- create a backend interface that is semantic, not paint-oriented
- allow `auxe` to call graphics operations without depending on Qt

### Recommended interface responsibilities

#### Backend creation/query operations

- create figure
- focus figure
- create axes
- create line
- create text
- delete object
- query current figure
- query current axes
- query/resolve object validity

#### Backend mutation operations

- set property
- get property
- update named-plot source binding
- notify source-variable updates

#### Backend notification operations

- object created
- object deleted
- property changed
- current figure changed
- current axes changed
- named-plot source updated

### Important rule

The interface should communicate:

- semantic object changes

not:

- draw commands
- Qt update calls
- paint-region requests

### Output of this phase

- a concrete C++ interface in `auxe`
- no Qt references in the interface itself

## Workstream 4: Register Graphics Builtins in `auxe`

This is the core migration from frontend bridge to runtime ownership.

### Goals

- make graphics commands true builtins
- make `gcf` and `gca` true runtime pseudo-vars or equivalent special lookups

### Builtins to register

- `figure`
- `axes`
- `plot`
- `line`
- `text`
- `delete`
- `gcf`
- `gca`

### Required runtime behavior

- parse and dispatch builtin arguments in `auxe`
- resolve method syntax through handle type semantics
- preserve reference aliasing on assignment
- validate handle types at runtime
- return proper handle objects instead of UI-only ids

### Migration rule

During transition:

- `auxlab2` bridge may remain as fallback or bootstrap
- but new logic should stop growing there once `auxe` builtin paths exist

### Output of this phase

- graphics becomes part of the AUX runtime model

## Workstream 5: Install Backend In `auxlab2`

This connects the runtime semantics to the existing Qt implementation.

### Goals

- have `auxlab2` register a real graphics backend with `auxe`
- map runtime handle operations to the existing `GraphicsManager`, `SignalGraphWindow`, and graphics model

### Responsibilities in `auxlab2`

- backend object registry backed by actual windows/models
- focus updates from UI routed back to runtime current-handle state
- semantic-change notifications mapped to repaint/update behavior
- named-plot source refresh tied to existing variable-window/update flows

### Migration preference

Reuse the current `auxlab2` graphics model where practical.

The current model already contains useful pieces:

- figure/axes/line/text objects
- current-window tracking
- named-plot source association
- renderer support for several graphics properties

### Output of this phase

- `auxlab2` becomes the installed GUI backend rather than the primary owner of graphics semantics

## Workstream 6: Add Null / Graceful Backend Path For `aux2`

This makes the runtime behavior clean in console mode.

### Goals

- let graphics builtins exist in `auxe`
- but fail gracefully in non-GUI frontends

### Recommended behavior

- `aux2` installs no backend or installs a null backend
- graphics builtins return a standardized runtime error when invoked

### Example operations that should fail gracefully

- `plot(1:10)`
- `figure()`
- `line(...)`
- `text(...)`
- property mutation on graphics handles

### Output of this phase

- graphics is a real runtime subsystem
- console frontend remains stable and honest about unsupported capabilities

## Proposed Execution Order

1. Harden semantics in the current implementation
2. Perform the safe `TYPEBIT_HANDLE` rename
3. Define the backend interface in `auxe`
4. Register graphics builtins in `auxe`
5. Install real backend implementation in `auxlab2`
6. Add null/graceful backend path in `aux2`
7. Reduce or remove the old `auxlab2` command interception layer

## Why This Order

### Harden semantics first

It gives us a stable behavioral target before we move logic across subsystem boundaries.

### Rename early but safely

It improves clarity without forcing a large architectural rename all at once.

### Interface before builtin migration

It prevents graphics builtins from becoming tangled with Qt or frontend-only assumptions.

### `auxlab2` backend before `aux2` polish

The GUI backend is the real feature path. Once that is solid, the null-backend path is straightforward.

## Open Design Questions

These do not block the roadmap, but they will need decisions during implementation:

- how much graphics object state should live in `auxe` versus the backend
- whether backend objects are fully backend-owned or mirrored by runtime-side handle metadata
- whether `gcf` and `gca` are implemented as pseudo-vars, special builtins, or special lookup hooks
- how named-plot source updates are triggered from variable mutation events
- whether repaint-worthy notifications are object-level or figure-level
- how much of the current `auxlab2` bridge should remain during transition

## Success Criteria

We can consider the migration successful when all of the following are true:

- graphics handles are true runtime reference objects
- graphics builtins are registered in `auxe`
- `gcf` and `gca` are runtime-level concepts
- deleting objects preserves the agreed current-handle semantics
- named plots update according to the agreed rules
- `auxlab2` renders through a backend interface instead of owning graphics semantics directly
- `aux2` reports graceful graphics-unavailable errors

## Related Docs

- [`GRAPHICS_RUNTIME_BACKEND_SPLIT.md`](/Users/bkwon/dev/aux_engine/GRAPHICS_RUNTIME_BACKEND_SPLIT.md)
- [`HANDLE_RENAME_PLAN.md`](/Users/bkwon/dev/aux_engine/HANDLE_RENAME_PLAN.md)
- [`GRAPHICS_HANDLE_IMPLEMENTATION_PLAN.md`](/Users/bkwon/dev/auxlab2/GRAPHICS_HANDLE_IMPLEMENTATION_PLAN.md)
