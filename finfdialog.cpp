#include "finfdialog.h"
#include "ui_finfdialog.h"
#include "mainwindow.h"

FinfDialog::FinfDialog(QWidget *parent) : QDialog(parent), ui(new Ui::FinfDialog) {
  ui->setupUi(this);
  setWindowTitle("Filter export table");
  shortcutClose = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  connect(shortcutClose, SIGNAL(activated()), this, SLOT(close()));

  shortcutFind = new QShortcut(QKeySequence(Qt::Key_Enter), this);
  connect(shortcutFind, SIGNAL(activated()), this, SLOT(find()));
}

FinfDialog::~FinfDialog() { delete ui; }

void FinfDialog::on_findButton_clicked() {
  if (!ui->findLineEdit->text().isEmpty()) {
    auto *mw = dynamic_cast<MainWindow *>(parentWidget());
    if (mw) {
      mw->fillExportTable(ui->findLineEdit->text());
    }
    close();
  }
}
