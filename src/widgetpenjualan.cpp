#include "widgetpenjualan.h"
#include "ui/ui_widgetpenjualan.h"
#include "pembuatnota.h"
#include "database.h"
#include "widgetinvoice.h"
#include "kaospoloswindow.h"
#include "editorpenjualan.h"
#include "askbox.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QSortFilterProxyModel>
#include <QDebug>

WidgetPenjualan::WidgetPenjualan(QWidget *parent)
  : ui(new Ui::WidgetPenjualan), kpw(nullptr), QWidget(parent)
{
  ui->setupUi(this);
  auto sqlModel = new QSqlQueryModel(this);
  sqlModel->setObjectName("sqlModel");
  sqlModel->setQuery(R"-(
    SELECT Produk.nama AS 'Produk', 
           Penjualan.qty AS 'Qty', 
           Penjualan.harga_total AS 'Nilai', 
           Konsumen.nama AS 'Konsumen',
           Invoice.id AS 'Invoice ID',
           Produk.id AS 'produk_id',
           Konsumen.id AS 'konsumen_id',
           Penjualan.id AS 'penjualan_id'
    FROM Penjualan 
    INNER JOIN Konsumen ON Konsumen.id = Invoice.konsumen_id
    INNER JOIN Produk ON Penjualan.produk_id = Produk.id 
    INNER JOIN Invoice ON Penjualan.invoice_id = Invoice.id
    WHERE date(Penjualan.sale_time) = date('now', 'localtime')
  )-");
  auto sortModel = new ModelAdapter(this);
  sortModel->setObjectName("sortModel");
  sortModel->setSourceModel(sqlModel);
  ui->harianView->setModel(sortModel);
  ui->harianView->horizontalHeader()->setStretchLastSection(true);
  ui->harianView->horizontalHeader()->hideSection(4);
  ui->harianView->horizontalHeader()->hideSection(5);
  ui->harianView->horizontalHeader()->hideSection(6);
  ui->harianView->horizontalHeader()->hideSection(7);
  // ui->harianView->setSortingEnabled(true);
}

WidgetPenjualan::~WidgetPenjualan(){ delete ui; }

void WidgetPenjualan::setDatabase(Database *d) { db = d; }

void WidgetPenjualan::on_jualButton_clicked() {
  PembuatNota *nt = new PembuatNota(kpw, this);
  // nt->setAttribute(Qt::WA_DeleteOnClose);
  connect(nt, &PembuatNota::penjualanSaved, this, &WidgetPenjualan::refreshData);
  if(kpw) {
    auto wi = kpw->findChild<WidgetInvoice*>("widgetInvoice");
    if (wi) {
      connect(nt, &PembuatNota::penjualanSaved, wi, &WidgetInvoice::refreshData);
    }
  }
  connect(nt, &PembuatNota::accepted, nt, &PembuatNota::deleteLater);
  connect(nt, &PembuatNota::rejected, nt, &PembuatNota::deleteLater);
  nt->open();
}

void WidgetPenjualan::on_harianView_customContextMenuRequested(const QPoint& p) {
  QMenu menuPenjualan;
  auto editAct = menuPenjualan.addAction("Edit");
  auto hapusAct = menuPenjualan.addAction("Hapus");
  auto at = ui->harianView->indexAt(p);
  if(at.isValid()) {
    auto sortModel = dynamic_cast<ModelAdapter*>(ui->harianView->model());
    auto realIx = sortModel->mapToSource(at);
    connect(editAct, &QAction::triggered, [this, &realIx](){ editPenjualan(realIx.row()); });
    connect(hapusAct, &QAction::triggered, [this, &realIx](){ hapusPenjualan(realIx.row()); });
  }
  menuPenjualan.exec(ui->harianView->viewport()->mapToGlobal(p));
}

void WidgetPenjualan::refreshData() {
  auto sm = findChild<QSqlQueryModel*>("sqlModel");
  if (sm) {
    sm->setQuery(sm->query().lastQuery());
  }
}

void WidgetPenjualan::editPenjualan(int r) {
  auto sm = findChild<QSqlQueryModel*>("sqlModel");
  auto ep = new EditorPenjualan(sm->record(r).value("penjualan_id").toInt(), kpw->database(), kpw);
  WidgetInvoice* wi = kpw->findChild<WidgetInvoice*>("widgetInvoice");
  connect(ep, &EditorPenjualan::penjualanUpdated, this, &WidgetPenjualan::refreshData);
  if (wi) {
    connect(ep, &EditorPenjualan::penjualanUpdated, wi, &WidgetInvoice::refreshData);
  }
  connect(ep, &QDialog::accepted, ep, &QObject::deleteLater);
  connect(ep, &QDialog::rejected, ep, &QObject::deleteLater);
  ep->open();
}

void WidgetPenjualan::hapusPenjualan(int r) {
  auto sm = findChild<QSqlQueryModel*>("sqlModel");
  auto pid = sm->record(r).value("penjualan_id").toInt();
  auto invId = sm->record(r).value("Invoice ID").toInt();
  auto db = kpw->database();
  auto pids = db->penjualanUntukNota(invId);

  Transaction tr;
  
  QSqlQuery dp;
  dp.prepare("DELETE FROM Penjualan WHERE id = ?");
  dp.addBindValue(pid);
  
  if(!dp.exec()) {
    QMessageBox::information(this, "Gagal menghapus", "Penjualan tidak dapat dihapuskan");
    return;
  }

  // Kembalikan Stock
  QSqlQuery srep;
  srep.prepare("UPDATE Produk SET stock = stock + :st WHERE id = :prid");
  srep.bindValue(":st", sm->record(r).value("Qty"));
  srep.bindValue(":prid", sm->record(r).value("produk_id"));
  
  if (!srep.exec()) {
    QMessageBox::information(this, "Update Gagal", "Gagal menambahkan kembali stock produk");
    return ;
  }

  if (pids.size() <= 1) {
    AskBox ab("Konfirmasi", "Nota terkait penjualan ini akan ikut terhapus.\n"
              "Hapus dan lanjutkan ?", this);
    if (ab.exec() == QMessageBox::No) { 
      return;
    }

    // Delete payments
    QSqlQuery pdel;
    pdel.prepare("DELETE FROM Pembayaran WHERE invoice_id = ?");
    pdel.addBindValue(invId);
    
    if(!pdel.exec()) {
      QMessageBox::information(this, "Gagal menghapus", "Pembayaran terkait tidak dapat dihapuskan");
      return;
    }

    // Delete invoice
    QSqlQuery iu;
    iu.prepare("DELETE FROM Invoice WHERE id = ?");
    iu.addBindValue(invId);

    if(!iu.exec()) {
      QMessageBox::information(this, "Gagal menghapus", "Nota terkait tidak dapat dihapuskan");
      return;  
    }

  } else {
    // just Update Invoice, we Dont need to delete
    QSqlQuery chk;
    chk.prepare("SELECT * FROM Invoice WHERE id = ?");
    chk.addBindValue(invId);
    chk.exec() && chk.next();
    auto irec = chk.record();
    if (irec.value("unpaid").toInt() < sm->record(r).value("Nilai").toInt()) {
      QMessageBox::information(this, "Tidak dapat dilakukan", 
        "Kelebihan bayar tidak dapat dihindari, hapus sebagian data pembayaran yang telah dilakukan");
      return ;
    }

    QSqlQuery cup;
    cup.prepare("UPDATE Invoice SET unpaid = unpaid - :up WHERE id = :inv");
    cup.bindValue(":up", sm->record(r).value("Nilai"));
    cup.bindValue(":inv", invId);
    if(!cup.exec()) {
      QMessageBox::information(this, "Gagal Menghapus", "Tidak dapat mengupdated data Invoice");
      return;
    }
  }

  if(!tr.commit()) {
    QMessageBox::information(this, "Gagal Menghapus", "Kegagalan terjadi saat hendak commit transaksi");
    return ;
  }

  WidgetInvoice* wi = kpw->findChild<WidgetInvoice*>("widgetInvoice");
  if (wi) {
    wi->refreshData();
  }
  refreshData();
}

void WidgetPenjualan::setKaosPolosWindow(KaosPolosWindow *k) {
  kpw = k;
}

WidgetPenjualan::ModelAdapter::ModelAdapter(QObject *parent) : QSortFilterProxyModel(parent) {}

QVariant WidgetPenjualan::ModelAdapter::data(const QModelIndex& mi, int role) const {
  if(role == Qt::TextAlignmentRole) {
    switch (mi.column()) {
      case 0:
        return Qt::AlignCenter;
      case 1:
      case 2:
      case 4:
        return (int) Qt::AlignRight | Qt::AlignVCenter;
      default :
        return QVariant();
    }
  } else if (role == Qt::DisplayRole) {
    auto va = QSortFilterProxyModel::data(mi, Qt::EditRole);
    switch (mi.column()) {
      case 1:
      case 2:
      case 4:
        return QLocale().toString(va.toInt());
      default :
        return va;
    }
  }
  return QSortFilterProxyModel::data(mi, role);
};