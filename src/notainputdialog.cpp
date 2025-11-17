#include "notainputdialog.h"
#include "ui/ui_notainputdialog.h"
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlRecord>
#include <QCompleter>
#include <QTableView>
#include <QHeaderView>

NotaInputDialog::NotaInputDialog(QWidget *parent)
  : ui(new Ui::NotaInputDialog), produkModel(new QSqlQueryModel(this)), QDialog(parent) 
{
  ui->setupUi(this);
  produkModel->setQuery("SELECT * FROM Produk");
  auto compView = new QTableView();
  ui->comboBox->blockSignals(true);
  ui->comboBox->setModel(produkModel);
  ui->comboBox->setModelColumn(1);
  ui->comboBox->setView(compView);
  ui->comboBox->setCurrentIndex(-1);
  ui->comboBox->blockSignals(false);
  // comp->setModel(produkModel);
  // comp->setCompletionColumn(1);
  // comp->setCaseSensitivity(Qt::CaseInsensitive);
  // comp->setPopup(compView);
  compView->verticalHeader()->setMinimumSectionSize(18);
  compView->verticalHeader()->setDefaultSectionSize(18);
  compView->verticalHeader()->hide();
  compView->horizontalHeader()->hideSection(0);
  compView->horizontalHeader()->hideSection(3);
  compView->horizontalHeader()->hideSection(4);
  compView->horizontalHeader()->hideSection(5);
  compView->horizontalHeader()->hide();
  auto hh = compView->horizontalHeader();
  compView->resizeColumnsToContents();
  compView->setMinimumWidth(hh->sectionSize(1) + hh->sectionSize(2));
  compView->setSelectionBehavior(compView->SelectRows);
  hh->setStretchLastSection(true);
  
  auto cpl = new QCompleter(this);
  ui->comboBox->setCompleter(cpl);
  cpl->setModel(ui->comboBox->model());
  cpl->setCompletionColumn(1);
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
  if (ui->comboBox->currentIndex() < 0) {
    QMessageBox::warning(this, "Kesalahan Input", "Nama Produk tidak valid / belum terdaftar");
    return;
  }
  if (ui->spinBox->value() == 0) {
    QMessageBox::warning(this, "Kesalahan Input", "Penjualan dengan jumlah produk 0 tidak diperbolehkan");
    return ;
  }
  emit doneEditing();
};


void NotaInputDialog::on_comboBox_currentIndexChanged(int i)
{
  auto rc = produkModel->record(i);
  if (rc.isEmpty()) {
    return;
  }
  if (rc.value("stock").toInt() < 1 && i != -1) {
    QMessageBox::information(this, "Stok Habis", "Stok produk ini telah habis");
  }
  ui->simpanButton->setDisabled(rc.value("stock").toInt() < 1);
  ui->spinBox->setMaximum(rc.value("stock").toInt());
  ui->spinBox->setValue(1);
  ui->spinBox2->setMinimum(rc.value("base_price").toInt());
  ui->spinBox2->setValue(rc.value("base_price").toInt());
}