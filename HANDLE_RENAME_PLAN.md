# Handle Rename Plan

## Goal

Rename `TYPEBIT_STRUTS` to a clearer name that reflects its real meaning:

- `TYPEBIT_STRUT` = value-like struct object
- `TYPEBIT_HANDLE` = reference-like object with identity and alias semantics

This plan focuses on terminology cleanup without breaking existing runtime behavior.

## Recommended Direction

Adopt:

- `TYPEBIT_STRUTS` -> `TYPEBIT_HANDLE`

And add:

- `ISHANDLE(x)` helper

Keep the deeper legacy names such as `struts`, `GOvars`, and `IsGO()` unchanged in the first pass.

## Why This Rename

`TYPEBIT_STRUTS` does not communicate the real distinction from `TYPEBIT_STRUT`.

The actual semantic difference is:

- `TYPEBIT_STRUT` stores value members and follows copy semantics
- `TYPEBIT_HANDLE` stores reference-bearing object identity and follows alias semantics

Example:

- `a.height = 170; b = a;`
  - `b` becomes a copy of `a`
- `h = plot(values); h2 = h;`
  - `h` and `h2` refer to the same underlying graphics object

## Current Usage Summary

`TYPEBIT_STRUTS` is actively used in `aux_engine`. It is not dead code.

Key locations:

- [`src/engine/aux_classes.h`](/Users/bkwon/dev/aux_engine/src/engine/aux_classes.h)
- [`src/engine/xcope.cpp`](/Users/bkwon/dev/aux_engine/src/engine/xcope.cpp)
- [`src/engine/csignals.cpp`](/Users/bkwon/dev/aux_engine/src/engine/csignals.cpp)
- [`src/engine/typecheck.cpp`](/Users/bkwon/dev/aux_engine/src/engine/typecheck.cpp)
- [`src/engine/operators.cpp`](/Users/bkwon/dev/aux_engine/src/engine/operators.cpp)
- [`src/engine/AuxScope.h`](/Users/bkwon/dev/aux_engine/src/engine/AuxScope.h)
- [`src/api/interface.cpp`](/Users/bkwon/dev/aux_engine/src/api/interface.cpp)
- [`src/func/cell_func.cpp`](/Users/bkwon/dev/aux_engine/src/func/cell_func.cpp)
- [`src/func/objchecker.cpp`](/Users/bkwon/dev/aux_engine/src/func/objchecker.cpp)
- [`/Users/bkwon/dev/auxlab2/src/AuxEngineFacade.cpp`](/Users/bkwon/dev/auxlab2/src/AuxEngineFacade.cpp)

## Phase 1: Safe Rename

This phase is intended to be low-risk and mostly mechanical.

### Changes

- Rename macro/constant:
  - `TYPEBIT_STRUTS` -> `TYPEBIT_HANDLE`
- Add macro/helper:
  - `ISHANDLE(x)` for direct handle-type checks
- Update comments and docs that describe `TYPEBIT_STRUTS`
- Update auxlab2 facade mirror constants:
  - `kTypeStruts` -> `kTypeHandle`

### Keep Unchanged In Phase 1

- `map<string, vector<CVar *>> struts`
- `GOvars`
- `CVar::IsGO()`
- `ISSTRUT(...)`
- `AuxScope::IsSTRUT(...)`

### Why Keep Them

These names are tied to broader runtime semantics and are used in many code paths beyond a single type bit rename.

Renaming them immediately would make the change much more invasive and harder to verify.

## Phase 2: Compatibility Helpers

After Phase 1 compiles cleanly, consider adding compatibility-oriented naming helpers.

### Candidate Additions

- `bool CVar::IsHandle() const`
- `static bool AuxScope::IsHANDLE(uint16_t tp)`
- `ISHANDLE(x)` macro next to `ISSTRUT(x)`

### Temporary Dual Naming

During transition, it may be helpful to preserve old helpers while introducing new ones:

- old names continue working
- new code prefers handle terminology

## Phase 3: Architectural Rename

This phase is more invasive and should only happen after the graphics subsystem direction is settled.

### Candidate Renames

- `struts` -> `handles` or `refs`
- `GOvars` -> `HandleVars`
- `IsGO()` -> `IsHandle()`
- `GetGOVariable(...)` -> `GetHandleVariable(...)`

### Why This Is Riskier

These names appear in:

- lookup paths
- assignment semantics
- deletion logic
- GO container semantics
- include/eval propagation across scopes

So this phase is not just terminology cleanup. It changes how core runtime concepts are described everywhere.

## Recommended Execution Order

1. Rename `TYPEBIT_STRUTS` to `TYPEBIT_HANDLE`
2. Add `ISHANDLE(...)` and similar helpers
3. Update local comments/docs in `aux_engine` and `auxlab2`
4. Verify no behavior change
5. Only later decide whether `struts`/`GOvars`/`IsGO()` should also be renamed

## Expected Disruption

### Low

- type-bit constant rename
- local comment updates
- auxlab2 mirror constant rename

### Medium

- helper/macros added alongside existing names

### Higher

- renaming `struts`
- renaming `GOvars`
- renaming `IsGO()`
- renaming runtime APIs around GO handling

## Suggested First Implementation Scope

If we actually do the rename, the first patch should likely touch only:

- [`src/engine/aux_classes.h`](/Users/bkwon/dev/aux_engine/src/engine/aux_classes.h)
- [`src/engine/typecheck.cpp`](/Users/bkwon/dev/aux_engine/src/engine/typecheck.cpp)
- [`src/engine/AuxScope.h`](/Users/bkwon/dev/aux_engine/src/engine/AuxScope.h)
- [`src/func/cell_func.cpp`](/Users/bkwon/dev/aux_engine/src/func/cell_func.cpp)
- [`src/func/objchecker.cpp`](/Users/bkwon/dev/aux_engine/src/func/objchecker.cpp)
- [`/Users/bkwon/dev/auxlab2/src/AuxEngineFacade.cpp`](/Users/bkwon/dev/auxlab2/src/AuxEngineFacade.cpp)

This keeps the first rename patch small and easy to review.
