#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextStream>
#include <QFileDialog>
#include <QShortcut>
#include <QMessageBox>
#include <QClipboard>
#include <QFontDatabase>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDir>
#include "finfdialog.h"
#include "version.h"
#include "config.h"
#include "demanglerules.h"

MainWindow::MainWindow(const QString &fileName, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      _fileName(fileName),
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
  initDemangleRules();
  if (!_fileName.isEmpty()) {
    reset();
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

const RulesMap &MainWindow::demangleRules() const { return _demangleRules; }

void MainWindow::reset() {
  if (!_fileName.isEmpty()) {
    qldd.reset(new QLdd(_fileName, qApp->applicationDirPath(), _demangleRules));
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
}

QFont MainWindow::getFixedFont() const { return fixedFont; }

void MainWindow::saveDemangleRules(const RulesMap &rules) {
  QDir d(QDir::home());
  if (!d.exists(DEMANGLE_RULES_DEFAULT_PATH)) {
    if (!d.mkdir(DEMANGLE_RULES_DEFAULT_PATH)) {
      QMessageBox::critical(this, "Demangle rules", "Can't createdirectory " + d.homePath() + DEMANGLE_RULES_DEFAULT_PATH);
    }
  }

  QString path = d.homePath() + "/" + DEMANGLE_RULES_DEFAULT_PATH + "/rules.json";
  QFile relesFile(path);
  if (relesFile.open(QFile::OpenModeFlag::ReadWrite | QFile::OpenModeFlag::Truncate | QFile::OpenModeFlag::Text)) {
    QJsonDocument doc;
    QJsonObject obj;
    QJsonArray arrRules;
    for (auto item = rules.begin(); item != rules.end(); ++item) {
      QJsonObject r;
      r[item->first] = item->second;
      arrRules.append(r);
    }
    obj["rules"] = arrRules;
    doc.setObject(obj);
    relesFile.write(doc.toJson());
    relesFile.close();
  }
}

void MainWindow::open() {
  _fileName = QFileDialog::getOpenFileName(this);
  reset();
}

void MainWindow::about() {
  QString application = tr("Application");
  QString fullVersion = tr("Full Version");
  QString qtVersion = tr("Qt Version");
  QString buildDate = tr("Build Date");
  QMessageBox::about(this,
                     "About",
                     tr("DependencyViewer shows all dependecies of a "
                        "given executable or dynamic library. It is a GUI wrapper for the ldd, file and nm commands.") +
                         QString("<qt><table><tr><td><b>") + application + ("</b></td><td>") + tr("DependencyViewerr") + QString("</td></tr>") +
                         QString("<tr><td><b>") + fullVersion + ("</b></td><td>") + PROJECT_VERSION + QString("</td></tr>") + QString("<tr><td><b>") +
                         qtVersion + ("</b></td><td>") + QT_VERSION_STR + QString("</td></tr>") + QString("<tr><td><b>") + buildDate +
                         ("</b></td><td>") + COMMITTER_DATE + QString("</td></tr>") + QString("</td></tr></table></qt>"));
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

void MainWindow::initDemangleRules() {
  QDir d(QDir::home());
  QString path = d.homePath() + "/" + DEMANGLE_RULES_DEFAULT_PATH + "/rules.json";
  QFile relesFile(path);
  if (!relesFile.exists()) {
    d.mkpath(DEMANGLE_RULES_DEFAULT_PATH);
    QFile::copy(":/rules.json", path);
    relesFile.setPermissions(QFile::ReadUser | QFile::WriteUser);
  }

  if (relesFile.open(QFile::OpenModeFlag::ReadOnly | QFile::OpenModeFlag::Text)) {
    _demangleRules.clear();
    QString val = relesFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject obj = doc.object();
    QJsonArray arrRules = obj["rules"].toArray();
    for (const auto &item : qAsConst(arrRules)) {
      auto itemObj = item.toObject();
      auto itemKeys = itemObj.keys();
      if (itemKeys.size() == 1) {
        auto key = itemKeys.first();
        _demangleRules.push_back({key, itemObj[key].toString()});
      }
    }
  }
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

void MainWindow::on_rulesButton_clicked() {
  auto dr = new demanglerules(this);
  dr->show();
}
