#ifndef PEMBUATNOTA_H
#define PEMBUATNOTA_H

#include <QDialog>

namespace Ui {
class PembuatNota;
}
class KaosPolosWindow;
class QSqlQueryModel;
class QStandardItemModel;
class DockKonsumen;
class PembuatNota : public QDialog {
  Q_OBJECT
 public:
  explicit PembuatNota(KaosPolosWindow* k, QWidget *p);
  ~PembuatNota();
  
  enum Role {
    NumberValueRole = Qt::UserRole + 1
  };
  
 public slots:
  void refreshKonsumen();
  void addOrder();

 private slots:
  void on_konsumenKombo_customContextMenuRequested(const QPoint &p);
  void on_notaTable_customContextMenuRequested(const QPoint &p);
  void on_simpan();
  void on_bayar();
  void createKonsumen();
  void processInputDialog();
  void updateGrandTotal();

signals:
  void penjualanSaved();

 private:
  Ui::PembuatNota *ui;
  KaosPolosWindow *kpw;
  DockKonsumen *dc;
  QStandardItemModel *sm;
  QSqlQueryModel *konsumenModel;
};

#endif  // PEMBUATNOTA_H
