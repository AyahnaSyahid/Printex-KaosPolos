#include "dockkonsumen.h"

#include "database.h"
#include "kaospoloswindow.h"
#include "konsumenmodel.h"

#include <QTableView>
#include <QVBoxLayout>

DockKonsumen::DockKonsumen(Database* _d, KaosPolosWindow *k)
  : db(_d), konsumenModel(new KonsumenModel(this)), QDockWidget("Konsumen", k)
{
  auto w1 = new QWidget(this);
  auto v = new QVBoxLayout();
  w1->setLayout(v);
  konsumenView = new QTableView(w1);
  konsumenView->setObjectName("konsumenView");
  konsumenView->setModel(konsumenModel);
  konsumenView->setSortingEnabled(true)
  
  v->addWidget(konsumenView);
  k->addItemHook("Konsumen", "Baru");
  connect(k, &KaosPolosWindow::triggerHook, this, &DockKonsumen::hookTriggered);
  setWidget(w1);
  setObjectName("dockKonsumen");
}

DockKonsumen::~DockKonsumen() {}

void DockKonsumen::addKonsumen() {
  // dialog add konsumen
}

void DockKonsumen::hookTriggered(const QString& p, const QString& i) {
  if (p == "Konsumen" && i == "Baru") {
    addKonsumen();
  }
}

void DockKonsumen::refreshModel() {
  konsumenModel->refresh();
}

void DockKonsumen::addKonsumenHandler() {
  // pass
}
