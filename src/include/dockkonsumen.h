#ifndef DOCKKONSUMEN_H
#define DOCKKONSUMEN_H

#include <QDockWidget>

class KaosPolosWindow;
class Database;
class KonsumenModel;
class QTableView;
class DockKonsumen : public QDockWidget {
  Q_OBJECT

 public:
  explicit DockKonsumen(Database *, KaosPolosWindow *);
  ~DockKonsumen();

 public slots:
  void addKonsumen();
  void addKonsumenHandler();
  void refreshModel();

 private slots:
  void editKonsumen(const QString &nama);
  void hookTriggered(const QString &p, const QString &i);
  void on_konsumenView_customContextMenuRequested(const QPoint &p);

 private:
  Database *db;
  KonsumenModel *konsumenModel;
  QTableView *konsumenView;
};

#endif
