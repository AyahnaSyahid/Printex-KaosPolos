#include "pembuatnota.h"

#include <qnamespace.h>

#include <QAction>
#include <QDateTime>
#include <QMenu>

#include "include/addkonsumen.h"
#include "include/pembuatnotamodel.h"
#include "ui/ui_pembuatnota.h"

PembuatNota::PembuatNota(QWidget *parent)
    : ui(new Ui::PembuatNota), sm(new PembuatNotaModel(this)), QDialog(parent) {
  ui->setupUi(this);
  auto menu = new QMenu(this);
  auto sim = menu->addAction("Simpan");
  auto bay = menu->addAction("Bayar");
  ui->pushButton->setMenu(menu);
  ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
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

void PembuatNota::createKonsumen() {
  AddKonsumen *ak = new AddKonsumen();
  ak->setAttribute(Qt::WA_DeleteOnClose);
  connect(ak, &AddKonsumen::finishEditing, this,
          &PembuatNota::addKonsumenReceiver);
}

void PembuatNota::on_konsumenCombo_customContextMenuRequested(const QPoint &p) {
  QMenu kmenu;
  QAction *act = kmenu.addAction("Konsumen Baru");
}
void PembuatNota::on_notaTable_customContextMenuRequested(const QPoint &p) {}
void PembuatNota::addKonsumenReceiver() 
{
  AddKonsumen *ak = qobject_cast<AddKonsumen*>(sender());
  if (!ak) return;
  
}
