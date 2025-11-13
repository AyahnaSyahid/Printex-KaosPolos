#ifndef INVOICEPRINTER_H
#define INVOICEPRINTER_H

#include <QObject>

class InvoicePrinter : public QObject
{
  Q_OBJECT

public:
  explicit InvoicePrinter(QObject *parent = nullptr);
  ~InvoicePrinter();

public slots:
  void printInvoice(int invoiceId);
  
signals:
  void printerFailed(const QString& message);
  
};

#endif