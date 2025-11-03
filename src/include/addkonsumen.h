#ifndef ADDKONSUMEN_H
#define ADDKONSUMEN_H

namespace Ui {
    class AddKonsumen;
}

#include <QDialog>

class AddKonsumen : public QDialog
{
  Q_OBJECT
public:
  explicit AddKonsumen(QWidget *parent=nullptr);
  ~AddKonsumen();
  
  QString name() const;
  QString phone() const;
  QString info() const;

private:
  Ui::AddKonsumen *ui;
};

#endif