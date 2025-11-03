#ifndef DOCKPRODUK_H
#define DOCKPRODUK_H

#include <QDockWidget>
#include <QTableView>

#include "database.h"
#include "produkmodel.h"

class DockProduk : public QDockWidget {
  Q_OBJECT

 public:
  DockProduk(Database *, QWidget * = nullptr);
  ~DockProduk();

 public slots:
  void addProduk();
  void refreshModel();

 private slots:
  void on_produkView_customContextMenuRequested(const QPoint &);
  void displayProduk(const QString &name);
  void addProdukHandler();

 signals:
  void produkAdded(const QString &name, bool ok);

 private:
  QTableView *produkView;
  ProdukModel *pm;
  Database *db;
};

#endif  // DOCKPRODUK_H
