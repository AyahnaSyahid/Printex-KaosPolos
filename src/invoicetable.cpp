#include "invoicetable.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QLocale>
#include <QStyledItemDelegate>
#include <QSortFilterProxyModel>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QItemSelectionModel>

class InvoiceTable::NumberDelegate : public QStyledItemDelegate
{
  public:
    using QStyledItemDelegate::QStyledItemDelegate;
    QString displayText(const QVariant&, const QLocale&) const override;
    void initStyleOption(QStyleOptionViewItem*, const QModelIndex&) const override;
};

class InvoiceTable::InvIDDelegate : public QStyledItemDelegate
{
  public:
    using QStyledItemDelegate::QStyledItemDelegate;
    
    QString displayText(const QVariant& val, const QLocale&) const override
    { return QString("%1").arg(val.toInt(), 8, 10, QChar('0')); }
    
    void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override
    { QStyledItemDelegate::initStyleOption(option, index); 
      option->displayAlignment = Qt::AlignCenter; }
};

InvoiceTable::InvoiceTable(QWidget* parent) : QTableView(parent) 
{
  auto qm = new QSqlQueryModel(this);
  qm->setObjectName("queryModel");
  
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
  
  auto sm = new QSortFilterProxyModel(this);
  sm->setObjectName("sortModel");
  sm->setSourceModel(qm);
  setModel(sm);
  setMinimumSize(600, 300);
  
  verticalHeader()->setMinimumSectionSize(15);
  verticalHeader()->setDefaultSectionSize(18);
  horizontalHeader()->setStretchLastSection(true);
  
  setSelectionMode(QTableView::SingleSelection);
  setSelectionBehavior(QTableView::SelectRows);
  setSortingEnabled(true);
  
  auto dlg = new NumberDelegate(this);
  setItemDelegateForColumn(2,dlg);
  setItemDelegateForColumn(3,dlg);
  setItemDelegateForColumn(4,dlg);
  auto idel = new InvIDDelegate(this);
  setItemDelegateForColumn(0, idel);
}

InvoiceTable::~InvoiceTable() {}

void InvoiceTable::update()
{
  auto qm = findChild<QSqlQueryModel*>("queryModel");
  auto lq = qm->query().lastQuery();
  qm->setQuery(lq);
}

void InvoiceTable::contextMenuEvent(QContextMenuEvent *ce)
{
  auto sm = selectionModel();
  if(!sm->hasSelection()) return;
  auto mi = sm->selectedRows(0)[0];
  auto iid = mi.data(Qt::EditRole).toInt();
  QMenu cm;
  auto aedit = cm.addAction("Edit");
  auto aprint = cm.addAction("Print");
  connect(aprint, &QAction::triggered, [this, &iid](){ emit printRequest(iid); });
  connect(aedit, &QAction::triggered, [this, &iid](){ emit editRequest(iid); });
  cm.exec(ce->globalPos());
}

QString InvoiceTable::NumberDelegate::displayText(const QVariant& val, const QLocale& loc) const
{
  bool ok = false;
  int vint = val.toInt(&ok);
  if(ok) return QString("%L1").arg(vint);
  return val.toString();
}

void InvoiceTable::NumberDelegate::initStyleOption(QStyleOptionViewItem* opt, const QModelIndex& im) const
{
  QStyledItemDelegate::initStyleOption(opt, im);
  switch(im.column()) {
    case 2:
    case 3:
      opt->displayAlignment = Qt::AlignRight | Qt::AlignVCenter;
      break;
    case 0:
    case 4:
      opt->displayAlignment = Qt::AlignHCenter | Qt::AlignVCenter;
      break;
  }
}