#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>
#include <QFileDialog>
#include <QShortcut>
#include <QMessageBox>
#include <QClipboard>
#include <QFontDatabase>
#include "finfdialog.h"

MainWindow::MainWindow(const QString &fileName, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      qldd(nullptr),
      shortcutClose(nullptr),
      fileMenu(nullptr),
      helpMenu(nullptr),
      openAct(nullptr),
      aboutAct(nullptr),
      aboutQtAct(nullptr),
      exitAct(nullptr) {
  ui->setupUi(this);

  int id = QFontDatabase::addApplicationFont(":/monofont.ttf");
  QString family = QFontDatabase::applicationFontFamilies(id).at(0);
  fixedFont = QFont(family);

  ui->listWidgetExportTable->setFont(fixedFont);
  ui->treeWidget->setFont(fixedFont);

  shortcutClose = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  connect(shortcutClose, SIGNAL(activated()), this, SLOT(myClose()));

  shortcutFind = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_F), this);
  connect(shortcutFind, SIGNAL(activated()), this, SLOT(find()));
  ui->filterButton->setIcon(ui->filterButton->style()->standardIcon(QStyle::SP_FileDialogListView));
  ui->resetButton->setIcon(ui->resetButton->style()->standardIcon(QStyle::SP_DialogCancelButton));

  createActions();
  createMenus();

  if (!fileName.isEmpty()) {
    reset(fileName);
  } else {
    QTreeWidgetItem *header = ui->treeWidget->headerItem();
    header->setText(0, "Dependency");
  }
  ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow() {
  delete ui;
  delete fileMenu;
  delete helpMenu;
}

void MainWindow::fillExportTable(const QString &filter) {
  if (qldd) {
    qldd->fillExportTable(*ui->listWidgetExportTable, filter);
  }
}

void MainWindow::reset(const QString &fileName) {
  qldd.reset(new QLdd(fileName, qApp->applicationDirPath()));
  QTreeWidgetItem *header = ui->treeWidget->headerItem();
  header->setText(0, "Dependency");
  qldd->fillDependency(*ui->treeWidget);
  fillExportTable("");

  ui->lineEditFileName->setText(qldd->getBinaryName());
  ui->lineEditFileSize->setText(qldd->getStringFileSize() + "( " + QString::number(qldd->getFileSize()) + " bytes )");
  ui->lineEditTimeAccess->setText(qldd->getAccessTime());
  ui->lineEditTimeStatus->setText(qldd->getCreatedTime());
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
  QMessageBox::about(this,
                     tr("About Application"),
                     tr("DependencyViewer shows all dependecies of a "
                        "given executable or dynamic library. It is a GUI wrapper for the ldd, file and nm command."));
}

void MainWindow::find() {
  auto fd = new FinfDialog(this);
  fd->show();
}

void MainWindow::myClose() {
  if (ui->tabWidget->currentIndex() == 1) {
    fillExportTable("");
  } else {
    close();
  }
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

  ui->listWidgetExportTable->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->listWidgetExportTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

void MainWindow::showContextMenu(const QPoint &pos) {
  // Handle global position
  QPoint globalPos = ui->listWidgetExportTable->mapToGlobal(pos);
  // Create menu and insert some actions
  QMenu myMenu;
  myMenu.addAction("Copy", this, SLOT(copyExportItem()));
  // Show context menu at handling position
  myMenu.exec(globalPos);
}

void MainWindow::copyExportItem() {
  QClipboard *clipboard = qApp->clipboard();
  // If multiple selection is on, we need to erase all selected items
  for (int i = 0; i < ui->listWidgetExportTable->selectedItems().size(); ++i) {
    // Get curent item on selected row
    QListWidgetItem *item = ui->listWidgetExportTable->item(ui->listWidgetExportTable->currentRow());
    // And copy text from it
    clipboard->setText(item->text());
  }
}
void MainWindow::on_checkBoxOwnerRead_clicked(bool checked) { ui->checkBoxOwnerRead->setChecked(!checked); }

void MainWindow::on_checkBoxOwnerWrite_clicked(bool checked) { ui->checkBoxOwnerWrite->setChecked(!checked); }

void MainWindow::on_checkBoxOwnerExec_clicked(bool checked) { ui->checkBoxOwnerExec->setChecked(!checked); }

void MainWindow::on_checkBoxGroupRead_clicked(bool checked) { ui->checkBoxGroupRead->setChecked(!checked); }

void MainWindow::on_checkBoxGroupWrite_clicked(bool checked) { ui->checkBoxGroupWrite->setChecked(!checked); }

void MainWindow::on_checkBoxGroupExec_clicked(bool checked) { ui->checkBoxGroupExec->setChecked(!checked); }

void MainWindow::on_checkBoxOtherRead_clicked(bool checked) { ui->checkBoxOtherRead->setChecked(!checked); }

void MainWindow::on_checkBoxOtherWrite_clicked(bool checked) { ui->checkBoxOtherWrite->setChecked(!checked); }

void MainWindow::on_checkBoxOtherExec_clicked(bool checked) { ui->checkBoxOtherExec->setChecked(!checked); }

void MainWindow::on_filterButton_clicked() { find(); }

void MainWindow::on_resetButton_clicked() { myClose(); }
