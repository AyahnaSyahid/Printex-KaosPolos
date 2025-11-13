#include "paymentdialog.h"
#include "ui/ui_paymentdialog.h"
#include "database.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QLineEdit>

PaymentDialog::PaymentDialog(int inv, Database* _d, QWidget* parent)
  :  ui(new Ui::PaymentDialog), 
     db(_d), 
     invoiceId(inv),
     tLunas(0),
     tSisa(0),
     tTotal(0),
     QDialog(parent)
{
  ui->setupUi(this);
  QSqlQuery q;
  q.prepare("SELECT * FROM Invoice WHERE id = ?");
  q.addBindValue(invoiceId);
  ui->labelInvoice->setText(QString("%1").arg(inv, 8, 10, QChar('0')));
  auto lebox = ui->comboBox->lineEdit();
  lebox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  ui->cashBox->setContentsMargins(0, 1, 0, 1);
  if (q.exec() && q.next()) {
    tLunas = q.value("paid").toInt();
    tSisa = q.value("unpaid").toInt();
    tTotal = q.value("total_value").toInt();
    ui->labelTotal->setText(QString("Rp. %L1").arg(tTotal));
    ui->labelSisa->setText(QString("Rp. %L1").arg(tSisa));
    ui->labelTelahLunas->setText(QString("Rp. %L1").arg(tLunas));
    connect(ui->cashBox, &QSpinBox::valueChanged, this, &PaymentDialog::updateKembalian);
  } else {
    invoiceNotFound();
  }
}

PaymentDialog::~PaymentDialog() { delete ui; }

void PaymentDialog::updateKembalian(int pay) {
  if(pay < tSisa) return;
  int np = tSisa - pay;
  ui->labelKembalian->setText(QString("Rp. %L1").arg(np * -1));
}

void PaymentDialog::reject() {
  QMessageBox ask(QMessageBox::Question, "Konfirmasi", "Pembayaran belum selesai, batalkan proses pembayaran ?", QMessageBox::Yes | QMessageBox::No, this);
  auto b = ask.button(QMessageBox::Yes);
  b->setText("Ya");
  b = ask.button(QMessageBox::No);
  b->setText("Tidak");
  if (ask.exec() == QMessageBox::Yes) {
    QDialog::reject();
  }
}

void PaymentDialog::on_bayarButton_clicked() {
  if (ui->cashBox->value() == 0) {
    QMessageBox::information(this, "Pembayaran tidak sah", "Melakukan pembayaran dengan nilai uang 0 (nol)");
    return;
  }
  auto res = db->createPayment(invoiceId, ui->cashBox->value(), ui->comboBox->currentText());
  if (res.success) {
    emit paymentSuccess();
    accept();
  } else {
    emit paymentFail(res.errorMessage);
  }
}

void PaymentDialog::invoiceNotFound()
{
  QMessageBox::critical(this, "Error", QString("Invoice %1 tidak ditemukan").arg(invoiceId, 8, 10, QChar('0')));
  reject();
}