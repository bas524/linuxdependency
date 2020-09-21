#ifndef FINFDIALOG_H
#define FINFDIALOG_H

#include <QDialog>
#include <QShortcut>

namespace Ui {
class FinfDialog;
}

class FinfDialog : public QDialog {
  Q_OBJECT

 public:
  explicit FinfDialog(QWidget *parent = nullptr);
  ~FinfDialog();

 private slots:
  void on_findButton_clicked();

 private:
  Ui::FinfDialog *ui;
  QShortcut *shortcutClose;
  QShortcut *shortcutFind;
};

#endif  // FINFDIALOG_H
