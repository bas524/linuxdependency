#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <iostream>

MainWindow::MainWindow(const QString &fileName, QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  qldd(NULL) {
  ui->setupUi(this);

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

MainWindow::~MainWindow() {
  delete ui;
  delete qldd;
}
