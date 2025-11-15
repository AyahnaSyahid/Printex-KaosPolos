#ifndef DOCKPRODUK_H
#define DOCKPRODUK_H

#include <QDockWidget>
#include <QTableView>

#include "database.h"
#include "produkmodel.h"
#include "kaospoloswindow.h"

class DockProduk : public QDockWidget {
  Q_OBJECT

 public:
  DockProduk(Database *, KaosPolosWindow * = nullptr);
  ~DockProduk();

 public slots:
  void addProduk();
  void refreshModel();

 private slots:
  void on_produkView_customContextMenuRequested(const QPoint &);
  void displayProduk(const QString &name);
  void addProdukHandler();
  void hookTriggered(const QString& p, const QString& i);

 signals:
  void produkAdded(const QString &name, bool ok);
  void produkUpdated();

 private:
  QTableView *produkView;
  ProdukModel *pm;
  Database *db;
};

#endif  // DOCKPRODUK_H
