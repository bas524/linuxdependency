#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qldd.h>

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(const QString &fileName, QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;
  QLdd *qldd;
};

#endif // MAINWINDOW_H
