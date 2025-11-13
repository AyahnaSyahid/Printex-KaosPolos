#include "widgetpenjualan.h"
#include "ui/ui_widgetpenjualan.h"
#include "pembuatnota.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QHeaderView>
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
           Konsumen.id AS 'konsumen_id'
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
  // ui->harianView->horizontalHeader()->hideSection(3);
  ui->harianView->horizontalHeader()->hideSection(4);
  ui->harianView->horizontalHeader()->hideSection(5);
  ui->harianView->horizontalHeader()->hideSection(6);
  // ui->harianView->resizeColumnsToContents();
}

WidgetPenjualan::~WidgetPenjualan(){ delete ui; }

void WidgetPenjualan::on_jualButton_clicked() {
  PembuatNota *nt = new PembuatNota(kpw, this);
  // nt->setAttribute(Qt::WA_DeleteOnClose);
  connect(nt, &PembuatNota::penjualanSaved, this, &WidgetPenjualan::refreshData);
  connect(nt, &PembuatNota::accepted, nt, &PembuatNota::deleteLater);
  connect(nt, &PembuatNota::rejected, nt, &PembuatNota::deleteLater);
  nt->open();
}

void WidgetPenjualan::on_harianView_customContextMenuRequested(const QPoint& p) {}

void WidgetPenjualan::refreshData() {
  auto sm = findChild<QSqlQueryModel*>("sqlModel");
  if (sm) {
    sm->setQuery(sm->query().lastQuery());
  }
}

void WidgetPenjualan::setKaosPolosWindow(KaosPolosWindow *k) {
  kpw = k;
}

WidgetPenjualan::ModelAdapter::ModelAdapter(QObject *parent) : QSortFilterProxyModel(parent) {}

QVariant WidgetPenjualan::ModelAdapter::data(const QModelIndex& mi, int role) const {
  if(role == Qt::TextAlignmentRole) {
    switch (mi.column()) {
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