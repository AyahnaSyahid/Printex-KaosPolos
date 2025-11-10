#include "pembuatnota.h"

#include <QAction>
#include <QDateTime>
#include <QMenu>
#include <QHeaderView>
#include <QSqlQueryModel>

#include "include/kaospoloswindow.h"
#include "include/dockkonsumen.h"
#include "include/pembuatnotamodel.h"
#include "ui/ui_pembuatnota.h"

PembuatNota::PembuatNota(KaosPolosWindow *kp, QWidget *parent)
    : ui(new Ui::PembuatNota), 
      kpw(kp), 
      sm(new PembuatNotaModel(this)), 
      konsumenModel(new QSqlQueryModel(this)), 
      QDialog(parent) {
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
  ui->notaTable->setModel(sm);
  ui->notaTable->horizontalHeader()->resizeSection(0, 32);
  ui->notaTable->horizontalHeader()->resizeSection(1, 32 * 5);
  ui->notaTable->horizontalHeader()->resizeSection(2, 36);
  ui->notaTable->horizontalHeader()->resizeSection(3, 65);
  ui->notaTable->horizontalHeader()->resizeSection(4, 32 * 4);

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

  konsumenModel->setQuery("SELECT * FROM Konsumen");
  ui->konsumenKombo->setModel(konsumenModel);
  ui->konsumenKombo->setModelColumn(1);
  ui->konsumenKombo->setCurrentIndex(-1);
  
  dc = kpw->findChild<DockKonsumen*>("dockKonsumen");
  if(dc) {
    connect(dc, &DockKonsumen::konsumenAdded, this, &PembuatNota::refreshKonsumen);
  }
}

PembuatNota::~PembuatNota() { delete ui; }

void PembuatNota::createKonsumen() {
  if (dc) {
    dc->addKonsumen();
  }
}

void PembuatNota::on_konsumenKombo_customContextMenuRequested(const QPoint &p) {
  QMenu kmenu;
  QAction *act = kmenu.addAction("Konsumen Baru");
  connect(act, &QAction::triggered, this, &PembuatNota::createKonsumen);
  kmenu.exec(ui->konsumenKombo->mapToGlobal(p));
}

void PembuatNota::on_notaTable_customContextMenuRequested(const QPoint &p)
{
  
}

void PembuatNota::refreshKonsumen() {
  konsumenModel->setQuery(konsumenModel->query().lastQuery());
  ui->konsumenKombo->setCurrentIndex(-1);
}