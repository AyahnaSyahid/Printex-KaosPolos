#ifndef AddProduk_H
#define AddProduk_H

#include <QDialog>

namespace Ui {
class AddProduk;
}

class AddProduk : public QDialog {
  Q_OBJECT

public:
  explicit AddProduk(QWidget *parent);
  ~AddProduk();

private slots:
  void on_saveButton_clicked();

signals:
  void productAdded(qint64, const QString &);

private:
  Ui::AddProduk *ui;
};

#endif // NewProductDialog_H
