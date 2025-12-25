#include "invoicetable.h"
#include <QSqlQuery>
#include <QSqlQueryModel>

#include <QSortFilterProxyModel>

namespace InvoiceTableNS {
  class SortFilterProxyModel : public QSortFilterProxyModel {
    public:
      SortFilterProxyModel(QObject* parent=nullptr) : QSortFilterProxyModel(parent) {}
      ~SortFilterProxyModel() {}
      QVariant data(const QModelIndex& ix, int role=Qt::DisplayRole) const;
  };
}

using InvoiceTableNS;

InvoiceTable::InvoiceTable(QWidget* parent) : QTableView(parent) 
{
  auto qm = new QSqlQueryModel(this);
  qm->setObjectName("queryModel");
  auto sm = new SortFilterProxyModel(this);
  sm->setObjectName("sortFilterModel");
  
  qm->setQuery(R"-(
  SELECT PRINTF('%08d', Invoice.id) AS [Invoice ID],
         Konsumen.nama AS Konsumen,
         Invoice.total_value AS Total,
         Invoice.unpaid AS Sisa,
         COALESCE(date(Invoice.last_payment), '') AS [Tgl Bayar]
    FROM Invoice
         INNER JOIN
         Konsumen ON Invoice.konsumen_id = Konsumen.id
   WHERE Invoice.unpaid <= 1
   ORDER BY Invoice.id DESC;
  )-");
  
  sm->setSourceModel(qm);
  setModel(sm);
  setMinimumSize(600, 300);
}

InvoiceTable::~InvoiceTable() {}

void InvoiceTable::update()
{
  auto qm = findChild<QSqlQueryModel*>("queryModel");
  auto lq = qm->query().lastQuery();
  qm->setQuery(lq);
}

QVariant SortFilterProxyModel::data(const QModelIndex& ix, int role) 
{
  if(!ix.isValid()) return QVariant();
  
}