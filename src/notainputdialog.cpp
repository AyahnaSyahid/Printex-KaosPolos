#include "notainputdialog.h"
#include "ui/ui_notainputdialog.h"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlRecord>

NotaInputDialog::NotaInputDialog(QWidget *parent)
  : ui(new Ui::NotaInputDialog), produkModel(new QSqlQueryModel(this)), QDialog(parent) 
{
  ui->setupUi(this);
  produkModel->setQuery("SELECT * FROM Produk");
  ui->comboBox->setModel(produkModel);
  ui->comboBox->setModelColumn(1);
  ui->comboBox->setCurrentIndex(-1);
}

NotaInputDialog::~NotaInputDialog() { delete ui; }

QString NotaInputDialog::namaProduk() const {
  return ui->comboBox->currentText();
}

int NotaInputDialog::qty() const {
  return ui->spinBox->value();
}

int NotaInputDialog::harga() const {
  return ui->spinBox2->value();
}

void NotaInputDialog::on_simpanButton_clicked() {
  if (ui->comboBox->currentIndex() < 0) return;
  emit doneEditing();
};


void NotaInputDialog::on_comboBox_currentIndexChanged(int i)
{
  auto rc = produkModel->record(i);
  if (rc.isEmpty()) {
    return;
  }
  ui->simpanButton->setDisabled(i < 0);
  ui->spinBox->setMaximum(rc.value("stock").toInt());
  ui->spinBox->setValue(1);
  ui->spinBox2->setMinimum(rc.value("base_price").toInt());
  ui->spinBox2->setValue(rc.value("base_price").toInt());
}