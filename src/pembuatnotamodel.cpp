#include "pembuatnotamodel.h"

#include <QStandardItem>

PembuatNotaModel::PembuatNotaModel(QObject *parent)
    : qm(new QSqlQueryModel(this)), QStandardItemModel(30, 5, parent) {
  QStandardItem *item;
  QList<QStandardItem *> row;
  for (int i = 0; i < 30; i++) {
    row.clear();
    for (int j = 0; j < 5; ++j) {
      item = new QStandardItem();
      if (j == 0 || j == 4) {
        item->setEditable(false);
      }
    }
  }

  qm->setQuery("SELECT nama, base_price, stock FROM Produk");
  while (qm->canFetchMore()) qm->fetchMore();
}

PembuatNotaModel::~PembuatNotaModel() {}

void PembuatNotaModel::setProduk(int row, const QString &nama) {
  auto ino = item(row);
  if (ino) {
    auto ix = indexFromItem(ino);
    auto ipro = itemFromIndex(ix.siblingAtColumn(1));
    auto iqty = itemFromIndex(ix.siblingAtColumn(2));
    auto ihrg = itemFromIndex(ix.siblingAtColumn(3));
    auto ittl = itemFromIndex(ix.siblingAtColumn(4));
    for (int i = 0; i < qm->rowCount(); ++i) {
      if (qm->index(i, 0).data().toString() == nama) {
        ino->setData(row + 1);
        ipro->setData(nama);
        iqty->setData(1);
        iqty->setData(qm->index(i, 2).data(Qt::EditRole), MaxQtyRole);
        ihrg->setData(qm->index(i, 1).data());
        ittl->setData(1 * qm->index(i, 1).data().toInt());
      }
    }
  }
}
