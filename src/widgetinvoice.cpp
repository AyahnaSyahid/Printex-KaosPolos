#include "widgetinvoice.h"
#include "ui/ui_widgetinvoice.h"

#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QHeaderView>
#include <QMenu>

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

void WidgetInvoice::on_unpaidInvoiceView_customContextMenuRequested(const QPoint& p) {
  QMenu m;
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
  switch (role) {
    case Qt::DisplayRole: {
      switch (mi.column()) {
        case 0:
          return QString("%1").arg(mapToSource(mi).data(Qt::EditRole).toInt(), 8, 10, QChar('0'));
        case 2:
        case 3:
          return QString("%L1").arg(mapToSource(mi).data(Qt::EditRole).toInt());
        default:
          return mapToSource(mi).data(role);
      }
    }
    case Qt::TextAlignmentRole: {
      switch (mi.column()) { 
        case 2:
        case 3:
          return (int) (Qt::AlignRight | Qt::AlignVCenter);
        case 0:
        case 4:
        return (int) Qt::AlignCenter;
        default:
          return mapToSource(mi).data(role);
      }
    }
  }
  return mapToSource(mi).data(role);
}
