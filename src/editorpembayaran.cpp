#include "editorpembayaran.h"
#include "ui/ui_editorpembayaran.h"

#include "kaospoloswindow.h"
#include "database.h"
#include "askbox.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDateTime>

EditorPembayaran::EditorPembayaran(int id, KaosPolosWindow *k)
  : ui(new Ui::EditorPembayaran), kpw(k), pembayaran_id(id), record(), validId(false), QDialog(k)
{
  ui->setupUi(this);
  QSqlQuery q;
  q.prepare(R"-(
    SELECT Invoice.id AS iid,
           Invoice.unpaid AS unpaid,
           Pembayaran.id AS pid,
           pay_time AS tBayar,
           info,
           value
      FROM Pembayaran
      INNER JOIN Invoice ON Invoice.id = Pembayaran.invoice_id
      WHERE Pembayaran.id = ?
  )-");
  q.addBindValue(pembayaran_id);
  if(!q.exec() || !q.next()) {
    return ;
  }
  record = q.record();
  ui->labelNotaId->setText(QString("%1").arg(record.value("iid").toInt(), 8, 10, QChar('0')));
  ui->dateTimeEdit->setDateTime(record.value("tBayar").toDateTime());
  auto ju = record.value("value").toInt();
  auto unp = record.value("unpaid").toInt();
  auto infoText = record.value("info").toString();
  ui->spinBox->setMaximum(ju + unp);
  ui->spinBox->setValue(ju);
  ui->comboBox->setCurrentText(infoText);
  validId = true;
  adjustSize();
}

EditorPembayaran::~EditorPembayaran() { delete ui; }

void EditorPembayaran::reject()
{
  if(record.isEmpty()) {
    return QDialog::reject();
  }
  if (hasModified()) {
    AskBox asb("Konfirmasi", "Perubahan data belum disimpan, abaikan perubahan saat ini ?", this);
    if(asb.exec() == QMessageBox::No) {
      return ;
    }
  }
  QDialog::reject();
}

bool EditorPembayaran::hasModified() const 
{
  auto recV = record.value("value").toInt();
  auto recI = record.value("info").toString();
  auto recP = record.value("tBayar").toDateTime();
  return recV != ui->spinBox->value() || recI != ui->comboBox->currentText() || recP != ui->dateTimeEdit->dateTime();
}

void EditorPembayaran::on_simpanButton_clicked()
{
  if(!hasModified()) { return accept(); }
  
  Transaction tr;
  QSqlQuery paymentUpdater, invoiceUpdater;
  paymentUpdater.prepare("UPDATE Pembayaran SET (pay_time, info, value, modified) = (:pt, :in, :va, datetime('now', 'localtime')) WHERE id = :pid");
  paymentUpdater.bindValue(":pt", ui->dateTimeEdit->dateTime().toString("yyyy-MM-dd HH:mm:ss"));
  paymentUpdater.bindValue(":in", ui->comboBox->currentText());
  paymentUpdater.bindValue(":va", ui->spinBox->value());
  paymentUpdater.bindValue(":pid", pembayaran_id);
  
  if(!paymentUpdater.exec()) {
    if(paymentUpdater.lastError().isValid()) {
      QMessageBox::critical(this, "SQLError", paymentUpdater.lastError().text());
      return ;
    }
  }
  
  invoiceUpdater.prepare("UPDATE Invoice SET (paid, unpaid, modified) = ( paid + :pd, unpaid - :pd, datetime('now', 'localtime') ) WHERE id = :inid");
  invoiceUpdater.bindValue(":pd", ui->spinBox->value() - record.value("value").toInt());
  invoiceUpdater.bindValue(":inid", record.value("iid"));
  
  if(!invoiceUpdater.exec()) {
    if(invoiceUpdater.lastError().isValid()) {
      QMessageBox::critical(this, "SQLError", invoiceUpdater.lastError().text());
      return ;
    }
  }
  
  if(!tr.commit()) {
    QMessageBox::critical(this, "SQLError", "Database COMMIT Failed");
    return ;
  }
  
  emit pembayaranUpdated();
  QDialog::accept();
}