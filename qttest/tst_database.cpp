// test/tst_database.cpp
#include <QtTest>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include "database.h"

class TestDatabase : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testAddAndGetProduk();
    void testStockUpdate();
    void testPenjualanValidation();
    void testCreateInvoice();
    void testRemoveWithDependency();
    void testErrorHandling();

private:
  QString testPath;
  QCoreApplication* core;
};

void TestDatabase::initTestCase()
{
    // Init Resources
    Q_INIT_RESOURCE(main_res);
    
    // Setup path sementara
    testPath = QDir::tempPath() + "/kdb_test_" + QString::number(QDateTime::currentMSecsSinceEpoch());
    QDir().mkpath(testPath);

    // Pastikan resource ada
    QVERIFY(QFile::exists(":/Database/KDB.db"));
}

void TestDatabase::cleanupTestCase()
{
    // Hapus folder sementara
    QDir(testPath).removeRecursively();
}

void TestDatabase::testAddAndGetProduk()
{
    Database db(testPath);
    QVERIFY2(db.initialized(), "Initialized True");
    
    QVERIFY2(db.addProduk("Espresso", 18000, 50), "Add OK");
    QVERIFY2(!db.hasError(), "No Errors");

    QCOMPARE(db.produkStock("Espresso"), quint64(50));
    QCOMPARE(db.produkBasePrice("Espresso"), quint64(18000));
    
}

void TestDatabase::testStockUpdate()
{
    Database db(testPath);
    QVERIFY(db.initialized());

    db.addProduk("Latte", 20000, 30);
    QVERIFY(db.addStock("Latte", 25));
    QCOMPARE(db.produkStock("Latte"), quint64(55));
}

void TestDatabase::testPenjualanValidation()
{
    Database db(testPath);
    QVERIFY(db.initialized());
    
    db.addProduk("Cappuccino", 17000, 10);

    QStringList produk = {"Cappuccino"};
    QList<int> qty = {5};
    QList<int> harga = {18000};  // > base price

    bool ok;
    auto result = db.addPenjualan(produk, qty, harga, &ok);
    QVERIFY(result.success);
    QCOMPARE(result.penjualanIds.size(), 1);
    QCOMPARE(db.produkStock("Cappuccino"), quint64(5));

    // Test: harga jual < base price
    QList<int> hargaRendah = {16000};
    auto result2 = db.addPenjualan(produk, qty, hargaRendah, &ok);
    QVERIFY(!result2.success);
    QVERIFY(result2.errorMessage.contains("dibawah harga standar"));

    // Test: qty > stock
    QList<int> qtyBanyak = {10};
    auto result3 = db.addPenjualan(produk, qtyBanyak, harga, &ok);
    QVERIFY(!result3.success);
    QVERIFY(result3.errorMessage.contains("melewati ketersediaan stock"));
}

void TestDatabase::testCreateInvoice()
{
    Database db(testPath);
    QVERIFY(db.initialized());

    db.addProduk("Americano", 15000, 20);
    db.addKonsumen("Andi", "08123", "VIP");

    QStringList produk = {"Americano"};
    QList<int> qty = {2};
    QList<int> harga = {16000};

    bool ok;
    auto presult = db.addPenjualan(produk, qty, harga, &ok);
    QVERIFY(presult.success);
    auto ciresult = db.createInvoice(presult.penjualanIds, "Andi", &ok);
    QVERIFY(ciresult.success);
    QVERIFY(ciresult.invoiceId > 0);
}

void TestDatabase::testRemoveWithDependency()
{
    Database db(testPath);
    QVERIFY(db.initialized());

    db.addProduk("Mocha", 19000, 15);
    db.addKonsumen("Cici");

    // Buat penjualan
    bool ok;
    auto presult = db.addPenjualan({"Mocha"}, {1}, {20000}, &ok);
    QVERIFY(presult.success);

    db.createInvoice(presult.penjualanIds, "Cici", &ok);

    // Coba hapus produk → harus gagal
    QVERIFY(!db.removeProduk("Mocha"));
    QVERIFY(db.lastError().contains("terkait beberapa Penjualan"));
    db.resetError();
    
    // Coba hapus konsumen → harus gagal
    QVERIFY(!db.removeKonsumen("Cici"));
    QVERIFY(db.lastError().contains("terkait dengan beberapa Invoice"));
    db.resetError();
}

void TestDatabase::testErrorHandling()
{
    Database db(testPath);
    QVERIFY(db.initialized());

    // Produk tidak ada
    QVERIFY(!db.addStock("TidakAda", 10));
    QVERIFY(db.lastError().contains("harus didaftarkan terlebih dahulu"));
    
    db.resetError();
    
    // Konsumen tidak ada
    QList<quint64> dummy;
    auto ciresult = db.createInvoice(dummy, "TidakAda");
    QVERIFY(!ciresult.success);
    QVERIFY(ciresult.errorMessage.contains("belum terdaftar"));
}

QTEST_MAIN(TestDatabase)
#include "tst_database.moc"