# Task 07 — Eliminate double string build in getInfo

**Priority:** Low  
**Status:** TODO  
**File:** `qldd.cpp:187–198`

## Problem

```cpp
QString buf;
execAndDoOnEveryLine(ss.str(), [&buf](const QString &line) { buf.append(line + "\n"); });
QStringList slTmp = buf.split(INFO_SPLITTER);
buf.clear();
for (const QString &v : slTmp) {
    buf.append(v.trimmed()).append("\n");
}
return buf;
```

The full output is first assembled into `buf`, then split into a `QStringList`, then reassembled. This is two full string allocations and two passes over the data. Since the lambda is called per line, trimming can happen there directly.

## Fix

```cpp
QString buf;
execAndDoOnEveryLine(ss.str(), [&buf](const QString &line) {
    // split by INFO_SPLITTER within each line and trim inline
    const auto parts = line.split(INFO_SPLITTER);
    for (const auto &part : parts) {
        buf.append(part.trimmed()).append("\n");
    }
});
return buf;
```

This eliminates the intermediate `QStringList` and the second pass.

## Acceptance criteria

- `buf.split(INFO_SPLITTER)` removed.
- Single-pass construction of the final string inside the lambda.
- Output is functionally identical (each comma/newline-separated field on its own trimmed line).
