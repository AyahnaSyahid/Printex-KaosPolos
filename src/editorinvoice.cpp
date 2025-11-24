#include "editorinvoice.h"
#include "ui/ui_editorinvoice.h"

#include "kaospoloswindow.h"
#include "database.h"
#include "widgetpenjualan.h"
#include "widgetinvoice.h"
#include "dockproduk.h"
#include "editorpenjualan.h"
#include "editorpembayaran.h"

#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QStandardItem>


using AskBox = EditorPenjualan::AskBox;

EditorInvoice::EditorInvoice(int invId, KaosPolosWindow *k, QWidget *parent)
: ui(new Ui::EditorInvoice), 
  kpw(k), db(k->database()), 
  penjualanModel(new QStandardItemModel(this)), 
  pembayaranModel(new QStandardItemModel(this)),
  penjualan(),
  pembayaran(),
  invRec(),
  QDialog(parent)
{
  ui->setupUi(this);
  setProperty("currentInvoiceId", invId);
  
  QSqlQuery ql;
  ql.prepare(R"-(
      SELECT Invoice.id AS [INVID], 
             Konsumen.nama AS [Konsumen],
             Invoice.paid AS [Paid],
             Invoice.unpaid AS [Unpaid],
             Invoice.total_value AS [Nilai]
        FROM Invoice INNER JOIN Konsumen ON Invoice.konsumen_id = Konsumen.id 
       WHERE Invoice.id = ?)-");
  
  ui->labelNotaId->setText(QString("%1").arg(invId, 8, 10, QChar('0')));
  ql.addBindValue(invId);
  ui->jualView->setModel(penjualanModel);
  ui->bayarView->setModel(pembayaranModel);

  if(ql.exec() && ql.next()) {
    invRec = ql.record();
    ui->labelKonsumen->setText(invRec.value("Konsumen").toString());
    ui->labelNilai->setText(QString("%L1").arg(invRec.value("Nilai").toInt()));
    ui->labelTerbayar->setText(QString("%L1").arg(invRec.value("Paid").toInt()));
    ui->labelSisa->setText(QString("%L1").arg(invRec.value("Unpaid").toInt()));

    WidgetPenjualan *wp= kpw->findChild<WidgetPenjualan*>("widgetPenjualan");
    WidgetInvoice *wi= kpw->findChild<WidgetInvoice*>("widgetInvoice");
    DockProduk *dp = kpw->findChild<DockProduk*>("dockProduk");
    if (wp) {
      connect(this, &EditorInvoice::produkUpdated, wp, &WidgetPenjualan::refreshData);
    }
    if (wi) {
      connect(this, &EditorInvoice::produkUpdated, wi, &WidgetInvoice::refreshData);
    }
    if(dp) {
      connect(this, &EditorInvoice::produkUpdated, dp, &DockProduk::refreshModel);
    }
    fetchRecords();
    initModelRecords();
  } else {
    invoiceNotFound();
  }
}

EditorInvoice::~EditorInvoice() { delete ui; }

void EditorInvoice::initModelRecords() 
{
  penjualanModel->clear();
  penjualanModel->setColumnCount(6);
  penjualanModel->setHorizontalHeaderLabels(QString("ID Tgl Produk Qty Harga Total").split(" "));
  for(auto rc : penjualan) {
    QList<QStandardItem*> columns;
    for(int c=0; c<rc.count(); ++c) {
      auto va = rc.value(c);
      QStandardItem* si = nullptr;
      switch (c) {
        case 0: { // ID
          si = new QStandardItem(va.toString());
          si->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
          break;
        }
        case 3:   // Qty
        case 4:   // Qty
        case 5: { // Harga
          si = new QStandardItem(QString("%L1").arg(va.toInt()));
          if (c == 4 || c == 5)
            si->setText(QString("Rp. %L1").arg(va.toInt()));
          si->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
          break;
        }
        case 1: { // Date
          si = new QStandardItem(va.toDateTime().toString("yyyy-MM-dd"));
          si->setToolTip(va.toString());
          si->setTextAlignment(Qt::AlignCenter);
          break;
        }
        default:
          si = new QStandardItem(va.toString());
      }
      // si->setEditable(false);
      columns << si;
    }
    penjualanModel->appendRow(columns);
  }
  ui->jualView->resizeColumnsToContents();
  
  pembayaranModel->clear();
  pembayaranModel->setColumnCount(4);
  pembayaranModel->setHorizontalHeaderLabels(QString("ID;Tgl;Nilai;Info").split(";"));
  
  for(auto rc : pembayaran) {
    QList<QStandardItem*> columns;
    for(int c=0; c<rc.count(); ++c) {
      auto va = rc.value(c);
      auto si = new QStandardItem();
      switch (c) {
        case 0: { // id
          si->setText(va.toString());
          si->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
          break;
        }
        case 1: {// tgl
          si->setText(va.toDateTime().toString("yyyy-MM-dd"));
          si->setToolTip(va.toString());
          si->setTextAlignment(Qt::AlignCenter);
          break;
        }
        case 2: { // nilai
          si->setText(QString("Rp. %L1").arg(va.toInt()));
          si->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
          break;
        }
        default: {
          si->setText(va.toString());
        }
      }
      columns << si;
    }
    pembayaranModel->appendRow(columns);
  }
  ui->bayarView->resizeColumnsToContents();
}

void EditorInvoice::invoiceNotFound() {
  QMessageBox::critical(this, "Error", QString("Data Invoice tidak ditemukan : %1").arg(property("currentInvoiceId").toInt()));
  reject();
}

void EditorInvoice::fetchRecords() {
    
    penjualan.clear();
    pembayaran.clear();
    
    Transaction tr;
    
    int invId = property("currentInvoiceId").toInt();
    QSqlQuery ql;
    ql.prepare(R"-(
    SELECT Invoice.id AS [INVID], 
           Konsumen.nama AS [Konsumen],
           Invoice.paid AS [Paid],
           Invoice.unpaid AS [Unpaid],
           Invoice.total_value AS [Nilai]
      FROM Invoice INNER JOIN Konsumen ON Invoice.konsumen_id = Konsumen.id 
     WHERE Invoice.id = ?)-");
    ql.addBindValue(invId);
    ql.exec() && ql.next();
    
    invRec = ql.record();
    
    ui->labelKonsumen->setText(invRec.value("Konsumen").toString());
    ui->labelNilai->setText(QString("%L1").arg(invRec.value("Nilai").toInt()));
    ui->labelTerbayar->setText(QString("%L1").arg(invRec.value("Paid").toInt()));
    ui->labelSisa->setText(QString("%L1").arg(invRec.value("Unpaid").toInt()));
    
    QSqlQuery saleQuery, paymentQuery;
    saleQuery.prepare(R"-(
      SELECT Penjualan.id AS [ID],
             Penjualan.sale_time AS [Tgl],
             Produk.nama AS [Produk],
             Penjualan.qty AS [Qty],
             Penjualan.harga_jual AS [Harga],
             Penjualan.harga_total AS [Total]
        FROM Penjualan INNER JOIN Produk ON Penjualan.produk_id = Produk.id
       WHERE Penjualan.invoice_id = :iid
      )-");
    saleQuery.bindValue(":iid", invId);
    saleQuery.exec();
    
    auto rc = saleQuery.record();
    penjualanModel->setColumnCount(rc.count());
    for(int c=0; c<rc.count(); ++c) {
      penjualanModel->setHeaderData(c, Qt::Horizontal, rc.fieldName(c), Qt::DisplayRole);
    }
    
    while(saleQuery.next()) {
      penjualan << saleQuery.record();
    }
    
    
    paymentQuery.prepare(R"-(
      SELECT id AS [ID],
             pay_time AS [Tgl],
             value AS [Nilai],
             info AS [Info]
        FROM Pembayaran
       WHERE invoice_id = :iid
    )-");
    paymentQuery.bindValue(":iid", invId);
    paymentQuery.exec();
    
    
    rc = paymentQuery.record();
    pembayaranModel->setColumnCount(rc.count());
    for(int c=0; c<rc.count(); ++c) {
      pembayaranModel->setHeaderData(c, Qt::Horizontal, rc.fieldName(c), Qt::DisplayRole);
    }
    while(paymentQuery.next()) {
      pembayaran << paymentQuery.record();
    }
}

void EditorInvoice::on_jualView_doubleClicked(const QModelIndex& ix) {
  editPenjualan(ix.siblingAtColumn(0).data().toInt());
}

void EditorInvoice::on_jualView_customContextMenuRequested(const QPoint &p) {
  QMenu jv;
  auto editAct = jv.addAction("Edit");
  auto delAct = jv.addAction("Hapus");
  auto ix = ui->jualView->indexAt(p);
  if(ix.isValid()) {
    int id = ix.siblingAtColumn(0).data().toInt();
    auto act = jv.exec(ui->jualView->viewport()->mapToGlobal(p));
    if (act == editAct) {
      editPenjualan(id);
    } else if (act == delAct) {
      hapusPenjualan(id);
    }
  }
}

void EditorInvoice::on_bayarView_customContextMenuRequested(const QPoint &p) {
  auto wi = kpw->findChild<WidgetInvoice*>("widgetInvoice");
  auto wp = kpw->findChild<WidgetPenjualan*>("widgetPenjualan");
  QMenu bv;
  auto editAct = bv.addAction("Edit");
  auto delAct = bv.addAction("Hapus");
  
  auto ix = ui->bayarView->indexAt(p);
  if(ix.isValid()) {
    int id = ix.siblingAtColumn(0).data().toInt();
    auto act = bv.exec(ui->bayarView->viewport()->mapToGlobal(p));
    if(act == editAct) {
      auto ep = new EditorPembayaran(id, kpw);
      if(!ep->isValid()) {
        QMessageBox::critical(kpw, "Kesalahan", QString("Pembayaran dengan ID : %1 tidak ditemukan").arg(id));
        ep->deleteLater();
        return ;
      }
      connect(ep, &EditorPembayaran::pembayaranUpdated, [this]() {
        fetchRecords();
        initModelRecords();
      });
      auto wi = kpw->findChild<WidgetInvoice*>("widgetInvoice");
      auto wp = kpw->findChild<WidgetPenjualan*>("widgetPenjualan");
      connect(ep, &EditorPembayaran::pembayaranUpdated, wi, &WidgetInvoice::refreshData);
      connect(ep, &EditorPembayaran::pembayaranUpdated, wp, &WidgetPenjualan::refreshData);
      connect(ep, &QDialog::accepted, ep, &QObject::deleteLater);
      connect(ep, &QDialog::rejected, ep, &QObject::deleteLater);
      ep->open();
    } else if (act == delAct) {
      AskBox asb("Konfirmasi", "Menghapus pembayaran mungkin akan mengubah status lunas invoice terkait, proses init tidak dapat dipulihkan.\nLanjutkan menghapus", kpw);
      if(asb.exec() == QMessageBox::No) {
        return ;
      }
      auto res = kpw->database()->removePayment(id);
      if(!res.success) {
        QMessageBox::information(this, "Gagal", res.errorMessage);
        return ;
      }
      wi->refreshData();
      wp->refreshData();
      fetchRecords();
      initModelRecords();
    }
  }
}


void EditorInvoice::hapusPenjualan(int pid) {
  auto wi = kpw->findChild<WidgetPenjualan*>("widgetPenjualan");
  if(wi->hapusPenjualan(pid)) {
    fetchRecords();
    initModelRecords();
    if(penjualan.count() < 1) {
      accept();
    }
    return ;
  }
}

void EditorInvoice::editPenjualan(int pjid)
{
  auto ep = new EditorPenjualan(pjid, kpw->database(), this);
  connect(ep, &EditorPenjualan::penjualanUpdated, this, &EditorInvoice::produkUpdated);
  connect(ep, &EditorPenjualan::accepted,
      [=]() { 
          fetchRecords();
          initModelRecords(); });
  ep->open();
}