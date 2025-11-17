#ifndef WIDGETPENJUALAN_H
#define WIDGETPENJUALAN_H

#include <QWidget>
#include <QSortFilterProxyModel>

namespace Ui {
  class WidgetPenjualan;
}

class Database;
class KaosPolosWindow;
class WidgetPenjualan : public QWidget
{
  Q_OBJECT

public:
  class ModelAdapter;
  WidgetPenjualan(QWidget *parent=nullptr);
  ~WidgetPenjualan();
  void setKaosPolosWindow(KaosPolosWindow *k);
  void setDatabase(Database *base);

public slots:
  void refreshData();

private slots:
  void on_jualButton_clicked();
  void on_harianView_customContextMenuRequested(const QPoint&);
  void editPenjualan(int);

public slots:
  bool hapusPenjualan(int);

signals:
  void konsumenAdded();
  void notaUpdated();

private:
  Ui::WidgetPenjualan *ui;
  KaosPolosWindow *kpw;
  Database *db;
};

class WidgetPenjualan::ModelAdapter : public QSortFilterProxyModel
{
public:
  ModelAdapter(QObject *parent);
  QVariant data(const QModelIndex& ix, int role = Qt::DisplayRole) const override;
};

#endif
