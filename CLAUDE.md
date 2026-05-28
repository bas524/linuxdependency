# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project overview

**Qldd** (DependencyViewer) is a Qt-based GUI application that shows all dependent libraries of a given executable or shared library on GNU/Linux and macOS. It wraps `ldd` (Linux) / `otool` (macOS), `file`, and `nm` shell commands and presents results in a tree/list view. There are no automated tests — the app is verified manually.

## Build

```bash
# Configure (from repo root)
cmake -DCMAKE_BUILD_TYPE=Release -Bcmake-build-release -H.
# Or for a debug build:
cmake -DCMAKE_BUILD_TYPE=Debug -Bcmake-build-debug -H.

# Build
cmake --build cmake-build-release/ --target all

# macOS: Qt5 must be installed via Homebrew and its prefix passed explicitly
cmake -DCMAKE_PREFIX_PATH=$(brew --prefix)/opt/qt@5 -DCMAKE_BUILD_TYPE=Release -Bcmake-build-release -H.
```

CMake auto-detects Qt5 first; falls back to Qt6. To force Qt6:
```bash
cmake -DFORCE_QT6=ON ...
```

### Packaging

```bash
cd cmake-build-release
cpack -G DEB     # Ubuntu/Debian → .deb
cpack -G RPM     # Fedora/CentOS → .rpm
cpack -G DragNDrop  # macOS → .dmg
```

## Architecture

The app is a single-window Qt application with no separate library targets.

| File | Role |
|---|---|
| `main.cpp` | Entry point — parses optional filename arg, constructs `MainWindow` |
| `mainwindow.{cpp,h,ui}` | Top-level Qt window; owns a `QLdd` instance, manages menus, file open dialog, export-filter UI, demangling rules, and permission checkboxes |
| `qldd.{cpp,h}` | Core logic — runs `ldd`/`otool`, `file`, `nm` via `popen`; parses output into tree nodes for `QTreeWidget` and list entries for `QListWidget`; reads file metadata (size, timestamps, owner/group, permissions) |
| `demanglerules.{cpp,h,ui}` | Dialog + data model for configuring regex-based symbol demangling rules; rules persist to/from a JSON file |
| `finfdialog.{cpp,h,ui}` | "File info" dialog that displays metadata gathered by `QLdd` |
| `customtypes.h` | Shared typedefs (`RulesMap`) used across the codebase |
| `resources/rules.json` | Default demangling rules shipped with the app |
| `config.h.in` / `version.h.in` | CMake-configured headers providing `CONFIG_PATH` and build version info |
| `cmake/modules/` | `FindVersionHeader.cmake` — extracts git SHA/date into `version.h`; `FindConfigPath.cmake` — resolves user config directory |

### Key design pattern

`execAndDoOnEveryLine` (defined in `qldd.h`) is a generic template that runs an arbitrary shell command via `popen` and calls an action callback for each output line, optionally async. All `ldd`/`otool`/`nm`/`file` invocations go through this helper.

### File-manager integrations

- `nautilus-python/extensions/` — Python extension for Nautilus (GNOME)
- `nemo/actions/` — Nemo action file
- `dolphin/` — KDE service menu entry

These are installed as separate CPack components (`nautilus-integration`, `dolphin-integration`) and are not part of the C++ build.

## Code style

`.clang-format` and `.clang-tidy` are present. Run clang-format before committing:
```bash
clang-format -i *.cpp *.h
```

## Pending performance / correctness tasks

Detailed task files live in `tasks/`. Do not implement fixes not listed here without first checking whether an existing task covers the change.

| ID | File(s) | Priority | Status | Summary |
|---|---|---|---|---|
| [task-01](tasks/task-01-fillexporttable-threading.md) | `qldd.cpp:145–177` | **Critical** | DONE | Qt widget calls from worker threads — undefined behavior / crash risk |
| [task-02](tasks/task-02-remove-debug-logging.md) | `qldd.cpp:162–164` | **High** | DONE | `qDebug` in hot symbol loop fires thousands of times in release builds |
| [task-03](tasks/task-03-demangle-heap-alloc.md) | `qldd.cpp:156` | Medium | DONE | `toStdString().c_str()` allocates heap per symbol; use `toUtf8().constData()` |
| [task-04](tasks/task-04-widget-update-batching.md) | `qldd.cpp:77–177` | **High** | DONE | Missing `setUpdatesEnabled(false/true)` causes per-item repaints |
| [task-05](tasks/task-05-qcolor-outside-loop.md) | `qldd.cpp:124` | Low | DONE | `QColor("red")` constructed per dependency line; replace with `Qt::red` |
| [task-06](tasks/task-06-cache-dynamic-cast.md) | `demanglerules.cpp:42` | Low | TODO | `dynamic_cast` repeated per row in loop; cache the result |
| [task-07](tasks/task-07-getinfo-double-build.md) | `qldd.cpp:187–198` | Low | TODO | `getInfo` builds string twice; eliminate intermediate `QStringList` |
| [task-08](tasks/task-08-copy-export-item-bug.md) | `mainwindow.cpp:263–272` | Medium | TODO | `copyExportItem` always copies `currentRow()` regardless of selection |
| [task-09](tasks/task-09-qdir-setcurrent-global-state.md) | `qldd.cpp:82,142` | Medium | TODO | `QDir::setCurrent` mutates process-global CWD; pass absolute path to command instead |
