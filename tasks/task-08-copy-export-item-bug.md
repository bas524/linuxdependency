# Task 08 — Fix copyExportItem always copying currentRow

**Priority:** Medium  
**Status:** DONE  
**File:** `mainwindow.cpp:263–272`

## Problem

```cpp
void MainWindow::copyExportItem() {
    QClipboard *clipboard = qApp->clipboard();
    for (int i = 0; i < ui->listWidgetExportTable->selectedItems().size(); ++i) {
        QListWidgetItem *item = ui->listWidgetExportTable->item(ui->listWidgetExportTable->currentRow());
        clipboard->setText(item->text());
    }
}
```

Two bugs:
1. The loop indexes with `i` but always fetches `currentRow()` — for multi-selection it overwrites the clipboard N times with the same item.
2. `selectedItems()` is called once per iteration (implicit in the loop condition re-evaluation).

## Fix

```cpp
void MainWindow::copyExportItem() {
    const auto selected = ui->listWidgetExportTable->selectedItems();
    if (!selected.isEmpty()) {
        qApp->clipboard()->setText(selected.last()->text());
    }
}
```

If multi-item copy (newline-joined) is desired:

```cpp
void MainWindow::copyExportItem() {
    QStringList texts;
    for (const auto *item : ui->listWidgetExportTable->selectedItems()) {
        texts << item->text();
    }
    if (!texts.isEmpty()) {
        qApp->clipboard()->setText(texts.join('\n'));
    }
}
```

## Acceptance criteria

- `currentRow()` removed from `copyExportItem`.
- Selected items iterated correctly.
- No redundant `selectedItems()` calls inside the loop condition.
