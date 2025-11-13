#include "widgetinvoice.h"
#include "ui/ui_widgetinvoice.h"

#include <QSqlQueryModel>
#include <QSqlQuery>

WidgetInvoice::WidgetInvoice(QWidget *parent)
  : ui(new Ui::WidgetInvoice), QWidget(parent)
{
  ui->setupUi(this);
  auto model = new UnpaidModel(this);
  model->setObjectName("unpaidModel");
  ui->unpaidInvoiceView->setModel(model);
}

WidgetInvoice::~WidgetInvoice() { delete ui; }

void WidgetInvoice::refreshData()
{
  auto qm = findChild<QSqlQueryModel*>("unpaidQueryModel");
  if(qm) {
    qm->setQuery(qm->query().lastQuery());
  }
}

// UnpaidModel
WidgetInvoice::UnpaidModel::UnpaidModel(QObject *parent)
  : QSortFilterProxyModel(parent)
{
  auto qm = new QSqlQueryModel(this);
  qm->setObjectName("unpaidQueryModel");
  qm->setQuery(R"-(
    SELECT Invoice.id AS [Invoice ID],
       Konsumen.nama AS Konsumen,
       Invoice.total_value AS Total,
       Invoice.unpaid AS Sisa,
       COALESCE(date(Invoice.last_payment), '') AS [Tgl Bayar]
  FROM Invoice
       INNER JOIN
       Konsumen ON Invoice.konsumen_id = Konsumen.id
 WHERE Invoice.unpaid > 0;
  )-");
  setSourceModel(qm);
}

QVariant WidgetInvoice::UnpaidModel::data(const QModelIndex& mi, int role) const {
  return QSortFilterProxyModel::data(mi, role);
}