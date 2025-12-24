#include "database.h"
#include <QRandomGenerator>
#include <QMap>

struct pinfo {
  QString name;
  int price, qty;
};


void initDummyData(Database* db) {
#ifdef ENABLE_DUMMY_DATA
  QList<pinfo> store;
  QList<QString> knames;
  for (int i=0; i<30; ++i) {
    auto kn = QString("Konsumen %1").arg(i + 1);
    db->addKonsumen( kn,
                    QString("Tel %1").arg(i + 1),
                    QString("Info %1").arg(i + 1));
    knames << kn;
    
    pinfo p { QString("Produk %1").arg(i + 1),
                  25000 + (1100 * (i + 1)),
                  2 * (i + 1) };
    db->addProduk(p.name, p.price, p.qty);
    
    store << p;
  }
  
  QRandomGenerator rg;
  int kid, // Random Konsumen ID
      pid, // Random Produk ID
      cpid, // Random Produk Count 1 = 6
      tmp;

  for(int i=0; i<30; ++i) {
    qDebug() << "Add Penjualan " << i + 1;
    kid = rg.bounded(30);
    cpid = rg.bounded(1, 6);
    // QList<pinfo> sales(cpid) ; // list of product to be sale
    QList<int> ql, prl;
    QList<QString> nl;
    for(int w=0; w<cpid; ++w) {
      int pMax = 0;
      while(!pMax) {
        tmp = rg.bounded(30);
        pMax = store[tmp].qty;
      }
      auto pSale = rg.bounded(1, pMax);
      store[tmp].qty -= pSale;
      nl << store[tmp].name;
      ql << pSale;
      prl << store[tmp].price + 1000;
    }
    auto ap = db->addPenjualan(nl, ql, prl);
    if(ap.success) {
      auto inv = db->createInvoice(ap.penjualanIds, knames[rg.bounded(30)]);
    } else {
    qDebug() << ap.errorMessage;
    }
  }
#endif
};