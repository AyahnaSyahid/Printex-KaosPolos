#ifndef AddProduk_H
#define AddProduk_H

#include <QDialog>
#include <QVariantMap>
#include <QAbstractItemModel>

namespace Ui {
class AddProduk;
}

class AddProduk : public QDialog {
  Q_OBJECT

public:
  explicit AddProduk(QAbstractItemModel *m, QWidget *parent=nullptr);
  ~AddProduk();

public slots:
  void produkAdded(const QString&, bool ok, const QString&);

private slots:
  void on_saveButton_clicked();

signals:
  void addProduk(const QVariantMap&);

private:
  Ui::AddProduk *ui;
  QAbstractItemModel *model;
};

#endif // NewProductDialog_H
