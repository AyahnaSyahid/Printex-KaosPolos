#ifndef KAOSPOLOSWINDOW_H
#define KAOSPOLOSWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include "database.h"

namespace Ui {
  class KaosPolosWindow;
}

class KaosPolosWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit KaosPolosWindow(Database *d, QWidget *p=nullptr);
  ~KaosPolosWindow();

public slots:
  void addProduk(); // show addProduk Dialog

private slots:
  void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *it, int);
  void registerProduk(const QVariantMap&);

signals:
  void produkAdded(const QString& nama, bool ok, const QString& error);

private:
  Database *db;
  Ui::KaosPolosWindow *ui;
};


#endif