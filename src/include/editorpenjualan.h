#ifndef EDITORPENJUALAN_H
#define EDITORPENJUALAN_H

#include <QDialog>
#include <QSqlRecord>
#include <QMessageBox>

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

  class AskBox;

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

class EditorPenjualan::AskBox : public QMessageBox {
public:
  explicit AskBox(const QString& tt, const QString& det, QWidget *parent=nullptr);
};

#endif