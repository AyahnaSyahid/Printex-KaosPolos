#ifndef PEMBUATNOTA_H
#define PEMBUATNOTA_H

#include <QDialog>

namespace Ui {
class PembuatNota;
}

class QStandardItemModel;
class PembuatNota : public QDialog {
  Q_OBJECT
 public:
  explicit PembuatNota(QWidget *p);
  ~PembuatNota();

 private slots:
  void on_konsumenCombo_customContextMenuRequested(const QPoint &p);
  void on_notaTable_customContextMenuRequested(const QPoint &p);
  void createKonsumen();
  void addKonsumenReceiver();

 private:
  Ui::PembuatNota *ui;
  QStandardItemModel *sm;
};

#endif  // PEMBUATNOTA_H
