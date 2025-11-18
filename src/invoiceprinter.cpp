#include "invoiceprinter.h"
#include "database.h"

#include <QSqlRecord>
#include <QSqlQuery>
#include <QPainter>

#include <QImage>
#include <QFont>
#include <QSize>


InvoicePrinter::InvoicePrinter(QObject *parent)
  : QObject(parent)
{
  setObjectName("invoicePrinter");
}

InvoicePrinter::~InvoicePrinter() {}

void InvoicePrinter::printInvoice(int invid)
{
  Transaction tr;
  QSqlQuery invq, pnjq, pmbq;
  invq.prepare("SELECT * FROM Invoice WHERE id = ?");
  invq.addBindValue(invid);
  invq.exec();
  invq.next();
  auto invRec = invq.record();
  
  QList<QSqlRecord> penjualan;
  pnjq.prepare("SELECT * FROM Penjualan WHERE invoice_id = ?");
  pnjq.addBindValue(invid);
  pnjq.exec();
  while(pnjq.next()) {
    penjualan << pnjq.record();
  }
  
  QList<QSqlRecord> pembayaran;
  pmbq.prepare("SELECT * FROM Pembayaran WHERE invoice_id = ?");
  pmbq.addBindValue(invid);
  pmbq.exec();
  while(pmbq.next()) {
    pembayaran << pmbq.record();
  }
  
  QImage test(QSize(1000, 1000), QImage::Format_);
  test.setDotsPerMeterX(11811);
  test.setDotsPerMeterY(11811);
  
  QPainter ptr(&test);
  auto metrics = ptr.fontMetrics();
  
}