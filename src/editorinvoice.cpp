#include "editorinvoice.h"
#include "ui/ui_editorinvoice.h"

#include "kaospoloswindow.h"
#include "database.h"
#include "widgetpenjualan.h"
#include "widgetinvoice.h"
#include "dockproduk.h"

#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlQueryModel>

class ProxyPenjualan : public EditorInvoice::ModelProxy
{
public:
  ProxyPenjualan(QObject *parent=nullptr) : EditorInvoice::ModelProxy(parent) {}
  QVariant data(const QModelIndex &mi, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &mi) const override;
};

class ProxyPembayaran : public EditorInvoice::ModelProxy
{
public:
  ProxyPembayaran(QObject *parent=nullptr) : EditorInvoice::ModelProxy(parent) {}
  QVariant data(const QModelIndex &mi, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &mi) const override;
};

EditorInvoice::EditorInvoice(int invId, KaosPolosWindow *k, QWidget *parent)
: ui(new Ui::EditorInvoice), kpw(k), db(k->database()), QDialog(parent)
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

  ql.addBindValue(invId);
  if(ql.exec() && ql.next()) {
    auto invRec = ql.record();
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
      connect(this, &EditorInvoice::produkUpdated, wp, &WidgetPenjualan::refreshData);
    }
    if(dp) {
      connect(this, &EditorInvoice::produkUpdated, dp, &DockProduk::refreshModel);
    }

    auto saleModel = new QSqlQueryModel(this);
    saleModel->setObjectName("saleModel");
    auto paymentModel = new QSqlQueryModel(this);
    paymentModel->setObjectName("paymentModel");
    
    QSqlQuery qs;
    qs.prepare(R"-(
      SELECT Produk.nama AS [Produk],
            Penjualan.qty AS [Qty],
            Penjualan.harga_jual AS [Harga],
            Penjualan.sale_time AS [Tgl],
            Penjualan.id AS [ID]
        FROM Penjualan INNER JOIN Produk ON Penjualan.produk_id = Produk.id
      WHERE Penjualan.invoice_id = :iid
      )-");
    qs.bindValue(":iid", invId);
    qs.exec();
    saleModel->setQuery(std::move(qs));

    QSqlQuery qp;
    qp.prepare(R"-(
      SELECT id AS [ID],
            pay_time AS [Tgl],
            value AS [Nilai],
            info AS [Info]
        FROM Pembayaran
      WHERE invoice_id = :iid
    )-");
    qp.bindValue(":iid", invId);
    qp.exec();
    paymentModel->setQuery(std::move(qp));

    auto pp = new ProxyPenjualan(this);
    auto pb = new ProxyPembayaran(this);
    pp->setSourceModel(saleModel);
    pb->setSourceModel(paymentModel);

    ui->jualView->setModel(pp);
    ui->jualView->resizeColumnsToContents();
    ui->bayarView->setModel(pb);
    ui->bayarView->resizeColumnsToContents();
    
    ui->labelNotaId->setText(QString("%1").arg(invId, 8, 10, QChar('0')));
  } else {
    invoiceNotFound();
  }
}

EditorInvoice::~EditorInvoice() { delete ui; }

void EditorInvoice::invoiceNotFound() {
  QMessageBox::critical(this, "Error", QString("Data Invoice tidak ditemukan : %1").arg(property("currentInvoiceId").toInt()));
  reject();
}

QVariant ProxyPembayaran::data(const QModelIndex &mi, int role) const
{
  switch (role) {
    case Qt::DisplayRole: {
      switch (mi.column()) {
        case 2:
          return QString("%L1").arg(mapToSource(mi).data(Qt::EditRole).toInt());
        default :
          return mapToSource(mi).data(role);
      }
    }
    case Qt::TextAlignmentRole: {
      switch (mi.column()) {
        case 0:
        case 2:
          return (int) (Qt::AlignRight | Qt::AlignVCenter);
        case 1:
          return (int) (Qt::AlignHCenter | Qt::AlignVCenter);
        default:
          return mapToSource(mi).data(role);
      }
    }
    default:
      return mapToSource(mi).data(role);
  }
}

Qt::ItemFlags ProxyPembayaran::flags(const QModelIndex &mi) const
{
  return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant ProxyPenjualan::data(const QModelIndex &mi, int role) const {
  switch (role) {
    case Qt::DisplayRole: {
      switch (mi.column()) {
        case 1:
        case 2:
        case 4:
          return QString("%L1").arg(mapToSource(mi).data(Qt::EditRole).toInt());
        default :
          return mapToSource(mi).data(role);
      }
    }
    case Qt::TextAlignmentRole: {
      switch (mi.column()) {
        case 1:
        case 2:
        case 4:
          return (int) (Qt::AlignRight | Qt::AlignVCenter);
        case 3:
          return (int) (Qt::AlignHCenter | Qt::AlignVCenter);
        default:
          return mapToSource(mi).data(role);
      }
    }
    default:
      return mapToSource(mi).data(role);
  }
}

Qt::ItemFlags ProxyPenjualan::flags(const QModelIndex &mi) const {
  return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}