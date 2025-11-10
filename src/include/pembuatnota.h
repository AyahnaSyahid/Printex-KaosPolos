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

 public slots:
  void refreshKonsumen();

 private slots:
  void on_konsumenKombo_customContextMenuRequested(const QPoint &p);
  void on_notaTable_customContextMenuRequested(const QPoint &p);
  void createKonsumen();

 private:
  Ui::PembuatNota *ui;
  KaosPolosWindow *kpw;
  DockKonsumen *dc;
  QStandardItemModel *sm;
  QSqlQueryModel *konsumenModel;
};

#endif  // PEMBUATNOTA_H
