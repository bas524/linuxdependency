# Task 02 — Remove debug logging from hot symbol-processing loop

**Priority:** High  
**Status:** DONE (resolved as part of task-01 rewrite)  
**File:** `qldd.cpp:162–164`

## Problem

```cpp
if (demangled.contains("string")) {
    qDebug() << "from->" << _demangleRule.first << " to->" << _demangleRule.second;
}
```

This fires inside the inner demangling loop for every rule applied to every exported symbol that contains `"string"`. For a typical C++ binary this means thousands of `qDebug()` calls in release builds. `qDebug()` formats and writes to stderr unconditionally unless `QT_NO_DEBUG_OUTPUT` is defined.

## Fix

Remove the three lines entirely. They appear to be leftover debugging from development. If retained for development purposes, guard with:

```cpp
#ifdef QT_DEBUG
if (demangled.contains("string")) {
    qDebug() << "from->" << _demangleRule.first << " to->" << _demangleRule.second;
}
#endif
```

## Acceptance criteria

- No `qDebug` output in release builds from this loop.
- Lines removed or wrapped in `#ifdef QT_DEBUG`.
