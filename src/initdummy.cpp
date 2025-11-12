#include "database.h"

void initDummyData(Database* db) {
  for (int i=0; i<30; ++i) {
    db->addKonsumen(QString("Konsumen %1").arg(i + 1),
                    QString("Tel %1").arg(i + 1),
                    QString("Info %1").arg(i + 1));
    db->addProduk(QString("Produk %1").arg(i + 1),
                  25000 + (1100 * (i + 1)),
                  2 * (i + 1));
  }
};