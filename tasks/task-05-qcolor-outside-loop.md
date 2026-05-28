# Task 05 — Move QColor construction outside per-line lambda

**Priority:** Low  
**Status:** TODO  
**File:** `qldd.cpp:124`

## Problem

```cpp
// inside the per-dependency-line lambda:
QColor redC("red");
```

`QColor("red")` parses the color name string on every invocation of the lambda (i.e., once per line of `ldd`/`otool` output). While cheap individually, it is unnecessary repeated work.

## Fix

Replace with the pre-defined Qt constant:

```cpp
tmp->setForeground(0, QBrush(Qt::red));
```

This removes the string parsing entirely. The `QBrush(Qt::red)` constructor uses a compile-time constant.

## Acceptance criteria

- `QColor("red")` removed from the lambda body.
- `Qt::red` used directly in the `setForeground` call.
