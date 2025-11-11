#ifndef NOTAINPUTDIALOG_H
#define NOTAINPUTDIALOG_H

#include <QDialog>
#include <QSqlQueryModel>

namespace Ui {
  class NotaInputDialog;
}

class NotaInputDialog : public QDialog
{
  Q_OBJECT

public:
  NotaInputDialog(QWidget* parent=nullptr);
  ~NotaInputDialog();
  
  QString namaProduk() const;
  int qty() const;
  int harga() const;

private slots:
  void on_simpanButton_clicked();
  void on_comboBox_currentIndexChanged(int);

signals:
  void doneEditing();

private:
  Ui::NotaInputDialog *ui;
  QSqlQueryModel *produkModel;
};

#endif