# Task 04 — Batch widget updates in fillDependency and fillExportTable

**Priority:** High  
**Status:** DONE  
**Files:** `qldd.cpp:77–143`, `qldd.cpp:145–177`

## Problem

`fillDependency` calls `treeWidget.addTopLevelItem()` for every dependency line, and `fillExportTable` calls `listWidget.addItem()` for every exported symbol. Each call triggers an incremental layout recalculation and repaint. For binaries with many dependencies or hundreds of exports the UI visibly redraws item-by-item and the calls collectively dominate render time.

## Fix

Wrap the population loops with `setUpdatesEnabled`:

```cpp
// fillDependency
treeWidget.setUpdatesEnabled(false);
// ... all addTopLevelItem / child item calls ...
treeWidget.setUpdatesEnabled(true);
```

```cpp
// fillExportTable
listWidget.setUpdatesEnabled(false);
// ... all addItem calls ...
listWidget.setUpdatesEnabled(true);
```

Ensure `setUpdatesEnabled(true)` is called even if an exception or early return occurs (use RAII guard or explicit cleanup paths).

## Acceptance criteria

- `setUpdatesEnabled(false/true)` wraps population in both methods.
- Filling a large binary's exports/deps appears instant rather than incrementally painted.
