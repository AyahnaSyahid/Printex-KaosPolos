#include "dockkonsumen.h"

#include <qnamespace.h>

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QTableView>
#include <QVBoxLayout>

#include "addkonsumen.h"
#include "database.h"
#include "kaospoloswindow.h"
#include "konsumenmodel.h"
#include "konsumeninfo.h"

DockKonsumen::DockKonsumen(Database* _d, KaosPolosWindow* k)
    : db(_d),
      konsumenModel(new KonsumenModel(this)),
      QDockWidget("Konsumen", k) {
  auto w1 = new QWidget(this);
  auto v = new QVBoxLayout();
  w1->setLayout(v);
  konsumenView = new QTableView(w1);
  konsumenView->setObjectName("konsumenView");
  konsumenView->setModel(konsumenModel);
  konsumenView->setSortingEnabled(true);
  konsumenView->setContextMenuPolicy(Qt::CustomContextMenu);

  v->addWidget(konsumenView);
  k->addItemHook("Konsumen", "Baru");
  connect(k, &KaosPolosWindow::triggerHook, this, &DockKonsumen::hookTriggered);
  setWidget(w1);
  setObjectName("dockKonsumen");
  QMetaObject::connectSlotsByName(this);
}

DockKonsumen::~DockKonsumen() {}

void DockKonsumen::addKonsumen() {
  AddKonsumen* adk = new AddKonsumen(this);
  adk->setAttribute(Qt::WA_DeleteOnClose);
  connect(adk, &AddKonsumen::finishEditing, this,
          &DockKonsumen::addKonsumenHandler);
  adk->open();
}

void DockKonsumen::hookTriggered(const QString& p, const QString& i) {
  if (p == "Konsumen" && i == "Baru") {
    addKonsumen();
  }
}

void DockKonsumen::refreshModel() { konsumenModel->refresh(); }

void DockKonsumen::addKonsumenHandler() {
  auto adk = qobject_cast<AddKonsumen*>(sender());
  if (!adk) return;
  auto added = db->addKonsumen(adk->name(), adk->phone(), adk->info());
  if (added) {
    konsumenModel->refresh();
    adk->accept();
    QMessageBox::information(this, "Berhasil",
                             "data Konsumen berhasil disimpan");
  } else {
    QMessageBox::information(this, "Gagal", "Gagal menyimpan data konsumen");
  }
}

void DockKonsumen::editKonsumen(const QString& nama) {
  KonsumenInfo *ki = new KonsumenInfo(nama, this);
  ki->setAttribute(Qt::WA_DeleteOnClose);
  connect(ki, &KonsumenInfo::finishEditing, this, &DockKonsumen::editKonsumenHandler);
  ki->open();
}

void DockKonsumen::editKonsumenHandler() {
  auto ki = qobject_cast<KonsumenInfo*>(sender());
  if(!ki) return;
  Transaction tr;
  bool ok = true;
  if (ok && ki->phoneChanged()) ok = db->setKonsumenPhone(ki->nama(), ki->updatePhone());
  if (ok && ki->infoChanged()) ok = db->setKonsumenInfo(ki->nama(), ki->updateInfo());
  if (ok && ki->namaChanged()) ok = db->setNamaKonsumen(ki->nama(), ki->updateNama());
  if(ok) {
    if(tr.commit()) {
      ki->accept();
      konsumenModel->refresh();
      return;
    }
  }
  QMessageBox::warning(this, "Update Gagal", "Tidak dapat menyimpan perubahan data Konsumen");
  return ;
}

void DockKonsumen::on_konsumenView_customContextMenuRequested(const QPoint& p) {
  QMenu ctx(this);
  auto ix = konsumenView->indexAt(p);
  if (ix.isValid()) {
    QString cn = ix.siblingAtColumn(1).data(Qt::DisplayRole).toString();
    auto edt = ctx.addAction("Edit");
    connect(edt, &QAction::triggered, [this, &cn]() { editKonsumen(cn); });
    ctx.addSeparator();
  }
  auto adk = ctx.addAction("Konsumen Baru");
  connect(adk, &QAction::triggered, this, &DockKonsumen::addKonsumen);
  ctx.exec(konsumenView->viewport()->mapToGlobal(p));
}
