#include "updatestockdialog.h"

#include <qnamespace.h>

#include <QMessageBox>

#include "ui/ui_updatestockdialog.h"

UpdateStockDialog::UpdateStockDialog(const QString& produk, int current,
                                     QWidget* parent)
    : ui(new Ui::UpdateStockDialog), QDialog(parent) {
  ui->setupUi(this);
  ui->namaProduk->setText(produk);
  ui->namaProduk->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  ui->jumlahStok->setText(locale().toString(current));
  ui->jumlahStok->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
}

UpdateStockDialog::~UpdateStockDialog() { delete ui; }

void UpdateStockDialog::on_simpanButton_clicked() {
  QMessageBox ask(
      QMessageBox::Question, "Konfirmasi",
      QString("Sejumlah %1 Produk %2 akan ditambahkan kedalam Stock")
          .arg(ui->jumlahStok->text())
          .arg(ui->namaProduk->text()),
      QMessageBox::Yes | QMessageBox::No);
  auto bt = ask.button(QMessageBox::Yes);
  bt->setText("Ya");
  bt = ask.button(QMessageBox::No);
  bt->setText("Batalkan");

  if (QMessageBox::No == ask.exec()) return;
  emit updateStockRequest(ui->namaProduk->text(), ui->spinAddStock->value());
  ui->simpanButton->setDisabled(true);
}

void UpdateStockDialog::resultReady(const QString& name, bool ok,
                                    const QString& errmsg) {
  if (name == ui->namaProduk->text()) {
    if (ok) {
      accept();
    } else {
      QMessageBox::information(this, "Gagal menambahkan stok", errmsg);
      ui->simpanButton->setEnabled(true);
    }
  }
}
