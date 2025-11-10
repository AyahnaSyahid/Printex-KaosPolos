#ifndef WIDGETPENJUALAN_H
#define WIDGETPENJUALAN_H

#include <QWidget>

namespace Ui {
  class WidgetPenjualan;
}

class KaosPolosWindow;
class WidgetPenjualan : public QWidget
{
  Q_OBJECT

public:  
  WidgetPenjualan(QWidget *parent=nullptr);
  ~WidgetPenjualan();
  void setKaosPolosWindow(KaosPolosWindow *k);

public slots:
  void refreshData();

private slots:
  void on_jualButton_clicked();
  void on_harianView_customContextMenuRequested(const QPoint&);

signals:
  void konsumenAdded();
  void notaUpdated();

private:
  Ui::WidgetPenjualan *ui;
  KaosPolosWindow *kpw;
};

#endif
