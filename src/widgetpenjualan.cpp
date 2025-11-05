#include "widgetpenjualan.h"
#include "ui/ui_widgetpenjualan.h"
#include "pembuatnota.h"

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>

WidgetPenjualan::WidgetPenjualan(QWidget *parent)
  : ui(new Ui::WidgetPenjualan), QWidget(parent)
{
  ui->setupUi(this);
  auto sqlModel = new QSqlQueryModel(this);
  sqlModel->setObjectName("sqlModel");
  sqlModel->setQuery("SELECT * FROM Penjualan");
  auto sortModel = new QSortFilterProxyModel(this);
  sortModel->setObjectName("sortModel");
  sortModel->setSourceModel(sqlModel);
  ui->harianView->setModel(sortModel);
}

WidgetPenjualan::~WidgetPenjualan(){ delete ui; }

void WidgetPenjualan::on_jualButton_clicked() {
  PembuatNota *nt = new PembuatNota(this);
  nt->setAttribute(Qt::WA_DeleteOnClose);
  nt->open();
}

void WidgetPenjualan::on_harianView_customContextMenuRequested(const QPoint& p) {}

void WidgetPenjualan::refreshData() {
  auto sm = qobject_cast<QSqlQueryModel*>(sender());
  if (sm) {
    sm->setQuery(sm->query().lastQuery());
  }
}