#include "kaospoloswindow.h"

#include <QDockWidget>
#include <QSqlQueryModel>

#include "dockproduk.h"
#include "dockkonsumen.h"
#include "widgetpenjualan.h"
#include "widgetinvoice.h"
#include "ui/ui_kaospoloswindow.h"
#include "invoiceprinter.h"

KaosPolosWindow::KaosPolosWindow(Database *d, QWidget *p)
    : ui(new Ui::KaosPolosWindow), db(d), QMainWindow(p) {
  ui->setupUi(this);
  ui->treeWidget->setColumnCount(1);
  auto dp = new DockProduk(db, this);
  addDockWidget(Qt::LeftDockWidgetArea, dp);
  auto dk = new DockKonsumen(db, this);
  addDockWidget(Qt::LeftDockWidgetArea, dk);
  
  WidgetPenjualan *wp = qobject_cast<WidgetPenjualan*>(ui->widgetPenjualan);
  if (wp) {
    wp->setKaosPolosWindow(this);
    wp->setDatabase(db);
    connect(dk, &DockKonsumen::konsumenModified, wp, &WidgetPenjualan::refreshData);
    connect(dp, &DockProduk::produkUpdated, wp, &WidgetPenjualan::refreshData);
  }
  
  WidgetInvoice *wi = qobject_cast<WidgetInvoice*>(ui->widgetInvoice);
  if(wi) {
    wi->setKaosPolosWindow(this);
    wi->setDatabase(db);
    connect(dk, &DockKonsumen::konsumenModified, wi, &WidgetInvoice::refreshData);
  }
  
  auto invoicePrinter = new InvoicePrinter(this);
  invoicePrinter->setObjectName("invoicePrinter");
  auto aboutQt = new QAction("Qt Frameworks", this);
  connect(aboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
  ui->menuTentang->addAction(aboutQt);
}

KaosPolosWindow::~KaosPolosWindow() { delete ui; }

void KaosPolosWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *it,
                                                      int column)
{
  auto topLevel = it->parent();
  if(!topLevel) return ;
  emit triggerHook(topLevel->text(0), it->text(0));
}

void KaosPolosWindow::addItemHook(const QString& r, const QString& t) {
  auto tree = ui->treeWidget;
  auto rfound = false;
  int topLevelIndex = 0;
  QTreeWidgetItem *ritem, *titem;
  
  for(int i=0; i<tree->topLevelItemCount(); ++i) {
    auto topLevelItem = tree->topLevelItem(i);
    if (topLevelItem->text(0) == r) {
      rfound = true;
      ritem = topLevelItem;
      break;
    }
  }
  if (!rfound) {;
    ritem = new QTreeWidgetItem();
    ritem->setText(0, r);
    tree->addTopLevelItem(ritem);
  }
  titem = new QTreeWidgetItem(ritem);
  titem->setText(0, t);
}
