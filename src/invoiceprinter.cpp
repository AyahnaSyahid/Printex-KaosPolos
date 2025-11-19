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
  
  QFont small("Courier New", 9);
  QFont normal("Courier New", 12);
  QFont bold("Courier New", 12);
  
  bold.setWeight(700);
  
  
  
  int lineCharCount = 42;
  
  
}