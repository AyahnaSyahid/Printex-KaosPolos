#include "addproduk.h"

#include <QMessageBox>
#include <QSqlQueryModel>
#include <QToolTip>
#include <QVariantMap>

#include "ui/ui_addproduk.h"

AddProduk::AddProduk(QWidget* parent)
    : ui(new Ui::AddProduk), model(new QSqlQueryModel(this)), QDialog(parent) {
  ui->setupUi(this);
  model->setQuery("SELECT nama FROM Produk");
}

AddProduk::~AddProduk() { delete ui; }

void AddProduk::on_saveButton_clicked() {
  QString newName = ui->nameEdit->text();
  bool textOk = !newName.isEmpty();
  bool inputOk =
      (ui->stockBox->value() > 0 && ui->priceBox->value() > 0 && textOk);
  if (!inputOk) {
    QMessageBox::information(this, "Input belum lengkap",
                             "Harap isi parameter produk dengan benar");
    return;
  }

  for (int r = 0; r < model->rowCount(); ++r) {
    if (model->index(r, 0).data(Qt::EditRole).toString() == newName) {
      QMessageBox::information(this, "Duplikasi",
                               "Nama produk ini telah digunakan");
      return;
    }
  }
  emit inputFinished();
}

QString AddProduk::nama() const { return ui->nameEdit->text(); }
int AddProduk::initialStock() const { return ui->stockBox->value(); }
int AddProduk::price() const { return ui->priceBox->value(); }
