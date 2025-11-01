#include "kaospoloswindow.h"
#include "ui/ui_kaospoloswindow.h"
#include "addproduk.h"
#include "dockproduk.h"
#include <QSqlQueryModel>
#include <QDockWidget>


KaosPolosWindow::KaosPolosWindow(Database *d, QWidget *p)
  : ui(new Ui::KaosPolosWindow), db(d), QMainWindow(p)
{
  ui->setupUi(this);
  auto dp = new DockProduk(this);
  dp->setObjectName("dockProduk");
  addDockWidget(Qt::LeftDockWidgetArea, dp);
  connect(this, &KaosPolosWindow::produkAdded, dp, &DockProduk::refreshModel);
  dp->show();
}

KaosPolosWindow::~KaosPolosWindow() {}

void KaosPolosWindow::addProduk() {
  QSqlQueryModel *qm = new QSqlQueryModel();
  qm->setQuery("SELECT nama FROM Produk");
  AddProduk *ap = new AddProduk(qm, this);
  ap->setAttribute(Qt::WA_DeleteOnClose);
  
  connect(ap, &AddProduk::addProduk, this, &KaosPolosWindow::registerProduk);
  connect(this, &KaosPolosWindow::produkAdded, ap, &AddProduk::produkAdded);
  
  ap->open();
}

void KaosPolosWindow::registerProduk(const QVariantMap& vm) {
  auto res = db->addProduk(vm["nama"].toString(), vm["price"].toInt(), vm["stock"].toInt());
  emit produkAdded(vm["nama"].toString(), res.success, res.errorMessage);
}

void KaosPolosWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *it, int)
{
  if(it->data(0, Qt::DisplayRole) == "Tambah Produk") {
    addProduk();
  }
}