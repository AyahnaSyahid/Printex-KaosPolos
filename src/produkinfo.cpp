#include "produkinfo.h"
#include "ui/ui_produkinfo.h"

#include "database.h"
#include <QString>
#include <QSqlQuery>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>

ProdukInfo::ProdukInfo(const QString& _nama, Database *_d, QWidget *parent)
    : ui(new Ui::ProdukInfo), cnama(_nama), db(_d), QDialog(parent)
{
  ui->setupUi(this);
  QSqlQuery q;
  q.prepare("SELECT * FROM Produk WHERE nama = ?");
  q.addBindValue(_nama);
  
  addStock = 0;
  if(!q.exec() || !q.next()) {
    ui->labelNama->setText("Tidak ditemukan");
    ui->labelHarga->setText("0");
    ui->labelStok->setText("0");
    on_produkNotFound();
  } else {
    cnama = q.value("nama").toString();
    charga = q.value("base_price").toInt();
    cstock = q.value("stock").toInt();
    nnama = cnama;
    nharga = charga;
    ui->labelNama->setText(cnama);
    ui->labelHarga->setText(locale().toString(charga));
    ui->labelStok->setText(locale().toString(cstock));
  }
  QSqlQuery q2;
  q2.prepare("SELECT SUM(qty) FROM Penjualan WHERE produk_id = ?");
  q2.addBindValue(q.value("id"));
  q2.exec() && q2.next();
  ui->sold->setText(locale().toString(q2.value(0).toInt()));
}

ProdukInfo::~ProdukInfo() {
  delete ui;
}

void ProdukInfo::on_produkNotFound() {
  QMessageBox::critical(this, "Kesalahan", "Produk tidak ditemukan");
  reject();
}

void ProdukInfo::on_ubahNamaButton_clicked() {
  bool ok = false;
  QString res = QInputDialog::getText(this, "Ubah nama", "Nama Baru", QLineEdit::Normal, nnama, &ok);
  if(!res.isEmpty() && ok) {
    nnama = res.trimmed();
    ui->labelNama->setText(nnama);
    return ;
  }
}

void ProdukInfo::on_ubahHargaButton_clicked() {
  bool ok = false;
  int res = QInputDialog::getInt(this, "Ubah harga jual", "Harga baru", charga, 0, 999999, 1000, &ok);
  if(ok && res != charga) {
    nharga = res;
    ui->labelHarga->setText(locale().toString(res));
    return;
  }
}

void ProdukInfo::on_tambahStockButton_clicked() {
  bool ok = false;
  int res = QInputDialog::getInt(this, "Tambah Stock", "Barang masuk", 0, 0, 999999, 10, &ok);
  if(ok && res > 0) {
    ui->labelStok->setText(locale().toString(cstock + res));
    addStock = res;
    return;
  }
}

void ProdukInfo::on_simpanButton_clicked() {
  Transaction tr;
  if ( nnama != cnama || nharga != charga) {
    QSqlQuery q1;
    q1.prepare("UPDATE Produk SET (nama, base_price, modified) = (:nn, :bp, datetime('now', 'localtime')) WHERE nama = :nm");
    q1.bindValue(":nn",nnama);
    q1.bindValue(":bp",nharga);
    q1.bindValue(":nm",cnama);
    if(!q1.exec()) {
      QMessageBox::warning(this, "Update Gagal", "Tidak dapat mengupdate data produk");
      return ;
    }
  }
  
  if (addStock != 0) {
    QSqlQuery q2;
    q2.prepare("UPDATE Produk SET (stock, modified) = (stock + :st, datetime('now', 'localtime')) WHERE nama = :nn");
    q2.bindValue(":st", addStock);
    q2.bindValue(":nn", nnama);
    if(!q2.exec()) {
      QMessageBox::warning(this, "Update Gagal", "Gagal menambahkan Stock");
      return ;
    }
  }
  
  if(!tr.commit()) {
    QMessageBox::warning(this, "Gagal menyimpan data", "COMMIT Database Failed");
    return ;
  }
  
  emit produkUpdated();
  accept();
}