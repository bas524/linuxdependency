# Task 06 — Cache dynamic_cast result in demanglerules::insertNewRow

**Priority:** Low  
**Status:** DONE  
**File:** `demanglerules.cpp:42`

## Problem

```cpp
void demanglerules::insertNewRow(int row, const QString &src, const QString &dst) {
    ui->tableWidget->insertRow(row);
    auto *m = dynamic_cast<MainWindow *>(parent());  // repeated on every row
    ...
}
```

`insertNewRow` is called in a loop during construction (once per rule). `dynamic_cast` walks the vtable/RTTI chain on each call. The result is always the same object.

## Fix

Option A — cache in the constructor and pass as a parameter:

```cpp
void demanglerules::insertNewRow(int row, const QString &src, const QString &dst, MainWindow *m) { ... }
```

Option B — store `MainWindow *m` as a member in the constructor and use it in `insertNewRow` without recasting.

The constructor already does the cast once (`demanglerules.cpp:16`), so reuse that result.

## Acceptance criteria

- `dynamic_cast<MainWindow*>(parent())` not called inside `insertNewRow`.
- Cached pointer used instead.
