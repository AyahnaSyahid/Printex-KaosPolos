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
};

#endif