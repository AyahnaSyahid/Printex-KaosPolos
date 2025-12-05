#ifndef EDITORINVOICE_H
#define EDITORINVOICE_H

#include <QDialog>
#include <QSqlRecord>

namespace Ui
{
  class EditorInvoice; 
} // namespace Ui

class KaosPolosWindow;
class Database;
class QStandardItemModel;
class EditorInvoice : public QDialog
{
  Q_OBJECT
public:
  explicit EditorInvoice(int invoiceId, KaosPolosWindow *k, QWidget *parent=nullptr);
  ~EditorInvoice();

private slots:
  void invoiceNotFound();
  void fetchRecords();
  void initModelRecords(); // bisa digunakan untuk mereset
  void editPenjualan(int id);
  void editPembayaran(int id);
  void on_jualView_doubleClicked(const QModelIndex& ix);
  void on_jualView_customContextMenuRequested(const QPoint &p);
  void on_bayarView_doubleClicked(const QModelIndex& ix);
  void on_bayarView_customContextMenuRequested(const QPoint &p);
  void hapusPenjualan(int pid);

public slots:

signals:
  void produkUpdated();
  void paymentUpdated();

private:
  QStandardItemModel *penjualanModel;
  QStandardItemModel *pembayaranModel;
  QList<QSqlRecord> penjualan;
  QList<QSqlRecord> pembayaran;
  QSqlRecord invRec;
  Ui::EditorInvoice *ui;
  Database *db;
  KaosPolosWindow *kpw;
};

#endif