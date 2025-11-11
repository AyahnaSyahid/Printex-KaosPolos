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
#include "notainputdialog.h"

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
  ui->notaTable->horizontalHeader()->resizeSection(4, 28 * 4);
  ui->notaTable->setToolTip("Klik kanan pada mouse\nmenampilkan menu lanjutan");
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

void PembuatNota::addOrder() {
  NotaInputDialog *nid = new NotaInputDialog(this);
  connect(nid, &NotaInputDialog::doneEditing, this, &PembuatNota::processInputDialog);
  nid->open();
}

void PembuatNota::on_notaTable_customContextMenuRequested(const QPoint &p)
{
  QMenu nMenu;
  QAction* add = nMenu.addAction("Buat");
  connect(add, &QAction::triggered, this, &PembuatNota::addOrder);
  
  nMenu.exec(ui->notaTable->viewport()->mapToGlobal(p));
}

void PembuatNota::processInputDialog() {
  NotaInputDialog *nid = qobject_cast<NotaInputDialog*>(sender());
  if (!nid) return ;
  
  auto nomorItem = new QStandardItem(QString::number(sm->rowCount() + 1));
  nomorItem->setData((int) Qt::AlignRight | Qt::AlignVCenter, Qt::TextAlignmentRole);
  auto produkItem = new QStandardItem(nid->namaProduk());
  produkItem->setData((int) Qt::AlignHCenter | Qt::AlignVCenter, Qt::TextAlignmentRole);
  auto qtyItem = new QStandardItem(locale().toString(nid->qty()));
  qtyItem->setData((int) Qt::AlignRight | Qt::AlignVCenter, Qt::TextAlignmentRole);
  qtyItem->setData(nid->qty(), Qt::EditRole);
  qtyItem->setData(locale().toString(nid->qty()), Qt::DisplayRole);
  auto priceItem = new QStandardItem();
  priceItem->setData((int) Qt::AlignRight | Qt::AlignVCenter, Qt::TextAlignmentRole);
  priceItem->setData(nid->harga(), Qt::EditRole);
  priceItem->setData(locale().toString(nid->harga()), Qt::DisplayRole);
  auto totalItem = new QStandardItem();
  totalItem->setData((int) Qt::AlignRight | Qt::AlignVCenter, Qt::TextAlignmentRole);
  totalItem->setData(nid->harga() * nid->qty(), Qt::EditRole);
  totalItem->setData(locale().toString(nid->harga() * nid->qty()), Qt::DisplayRole);
  
  QList<QStandardItem*> row {nomorItem, produkItem, qtyItem, priceItem, totalItem}; 
  sm->appendRow(row);
  updateGrandTotal();
  
  nid->accept();
  nid->deleteLater();
}

void PembuatNota::updateGrandTotal() {
  int f = 0;
  for (int i=0; i < sm->rowCount(); ++i) {
    f += sm->index(i, 4).data(Qt::EditRole).toInt();
  }
  ui->totalLineEdit->setReadOnly(false);
  ui->totalLineEdit->setText(locale().toString(f));
  ui->totalLineEdit->setReadOnly(true);
}

void PembuatNota::refreshKonsumen() {
  konsumenModel->setQuery(konsumenModel->query().lastQuery());
  ui->konsumenKombo->setCurrentIndex(-1);
}