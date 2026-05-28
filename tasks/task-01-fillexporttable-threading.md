# Task 01 — Fix thread-safety bug in fillExportTable

**Priority:** Critical  
**Status:** TODO  
**File:** `qldd.cpp:145–177`

## Problem

`fillExportTable` uses `Exec::ASYNC` which calls `std::async(std::launch::async, ...)` for every line of `nm` output. This can spawn hundreds or thousands of threads for a large binary. More critically, `QListWidget::addItem` is called from worker threads — Qt widgets must only be touched from the main thread. The mutex around `addItem` does not make it safe; this is undefined behavior and a latent crash.

## Fix

1. Switch the lambda to collect results into a `QVector<QString>` (protected by the mutex, or using a lock-free append into a pre-allocated vector).
2. After all async work completes (futures resolved), post the collected items back to the main thread and populate the widget there — or use `QMetaObject::invokeMethod(..., Qt::QueuedConnection)` from inside the lambda to queue each `addItem` call to the main thread.
3. Alternatively, drop `Exec::ASYNC` here entirely and run symbol parsing on a `QThread` / `QtConcurrent::run`, emitting a signal when done.

## Acceptance criteria

- No Qt widget calls from worker threads.
- UI remains responsive during `nm` parsing for large binaries.
- `nm` output is still processed in parallel (demangling + regex replacements are CPU-bound and safe to parallelize).
