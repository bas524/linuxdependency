#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <iostream>
#include <QFileDialog>
#include <QShortcut>
#include <QMessageBox>

MainWindow::MainWindow(const QString &fileName, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  qldd(NULL),
  shortcutClose(NULL),
  fileMenu(NULL),
  helpMenu(NULL),
  openAct(NULL),
  aboutAct(NULL),
  aboutQtAct(NULL),
  exitAct(NULL) {
  ui->setupUi(this);

  shortcutClose = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  connect(shortcutClose, SIGNAL(activated()), this, SLOT(close()));

  createActions();
  createMenus();

  if (!fileName.isEmpty()) {
    reset(fileName);
  }
}

MainWindow::~MainWindow() {
  delete ui;
  if (qldd) {
    delete qldd;
  }
  delete fileMenu;
  delete helpMenu;
  delete openAct;
  delete aboutAct;
  delete aboutQtAct;
  delete exitAct;
}

void MainWindow::reset(const QString &fileName) {
  if (qldd) {
    delete qldd;
    qldd = NULL;
  }
  qldd = new QLdd(fileName, qApp->applicationDirPath());
  QTreeWidgetItem *header = ui->treeWidget->headerItem();
  header->setText(0, "Dependency");
  qldd->fillDependency(*ui->treeWidget);

  ui->labelName->setText("File Name   ");
  ui->lineEditFileName->setText(qldd->getBinaryName());
  ui->labelSize->setText("File Size   ");
  ui->lineEditFileSize->setText(qldd->getStringFileSize() + "( " + QString::number(qldd->getFileSize()) + " bytes )");
  ui->labelTimeAccess->setText("Access Time ");
  ui->lineEditTimeAccess->setText(qldd->getAccessTime());
  ui->labelTimeStatus->setText("Status Time ");
  ui->lineEditTimeStatus->setText(qldd->getStatusTime());
  ui->labelTimeModify->setText("Modify Time ");
  ui->lineEditTimeModify->setText(qldd->getModifyTime());


  ui->lineEditOwner->setText(qldd->getOwnerName());
  ui->lineEditGroup->setText(qldd->getGroupName());

  ui->textEditInformation->setText(qldd->getInfo());
  QMOD owner = qldd->getOwnerMod();
  QMOD group = qldd->getGroupMod();
  QMOD other = qldd->getOtherMod();

  ui->checkBoxOwnerRead->setChecked(owner.read);
  ui->checkBoxOwnerWrite->setChecked(owner.write);
  ui->checkBoxOwnerExec->setChecked(owner.execute);

  ui->checkBoxGroupRead->setChecked(group.read);
  ui->checkBoxGroupWrite->setChecked(group.write);
  ui->checkBoxGroupExec->setChecked(group.execute);

  ui->checkBoxOtherRead->setChecked(other.read);
  ui->checkBoxOtherWrite->setChecked(other.write);
  ui->checkBoxOtherExec->setChecked(other.execute);
}

void MainWindow::open() {

  QString fileName = QFileDialog::getOpenFileName(this);
  if (!fileName.isEmpty()) {
    reset(fileName);
  }

}

void MainWindow::about() {
  QMessageBox::about(this, tr("About Application"),
                     tr("Linuxdependency shows all dependent libraries of a given executable or dynamic library on Linux. It is a GUI replacement for the ldd command."));
}

void MainWindow::createActions() {
  openAct = new QAction(QIcon("gtk-open"), tr("&Open..."), this);
  openAct->setShortcuts(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcuts(QKeySequence::Quit);
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus() {
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}
