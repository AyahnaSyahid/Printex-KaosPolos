#ifndef AddProduk_H
#define AddProduk_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QVariantMap>

namespace Ui {
class AddProduk;
}

class AddProduk : public QDialog {
  Q_OBJECT

 public:
  explicit AddProduk(QWidget *parent = nullptr);
  ~AddProduk();
  QString nama() const;
  int initialStock() const;
  int price() const;

 private slots:
  void on_saveButton_clicked();

 signals:
  void inputFinished();

 private:
  Ui::AddProduk *ui;
  QSqlQueryModel *model;
};

#endif  // NewProductDialog_H
