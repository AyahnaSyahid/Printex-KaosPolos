#ifndef PAYMENTDIALOG_H
#define PAYMENTDIALOG_H

#include <QDialog>

namespace Ui {
  class PaymentDialog;
};

class Database;
class PaymentDialog : public QDialog
{
  Q_OBJECT

public:
  explicit PaymentDialog(int paymentId, Database* _d, QWidget* parent=nullptr);
  ~PaymentDialog();

private slots:
  void updateKembalian(int pay);
  void on_bayarButton_clicked();

public slots:
  void reject() override;

signals:
  void paymentSuccess();
  void paymentFail(const QString& reason);

private:
  void invoiceNotFound();
  Ui::PaymentDialog* ui;
  Database* db;
  int invoiceId;
  int tLunas;
  int tSisa;
  int tTotal;
};

#endif