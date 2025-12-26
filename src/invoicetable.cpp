#include "invoicetable.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QLocale>
#include <QSortFilterProxyModel>
#include <QHeaderView>



InvoiceTable::InvoiceTable(QWidget* parent) : QTableView(parent) 
{
  auto qm = new QSqlQueryModel(this);
  qm->setObjectName("queryModel");
  auto sm = new SortFilterProxyModel(this);
  sm->setObjectName("sortFilterModel");
  
  qm->setQuery(R"-(
  SELECT Invoice.id AS [Invoice ID],
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
  
  verticalHeader()->setMinimumSectionSize(15);
  verticalHeader()->setDefaultSectionSize(18);
  horizontalHeader()->setStretchLastSection(true);
  
  setSelectionMode(QTableView::SingleSelection);
  setSelectionBehavior(QTableView::SelectRows);
  setSortingEnabled(true);
}

InvoiceTable::~InvoiceTable() {}

void InvoiceTable::update()
{
  auto qm = findChild<QSqlQueryModel*>("queryModel");
  auto lq = qm->query().lastQuery();
  qm->setQuery(lq);
}
