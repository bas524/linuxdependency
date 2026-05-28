# Task 09 — Remove QDir::setCurrent global state from fillDependency

**Priority:** Medium  
**Status:** TODO  
**File:** `qldd.cpp:82, 142`

## Problem

```cpp
QDir::setCurrent(getPathOfBinary());   // line 82
// ... run ldd ...
QDir::setCurrent(_lddDirPath);         // line 142
```

`QDir::setCurrent` changes the working directory of the entire process. This is:
- Not thread-safe: if two `QLdd` instances ever run concurrently, the CWD races.
- Fragile: an early return or exception between lines 82 and 142 leaves the process in a wrong directory.
- Unnecessary: `ldd`/`otool` accept absolute paths.

## Fix

Pass the absolute path directly to the command and remove both `QDir::setCurrent` calls:

```cpp
// Already using _fileName which is absolute — no CWD change needed.
ss << CMD_LDD << " \"" << _fileName.toStdString() << "\"";
```

Verify `_fileName` is always stored as an absolute path (it comes from `QFileInfo::absoluteFilePath()` or the dialog, both of which provide absolute paths).

If there is a scenario where a relative path is passed to the constructor, normalize it to absolute in the constructor using `QFileInfo(_fileName).absoluteFilePath()`.

## Acceptance criteria

- Both `QDir::setCurrent` calls removed from `fillDependency`.
- `_fileName` guaranteed absolute (assert or normalize in constructor).
- No process-global CWD mutation.
