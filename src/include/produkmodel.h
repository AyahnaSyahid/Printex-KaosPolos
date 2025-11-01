#ifndef PRODUKMODEL_H
#define PRODUKMODEL_H

#include <QSortFilterProxyModel>
#include <QSqlQueryModel>

class ProdukModel : public QSortFilterProxyModel
{
  
public:
  ProdukModel(QObject *parent);
  ~ProdukModel();
  
  void refresh();

private:
  QSqlQueryModel *src;

};

#endif // PRODUKMODEL_H