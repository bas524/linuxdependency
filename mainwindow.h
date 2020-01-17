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

  void on_checkBoxOwnerRead_clicked(bool checked);

  void on_checkBoxOwnerWrite_clicked(bool checked);

  void on_checkBoxOwnerExec_clicked(bool checked);

  void on_checkBoxGroupRead_clicked(bool checked);

  void on_checkBoxGroupWrite_clicked(bool checked);

  void on_checkBoxGroupExec_clicked(bool checked);

  void on_checkBoxOtherRead_clicked(bool checked);

  void on_checkBoxOtherWrite_clicked(bool checked);

  void on_checkBoxOtherExec_clicked(bool checked);

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

#endif  // MAINWINDOW_H
