#include "konsumeninfo.h"
#include "ui/ui_konsumeninfo.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QSqlQuery>

KonsumenInfo::KonsumenInfo(const QString& nama, QWidget *parent)
  : ui(new Ui::KonsumenInfo), m_nama(nama), QDialog(parent) 
{
  ui->setupUi(this);
  QSqlQuery q;
  q.prepare("SELECT * FROM Konsumen WHERE nama = ?");
  q.addBindValue(nama);
  if(!q.exec() || !q.next()) {
    on_konsumenNotFound();
  } else {
    m_phone = q.value("phone").toString();
    m_info = q.value("info").toString();
    ui->labelPhone->setText(m_phone);
    ui->labelInfo->setText(m_info);
  }
  ui->labelNama->setText(m_nama);
}

KonsumenInfo::~KonsumenInfo() {
  delete ui;
}

void KonsumenInfo::on_konsumenNotFound() {
  QMessageBox::critical(this, "Tidak Ditemukan", "Konsumen tidak ditemukan dalam database");
  reject();
}

void KonsumenInfo::on_simpanButton_clicked(){
  if(u_phone != "" || u_info != "" || u_nama != "")
    emit finishEditing();
}

void KonsumenInfo::on_phoneEditButton_clicked(){
  u_phone = QInputDialog::getText(this, "Ubah Telepon", "No Telepon", QLineEdit::Normal, m_phone);
  if (u_phone == m_phone) u_phone = "";
  else ui->labelPhone->setText(u_phone);
}
void KonsumenInfo::on_namaEditButton_clicked(){
  u_nama = QInputDialog::getText(this, "Ubah Nama", "Nama Konsumen", QLineEdit::Normal, m_nama);
  if (u_nama == m_nama) {
    u_nama = "";
    return;
  }
  ui->labelNama->setText(u_nama);
}

void KonsumenInfo::on_infoEditButton_clicked(){
  u_info = QInputDialog::getMultiLineText(this, "Ubah Informasi (Keterangan)", "Info Konsumen", m_info);
  if (u_info == m_info) u_info = "";
  else ui->labelInfo->setText(u_info);
}