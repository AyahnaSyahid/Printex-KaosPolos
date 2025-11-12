#ifndef WIDGETPENJUALAN_H
#define WIDGETPENJUALAN_H

#include <QWidget>
#include <QSortFilterProxyModel>

namespace Ui {
  class WidgetPenjualan;
}

class KaosPolosWindow;
class WidgetPenjualan : public QWidget
{
  Q_OBJECT

public:
  class ModelAdapter;
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

class WidgetPenjualan::ModelAdapter : public QSortFilterProxyModel
{
public:
  ModelAdapter(QObject *parent);
  QVariant data(const QModelIndex& ix, int role = Qt::DisplayRole) const override;
};

#endif
