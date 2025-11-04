#include "addkonsumen.h"

#include <QMessageBox>
#include <QSqlQuery>

#include "ui/ui_addkonsumen.h"

AddKonsumen::AddKonsumen(QWidget *parent)
    : ui(new Ui::AddKonsumen), QDialog(parent) {
  ui->setupUi(this);
}

AddKonsumen::~AddKonsumen() { delete ui; }

void AddKonsumen::on_simpanButton_clicked() {
  if (name().size() < 2) {
    QMessageBox::information(
        this, "Minimal input",
        "Nama Konsumen tidak boleh kurang dari 2 karakter");
    return;
  }
  QSqlQuery q;
  q.prepare("SELECT nama FROM Konsumen WHERE nama = ?");
  q.addBindValue(name());
  if (q.exec() && q.next()) {
    QMessageBox::information(
        this, "Duplikat", "Nama Konsumen ini telah terdaftar dalam database");
    return;
  }
  emit finishEditing();
}

QString AddKonsumen::name() const { return ui->namaEdit->text(); }
QString AddKonsumen::phone() const { return ui->teleponEdit->text(); }
QString AddKonsumen::info() const { return ui->infoEdit->toPlainText(); }
