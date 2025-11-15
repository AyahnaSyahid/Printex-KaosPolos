#include "paymentdialog.h"
#include "kaospoloswindow.h"
#include "widgetinvoice.h"
#include "ui/ui_paymentdialog.h"
#include "database.h"

#include <QSqlQuery>
#include <QMessageBox>
#include <QLineEdit>

PaymentDialog::PaymentDialog(int inv, Database* _d, QWidget* parent)
  :  ui(new Ui::PaymentDialog), 
     db(_d), 
     record(),
     QDialog(parent)
{
  ui->setupUi(this);
  QSqlQuery q;
  q.prepare("SELECT * FROM Invoice WHERE id = ?");
  q.addBindValue(inv);
  ui->labelInvoice->setText(QString("%1").arg(inv, 8, 10, QChar('0')));
  auto lebox = ui->comboBox->lineEdit();
  lebox->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  ui->cashBox->setContentsMargins(0, 1, 0, 1);
  if (q.exec() && q.next()) {
    record = q.record();
    ui->labelSisa->setText(locale().toString(record.value("unpaid").toInt()));
    ui->labelTelahLunas->setText(locale().toString(record.value("paid").toInt()));
    ui->labelTotal->setText(locale().toString(record.value("total_value").toInt()));
  } else {
    invoiceNotFound();
  }
  connect(ui->cashBox, &QSpinBox::valueChanged, this, &PaymentDialog::updateKembalian);
}

PaymentDialog::~PaymentDialog() { delete ui; }

void PaymentDialog::updateKembalian(int pay) {
  if(pay < record.value("unpaid").toInt()) return;
  int np = record.value("unpaid").toInt() - pay;
  ui->labelKembalian->setText(QString("Rp. %L1").arg(np * -1));
}

void PaymentDialog::reject() {
  QMessageBox ask(QMessageBox::Question, 
    "Konfirmasi", 
    "Pembayaran belum selesai, batalkan proses pembayaran ?", 
    QMessageBox::Yes | QMessageBox::No, this);
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
  int bayar = ui->cashBox->value() > record.value("unpaid").toInt() ? record.value("unpaid").toInt() : ui->cashBox->value(); 
  auto res = db->createPayment(record.value("id").toInt(), bayar, ui->comboBox->currentText());
  if (res.success) {
    KaosPolosWindow* kpw = qobject_cast<KaosPolosWindow*>(parent());
    if (kpw) {
      WidgetInvoice* wi = kpw->findChild<WidgetInvoice*>("widgetInvoice");
      if(wi) {
        wi->refreshData();
      }
    }
    emit paymentSuccess();
    accept();
  } else {
    qDebug() << res.errorMessage;
    emit paymentFail(res.errorMessage);
  }
}

void PaymentDialog::invoiceNotFound()
{
  QMessageBox::critical(this, "Error", QString("Invoice %1 tidak ditemukan")
    .arg(record.value("id").toInt(), 8, 10, QChar('0')));
  reject();
}