#include "dockproduk.h"

#include <qmessagebox.h>
#include <qnamespace.h>
#include <qsharedpointer.h>

#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QVBoxLayout>

#include "addproduk.h"
#include "database.h"

DockProduk::DockProduk(Database *_d, QWidget *parent)
    : pm(new ProdukModel(this)),
      db(_d),
      produkView(new QTableView(this)),
      QDockWidget("Produk", parent) {
  produkView->setModel(pm);
  produkView->setContextMenuPolicy(Qt::CustomContextMenu);
  produkView->setObjectName("produkView");
  produkView->setHorizontalScrollMode(produkView->ScrollPerPixel);
  produkView->setVerticalScrollMode(produkView->ScrollPerPixel);
  produkView->hideColumn(0);
  produkView->hideColumn(4);
  produkView->hideColumn(5);
  produkView->horizontalHeader()->setStretchLastSection(true);
  produkView->verticalHeader()->hide();

  pm->setHeaderData(1, Qt::Horizontal, "Nama");
  pm->setHeaderData(2, Qt::Horizontal, "Stok");
  pm->setHeaderData(3, Qt::Horizontal, "Harga");

  auto w1 = new QWidget(this);
  auto lh1 = new QVBoxLayout(w1);
  lh1->addWidget(produkView);
  setWidget(w1);

  produkView->resizeColumnsToContents();
  setObjectName("dockProduk");

  QMetaObject::connectSlotsByName(this);
}

DockProduk::~DockProduk() {}

void DockProduk::on_produkView_customContextMenuRequested(const QPoint &p) {
  QMenu menu;
  auto contextIndex = produkView->indexAt(p);
  auto np = menu.addAction("Produk baru");
  connect(np, &QAction::triggered, this, &DockProduk::addProduk);
  if (contextIndex.isValid()) {
    // add another
  }
  auto namaProduk =
      contextIndex.siblingAtColumn(1).data(Qt::EditRole).toString();
  auto sp = produkView->viewport()->mapToGlobal(p);
  menu.exec(sp);
}

void DockProduk::addProduk() {
  auto ap = new AddProduk(this);
  ap->setAttribute(Qt::WA_DeleteOnClose);
  connect(ap, &AddProduk::inputFinished, this, &DockProduk::addProdukHandler);
  ap->open();
}

void DockProduk::addProdukHandler() {
  AddProduk *ap = qobject_cast<AddProduk *>(sender());
  if (ap) {
    auto res = db->addProduk(ap->nama(), ap->price(), ap->initialStock());
    if (res.success) {
      refreshModel();
      ap->accept();
    } else {
      QMessageBox::information(ap, "Gagal menambahkan Produk",
                               res.errorMessage);
    }
  }
}

void DockProduk::displayProduk(const QString &nama) {}
void DockProduk::refreshModel() { pm->refresh(); }
