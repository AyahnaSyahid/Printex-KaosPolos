#include "pembuatnota.h"
#include "ui/ui_pembuatnota.h"
#include <QStandardItemModel>
#include <QAction>
#include <QMenu>

PembuatNota::PembuatNota(QWidget *parent)
  : ui(new Ui::PembuatNota), sm(new QStandardItemModel(30, 5)), QDialog(parent)
{
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
  
  QStandardItem *item = nullptr;
  for(int i=0; i<30; ++i) {
    item  = sm->item(i, 0);
    item->setEditable(false);
    item  = sm->item(i, 4);
    item->setEditable(false);
  }
}

PembuatNota::~PembuatNota() { delete ui; }
