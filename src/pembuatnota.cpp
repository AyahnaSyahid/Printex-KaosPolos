#include "pembuatnota.h"

#include <qstandarditemmodel.h>

#include <QAction>
#include <QMenu>
#include <QStandardItemModel>

#include "ui/ui_pembuatnota.h"

PembuatNota::PembuatNota(QWidget *parent)
    : ui(new Ui::PembuatNota),
      sm(new QStandardItemModel(30, 5)),
      QDialog(parent) {
  ui->setupUi(this);
  auto menu = new QMenu(this);
  auto sim = menu->addAction("Simpan");
  auto bay = menu->addAction("Bayar");
  ui->pushButton->setMenu(menu);

  sm->setHeaderData(0, Qt::Horizontal, "No.");
  sm->setHeaderData(1, Qt::Horizontal, "Produk");
  sm->setHeaderData(2, Qt::Horizontal, "Qty");
  sm->setHeaderData(3, Qt::Horizontal, "Harga");
  sm->setHeaderData(4, Qt::Horizontal, "Total");
  ui->notaTabel->setModel(sm);

  QStandardItem *item;
  for (int i = 0; i < 30; ++i) {
    QList<QStandardItem *> columns;
    for (int j = 0; j < 5; ++j) {
      item = new QStandardItem();
      if (j == 0 || j == 4) {
        item->setEditable(false);
      }
    }
    sm->appendRow(columns);
  }
}

PembuatNota::~PembuatNota() { delete ui; }
