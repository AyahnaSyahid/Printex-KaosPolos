#include "addproduk.h"
#include "ui/ui_addproduk.h"

AddProduk::AddProduk(QWidget *parent) 
  : ui(new Ui::AddProduk), QDialog(parent) {
  ui->setupUi(this);
}

AddProduk::~AddProduk() { delete ui; }

void AddProduk::on_saveButton_clicked() {}
