#include "produkmodel.h"

ProdukModel::ProdukModel(QObject *parent)
  : src(new QSqlQueryModel(this)), QSortFilterProxyModel(parent)
{
  src->setQuery("SELECT * FROM Produk");
  setSourceModel(src);
}

ProdukModel::~ProdukModel()
{
  
}

void ProdukModel::refresh()
{
  src->setQuery("SELECT * FROM Produk");
}