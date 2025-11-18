#include "stockmodel.h"

StockModel::StockModel(QObject *parent)
: QSqlTableModel(parent) {
  setObjectName("stockModel");
  setEditStrategy(QSqlTableModel::OnManualSubmit);
  setTable("Produk");
  select();
}

QVariant StockModel::data(const QModelIndex &mi, int role) const {
  switch (role) {
    case Qt::DisplayRole: {
      switch (mi.column()) {
        case 3:
          return QString("Rp. %L1").arg(QSqlTableModel::data(mi, Qt::EditRole).toInt());
        default:
          return QSqlTableModel::data(mi, role);
      }
      break;
    }
    case Qt::TextAlignmentRole: {
      switch (mi.column()) {
        case 2:
        case 3:
          return (int) (Qt::AlignRight | Qt::AlignVCenter);
        default:
          return QSqlTableModel::data(mi, role);
      }
      break;
    }
    default:
      return QSqlTableModel::data(mi, role);
  }
  return QSqlTableModel::data(mi, role);
}

bool StockModel::ambilProduk(const QString& nama, int qty) {
  auto ip = indexOf(nama);
  if(ip == -1) {
    return false;
  }
  auto bf = index(ip, 2).data(Qt::EditRole).toInt();
  if (bf - qty < 0) {
    return false;
  }
  return setData(index(ip, 2), bf - qty);
}

bool StockModel::simpanProduk(const QString& nama, int qty) {
  auto ip = indexOf(nama);
  if(ip == -1) {
    return false;
  }
  auto bf = index(ip, 2).data(Qt::EditRole).toInt();
  return setData(index(ip, 2), bf + qty);
}

int StockModel::indexOf(const QString &name) const {
  int r = -1;
  for(int row=0; row < rowCount(); ++row) {
    if(index(row, 1).data().toString() == name) {
      return row;
    }
  }
  return r;
}