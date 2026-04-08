# Graphics Runtime / Backend Split

## Goal

Define a clean architectural split between:

- `auxe` as the language/runtime layer
- `auxlab2` as the GUI/rendering backend

The key principle is:

- actual rendering belongs entirely to `auxlab2`
- graphics semantics and repaint-worthy state changes may still be owned by `auxe`

## Core Position

`auxe` should not render graphics.

That means `auxe` should not know about:

- Qt
- windows/widgets
- paint events
- device contexts
- pens/brushes/fonts
- pixel-level drawing
- GUI invalidation APIs

Those are frontend/backend concerns and belong in `auxlab2`.

At the same time, `auxe` may still need to own graphics runtime semantics, including changes that imply a redraw is needed.

## Important Distinction

Rendering and repainting are not the same thing.

### Rendering

Rendering is the actual GUI work:

- drawing axes
- drawing lines
- drawing text
- drawing grids/ticks/labels
- laying out windows and plot regions

This is 100% `auxlab2`.

### Repaint-Worthy Change Detection

This is language/runtime logic:

- a graphics handle property changed
- a figure/axes/line/text object was created
- an object was deleted
- `gcf` or `gca` changed
- a named plot's source data changed
- a graphics operation changed visible state

This may reasonably be decided by `auxe`.

## Recommended Split

### `auxe` owns

- graphics builtin registration:
  - `figure`
  - `axes`
  - `plot`
  - `line`
  - `text`
  - `delete`
  - `gcf`
  - `gca`
- graphics handle type semantics
- property access and mutation semantics
- parent/child identity relationships
- current object semantics
- handle validity/invalidation rules
- named-plot semantics
- determining when a state change is repaint-worthy
- emitting backend-facing graphics change notifications

### `auxlab2` owns

- actual figure windows
- Qt object lifetime
- all painting and redraw scheduling
- window focus behavior from GUI interactions
- dirty-region or coalesced repaint strategy
- backend object registries tied to actual windows/widgets
- translating runtime change notifications into UI refreshes

## Recommended Interface Style

The boundary should be event-oriented, not paint-oriented.

That means `auxe` should not say:

- "call Qt update() now"
- "invalidate this rectangle"
- "draw this line"

Instead, `auxe` should communicate semantic events such as:

- object created
- object deleted
- property changed
- current figure changed
- current axes changed
- named-plot data updated

Then `auxlab2` decides how to realize those changes visually.

## Example Backend Notifications

Possible backend hooks could look like:

- `on_graphics_created(handle)`
- `on_graphics_deleted(handle)`
- `on_graphics_property_changed(handle, property_name)`
- `on_current_figure_changed(handle_or_null)`
- `on_current_axes_changed(handle_or_null)`
- `on_named_plot_source_updated(handle)`

These names are only illustrative. The important idea is the semantic level of the interface.

## Why This Split Is Good

### Keeps `auxe` portable

`auxe` remains usable without a GUI toolkit dependency.

### Keeps `auxlab2` in charge of GUI reality

Only `auxlab2` knows how to draw, batch repaints, handle focus, and optimize screen updates.

### Supports multiple frontends

In the future:

- `auxlab2` can install a real graphics backend
- `aux2` can install no backend or a null backend

Then graphics functions can fail gracefully in non-GUI frontends.

### Matches historical direction

The old `auxlab` / `graffy` code already suggested a split between:

- runtime-facing graphics semantics
- platform-specific rendering implementation

This document formalizes that idea for current `aux_engine` and `auxlab2`.

## Repaint Policy

Repaint policy should live in `auxlab2`, not in `auxe`.

That includes:

- repaint immediately vs defer
- coalescing multiple property changes
- dirty-region tracking
- handling invisible or minimized windows

However, `auxe` may still classify certain changes as repaint-worthy.

That means:

- `auxe` decides that a visual state change occurred
- `auxlab2` decides how and when to repaint

## Recommended Future Direction

1. Move graphics builtins and handle semantics into `auxe`
2. Define a graphics backend interface with semantic notifications
3. Let `auxlab2` implement the backend using Qt
4. Let `aux2` provide no backend or a null backend with graceful errors
5. Keep all actual paint logic in `auxlab2`

## Bottom Line

The right balance is:

- rendering: fully `auxlab2`
- graphics semantics: largely `auxe`
- repaint scheduling: `auxlab2`
- repaint-worthy semantic change detection: can be signaled by `auxe`

