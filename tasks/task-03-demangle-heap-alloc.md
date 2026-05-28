# Task 03 — Eliminate redundant heap allocation in __cxa_demangle call

**Priority:** Medium  
**Status:** TODO  
**File:** `qldd.cpp:156`

## Problem

```cpp
char *realname = abi::__cxa_demangle(info.at(2).toStdString().c_str(), nullptr, nullptr, &status);
```

`toStdString()` allocates a `std::string` on the heap just to call `.c_str()` on it. This happens for every exported symbol.

## Fix

Replace with `toUtf8().constData()` which returns a pointer into Qt's internal copy-on-write buffer without a heap allocation:

```cpp
char *realname = abi::__cxa_demangle(info.at(2).toUtf8().constData(), nullptr, nullptr, &status);
```

Note: the `QByteArray` returned by `toUtf8()` must stay alive for the duration of the `constData()` pointer use. Since `__cxa_demangle` returns before the temporary is destroyed, this is safe as a function argument.

## Acceptance criteria

- `toStdString()` call removed at `qldd.cpp:156`.
- No intermediate `std::string` heap allocation per symbol.
