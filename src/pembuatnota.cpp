#include "pembuatnota.h"
#include "database.h"

#include <QAction>
#include <QDateTime>
#include <QMenu>
#include <QHeaderView>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QCompleter>

#include "include/kaospoloswindow.h"
#include "include/dockkonsumen.h"
#include "include/dockproduk.h"
#include "include/pembuatnotamodel.h"
#include "ui/ui_pembuatnota.h"
#include "notainputdialog.h"
#include "paymentdialog.h"

PembuatNota::PembuatNota(KaosPolosWindow *kp, QWidget *parent)
    : ui(new Ui::PembuatNota), 
      kpw(kp), 
      sm(new PembuatNotaModel(this)), 
      konsumenModel(new QSqlQueryModel(this)), 
      QDialog(parent) {
  ui->setupUi(this);
  auto menu = new QMenu(this);
  auto bay = menu->addAction("Bayar");
  auto sim = menu->addAction("Simpan");
  connect(sim, &QAction::triggered, this, &PembuatNota::on_simpan);
  connect(bay, &QAction::triggered, this, &PembuatNota::on_bayar);
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
  auto comp = new QCompleter(this);
  auto compView = new QTableView();
  compView->verticalHeader()->setMinimumSectionSize(18);
  compView->verticalHeader()->setDefaultSectionSize(18);
  comp->setPopup(compView);
  comp->setModel(konsumenModel);
  comp->setCompletionColumn(1);
  comp->setCaseSensitivity(Qt::CaseInsensitive);
  ui->konsumenKombo->setCompleter(comp);
  ui->konsumenKombo->setModel(konsumenModel);
  ui->konsumenKombo->setModelColumn(1);
  ui->konsumenKombo->setCurrentIndex(-1);
  compView->horizontalHeader()->hideSection(0);
  compView->horizontalHeader()->hideSection(2);
  compView->horizontalHeader()->hideSection(4);
  compView->horizontalHeader()->hideSection(5);
  compView->verticalHeader()->hide();
  compView->horizontalHeader()->hide();
  compView->resizeColumnsToContents();
  compView->setSelectionBehavior(compView->SelectRows);
  auto hh = compView->horizontalHeader();
  compView->setMinimumWidth(hh->sectionSize(1) + hh->sectionSize(3) + 20);
  hh->setStretchLastSection(true);
  
  dc = kpw->findChild<DockKonsumen*>("dockKonsumen");
  // Rasanya dock konsumen tidak ada kaitannya dengan penjual/belian barang
  if(dc) {
    connect(dc, &DockKonsumen::konsumenAdded, this, &PembuatNota::refreshKonsumen);
  }
  auto dp = kpw->findChild<DockProduk*>("dockProduk");
  if (dp) {
    connect(this, &PembuatNota::penjualanSaved, dp, &DockProduk::refreshModel);
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
  // connect(nid, &NotaInputDialog::accepted, nid, &NotaInputDialog::deleteLater);
  // nid deleted on receiverSlot automaticaly
  connect(nid, &NotaInputDialog::rejected, nid, &NotaInputDialog::deleteLater);
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
  qtyItem->setData(nid->qty(), NumberValueRole);
  qtyItem->setData(locale().toString(nid->qty()), Qt::DisplayRole);
  
  auto priceItem = new QStandardItem();
  priceItem->setData((int) Qt::AlignRight | Qt::AlignVCenter, Qt::TextAlignmentRole);
  priceItem->setData(nid->harga(), NumberValueRole);
  priceItem->setData(locale().toString(nid->harga()), Qt::DisplayRole);
  
  auto totalItem = new QStandardItem();
  totalItem->setData((int) Qt::AlignRight | Qt::AlignVCenter, Qt::TextAlignmentRole);
  totalItem->setData(nid->harga() * nid->qty(), NumberValueRole);
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
    f += sm->index(i, 4).data(NumberValueRole).toInt();
  }
  // ui->totalLineEdit->setReadOnly(false);
  ui->totalLineEdit->setText(locale().toString(f));
  // ui->totalLineEdit->setReadOnly(true);
}

void PembuatNota::refreshKonsumen() {
  konsumenModel->setQuery(konsumenModel->query().lastQuery());
  ui->konsumenKombo->setCurrentIndex(-1);
}

void PembuatNota::on_simpan() {
  Database *db = kpw->database();
  if (ui->konsumenKombo->currentIndex() < 0) {
    QMessageBox::information(this, "Tidak dapat menyimpan", "Anda belum menentukan Konsumen Terdaftar");
    return;
  }

  QList<QString> produkList;
  QList<int> qtyList, hargaList;
  for (int i=0; i < sm->rowCount() ; ++i) {
    produkList << sm->index(i, 1).data().toString();
    qtyList << sm->index(i, 2).data(NumberValueRole).toInt();
    hargaList << sm->index(i, 3).data(NumberValueRole).toInt();
  }
  
  if (produkList.count() < 1) {
    QMessageBox::information(this, "Tidak dapat menyimpan", "Tidak ada satupun order yang dicatat");
    return;
  }
  
  auto r = db->addPenjualan(produkList, qtyList, hargaList);
  if ( r.success ) {
    auto ir = db->createInvoice(r.penjualanIds, ui->konsumenKombo->currentText());
    if (ir.success) {
      emit penjualanSaved();
      accept();
    }
  }
}

void PembuatNota::on_bayar() {
  Database *db = kpw->database();
  if (ui->konsumenKombo->currentIndex() < 0) {
    QMessageBox::information(this, "Tidak dapat menyimpan", "Anda belum menentukan Konsumen Terdaftar");
    return;
  }

  QList<QString> produkList;
  QList<int> qtyList, hargaList;
  for (int i=0; i < sm->rowCount() ; ++i) {
    produkList << sm->index(i, 1).data().toString();
    qtyList << sm->index(i, 2).data(NumberValueRole).toInt();
    hargaList << sm->index(i, 3).data(NumberValueRole).toInt();
  }

  if (produkList.count() < 1) {
    QMessageBox::information(this, "Tidak dapat menyimpan", "Tidak ada satupun order yang dicatat");
    return;
  }

  auto r = db->addPenjualan(produkList, qtyList, hargaList);
  if ( r.success ) {
    auto ir = db->createInvoice(r.penjualanIds, ui->konsumenKombo->currentText());
    if (ir.success) {
      emit penjualanSaved();
      auto pd = new PaymentDialog(ir.invoiceId, kpw->database(), kpw);
      connect(pd, &QDialog::accepted, pd, &QObject::deleteLater);
      connect(pd, &QDialog::rejected, pd, &QObject::deleteLater);
      pd->open();
      accept();
    }
  }
}