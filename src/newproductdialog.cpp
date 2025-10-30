#include "newproductdialog.h"
#include "ui/ui_newproductdialog.h"


NewProductDialog::NewProductDialog(QWidget *parent) 
  : ui(new Ui::NewProductDialog), QDialog(parent)
{
  ui->setupUi(this);
}

NewProductDialog::~NewProductDialog() {
  delete ui;
}

void NewProductDialog::on_saveButton_clicked() {
  
}