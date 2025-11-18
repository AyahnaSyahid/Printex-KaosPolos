#ifndef STOCKMODEL_H
#define STOCKMODEL_H

#include <QSqlTableModel>

class StockModel : public QSqlTableModel
{
  Q_OBJECT
public:
  
  explicit StockModel(QObject *parent = nullptr);
  QVariant data(const QModelIndex &mi, int role=Qt::DisplayRole) const;
  bool ambilProduk(const QString& name, int qty);
  bool simpanProduk(const QString& name, int qty);

private:
  int indexOf(const QString& nama) const;
};

#endif