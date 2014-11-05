#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qldd.h>
#include <QShortcut>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(const QString &fileName, QWidget *parent = 0);
  ~MainWindow();
private slots:
  void open();
  void about();

private:
  void createActions();
  void createMenus();
  void reset(const QString &fileName);
  Ui::MainWindow *ui;
  QLdd *qldd;
  QShortcut *shortcutClose;
  QMenu *fileMenu;
  QMenu *helpMenu;
  QAction *openAct;
  QAction *aboutAct;
  QAction *aboutQtAct;
  QAction *exitAct;
};

#endif // MAINWINDOW_H
