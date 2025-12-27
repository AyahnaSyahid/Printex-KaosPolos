#ifndef INVOICEPRINTER_H
#define INVOICEPRINTER_H

#include <QObject>

class QAction;
class InvoicePrinter : public QObject
{
  Q_OBJECT

public:
  explicit InvoicePrinter(QObject *parent = nullptr);
  ~InvoicePrinter();

public slots:
  void printInvoice(int invoiceId);
  
signals:
  void printerBegin(const QString& invId);
  void printerEnd();
  void printerFailed(const QString& message);

private slots:
  void setDefaultInvoiceDir();

private:
  QString m_pdfOutputDir;
  QAction *setDefaultInvoiceDirAction;
  QAction *editCompanyInfoAction;
};

#endif