#include "editorpenjualan.h"
#include "ui/ui_editorpenjualan.h"
#include "database.h"

#include <QSqlQuery>
#include <QMessageBox>


EditorPenjualan::EditorPenjualan(int pid, Database *b, QWidget *parent)
  : ui(new Ui::EditorPenjualan), db(b), record(), QDialog(parent)
{
  ui->setupUi(this);
  QSqlQuery q;
  q.prepare(R"-(
    SELECT Produk.nama AS namaProduk,
           Produk.stock AS produkStock,
           Penjualan.qty AS penjualanQty,
           Penjualan.harga_jual AS penjualanPrice,
           Penjualan.harga_total AS penjualanTotal,
           Produk.id AS produkId,
           Produk.base_price AS produkBasePrice,
           Invoice.id AS invoiceId,
           Invoice.total_value AS invoiceTotalValue,
           Invoice.paid AS invoicePaid,
           Invoice.unpaid AS invoiceUnpaid
      FROM Penjualan
           INNER JOIN
           Produk ON Produk.id = Penjualan.produk_id
           INNER JOIN
           Invoice ON Penjualan.invoice_id = Invoice.id
     WHERE Penjualan.id = ?;
  )-");
  q.addBindValue(pid);
  if(!q.exec() || !q.next()) {
    QMessageBox::information(this, "Kesalahan", "Penjualan tidak ditemukan");
    reject();
  }
  record = q.record();
  ui->labelNama->setText(q.value("namaProduk").toString());
  ui->qtyBox->setMinimum(1);
  ui->qtyBox->setMaximum(q.value("produkStock").toInt() + q.value("penjualanQty").toInt());
  ui->qtyBox->setValue(q.value("penjualanQty").toInt());
  ui->priceBox->setValue(q.value("penjualanPrice").toInt());
  ui->priceBox->setMinimum(q.value("produkBasePrice").toInt());
}

EditorPenjualan::~EditorPenjualan() { delete ui; }

void EditorPenjualan::reject() {
  if(isModified()) {
    QMessageBox m(QMessageBox::Question, "Belum disimpan", "Perubahan data belum disimpan, Abaikan perubahan ?", QMessageBox::Yes | QMessageBox::No, this);
    auto bt = m.button(QMessageBox::Yes);
    bt->setText("Ya");
    bt = m.button(QMessageBox::No);
    bt->setText("Tidak");
    if (m.exec() == QMessageBox::Yes) {
      return QDialog::reject();
    }
  }
}

bool EditorPenjualan::isModified() const {
  return ui->qtyBox->value() != record.value("penjualanQty").toInt() &&
         ui->priceBox->value() != record.value("penjualanPrice").toInt();
};

void EditorPenjualan::on_simpanButton_clicked() {
  if (!isModified()) {
    return accept();
  }

  auto newQty = ui->qtyBox->value(),
       newPrice = ui->priceBox->value(),
       lastTotal = record.value("penjualanTotal").toInt();
  
  if (lastTotal < newQty * newPrice) { // nilai penjualan bertambah
    if (record.value("invoiceUnpaid").toInt() == 0) { // nota sebelumnya telah lunas
      AskBox ask("Konfirmasi Perubahan", 
                 "Perubahan data penjualan ini akan mempengaruhi status lunas Nota terkait, tetap lanjutkan ?", this);
      if (ask.exec() != QMessageBox::Yes) {
        QMessageBox::information(this, "Dibatalkan", "Operasi perubahhan data penjualan dibatalkan");
        ui->qtyBox->setValue(record.value("penjualanQty").toInt());
        ui->priceBox->setValue(record.value("penjualanPrice").toInt());
        reject();
      }
    }
  }

  Transaction tr;
  QSqlQuery q;
  q.prepare(R"-(
    UPDATE Penjualan 
      SET ( harga_jual, qty, harga_total, modified) 
        = ( :hj, :qt, :ht, datetime('now', 'localtime'))
    )-");
  q.bindValue(":hj", ui->priceBox->value());
  q.bindValue(":qt", ui->qtyBox->value());
  q.bindValue(":ht", ui->qtyBox->value() * ui->priceBox->value());

};

EditorPenjualan::AskBox::AskBox(const QString& tt, const QString& det, QWidget *parent)
  : QMessageBox(QMessageBox::Question, tt, det, QMessageBox::Yes | QMessageBox::No, parent)
{
  auto bYes = button(QMessageBox::Yes);
  auto bNo = button(QMessageBox::No);

  bYes->setText("Ya");
  bNo->setText("Tidak");
}