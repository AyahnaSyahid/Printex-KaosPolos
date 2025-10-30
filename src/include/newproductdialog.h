#ifndef NewProductDialog_H
#define NewProductDialog_H

#include <QDialog>

namespaces Ui {
    class NewProductDialog;
}

class NewProductDialog : public QDialog {
  Q_OBJECT

public:
  explicit NewProductDialog(QWidget *parent);
  ~NewProductDialog();

private slots:
  void on_saveButton_clicked();

signals:
  void productAdded(qint64, const QString&)

private:
  Ui::NewProductDialog* ui;
};

#endif NewProductDialog_H