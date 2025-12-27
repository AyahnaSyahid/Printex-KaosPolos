#ifndef InvoiceTable_H
#define InvoiceTable_H

#include <QTableView>

class QAbstractItemModel;

class InvoiceTable : public QTableView
{
  Q_OBJECT
public:
  class NumberDelegate;
  class InvIDDelegate;
  InvoiceTable(QWidget* = nullptr);
  ~InvoiceTable();

public slots:
  void update();

private slots:
  void contextMenuEvent(QContextMenuEvent*) override;

signals:
  // connect(print, &QAction::triggered, [this, &iid](){ kpw->findChild<InvoicePrinter*>("invoicePrinter")->printInvoice(iid);});
  void printRequest(int iid);
  void editRequest(int iid);
};

#endif