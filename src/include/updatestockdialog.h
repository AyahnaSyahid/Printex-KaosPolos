#ifndef UPDATESTOCKDIALOG_H
#define UPDATESTOCKDIALOG_H

#include <QDialog>

namespace Ui {
class UpdateStockDialog;
}

class UpdateStockDialog : public QDialog {
  Q_OBJECT

 public:
  explicit UpdateStockDialog(const QString& name, int current,
                             QWidget* parent = nullptr);
  ~UpdateStockDialog();

 public slots:
  void resultReady(const QString& name, bool ok, const QString& errmsg);

 private slots:
  void on_simpanButton_clicked();

 signals:
  void updateStockRequest(const QString& produk, int addStock);

 private:
  Ui::UpdateStockDialog* ui;
};
#endif  // UPDATESTOCKDIALOG_H
