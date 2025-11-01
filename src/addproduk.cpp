#include "addproduk.h"
#include "ui/ui_addproduk.h"
#include <QMessageBox>
#include <QVariantMap>
#include <QToolTip>


AddProduk::AddProduk(QAbstractItemModel *m, QWidget *parent) 
  : ui(new Ui::AddProduk), model(m), QDialog(parent) {
  ui->setupUi(this);
}

AddProduk::~AddProduk() { 
  model->deleteLater();
  delete ui;
}

void AddProduk::produkAdded(const QString& name, bool ok, const QString& msg)
{
  if(ui->nameEdit->text() == name) {
    if(ok) {
      QMessageBox ask(QMessageBox::Question, 
            "Lakukan lagi", 
            "Produk sudah berhasil ditambahkan,\nMasukkan Produl lainnya ?",
            QMessageBox::Yes | QMessageBox::No);
      auto yes = ask.button(QMessageBox::Yes);
      auto no = ask.button(QMessageBox::No);
      yes->setText("Ya");
      no->setText("Tidak");
      
      if(ask.exec() == QMessageBox::Yes) {
        ui->nameEdit->clear();
        ui->priceBox->setValue(0);
        ui->stockBox->setValue(0);
        ui->saveButton->setDisabled(false);
        return ;
      }
      return accept();
    } else {
    QMessageBox::warning(this, "Gagal menambahkan Produk", msg);
    ui->saveButton->setDisabled(false);
    }
  }
  // not for me
}

void AddProduk::on_saveButton_clicked()
{
  QString newName = ui->nameEdit->text();
  bool textOk = !newName.isEmpty();
  bool inputOk = ( ui->stockBox->value() > 0 && ui->priceBox->value() > 0 && textOk );
  if(!inputOk) {
    QMessageBox::information(this, "Input belum lengkap", "Harap isi parameter produk dengan benar");
    return;
  }
  
  for(int r=0; r<model->rowCount(); ++r) {
    if(model->index(r, 0).data(Qt::EditRole).toString() == newName ) {
      QMessageBox::information(this, "Duplikasi", "Nama produk ini telah digunakan");
      return;
    }
  }
  QVariantMap vm {{"nama", ui->nameEdit->text()},
                  {"stock", ui->stockBox->value()},
                  {"price", ui->priceBox->value()}};
  ui->saveButton->setDisabled(true);
  emit addProduk(vm);
}