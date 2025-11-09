#ifndef PEMBUATNOTAMODEL_H
#define PEMBUATNOTAMODEL_H

#include <QSqlQueryModel>
#include <QStandardItemModel>

class PembuatNotaModel : public QStandardItemModel {
  Q_OBJECT

 public:
  explicit PembuatNotaModel(QObject *parent = nullptr);
  ~PembuatNotaModel();

  struct DataPenjualan;
  enum DataRole { MinPriceRole = 2001, MaxQtyRole };

  QList<DataPenjualan> sales() const;

  Qt::ItemFlags flags(const QModelIndex &);

 public slots:
  void setProduk(int row, const QString &nama);

 private:
  QSqlQueryModel *qm;
};

struct PembuatNotaModel::DataPenjualan {
  QString namaProduk;
  int qty;
  int price;
  int total() const;
};

#endif  // "pembuatnotamodel.h"
