#ifndef EDITORPENJUALAN_H
#define EDITORPENJUALAN_H

#include <QDialog>

namespace Ui {
  class EditorPenjualan;
};

class Database;
class EditorPenjualan : public QDialog
{
  Q_OBJECT

public:
  explicit EditorPenjualan(int idp, Database *b, QWidget *parent=nullptr);
  ~EditorPenjualan();

public slots:
  void reject() override;

private slots:
  void on_simpanButton_clicked();

signals:
  void penjualanUpdated();

private:
  bool isModified() const;
  QSqlRecord record;
  Database* db;
  Ui::EditorPenjualan *ui;
};

#endif