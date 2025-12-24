#ifndef PRODUKINFO_H
#define PRODUKINFO_H

#include <QDialog>

namespace Ui {
  class ProdukInfo;
}

class Database;
class ProdukInfo : public QDialog
{
  Q_OBJECT

public:
  explicit ProdukInfo(const QString &pname, Database *, QWidget* = nullptr);
  ~ProdukInfo();
  
  bool isModified() const;

private slots:
  void on_ubahNamaButton_clicked();
  void on_ubahHargaButton_clicked();
  void on_tambahStockButton_clicked();
  void on_simpanButton_clicked();

signals:
  void produkUpdated();

private:
  void on_produkNotFound();

  QString cnama, nnama;
  int cstock, addStock;
  int charga, nharga;
  
  Ui::ProdukInfo *ui;
  Database *db;
};

#endif // PRODUKINFO_H