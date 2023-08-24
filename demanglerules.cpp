#include "demanglerules.h"
#include "ui_demanglerules.h"
#include "mainwindow.h"
#include <QStringList>
#include <QCheckBox>
#include <QList>
#include <QDebug>

demanglerules::demanglerules(QWidget *parent) : QDialog(parent), ui(new Ui::demanglerules) {
  ui->setupUi(this);
  auto *m = dynamic_cast<MainWindow *>(parent);
  ui->tableWidget->setColumnCount((int)Fields::COUNT);
  const auto &rulesRef = m->demangleRules();
  QStringList tableHeader;
  tableHeader << "#"
              << "find"
              << "replace wih";
  ui->tableWidget->setHorizontalHeaderLabels(tableHeader);
  ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
  ui->tableWidget->horizontalHeader()->resizeSection(0, 60);
  ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignHCenter);
  ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
  int i = 0;
  for (const auto &item : rulesRef) {
    insertNewRow(i, item.first, item.second);
    ++i;
  }
}

demanglerules::~demanglerules() { delete ui; }

void demanglerules::insertNewRow(int row, const QString &src, const QString &dst) {
  ui->tableWidget->insertRow(row);
  auto *m = dynamic_cast<MainWindow *>(parent());

  auto *checkBoxWidget = new QWidget();
  auto *checkBox = new QCheckBox();
  auto *layoutCheckBox = new QHBoxLayout(checkBoxWidget);
  layoutCheckBox->addWidget(checkBox);
  layoutCheckBox->setAlignment(Qt::AlignCenter);
  layoutCheckBox->setContentsMargins(0, 0, 0, 0);

  checkBox->setProperty("row", row);
  connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(selectRow(bool)));
  ui->tableWidget->setCellWidget(row, (int)Fields::Selection, checkBoxWidget);

  auto *itemSrc = new QTableWidgetItem(src);
  itemSrc->setToolTip(src);
  itemSrc->setFont(m->getFixedFont());
  ui->tableWidget->setItem(row, (int)Fields::Source, itemSrc);
  auto *itemDst = new QTableWidgetItem(dst);
  itemDst->setToolTip(dst);
  itemDst->setFont(m->getFixedFont());
  ui->tableWidget->setItem(row, (int)Fields::Destination, itemDst);
}

void demanglerules::selectRow(bool flag) {
  int row = sender()->property("row").toInt();
  QModelIndex index0 = ui->tableWidget->model()->index(row, (int)Fields::Selection);
  QModelIndex index1 = ui->tableWidget->model()->index(row, (int)Fields::Source);
  QModelIndex index2 = ui->tableWidget->model()->index(row, (int)Fields::Destination);
  if (flag == true) {
    ui->tableWidget->selectionModel()->select(index0, QItemSelectionModel::Select);
    ui->tableWidget->selectionModel()->select(index1, QItemSelectionModel::Select);
    ui->tableWidget->selectionModel()->select(index2, QItemSelectionModel::Select);
  } else {
    ui->tableWidget->selectionModel()->select(index0, QItemSelectionModel::Deselect);
    ui->tableWidget->selectionModel()->select(index1, QItemSelectionModel::Deselect);
    ui->tableWidget->selectionModel()->select(index2, QItemSelectionModel::Deselect);
  }
}

void demanglerules::on_pBAddRule_clicked() {
  auto row = ui->tableWidget->rowCount();
  insertNewRow(row, "", "");
  ui->tableWidget->setCurrentCell(row, (int)Fields::Source);
  ui->tableWidget->editItem(ui->tableWidget->currentItem());
}

void demanglerules::on_pBRemoveRule_clicked() {
  QModelIndexList indexes = ui->tableWidget->selectionModel()->selectedRows();
  int countRow = indexes.count();

  for (int i = countRow; i > 0; i--) {
    ui->tableWidget->model()->removeRow(indexes.at(i - 1).row(), QModelIndex());
  }
}

void demanglerules::on_buttonBox_accepted() {
  auto *m = dynamic_cast<MainWindow *>(parent());
  RulesMap rules;
  for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
    auto *itemSrc = ui->tableWidget->item(i, (int)Fields::Source);
    auto *itemDst = ui->tableWidget->item(i, (int)Fields::Destination);
    rules.push_back({itemSrc->text(), itemDst->text()});
    qDebug() << itemSrc->text() << "->" << itemDst->text();
  }
  if (!rules.empty()) {
    m->saveDemangleRules(rules);
    m->initDemangleRules();
    m->reset();
  }
}
