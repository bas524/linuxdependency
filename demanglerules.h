#ifndef DEMANGLERULES_H
#define DEMANGLERULES_H

#include <QDialog>

namespace Ui {
class demanglerules;
}

class demanglerules : public QDialog {
  Q_OBJECT

 public:
  enum class Fields { Selection = 0, Source = 1, Destination = 2, COUNT };
  explicit demanglerules(QWidget *parent = nullptr);
  ~demanglerules();

 private:
  Ui::demanglerules *ui;
  void insertNewRow(int row, const QString &src, const QString &dst);
 private slots:
  void selectRow(bool flag);
  void on_pBAddRule_clicked();
  void on_pBRemoveRule_clicked();
  void on_buttonBox_accepted();
};

#endif  // DEMANGLERULES_H
