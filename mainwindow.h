#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qldd.h>
#include <QShortcut>
#include <QScopedPointer>
#include <QMap>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QString fileName, QWidget *parent = nullptr);
  ~MainWindow() override;
  void fillExportTable(const QString &filter);
  const RulesMap &demangleRules() const;
  QFont getFixedFont() const;
  void saveDemangleRules(const RulesMap &rules);
  void initDemangleRules();
  void reset();

 private slots:
  void open();
  void about();
  void find();
  void myClose();

  void showContextMenu(const QPoint &pos);

  void copyExportItem();

  void on_checkBoxOwnerRead_clicked(bool checked);

  void on_checkBoxOwnerWrite_clicked(bool checked);

  void on_checkBoxOwnerExec_clicked(bool checked);

  void on_checkBoxGroupRead_clicked(bool checked);

  void on_checkBoxGroupWrite_clicked(bool checked);

  void on_checkBoxGroupExec_clicked(bool checked);

  void on_checkBoxOtherRead_clicked(bool checked);

  void on_checkBoxOtherWrite_clicked(bool checked);

  void on_checkBoxOtherExec_clicked(bool checked);

  void on_filterButton_clicked();

  void on_resetButton_clicked();

  void on_rulesButton_clicked();

 private:
  void createActions();
  void createMenus();
  Ui::MainWindow *ui;
  QString _fileName;
  QScopedPointer<QLdd> qldd;
  QShortcut *shortcutClose;
  QShortcut *shortcutFind;
  QMenu *fileMenu;
  QMenu *helpMenu;
  QAction *openAct;
  QAction *aboutAct;
  QAction *aboutQtAct;
  QAction *exitAct;
  QFont fixedFont;
  RulesMap _demangleRules;
};

#endif  // MAINWINDOW_H
