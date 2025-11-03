#ifndef KAOSPOLOSWINDOW_H
#define KAOSPOLOSWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>

#include "database.h"

namespace Ui {
class KaosPolosWindow;
}

class KaosPolosWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit KaosPolosWindow(Database *d, QWidget *p = nullptr);
  ~KaosPolosWindow();

 public slots:

 private slots:
  void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *it, int);

 signals:

 private:
  Database *db;
  Ui::KaosPolosWindow *ui;
};

#endif
