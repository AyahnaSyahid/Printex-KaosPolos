#ifndef DOCKPRODUK_H
#define DOCKPRODUK_H

#include <QDockWidget>
#include <QTableView>
#include "produkmodel.h"


class DockProduk : public QDockWidget
{
  Q_OBJECT

public:
  DockProduk(QWidget * =nullptr);
  ~DockProduk();

public slots:
  void refreshModel();
  
private slots:
  void on_produkView_customContextMenuRequested(const QPoint&);
  
signals:
  
  
private:
  QTableView *produkView;
  ProdukModel *pm;
};

#endif // DOCKPRODUK_H