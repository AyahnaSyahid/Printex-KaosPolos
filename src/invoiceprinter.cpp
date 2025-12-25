#include "invoiceprinter.h"
#include "database.h"

#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QPainter>

#include <QImage>
#include <QFont>
#include <QSize>
#include <QChar>
#include <QDateTime>

#include <QPrinter>
#include <QPrinterInfo>
#include <QPainter>


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
  invq.prepare("SELECT Invoice.id, "
               "total_value, "
               "paid, unpaid, "
               "Konsumen.nama AS nama, "
               "Invoice.created AS tgl "
               "FROM Invoice "
               "INNER JOIN Konsumen ON Invoice.konsumen_id = Konsumen.id "
               "WHERE Invoice.id = ?");
  invq.addBindValue(invid);
  if(!invq.exec()) {
    qDebug() << invq.lastError().text();
  }
  invq.next();
  auto invRec = invq.record();
  
  QList<QSqlRecord> penjualan;
  pnjq.prepare("SELECT Produk.nama AS produk, "
               "harga_jual, harga_total, qty "
               "FROM Penjualan "
               "INNER JOIN Produk ON Penjualan.produk_id = Produk.id "
               "WHERE invoice_id = ?");
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
  
  QFont normal("Courier New", 9);
  QFont thin("Courier New", 9);
  QFont bold("Courier New", 9);
  thin.setWeight(QFont::Thin);
  bold.setWeight(QFont::Bold);
  
  int imgHeight = 6;
  int printStampHeight = 5;
  int invStampHeight = 5;
  int invHeader = 3;
  int salesHeight = penjualan.count() * 2;
  int summaryLines = 2;
  int paymentLines = pembayaran.count() + 1;
  int resLines = 2;
  // int thanksLine = 6 + 1; // 1 margin down
  
  int sumall = imgHeight + printStampHeight +
               invStampHeight + invHeader +
               salesHeight + summaryLines +
               paymentLines + resLines;
               // thanksLine;
  
  QPrinter prt(QPrinterInfo::printerInfo("Adobe PDF"), QPrinter::HighResolution);
  if (!prt.isValid()) {
    qDebug() << "Printer not ready";
  }
  
  prt.setOutputFileName("test.pdf");
  QPageSize pageSize(QSize(227, sumall * 12), "Rolls", QPageSize::ExactMatch);
  prt.setPageSize(pageSize);
  prt.setResolution(300);
  prt.setFontEmbeddingEnabled(true);
  prt.setFullPage(true);
  
  QPainter p(&prt);
  p.save();
  p.setFont(normal);
  auto lineSpacing = p.fontMetrics().lineSpacing();
  auto pr = prt.pageRect(QPrinter::DevicePixel);
  auto imRect = QRect(0, 0, pr.width(), lineSpacing * 6); 
  QImage logo("coca-cola-logo.jpg");
  logo = logo.scaled(imRect.size(), Qt::KeepAspectRatio);
  auto logoRect = logo.rect();
  logoRect.moveCenter(imRect.center());
  p.drawImage(logoRect, logo);
  
  QRect crl(imRect.bottomLeft(), QSize(imRect.width(), lineSpacing)), cru;
  p.drawText(crl, 0, QString("*").repeated(42), &cru);
  crl.moveTop(crl.bottom());
  
  p.drawText(crl, 0, QString(" Printed : %L1")
      .arg(QLocale().toString(QDateTime::currentDateTime(), "dddd, yyyy-MM-dd HH:mm")), &cru);
  crl.moveTop(crl.bottom());
  
  p.drawText(crl, 0, QString(" ID      : %1")
      .arg(invid, 8, 10, QChar('0')), &cru);
  crl.moveTop(crl.bottom());
  
  p.drawText(crl, 0, QString(" TK/TN   : %1")
      .arg(invRec.value("nama").toString()), &cru);
  crl.moveTop(crl.bottom());
  
  p.drawText(crl, 0, QString("*").repeated(42), &cru);
  crl.moveTop(crl.bottom());
  
  p.setFont(bold);
  p.drawText(crl, 0, QString("%1")
    .arg(QString("=").repeated((42 - 9) / 2) + " INVOICE " + QString("=").repeated((42 - 9) / 2)), &cru);
  crl.moveTop(crl.bottom());
  p.setFont(normal);

  p.drawText(crl, 0, QString(" Tanggal : %1")
      .arg(QLocale().toString(invRec.value("tgl").toDateTime(), "dddd, yyyy-MM-dd")), &cru);
  crl.moveTop(crl.bottom());

  p.drawText(crl, 0, QString(" T Items : %1").arg(penjualan.count()), &cru);
  crl.moveTop(crl.bottom());
  
  p.drawText(crl, 0, " Total Harga :", &cru);
  p.setFont(bold);
  cru.moveLeft(cru.right());
  p.drawText(cru, 0, QString(" Rp. %L1")
    .arg(invRec.value("total_value").toInt()), &cru);
  crl.moveTop(crl.bottom());
  
  p.drawText(crl, 0, QString("=").repeated(42));
  crl.moveTop(crl.bottom());
  
  p.setFont(normal);
  p.drawText(crl, 0, " No. Produk");
  crl.moveTop(crl.bottom());
  p.drawText(crl, 0, "     Qty          Harga         SubTotal");
  crl.moveTop(crl.bottom());
  p.drawText(crl, 0, QString("-").repeated(42));
  crl.moveTop(crl.bottom());
  
  for( auto pj : penjualan) {
    p.drawText(crl, 0, QString(" %1. %2")
      .arg(penjualan.indexOf(pj) + 1, 2, 10, QChar('0'))
      .arg(pj.value("produk").toString()), &cru);
    crl.moveTop(crl.bottom());
    p.drawText(crl, 0, 
      QString(" %L1    x %L2    %L3")
        .arg(pj.value("qty").toInt(), 6, 10)
        .arg(pj.value("harga_jual").toInt(), 11, 10)
        .arg(pj.value("harga_total").toInt(), 12, 10),
        &cru
      );
    crl.moveTop(crl.bottom());
  }
  
  p.drawText(crl, 0, QString("=").repeated(42), &cru);
  crl.moveTop(crl.bottom());
  p.setFont(bold);
  p.drawText(crl, 0, QString("                  Total%L1")
              .arg(invRec.value("total_value").toInt(), 17, 10), &cru);
  crl.moveTop(crl.bottom());
  
  for( auto pb : pembayaran) {
    p.drawText(crl, 0, QString("    P %1%L2%L3")
        .arg(pb.value("pay_time").toDateTime().toString("yyyy-MM-dd"))
        .arg(pembayaran.indexOf(pb) + 1, 3, 10)
        .arg(pb.value("value").toInt(), 21, 10), 
    &cru);
    crl.moveTop(crl.bottom());
  }
  p.setFont(normal);
  p.drawText(crl, 0, QString("%1")
      .arg(QString("-").repeated(24), 42));
  crl.moveTop(crl.bottom());
  
  p.setFont(bold);
  p.drawText(crl, 0, QString("                   Sisa%L1")
              .arg(invRec.value("unpaid").toInt(), 17, 10), &cru);
  crl.moveTop(crl.bottom());
  p.setFont(normal);
  
  p.drawText(crl, 0, QString("*").repeated(42), &cru);
  crl.moveTop(crl.bottom());
  
  p.drawText(crl, Qt::AlignCenter, "Terimakasih", &cru);
  crl.moveTop(crl.bottom());
  
  p.drawText(crl, Qt::AlignCenter, "PRINTEX Digital Printing", &cru);
  crl.moveTop(crl.bottom());
  
  p.drawText(crl, Qt::AlignCenter, "Alamat Printex", &cru);
  crl.moveTop(crl.bottom());
  
  p.drawText(crl, Qt::AlignCenter, QString("WA : 0882836653672"), &cru);
  crl.moveTop(crl.bottom());
  
  p.drawText(crl, 0, QString("=").repeated(42), &cru);
  crl.moveTop(crl.bottom());
  
  p.restore();
  p.end();
}