#include "dockproduk.h"
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>


DockProduk::DockProduk(QWidget *parent)
  : pm(new ProdukModel(this)), produkView(new QTableView(this)), QDockWidget("Produk", parent)
{
  produkView->setModel(pm);
  produkView->setContextMenuPolicy(Qt::CustomContextMenu);
  produkView->setObjectName("produkView");
  produkView->setHorizontalScrollMode(produkView->ScrollPerPixel);
  produkView->setVerticalScrollMode(produkView->ScrollPerPixel);
  auto w1 = new QWidget(this);
  auto lh1 = new QVBoxLayout(w1);
  lh1->addWidget(produkView);
  setWidget(w1);
  QMetaObject::connectSlotsByName(this); 
}

DockProduk::~DockProduk() {}

void DockProduk::on_produkView_customContextMenuRequested(const QPoint& p)
{
  QMenu menu;
  auto act = menu.addAction("DummyAction");
  menu.exec(produkView->viewport()->mapToGlobal(p));
}

void DockProduk::refreshModel()
{
  pm->refresh();
}