#ifndef PEMBUATNOTA_H
#define PEMBUATNOTA_H

#include <QDialog>

namespace Ui {
  class PembuatNota;
}

class QStandardItemModel;
class PembuatNota : public QDialog
{
  Q_OBJECT
public:
  
  explicit PembuatNota(QWidget *p);
  ~PembuatNota();
  
private:
  Ui::PembuatNota *ui;
  QStandardItemModel *sm;
};

#endif // PEMBUATNOTA_H