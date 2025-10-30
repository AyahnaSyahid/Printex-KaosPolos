#include "database.h"
#include <QSqlDatabase>
#include <QStandardPaths>
#include <QApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QMutex>
#include <QMutexLocker>
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

Transaction::Transaction(QSqlDatabase base)
  :dft(base), m_commited(false)
{
  if(!dft.isValid())
    dft = QSqlDatabase::database();
  if(!dft.transaction()) {
    m_commited = true;
    qDebug() << "Tidak dapat memulai transaksi database";
  }

}

Transaction::~Transaction()
{
  if(!m_commited)
    dft.rollback();
}

bool Transaction::commit()
{
  if(!m_commited) {
    m_commited = dft.commit();
  }
  return m_commited;
}

Database::Database(const QString& dp)
  : _success(false), m_dataPath(dp)
{
  initializeDatabase();
}

Database::~Database()
{
  
}

void Database::initializeDatabase() {
  QString ap = m_dataPath;
  if(ap.isEmpty()) {
    ap = qApp->property("appDataPath").toString();
  }
  QDir apd(ap);
  QString dbPath = apd.absoluteFilePath("KDB.db");
  if(!QFileInfo::exists(dbPath)) {
    qDebug() << "try to copy KDB.db";
    QFile res(":/Database/KDB.db");
    auto dbPath = apd.absoluteFilePath("KDB.db");
    if(!res.copy(dbPath)) {
      qDebug() << "unable to copy res file to " << dbPath;
      return;
    }
    qDebug() << "Database Copied sucessfully";
    QFile dbFile(dbPath);
    if(!dbFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner)) {
      qDebug() << "Unable to set right permissions to DatabaseFile";
      return;
    }
    qDebug() << "Database Permissions corrected";
  } else {
    qDebug() << "no need to create KDB.db";
  }
  
  db = QSqlDatabase::database();
  if(!db.isValid()) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);
  }
  if(!db.open()) {
    qDebug() << "Unable to open database";
    return;
  }
  QSqlQuery q("pragma foreign_keys = ON;");
  if(q.lastError().isValid())
    return;
  _success = true;
  qDebug() << "Database Initialization Success";
}

// CRUD PRODUK
bool Database::addProduk(const QString& uniqueName, int price, int stock) {
  if(!_lastError.isEmpty())
    return false;
  qDebug() << "Creating object tr";
  Transaction tr;
  qDebug() << "tr created";
  QSqlQuery q;
  q.prepare("INSERT INTO Produk (nama, stock, base_price) VALUES (?, ?, ?)");
  q.addBindValue(uniqueName);
  q.addBindValue(stock);
  q.addBindValue(price);
  auto ex_ok = q.exec();
  if(!ex_ok) {
    _lastError = q.lastError().text();
    return false;
  }
  tr.commit();
  qDebug() << "addProduk success";
  return true;
}

bool Database::removeProduk(const QString& uniqueName) {
  if(hasError())
    return false;
  QSqlQuery q;
  // track penjualan
  q.prepare(R"-(
    SELECT COUNT(pj.id) AS CNT 
    FROM Produk pr 
    JOIN Penjualan pj ON pj.produk_id = pr.id 
    WHERE pr.nama = ?;)-");

  q.addBindValue(uniqueName);
  if(q.exec() && q.next()) {
    if(q.value(0).toInt() > 0) {
      _lastError = QString("Produk '%1' tidak dapat dihapus karena terkait beberapa Penjualan");
      return false;
    }
  }

  // tidak ada penjualan terkait
  q.prepare("DELETE FROM Produk WHERE nama = ?");
  q.addBindValue(uniqueName);
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      _lastError = q.lastError().text();
      return false;
    }
  }
  return q.numRowsAffected() > 0;
}

quint64 Database::produkIdFromName(const QString& uniqueName) const {
  QSqlQuery q;
  q.prepare("SELECT id FROM Produk WHERE nama = ?");
  q.addBindValue(uniqueName);
  if(q.exec() && q.next()) {
    return q.value("id").toLongLong();
  }
  return 0;
}

bool Database::setProdukPrice(const QString& name, int price) {
  if(hasError()) {
    return false;
  }
  QSqlQuery q;
  q.prepare("UPDATE Produk SET base_price = ? WHERE nama = ?");
  q.addBindValue(price);
  q.addBindValue(name);
  if(!q.exec()) {
    _lastError = q.lastError().text();
    return false;
  }
  return true;
}

bool Database::setProdukName(const QString& from, const QString& to) {
  if(hasError())
    return false;
  QSqlQuery q;
  q.prepare("SELECT COUNT(nama) FROM Produk WHERE nama = ?");
  q.addBindValue(from);
  if( ! (q.exec() && q.next()) ) {
    return false;
  }
  if(q.value(0).toInt() < 1)
    return false;

  q.prepare("UPDATE Produk SET nama = ? WHERE nama = ?");
  q.addBindValue(to);
  q.addBindValue(from);
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      _lastError = q.lastError().text();
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

bool Database::addStock(const QString& nama, quint64 stock) {
  if(hasError())
    return false;
  QSqlQuery q;
  q.prepare("SELECT COUNT(id) AS CNT FROM Produk WHERE nama = ?");
  q.addBindValue(nama);
  if(q.exec() && q.next()) {
    if(q.value(0).toInt() < 1) {
      _lastError = QString("Produk '%1' harus didaftarkan terlebih dahulu sebelum dapat mengupdate stoknya");
      return false;
    }
  } else {
    if(q.lastError().isValid()) {
      _lastError = q.lastError().text();
      return false;
    }
  }
  q.prepare("UPDATE Produk SET stock = stock + ? WHERE nama = ?");
  q.addBindValue(stock);
  q.addBindValue(nama);
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      _lastError = q.lastError().text();
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

quint64 Database::produkBasePrice(const QString& nama) const {
  QSqlQuery q;
  q.prepare("SELECT base_price FROM Produk WHERE nama = ?");
  q.addBindValue(nama);
  q.exec();
  if(!q.next()) {
    return 0;
  }
  return q.value(0).toLongLong();
}

quint64 Database::produkStock(const QString& nama) const {
  QSqlQuery q;
  q.prepare("SELECT stock FROM Produk WHERE nama = ?");
  q.addBindValue(nama);
  q.exec();
  if(!q.next())
    return 0;
  return q.value(0).toLongLong();
}

// END CRUD PRODUK

// CRUD KONSUMEN
bool Database::addKonsumen(const QString& nama, const QString& phone, const QString& info)
{
  if(hasError()) 
    return false;
  QSqlQuery q;
  q.prepare("INSERT INTO Konsumen (nama, phone, info) VALUES (?, ?, ?);");
  q.addBindValue(nama);
  q.addBindValue(phone);
  q.addBindValue(info);
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      _lastError = q.lastError().text();
      return false;
    }
  }
  return true;
}

bool Database::removeKonsumen(const QString& nama)
{
  if(hasError())
    return false;
  QSqlQuery q;
  q.prepare(R"--(
    SELECT COUNT(i.id) AS CNT 
    FROM Invoice i
    JOIN Konsumen k ON i.konsumen_id = k.id 
    WHERE k.nama = ?;)--");

  q.addBindValue(nama);
  if(!q.exec() || !q.next()) {
    if(q.lastError().isValid()) {
      _lastError = q.lastError().text();
      return false;
    }
  }
  if(q.value(0).toInt() > 0) {
    _lastError = QString("Konsumen '%1' tidak dapat dihapus karena terkait dengan beberapa Invoice").arg(nama);
    return false;
  }
  q.prepare(R"-(
    DELETE FROM Konsumen 
    WHERE nama = ?;)-");
  q.addBindValue(nama);
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      _lastError = q.lastError().text();
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

bool Database::setKonsumenInfo(const QString& nama, const QString& info)
{
  if(hasError())
    return false;
  QSqlQuery q;
  q.prepare(R"-(
    UPDATE Konsumen 
    SET info = ?
    WHERE nama = ?;
      )-");
  q.addBindValue(info);
  q.addBindValue(nama);
  
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      _lastError = q.lastError().text();
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

bool Database::setKonsumenPhone(const QString& nama, const QString& phone)
{
  if(hasError())
    return false;
  QSqlQuery q;
  q.prepare(R"-(
    UPDATE Konsumen 
    SET phone = ?
    WHERE nama = ?;
      )-");
  q.addBindValue(phone);
  q.addBindValue(nama);
  
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      _lastError = q.lastError().text();
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

bool Database::setNamaKonsumen(const QString& nama_old, const QString& nama_new)
{
  if(hasError())
    return false;
  QSqlQuery q;
  q.prepare(R"-(
    UPDATE Konsumen 
    SET nama = ?
    WHERE nama = ?;
      )-");
  q.addBindValue(nama_new);
  q.addBindValue(nama_old);
  
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      _lastError = q.lastError().text();
    }
    return false;
  }
  return q.numRowsAffected() > 0;
}

// END CRUD KONSUMEN

// CRUD Penjualan 
const AddPenjualanResult Database::addPenjualan(const QStringList produkList, 
          const QList<int> qtyList, 
          const QList<int> priceList, bool* ok)
{
  AddPenjualanResult result;
  if(ok) *ok = false;
  if(hasError() || produkList.count() < 1) {
    qDebug() << "DB Error: " << _lastError;
    return result;
  }
  
  int plc = produkList.count();
  if(qtyList.count() != plc || priceList.count() != plc) {
    result.errorMessage = "Jumlah parameter addPenjualan tidak sama";
    qDebug() << "Err: " << result.errorMessage;
    return result;
  }

  Transaction tr;
  QSqlQuery q;

  // Mendapatkan id produk untuk setiap nama produk
  QList<quint64> produkIds;
  
  for(int i = 0; i < produkList.count(); ++i) {
    auto pr = produkList.at(i);
    auto pifn = produkIdFromName(pr);
    if(pifn < 1) {
      result.errorMessage = QString("Tidak dapat menemukan ID Produk untuk '%1'").arg(pr);
      qDebug() << "Err: " << result.errorMessage;
      return result;
    } else {
      produkIds << pifn;
    }
    if(produkBasePrice(pr) > priceList.at(i)) {
      result.errorMessage = QString("Penjualan dibawah harga standar tidak diizinkan, Produk : '%1'").arg(pr);
      qDebug() << "Err: " << result.errorMessage;
      return result;
    }
    if(produkStock(pr) < qtyList.at(i)) {
      result.errorMessage = QString("Tidak dapat menjual bahan melewati ketersediaan stock, Produk : '%1'").arg(pr);
      qDebug() << "Err: " << result.errorMessage;
      return result;
    }
  }
  
  // Check Passed
  
  QList<quint64> insertIds {};
  QString produk, prepareStatement;
  int price, qty;
  
  prepareStatement = "INSERT INTO Penjualan (produk_id, harga_jual, qty, harga_total) VALUES (?, ?, ?, ?);";
  
  for(int c=0; c < produkList.count(); c++) {
    price = priceList.at(c);
    qty = qtyList.at(c);
    q.prepare(prepareStatement);
    q.addBindValue(produkIds.at(c));
    q.addBindValue(price);
    q.addBindValue(qty);
    q.addBindValue(price * qty);
    if(!q.exec()) {
      if(q.lastError().isValid()) {
        result.errorMessage = q.lastError().text();
      } else {
        result.errorMessage = QString("Tidak dapat menyimpan penjualan untuk %1 %2 dengan harga %3")
                  .arg(qty)
                  .arg(produkList.at(c))
                  .arg(price);
      }
      qDebug() << "Err: " << result.errorMessage;
      return result;
    }
    
    insertIds << q.lastInsertId().toLongLong();
    
    q.prepare("UPDATE Produk SET stock = stock - ? WHERE id = ?;");
    q.addBindValue(qty);
    q.addBindValue(produkIds.at(c));
    if(!q.exec()) {
      if (q.lastError().isValid()) {
        result.errorMessage = q.lastError().text();
      } else {
        result.errorMessage = "Gagal mengupdate stock";
      }
      qDebug() << "Err: " << result.errorMessage;
      return result;
    }
  }
  if(!tr.commit()) {
    result.errorMessage = "Tidak dapat melakukan COMMIT";
    qDebug() << "Err: " << result.errorMessage;
    return result;
  }
  if(ok) *ok = true;
  result.success = true;
  result.penjualanIds = insertIds;
  return result;
}

const DatabaseResult Database::setPenjualanPrice(quint64 pjid, int newPrice, bool modifyInvoiced)
{
  // Merubah harga penjualan 
  // pjid -> id penjualan
  // newPrice -> harga baru
  // modifyInvoiced -> ubah meskipun telah memiliki invoice
  
  DatabaseResult result;
  
  Transaction tr;
  QSqlQuery q;
  
  // cek existensi penjualan
  q.prepare("SELECT * FROM Penjualan WHERE id = ?;");
  q.addBindValue(pjid);
  
  if(!q.exec() || !q.next()) {
    result.errorMessage = "Data penjualan tidak ditemukan";
    return result;
  }
  
  int currentPrice = q.value("harga_jual").toInt();
  int currentQty = q.value("qty").toInt();
  int currentTotal = q.value("harga_total").toInt();
  int newTotal = newPrice * currentQty;
  
  quint64 produk_id = q.value("produk_id").toULongLong();
  bool hasInvoice = !q.value("invoice_id").isNull();
  quint64 invoice_id = q.value("invoice_id").toULongLong();
  
  if(currentPrice == newPrice) {
    result.errorMessage = "Tidak ada perubahan yang diperlukan";
    return result;
  }
  
  q.prepare("UPDATE Penjualan SET (harga_jual, harga_total) = (:hj, :ht) WHERE id = :pid;");
  q.bindValue(":hj", newPrice);
  q.bindValue(":ht", newTotal);
  q.bindValue(":pid", pjid);
  
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      result.errorMessage = q.lastError().text();
    } else {
      result.errorMessage = "Update Penjualan Gagal";
    }
    return result;
  }
  
  if(hasInvoice) { // berinvoice
    if(!modifyInvoiced) { // jangan ubah invoice
      result.errorMessage = "Penjualan telah memiliki invoice";
      return result;
    }
    
    q.prepare("SELECT * FROM Invoice WHERE id = ?");
    q.addBindValue(invoice_id);
    if(!q.exec() || !q.next()) {
      if(q.lastError().isValid()) {
        result.errorMessage = q.lastError().text();
      } else {
        result.errorMessage = QString("Tidak dapat menemukan data Invoice (ID : %1)").arg(invoice_id);
      }
      return result;
    }
    
    int inv_paid = q.value("paid").toInt();
    int inv_total = q.value("total_value").toInt() - currentTotal + newTotal;
    int inv_unpaid = inv_total - inv_paid;
    
    q.prepare("UPDATE Invoice SET (total_value, unpaid) = (:tv, :unp) WHERE id = :iid;");
    q.bindValue(":tv", inv_total);
    q.bindValue(":unp", inv_unpaid);
    q.bindValue(":iid", invoice_id);
    
    if(!q.exec()) {
      if(q.lastError().isValid()) {
        result.errorMessage = q.lastError().text();
      } else {
        result.errorMessage = "Tidak dapat mengupdate Invoice";
      }
      return result;
    }
  }
    
  if(!tr.commit()) {
    result.errorMessage = "Tidak dapat melakukan COMMIT;";
    return result;
  }
  result.success = true;
  return result;
}

const DatabaseResult Database::setPenjualanQty(quint64 pjid, int newQty, bool modifyInvoiced)
{
  // Merubah harga penjualan 
  // pjid -> id penjualan
  // newQty -> qty baru
  // modifyInvoiced -> ubah meskipun telah memiliki invoice
  
  DatabaseResult result;
  
  Transaction tr;
  QSqlQuery q;
  
  // cek existensi penjualan
  q.prepare("SELECT * FROM Penjualan WHERE id = ?;");
  q.addBindValue(pjid);
  
  if(!q.exec() || !q.next()) {
    result.errorMessage = "Data penjualan tidak ditemukan";
    return result;
  }
  
  int currentPrice = q.value("harga_jual").toInt();
  int currentQty = q.value("qty").toInt();
  int currentTotal = q.value("harga_total").toInt();
  int newTotal = newQty * currentPrice;
  
  quint64 produk_id = q.value("produk_id").toULongLong();
  bool hasInvoice = !q.value("invoice_id").isNull();
  quint64 invoice_id = q.value("invoice_id").toULongLong();
  
  if(currentQty == newQty) {
    result.errorMessage = "Tidak ada perubahan yang diperlukan";
    return result;
  }
  
  // Cek Stock saat ini
  q.prepare("SELECT * FROM Produk WHERE id = ?");
  q.addBindValue(produk_id);
  if(!q.exec() || !q.next()) {
    if(q.lastError().isValid()) {
      result.errorMessage = q.lastError().text();
    } else {
      result.errorMessage = "Tidak dapat menemukan data produk";
    }
    return result;
  }
  int currentStock = q.value("stock").toInt();
  int newStock = currentStock + currentQty - newQty;
  if( newStock < 0 ) {
    result.errorMessage = "Tidak dapat merubah Qty karena stock Produk tidak cukup";
    return result;
  }
  
  q.prepare("UPDATE Produk SET stock = :ns WHERE id = :pid");
  q.bindValue(":ns", newStock);
  q.bindValue(":pid", produk_id);
  
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      result.errorMessage = q.lastError().text();
    } else {
      result.errorMessage = "Tidak dapat mengupdate Stock";
    }
    return result;
  }
  
  q.prepare("UPDATE Penjualan SET (qty, harga_total) = (:qty, :ht) WHERE id = :pid;");
  q.bindValue(":qty", newQty);
  q.bindValue(":ht", newTotal);
  q.bindValue(":pid", pjid);
  
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      result.errorMessage = q.lastError().text();
    } else {
      result.errorMessage = "Update Penjualan Gagal";
    }
    return result;
  }
  
  if(hasInvoice) { // berinvoice
    if(!modifyInvoiced) { // jangan ubah invoice
      result.errorMessage = "Penjualan telah memiliki invoice";
      return result;
    }
    
    q.prepare("SELECT * FROM Invoice WHERE id = ?");
    q.addBindValue(invoice_id);
    if(!q.exec() || !q.next()) {
      if(q.lastError().isValid()) {
        result.errorMessage = q.lastError().text();
      } else {
        result.errorMessage = QString("Tidak dapat menemukan data Invoice (ID : %1)").arg(invoice_id);
      }
      return result;
    }
    
    int inv_paid = q.value("paid").toInt();
    int inv_total = q.value("total_value").toInt() - currentTotal + newTotal;
    int inv_unpaid = inv_total - inv_paid;
    
    q.prepare("UPDATE Invoice SET (total_value, unpaid) = (:tv, :unp) WHERE id = :iid;");
    q.bindValue(":tv", inv_total);
    q.bindValue(":unp", inv_unpaid);
    q.bindValue(":iid", invoice_id);
    
    if(!q.exec()) {
      if(q.lastError().isValid()) {
        result.errorMessage = q.lastError().text();
      } else {
        result.errorMessage = "Tidak dapat mengupdate Invoice";
      }
      return result;
    }
  }
    
  if(!tr.commit()) {
    result.errorMessage = "Tidak dapat melakukan COMMIT;";
    return result;
  }
  result.success = true;
  return result;
}

const DatabaseResult Database::removePenjualan(quint64 pjid, bool force)
{
  DatabaseResult res;
  Transaction tr;
  QSqlQuery q;
  
  q.prepare("SELECT * FROM Penjualan WHERE id = ?");
  q.addBindValue(pjid);
  
  if(!q.exec() || !q.next()) {
    res.errorMessage = QString("Tidak dapat menemukan data Penjualan (ID %1)").arg(pjid);
    return res;
  }
  
  bool hasInvoice = !q.value("invoice_id").isNull();
  auto invoice_id = q.value("invoice_id").toULongLong();
  int harga_total = q.value("harga_total").toInt();
  
  q.prepare("DELETE FROM Penjualan WHERE id = ?");
  q.addBindValue(pjid);
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      res.errorMessage = q.lastError().text();
    } else {
      res.errorMessage = "Penghapusan Penjualan Gagal";
    }
    return res;
  }

  if(hasInvoice) {
    if(!force) {
      res.errorMessage = QString("Data penjualan memilikin Invoice terkait");
      return res;
    }
    q.prepare("SELECT * FROM Invoice WHERE id = ?");
    q.addBindValue("invoice_id");
    if(!q.exec() || !q.next()) {
      if(q.lastError().isValid()) {
        res.errorMessage = q.lastError().text();
      } else {
        res.errorMessage = QString("Tidak dapat menemukan Invoice (ID %1)").arg(invoice_id);
      }
      return res;
    }
    
    int inv_paid = q.value("paid").toInt();
    int inv_unpaid = q.value("unpaid").toInt();
    int inv_total = q.value("total_value").toInt();
    int update_total = inv_total - harga_total;
    if(inv_paid > update_total) {
      // jadi lebih pembayaran
      res.errorMessage = "Terjadi pembayaran berlebih jika penjualan dihapus, "
                         "jika benar-benar diperlukan anda bisa mensiasatinya dengan menghapus pembayaran terlebuh dahulu";
      return res;
    }
    
    q.prepare("UPDATE Invoice SET (total_value, unpaid) = (:tv, :up) WHERE id = :iid");
    q.bindValue(":tv", update_total);
    q.bindValue(":up", update_total - inv_paid);
    q.bindValue(":iid", invoice_id);
    
    if(!q.exec()) {
      if(q.lastError().isValid()) {
        res.errorMessage = q.lastError().text();
      } else {
        res.errorMessage = QString("Tidak dapat melakukan UPDATE Invoice (ID %1)").arg(invoice_id);
      }
      return res;
    }
  }
    
  if(!tr.commit()) {
    res.errorMessage = "Tidak dapat melakukan COMMIT : " + QSqlDatabase::database().lastError().text();
    return res;
  }
  res.success = true;
  return res;
}


// CRUD Invoice
const CreateInvoiceResult Database::createInvoice(QList<quint64> penjualan, const QString& konsumen, bool* ok)
{
  CreateInvoiceResult result;
  if(ok) *ok = false;
  if(hasError()) {
    result.errorMessage = QString("Error sebelumnya belum ditangani: %1").arg(_lastError);
    return result;
  }
  
  Transaction tr;
  QSqlQuery q;
  
  q.prepare("SELECT id FROM Konsumen WHERE nama = ?");
  q.addBindValue(konsumen);
  if(!q.exec() || !q.next() ) {
    result.errorMessage = QString("Konsumen dengan nama : '%1' belum terdaftar").arg(konsumen);
    qDebug() << "Err: " << result.errorMessage;
    return result;
  }
  
  int k_id = q.value(0).toInt();
  int total_value = 0,
      unpaid = 0;
  
  for(quint64 p_id : penjualan) {
    q.prepare("SELECT * FROM Penjualan WHERE id = ? LIMIT 1");
    q.addBindValue(p_id);
    if( !q.exec() || !q.next() ) {
      result.errorMessage = QString("Tidak dapat menemukan Penjualan dengan ID : %1").arg(p_id);
      qDebug() << "Err: " << result.errorMessage;
      return result;
    }
    total_value += q.value("harga_total").toInt();
  }
  
  q.prepare("INSERT INTO Invoice (konsumen_id, total_value, paid, unpaid) VALUES (:konid, :toval, :paid, :toval);");
  q.bindValue(":konid", k_id);
  q.bindValue(":toval", total_value);
  q.bindValue(":paid", 0);
  if(!q.exec()) {
    if(q.lastError().isValid()) {
      result.errorMessage = q.lastError().text();
    } else {
      result.errorMessage = "Eksekusi Inser Invoice Gagal";
    }
    qDebug() << "Err: " << result.errorMessage;
    return result;
  }
  
  int inv_id = q.lastInsertId().toInt();
  
  for(quint64 p_id : penjualan) {
    q.prepare("UPDATE Penjualan SET invoice_id = :inv_id WHERE id = :p_id;");
    q.bindValue(":inv_id", inv_id);
    q.bindValue(":p_id", p_id);
    if(!q.exec()) {
      if(q.lastError().isValid()) {
        result.errorMessage = q.lastError().text();
      } else {
        result.errorMessage = "Gagal saat mengupdate invoice_id pada setiap penjualan";
      }
      qDebug() << "Err: " << result.errorMessage;
      return result;
    }
  }
  
  if(!tr.commit()) {
    result.errorMessage = "Tidak dapat melakukan COMMIT pada database";
    qDebug() << "Err: " << result.errorMessage;
    return result;
  }
  
  if(ok) *ok = true;
  result.success = true;
  result.invoiceId = inv_id;
  return result;
}
