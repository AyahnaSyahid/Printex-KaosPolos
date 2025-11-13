#include "produkmodel.h"
#include <QAbstractItemModel>
#include <QLocale>

ProdukModel::ProdukModel(QObject *parent)
    : src(new QSqlQueryModel(this)), QSortFilterProxyModel(parent) {
  src->setQuery("SELECT * FROM Produk");
  setSourceModel(src);
  setObjectName("produkModel");
}

ProdukModel::~ProdukModel() {}

QVariant ProdukModel::data(const QModelIndex &mi, int role) const {
  if (role == Qt::TextAlignmentRole) {
    if (mi.column() > 1 && mi.column() < 4) {
      return (int)Qt::AlignRight | Qt::AlignVCenter;
    } else if (mi.column() == 1) {
      return (int)Qt::AlignVCenter | Qt::AlignHCenter;
    }
  } else if (role == Qt::DisplayRole) {
    if (mi.column() == 2 || mi.column() == 3) {
      return QLocale().toString(
          QSortFilterProxyModel::data(mi, role).toInt());
    }
  }
  return QSortFilterProxyModel::data(mi, role);
}

void ProdukModel::refresh() { src->setQuery("SELECT * FROM Produk"); }
