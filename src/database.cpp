#include "database.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <qlogging.h>

Transaction::Transaction(QSqlDatabase base) : dft(base), m_commited(false) {
  if (!dft.isValid())
    dft = QSqlDatabase::database();
  if (!dft.transaction()) {
    m_commited = true;
    qDebug() << "Tidak dapat memulai transaksi database";
  }
}

Transaction::~Transaction() {
  if (!m_commited)
    dft.rollback();
}

bool Transaction::commit() {
  if (!m_commited) {
    m_commited = dft.commit();
  }
  return m_commited;
}

Database::Database(const QString &dp) : _success(false), m_dataPath(dp) {
  initializeDatabase();
}

Database::~Database() {}

void Database::initializeDatabase() {
  QString ap = m_dataPath;
  if (ap.isEmpty()) {
    ap = qApp->property("appDataPath").toString();
  }
  QDir apd(ap);
  QString dbPath = apd.absoluteFilePath("KDB.db");
  if (!QFileInfo::exists(dbPath)) {
    qDebug() << "try to copy KDB.db";
    QFile res(":/Database/KDB.db");
    auto dbPath = apd.absoluteFilePath("KDB.db");
    if (!res.copy(dbPath)) {
      qDebug() << "unable to copy res file to " << dbPath;
      return;
    }
    qDebug() << "Database Copied sucessfully";
    QFile dbFile(dbPath);
    if (!dbFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner)) {
      qDebug() << "Unable to set right permissions to DatabaseFile";
      return;
    }
    qDebug() << "Database Permissions corrected";
  } else {
    qDebug() << "no need to create KDB.db";
  }

  db = QSqlDatabase::database();
  if (!db.isValid()) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
  }
  if (!db.open()) {
    qDebug() << "Unable to open database";
    return;
  }
  QSqlQuery q("pragma foreign_keys = ON;");
  if (q.lastError().isValid())
    return;
  _success = true;
  qDebug() << "Database Initialization Success";
}

// CRUD PRODUK
DatabaseResult Database::addProduk(const QString &uniqueName, int price, int stock) {
  DatabaseResult res;
  QSqlQuery q;
  q.prepare("INSERT INTO Produk (nama, stock, base_price) VALUES (?, ?, ?)");
  q.addBindValue(uniqueName);
  q.addBindValue(stock);
  q.addBindValue(price);
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      auto error = q.lastError();
      res.errorMessage = error.text();
    } else {
      res.errorMessage = "addProduk Failed;";
    }
    return res;
  }
  res.success = true;
  return res;
}

bool Database::removeProduk(const QString &uniqueName)
{
  QSqlQuery q;
  q.prepare("DELETE FROM Produk WHERE nama = :nm");
  q.bindValue(":nm", uniqueName);
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    } else {
      qWarning() << "Tidak dapat menghapus produk";
    }
    return false;
  }
  return true;
}

quint64 Database::produkIdFromName(const QString &uniqueName) const {
  QSqlQuery q;
  q.prepare("SELECT id FROM Produk WHERE nama = ?");
  q.addBindValue(uniqueName);
  if (!q.exec() || !q.next()) {
    if(q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    } else {
      qWarning() << QString("Produk %1 tidak ditemukan").arg(uniqueName);      
    }
    return 0;
  }
  return q.value("id").toLongLong();
}

bool Database::setProdukPrice(const QString &name, int price) {
  QSqlQuery q;
  q.prepare("UPDATE Produk SET base_price = ? WHERE nama = ?");
  q.addBindValue(price);
  q.addBindValue(name);
  if (!q.exec()) {
    if(q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

bool Database::setProdukName(const QString &from, const QString &to) {
  QSqlQuery q;
  q.prepare("UPDATE Produk SET nama = :to WHERE nama = :fr");
  q.bindValue(":to", to);
  q.bindValue(":fr", from);
  if (!q.exec()) {
    if (q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    } else {
      qWarning() << "Gagal menyetel nama Produk";
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

bool Database::addStock(const QString &nama, quint64 stock) {
  QSqlQuery q;
  q.prepare("UPDATE Produk SET stock = stock + ? WHERE nama = ?");
  q.addBindValue(stock);
  q.addBindValue(nama);
  if (!q.exec()) {
    if (q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    } else {
      qWarning() << "Gagal mengupdate Stock Produk";
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

int Database::produkBasePrice(const QString &nama) const {
  QSqlQuery q;
  q.prepare("SELECT base_price FROM Produk WHERE nama = ?");
  q.addBindValue(nama);
  if (!q.exec() || !q.next()) {
    if (q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    } else {
      qWarning() << QString("Produk (%1) belum terdaftar").arg(nama);
    }
    return 0;
  }
  return q.value(0).toLongLong();
}

int Database::produkStock(const QString &nama) const {
  QSqlQuery q;
  q.prepare("SELECT stock FROM Produk WHERE nama = ?");
  q.addBindValue(nama);
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    }
    return 0;
  }
  if (!q.next()) {
    qWarning() << QString("Produk (%1) belum terdaftar").arg(nama);
    return 0;
  }
  return q.value(0).toInt();
}
// END CRUD PRODUK

// CRUD KONSUMEN
bool Database::addKonsumen(const QString &nama, const QString &phone,
                           const QString &info) {
  QSqlQuery q;
  q.prepare("INSERT INTO Konsumen (nama, phone, info) VALUES (?, ?, ?);");
  q.addBindValue(nama);
  q.addBindValue(phone);
  q.addBindValue(info);
  if (!q.exec()) {
    if (q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    }
    return false;
  }
  return true;
}

bool Database::removeKonsumen(const QString &nama) {
  QSqlQuery q;
  q.prepare(R"-(
    DELETE FROM Konsumen 
    WHERE nama = ?;)-");
  q.addBindValue(nama);
  
  if (!q.exec()) {
    if (q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

bool Database::setKonsumenInfo(const QString &nama, const QString &info) {
  QSqlQuery q;
  q.prepare(R"-(
    UPDATE Konsumen 
    SET (info, updated) = (?, datetime('now', 'localtime'))
    WHERE nama = ?;
      )-");
  q.addBindValue(info);
  q.addBindValue(nama);

  if (!q.exec()) {
    if (q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

bool Database::setKonsumenPhone(const QString &nama, const QString &phone) {
  QSqlQuery q;
  q.prepare(R"-(
    UPDATE Konsumen 
    SET (phone, updated) = (?, datetime('now', 'localtime'))
    WHERE nama = ?;
      )-");

  q.addBindValue(phone);
  q.addBindValue(nama);

  if (!q.exec()) {
    if (q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

bool Database::setNamaKonsumen(const QString &nama_old,
                               const QString &nama_new) {
  QSqlQuery q;
  q.prepare(R"-(
    UPDATE Konsumen 
    SET (nama, updated) = (?, datetime('now', 'localtime'))
    WHERE nama = ?;
      )-");
  q.addBindValue(nama_new);
  q.addBindValue(nama_old);

  if (!q.exec()) {
    if (q.lastError().isValid()) {
      auto error = q.lastError();
      qWarning() << error.text();
    } else {
      qWarning() << "Gagal mengubah nama konsumen";
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}
// END CRUD KONSUMEN

// CRUD Penjualan
const AddPenjualanResult Database::addPenjualan(const QStringList produkList,
                                                const QList<int> qtyList,
                                                const QList<int> priceList) {
  AddPenjualanResult res;

  int plc = produkList.count();

  if (qtyList.count() != plc || priceList.count() != plc) {
    res.errorMessage = "Jumlah parameter addPenjualan tidak sama";
    return res;
  }

  Transaction tr;
  QSqlQuery q;

  // Mendapatkan id produk untuk setiap nama produk
  QList<quint64> produkIds;
  for (int i = 0; i < produkList.count(); ++i) {
    auto pr = produkList.at(i);
    auto pifn = produkIdFromName(pr);
    
    // produk id > 0
    if (pifn < 1) {
      res.errorMessage =
          QString("Tidak dapat menemukan ID Produk untuk '%1'").arg(pr);
      qDebug() << "Err: " << res.errorMessage;
      return res;
    } else {
      produkIds << pifn;
    }
    
    // base_price <= harga jual
    if (produkBasePrice(pr) > priceList.at(i)) {
      res.errorMessage =
          QString(
              "Penjualan dibawah harga standar tidak diizinkan, Produk : '%1' (%2 < %3)")
              .arg(pr).arg(priceList.at(i)).arg(produkBasePrice(pr));
      qDebug() << "Err: " << res.errorMessage;
      return res;
    }
    
    // stock >= qty
    if (produkStock(pr) < qtyList.at(i)) {
      res.errorMessage = QString("Tidak dapat menjual Produk"
                                    "ketersediaan stock terbatas, Produk : '%1'")
                                .arg(pr);
      qDebug() << "Err: " << res.errorMessage;
      return res;
    }
  }

  // Check Passed
  QList<quint64> insertIds{};
  int price, qty;
  QString produk,
          prepareStatement("INSERT INTO Penjualan (produk_id, harga_jual, qty, "
                     "harga_total) VALUES (?, ?, ?, ?);");

  for (int c = 0; c < produkList.count(); c++) {
    price = priceList.at(c);
    qty = qtyList.at(c);
    q.prepare(prepareStatement);
    q.addBindValue(produkIds.at(c));
    q.addBindValue(price);
    q.addBindValue(qty);
    q.addBindValue(price * qty);
    if (!q.exec()) {
      if (q.lastError().isValid()) {
        res.errorMessage = q.lastError().text();
      } else {
        res.errorMessage =
            QString(
                "Tidak dapat menyimpan penjualan untuk %1 %2 dengan harga %3")
                .arg(qty)
                .arg(produkList.at(c))
                .arg(price);
      }
      qDebug() << "Err: " << res.errorMessage;
      return res;
    }

    insertIds << q.lastInsertId().toLongLong();

    q.prepare("UPDATE Produk SET stock = stock - ? WHERE id = ?;");
    q.addBindValue(qty);
    q.addBindValue(produkIds.at(c));
    if (!q.exec()) {
      if (q.lastError().isValid()) {
        res.errorMessage = q.lastError().text();
      } else {
        res.errorMessage = "Gagal mengupdate stock";
      }
      qDebug() << "Err: " << res.errorMessage;
      return res;
    }
  }
  if (!tr.commit()) {
    res.errorMessage = "Tidak dapat melakukan COMMIT";
    qDebug() << "Err: " << res.errorMessage;
    return res;
  }

  res.success = true;
  res.penjualanIds = insertIds;
  return res;
}

const DatabaseResult Database::setPenjualanPrice(quint64 pjid, int newPrice,
                                                 bool modifyInvoiced) {
  // Merubah harga penjualan
  // pjid -> id penjualan
  // newPrice -> harga baru
  // modifyInvoiced -> ubah meskipun telah memiliki invoice

  DatabaseResult res;

  Transaction tr;
  QSqlQuery q;

  // mendapatkan info penjualan saat ini
  q.prepare("SELECT * FROM Penjualan WHERE id = ?;");
  q.addBindValue(pjid);

  if (!q.exec() || !q.next()) {
    res.errorMessage = "Data penjualan tidak ditemukan";
    return res;
  }

  int currentPrice = q.value("harga_jual").toInt();
  int currentQty = q.value("qty").toInt();
  int currentTotal = q.value("harga_total").toInt();
  int newTotal = newPrice * currentQty;

  quint64 produk_id = q.value("produk_id").toULongLong();
  bool hasInvoice = !q.value("invoice_id").isNull();
  quint64 invoice_id = q.value("invoice_id").toULongLong();

  if (currentPrice == newPrice) {
    res.errorMessage = "Tidak ada perubahan yang diperlukan";
    return res;
  }

  q.prepare("UPDATE Penjualan SET (harga_jual, harga_total) = (:hj, :ht) WHERE id = :pid;");
  q.bindValue(":hj", newPrice);
  q.bindValue(":ht", newTotal);
  q.bindValue(":pid", pjid);

  if (!q.exec()) {
    if (q.lastError().isValid()) {
      res.errorMessage = q.lastError().text();
    } else {
      res.errorMessage = "Update Penjualan Gagal";
    }
    return res;
  }

  if (hasInvoice) {        // berinvoice
    if (!modifyInvoiced) { // jangan ubah invoice
      res.errorMessage = "Penjualan telah memiliki invoice";
      return res;
    }

    q.prepare("SELECT * FROM Invoice WHERE id = ?");
    q.addBindValue(invoice_id);
    if (!q.exec() || !q.next()) {
      if (q.lastError().isValid()) {
        res.errorMessage = q.lastError().text();
      } else {
        res.errorMessage =
            QString("Tidak dapat menemukan data Invoice (ID : %1)")
                .arg(invoice_id);
      }
      return res;
    }

    int inv_paid = q.value("paid").toInt();
    int inv_total = q.value("total_value").toInt() - currentTotal + newTotal;
    int inv_unpaid = inv_total - inv_paid;

    q.prepare("UPDATE Invoice SET (total_value, unpaid) = (:tv, :unp) WHERE id = :iid;");
    q.bindValue(":tv", inv_total);
    q.bindValue(":unp", inv_unpaid);
    q.bindValue(":iid", invoice_id);

    if (!q.exec()) {
      if (q.lastError().isValid()) {
        res.errorMessage = q.lastError().text();
      } else {
        res.errorMessage = "Tidak dapat mengupdate Invoice";
      }
      return res;
    }
  }

  if (!tr.commit()) {
    res.errorMessage = "Tidak dapat melakukan COMMIT;";
    return res;
  }
  res.success = true;
  return res;
}

const DatabaseResult Database::setPenjualanQty(quint64 pjid, int newQty,
                                               bool modifyInvoiced) {
  // Merubah harga penjualan
  // pjid -> id penjualan
  // newQty -> qty baru
  // modifyInvoiced -> ubah meskipun telah memiliki invoice

  DatabaseResult res;

  Transaction tr;
  QSqlQuery q;

  // mendapatkan info penjualan saat ini
  q.prepare("SELECT * FROM Penjualan WHERE id = ?;");
  q.addBindValue(pjid);

  if (!q.exec() || !q.next()) {
    res.errorMessage = "Data penjualan tidak ditemukan";
    return res;
  }

  int currentPrice = q.value("harga_jual").toInt();
  int currentQty = q.value("qty").toInt();
  int currentTotal = q.value("harga_total").toInt();
  int newTotal = newQty * currentPrice;

  quint64 produk_id = q.value("produk_id").toULongLong();
  bool hasInvoice = !q.value("invoice_id").isNull();
  quint64 invoice_id = q.value("invoice_id").toULongLong();

  if (currentQty == newQty) {
    res.errorMessage = "Tidak ada perubahan yang diperlukan";
    return res;
  }

  // Cek Stock saat ini
  q.prepare("SELECT * FROM Produk WHERE id = ?");
  q.addBindValue(produk_id);
  if (!q.exec() || !q.next()) {
    if (q.lastError().isValid()) {
      res.errorMessage = q.lastError().text();
    } else {
      res.errorMessage = "Tidak dapat menemukan data produk";
    }
    return res;
  }
  
  int currentStock = q.value("stock").toInt();
  int newStock = currentStock + currentQty - newQty;
  
  if (newStock < 0) {
    res.errorMessage =
        "Tidak dapat merubah Qty karena stock Produk tidak cukup";
    return res;
  }
  
  q.prepare(
      "UPDATE Penjualan SET (qty, harga_total) = (:qty, :ht) WHERE id = :pid;");
  q.bindValue(":qty", newQty);
  q.bindValue(":ht", newTotal);
  q.bindValue(":pid", pjid);

  if (!q.exec()) {
    if (q.lastError().isValid()) {
      res.errorMessage = q.lastError().text();
    } else {
      res.errorMessage = "Update Penjualan Gagal";
    }
    return res;
  }

  q.prepare("UPDATE Produk SET stock = :ns WHERE id = :pid");
  q.bindValue(":ns", newStock);
  q.bindValue(":pid", produk_id);

  if (!q.exec()) {
    if (q.lastError().isValid()) {
      res.errorMessage = q.lastError().text();
    } else {
      res.errorMessage = "Tidak dapat mengupdate Stock";
    }
    return res;
  }
  
  if (hasInvoice) { // berinvoice
    if (!modifyInvoiced) { // jangan ubah invoice
      res.errorMessage = "Penjualan telah memiliki invoice";
      return res;
    }

    q.prepare("SELECT * FROM Invoice WHERE id = ?");
    q.addBindValue(invoice_id);
    if (!q.exec() || !q.next()) {
      if (q.lastError().isValid()) {
        res.errorMessage = q.lastError().text();
      } else {
        res.errorMessage =
            QString("Tidak dapat menemukan data Invoice (ID : %1)")
                .arg(invoice_id);
      }
      return res;
    }

    int inv_paid = q.value("paid").toInt();
    int inv_total = q.value("total_value").toInt() - currentTotal + newTotal;
    int inv_unpaid = inv_total - inv_paid;

    q.prepare("UPDATE Invoice SET (total_value, unpaid) = (:tv, :unp) WHERE id "
              "= :iid;");
    q.bindValue(":tv", inv_total);
    q.bindValue(":unp", inv_unpaid);
    q.bindValue(":iid", invoice_id);

    if (!q.exec()) {
      if (q.lastError().isValid()) {
        res.errorMessage = q.lastError().text();
      } else {
        res.errorMessage = "Tidak dapat mengupdate Invoice";
      }
      return res;
    }
  }

  if (!tr.commit()) {
    res.errorMessage = "Tidak dapat melakukan COMMIT;";
    return res;
  }
  res.success = true;
  return res;
}

const DatabaseResult Database::removePenjualan(quint64 pjid, bool updateInvoice) {
  DatabaseResult res;
  Transaction tr;
  QSqlQuery q;

  // mendapatkan info penjualan
  q.prepare("SELECT * FROM Penjualan WHERE id = ?");
  q.addBindValue(pjid);

  if (!q.exec() || !q.next()) {
    res.errorMessage =
        QString("Tidak dapat menemukan data Penjualan (ID %1)").arg(pjid);
    return res;
  }
  
  int pqty = q.value("qty").toInt();
  quint64 produk_id = q.value("produk_id").toULongLong();
  bool hasInvoice = !q.value("invoice_id").isNull();
  auto invoice_id = q.value("invoice_id").toULongLong();
  int harga_total = q.value("harga_total").toInt();

  q.prepare("DELETE FROM Penjualan WHERE id = ?");
  q.addBindValue(pjid);
  if (!q.exec()) {
    if (q.lastError().isValid()) {
      res.errorMessage = q.lastError().text();
    } else {
      res.errorMessage = "Penghapusan Penjualan Gagal";
    }
    return res;
  }

  if (hasInvoice) {
    if (!updateInvoice) {
      res.errorMessage = QString("Data penjualan memiliki Invoice terkait");
      return res;
    }
    q.prepare("SELECT * FROM Invoice WHERE id = ?");
    q.addBindValue("invoice_id");
    if (!q.exec() || !q.next()) {
      if (q.lastError().isValid()) {
        res.errorMessage = q.lastError().text();
      } else {
        res.errorMessage =
            QString("Tidak dapat menemukan Invoice (ID %1)").arg(invoice_id);
      }
      return res;
    }

    int inv_paid = q.value("paid").toInt();
    int inv_unpaid = q.value("unpaid").toInt();
    int inv_total = q.value("total_value").toInt();
    int update_total = inv_total - harga_total;
    if (inv_paid > update_total) {
      // jadi lebih pembayaran
      res.errorMessage = "Terjadi pembayaran berlebih jika penjualan ini dihapus, "
                         "jika benar-benar diperlukan anda bisa menyiasatinya "
                         "dengan menghapus pembayaran terlebuh dahulu";
      return res;
    }

    q.prepare("UPDATE Invoice SET (total_value, unpaid) = (:tv, :up) WHERE id "
              "= :iid");
    q.bindValue(":tv", update_total);
    q.bindValue(":up", update_total - inv_paid);
    q.bindValue(":iid", invoice_id);

    if (!q.exec()) {
      if (q.lastError().isValid()) {
        res.errorMessage = q.lastError().text();
      } else {
        res.errorMessage =
            QString("Tidak dapat melakukan UPDATE Invoice (ID %1)")
                .arg(invoice_id);
      }
      return res;
    }
    
    q.prepare("UPDATE Produk SET stock = stock - :mst WHERE id = :pid");
    q.bindValue(":mst", pqty);
    q.bindValue(":mst", produk_id);
    
    if(!q.exec()) {
      if(q.lastError().isValid()) {
        auto error = q.lastError();
        res.errorMessage = error.text();
      } else {
        res.errorMessage = "Terjadi error saat melakukan Update Stock";
      }
      return res;
    }
  }

  if (!tr.commit()) {
    res.errorMessage = "Tidak dapat melakukan COMMIT : " +
                       QSqlDatabase::database().lastError().text();
    return res;
  }
  res.success = true;
  return res;
}

// CRUD Invoice
const CreateInvoiceResult Database::createInvoice(QList<quint64> penjualan,
                                                  const QString &konsumen) {
  CreateInvoiceResult res;

  Transaction tr;
  QSqlQuery q;

  q.prepare("SELECT id FROM Konsumen WHERE nama = ?");
  q.addBindValue(konsumen);
  if (!q.exec() || !q.next()) {
    res.errorMessage =
        QString("Konsumen dengan nama : '%1' belum terdaftar").arg(konsumen);
    qDebug() << "Err: " << res.errorMessage;
    return res;
  }

  int konsumen_id = q.value(0).toInt();
  int total_value = 0, unpaid = 0;

  for (quint64 p_id : penjualan) {
    q.prepare("SELECT * FROM Penjualan WHERE id = ? LIMIT 1");
    q.addBindValue(p_id);
    if (!q.exec() || !q.next()) {
      res.errorMessage =
          QString("Tidak dapat menemukan Penjualan dengan ID : %1").arg(p_id);
      qDebug() << "Err: " << res.errorMessage;
      return res;
    }
    total_value += q.value("harga_total").toInt();
  }

  q.prepare("INSERT INTO Invoice (konsumen_id, total_value, paid, unpaid) "
            "VALUES (:konid, :toval, :paid, :toval);");
  q.bindValue(":konid", konsumen_id);
  q.bindValue(":toval", total_value);
  q.bindValue(":paid", 0);
  if (!q.exec()) {
    if (q.lastError().isValid()) {
      res.errorMessage = q.lastError().text();
    } else {
      res.errorMessage = "Tidak dapat membuat dan menyimpan invoice";
    }
    return res;
  }

  int inv_id = q.lastInsertId().toInt();

  for (auto p_id : penjualan) {
    q.prepare("UPDATE Penjualan SET invoice_id = :inv_id WHERE id = :p_id;");
    q.bindValue(":inv_id", inv_id);
    q.bindValue(":p_id", p_id);
    if (!q.exec()) {
      if (q.lastError().isValid()) {
        res.errorMessage = q.lastError().text();
      } else {
        res.errorMessage =
            "Gagal saat mengupdate invoice_id pada setiap penjualan";
      }
      qDebug() << "Err: " << res.errorMessage;
      return res;
    }
  }

  if (!tr.commit()) {
    res.errorMessage = "Tidak dapat melakukan COMMIT saat membuat Invoice";
    qDebug() << "Err: " << res.errorMessage;
    return res;
  }
  res.success = true;
  res.invoiceId = inv_id;
  return res;
}

const CreatePaymentResult Database::createPayment(quint64 invoice_id, 
            int value,
            const QString& info,
            const QDateTime& pay_time)
{
  CreatePaymentResult res;
  
  Transaction tr;
  QSqlQuery iG; // invoice Getter
  QSqlQuery iU; // invoice Updatter
  QSqlQuery pC; // payment Inserter
  
  iG.prepare("SELECT * FROM Invoice WHERE id = ?");
  iG.addBindValue(invoice_id);
  if(!iG.exec() || !iG.next()) {
    if(iG.lastError().isValid()) {
      auto error = iG.lastError();
      res.errorMessage = error.text();
    } else {
      res.errorMessage = QString("Tidak dapat menemukan invoice dengan ID %1").arg(invoice_id);
    }
    return res;
  }
  
  int inv_unpaid = iG.value("unpaid").toInt(),
      inv_paid = iG.value("paid").toInt();
  if(inv_unpaid < value) {
    res.errorMessage = "Terdeteksi kelebihan bayar pada pembayaran ini";
    return res;
  }
  
  iU.prepare("UPDATE Invoice SET (paid, unpaid, last_payment, modified) = ( :pd, :upd, :pt, :mdf) WHERE id = :iid ");
  iU.bindValue(":pd", inv_paid + value);
  iU.bindValue(":upd", inv_unpaid - value);
  iU.bindValue(":pt", pay_time.toString("yyyy-MM-dd HH:mm:ss"));
  iU.bindValue(":mdf", QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
  iU.bindValue(":iid", invoice_id);
  
  if(!iU.exec()) {
    if(iU.lastError().isValid()) {
      auto error = iU.lastError();
      res.errorMessage = error.text();
    } else {
      res.errorMessage = "Tidak dapat memperbarui Invoice saat melakukan Pembayaran";
    }
    return res;
  }
  
  pC.prepare("INSERT INTO Pembayaran (pay_time, info, value, invoice_id) VALUES (:pt, :inf, :va, :inv)");
  pC.bindValue(":pt", pay_time.toString("yyyy-MM-dd HH:mm:ss"));
  pC.bindValue(":inf", info);
  pC.bindValue(":va", value);
  pC.bindValue(":inv", invoice_id);
  
  if(!pC.exec()) {
    if(pC.lastError().isValid()) {
      auto error = pC.lastError();
      res.errorMessage = error.text();
    } else {
      res.errorMessage = "Tidak dapat menyimpan data pembayaran";
    }
    return res;
  }
  
  res.paymentId = pC.lastInsertId().toULongLong();
  res.success = res.paymentId != 0;
  if (!tr.commit()) {
    res.success = false;
    res.errorMessage = "Unable To Commit";
    return res;
  }
  return res;
}