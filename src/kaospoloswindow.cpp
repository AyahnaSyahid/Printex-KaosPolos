#include "kaospoloswindow.h"

#include <QDockWidget>
#include <QSqlQueryModel>

#include "dockproduk.h"
#include "ui/ui_kaospoloswindow.h"

KaosPolosWindow::KaosPolosWindow(Database *d, QWidget *p)
    : ui(new Ui::KaosPolosWindow), db(d), QMainWindow(p) {
  ui->setupUi(this);
  auto dp = new DockProduk(db, this);
  addDockWidget(Qt::LeftDockWidgetArea, dp);
}

KaosPolosWindow::~KaosPolosWindow() {}

void KaosPolosWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *it,
                                                      int) {}
