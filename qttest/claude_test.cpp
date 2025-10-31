#include <QtTest>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QTemporaryDir>
#include <QFile>
#include "database.h"

class ClaudeTest : public QObject
{
    Q_OBJECT

private:
    Database* db;
    QTemporaryDir* tempDir;
    QString testDbPath;

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    // Test CRUD Produk
    void testAddProduk();
    void testAddProdukDuplicate();
    void testRemoveProduk();
    void testRemoveProdukWithPenjualan();
    void testSetProdukPrice();
    void testSetProdukName();
    void testAddStock();
    void testProdukHelpers();

    // Test CRUD Konsumen
    void testAddKonsumen();
    void testAddKonsumenDuplicate();
    void testRemoveKonsumen();
    void testRemoveKonsumenWithInvoice();
    void testSetKonsumenInfo();
    void testSetKonsumenPhone();
    void testSetNamaKonsumen();

    // Test CRUD Penjualan
    void testAddPenjualan();
    void testAddPenjualanInvalidData();
    void testAddPenjualanBelowPrice();
    void testAddPenjualanInsufficientStock();
    void testSetPenjualanPrice();
    void testSetPenjualanPriceWithInvoice();
    void testSetPenjualanQty();
    void testSetPenjualanQtyInsufficientStock();
    void testRemovePenjualan();
    void testRemovePenjualanWithInvoice();

    // Test CRUD Invoice
    void testCreateInvoice();
    void testCreateInvoiceInvalidKonsumen();
    void testCreateInvoiceInvalidPenjualan();

private:
    void createTestSchema();
    void clearTestData();
};

void ClaudeTest::initTestCase()
{
    // Setup temporary directory untuk database test
    tempDir = new QTemporaryDir();
    QVERIFY(tempDir->isValid());
    testDbPath = tempDir->path();
    qApp->setProperty("appDataPath", testDbPath);
    Q_INIT_RESOURCE(main_res);
}

void ClaudeTest::cleanupTestCase()
{
    delete tempDir;
}

void ClaudeTest::init()
{
    // Create test schema sebelum setiap test
    // createTestSchema();
    
    // Initialize database
    db = new Database(testDbPath);
    QVERIFY(db->initialized());
}

void ClaudeTest::cleanup()
{
    delete db;
    clearTestData();
}

void ClaudeTest::createTestSchema()
{
    QString dbFile = testDbPath + "/KDB.db";
    
    QSqlDatabase testDb = QSqlDatabase::addDatabase("QSQLITE", "test_connection");
    testDb.setDatabaseName(dbFile);
    QVERIFY(testDb.open());

    QSqlQuery q(testDb);
    
    // Create tables
    QVERIFY(q.exec("CREATE TABLE IF NOT EXISTS Produk ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "nama TEXT UNIQUE NOT NULL, "
                   "stock INTEGER NOT NULL DEFAULT 0, "
                   "base_price INTEGER NOT NULL DEFAULT 0)"));

    QVERIFY(q.exec("CREATE TABLE IF NOT EXISTS Konsumen ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "nama TEXT UNIQUE NOT NULL, "
                   "phone TEXT, "
                   "info TEXT)"));

    QVERIFY(q.exec("CREATE TABLE IF NOT EXISTS Penjualan ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "produk_id INTEGER NOT NULL, "
                   "harga_jual INTEGER NOT NULL, "
                   "qty INTEGER NOT NULL, "
                   "harga_total INTEGER NOT NULL, "
                   "invoice_id INTEGER, "
                   "FOREIGN KEY(produk_id) REFERENCES Produk(id), "
                   "FOREIGN KEY(invoice_id) REFERENCES Invoice(id))"));

    QVERIFY(q.exec("CREATE TABLE IF NOT EXISTS Invoice ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "konsumen_id INTEGER NOT NULL, "
                   "total_value INTEGER NOT NULL, "
                   "paid INTEGER NOT NULL DEFAULT 0, "
                   "unpaid INTEGER NOT NULL, "
                   "FOREIGN KEY(konsumen_id) REFERENCES Konsumen(id))"));

    QVERIFY(q.exec("PRAGMA foreign_keys = ON"));
    
    testDb.close();
    QSqlDatabase::removeDatabase("test_connection");
}

void ClaudeTest::clearTestData()
{
    QString dbFile = testDbPath + "/KDB.db";
    QFile::remove(dbFile);
}

// ========== TEST PRODUK ==========

void ClaudeTest::testAddProduk()
{
  QVERIFY(db->addProduk("Produk A", 10000, 100));
  QCOMPARE(db->produkStock("Produk A"), (quint64)100);
  QCOMPARE(db->produkBasePrice("Produk A"), (quint64)10000);
  QVERIFY(db->produkIdFromName("Produk A") > 0);
}

void ClaudeTest::testAddProdukDuplicate()
{
  QVERIFY(db->addProduk("Produk B", 10000, 100));
  QVERIFY(!db->addProduk("Produk B", 15000, 50));
}

void ClaudeTest::testRemoveProduk()
{
  QVERIFY(db->removeProduk("Produk B"));
  QCOMPARE(db->produkIdFromName("Produk B"), (quint64) 0);
}

void ClaudeTest::testRemoveProdukWithPenjualan()
{
    // Setup
    db->addProduk("Produk C", 10000, 100);
    db->addKonsumen("Konsumen A", "081234567890", "Test");
    
    QStringList produkList = {"Produk C"};
    QList<int> qtyList = {5};
    QList<int> priceList = {10000};
    bool ok;
    db->addPenjualan(produkList, qtyList, priceList);
    
    // Test
    QVERIFY(!db->removeProduk("Produk C"));
    QVERIFY(db->produkIdFromName("Produk C") > 0);
}

void ClaudeTest::testSetProdukPrice()
{
    db->addProduk("Produk D", 10000, 100);
    QVERIFY(db->setProdukPrice("Produk D", 15000));
    QCOMPARE(db->produkBasePrice("Produk D"), (quint64)15000);
}

void ClaudeTest::testSetProdukName()
{
    db->addProduk("Produk E", 10000, 100);
    QVERIFY(db->setProdukName("Produk E", "Produk E Renamed"));
    QCOMPARE(db->produkIdFromName("Produk E"), (quint64)0);
    QVERIFY(db->produkIdFromName("Produk E Renamed") > 0);
}

void ClaudeTest::testAddStock()
{
    db->addProduk("Produk F", 10000, 100);
    QVERIFY(db->addStock("Produk F", 50));
    QCOMPARE(db->produkStock("Produk F"), (quint64)150);
}

void ClaudeTest::testProdukHelpers()
{
    db->addProduk("Produk G", 25000, 75);
    
    QVERIFY(db->produkIdFromName("Produk G") > 0);
    QCOMPARE(db->produkBasePrice("Produk G"), (quint64)25000);
    QCOMPARE(db->produkStock("Produk G"), (quint64)75);
    
    // Test non-existent
    QCOMPARE(db->produkIdFromName("Non Existent"), (quint64)0);
    QCOMPARE(db->produkBasePrice("Non Existent"), (quint64)0);
    QCOMPARE(db->produkStock("Non Existent"), (quint64)0);
}

// ========== TEST KONSUMEN ==========

void ClaudeTest::testAddKonsumen()
{
    QVERIFY(db->addKonsumen("John Doe", "081234567890", "Customer VIP"));
}

void ClaudeTest::testAddKonsumenDuplicate()
{
    QVERIFY(db->addKonsumen("Jane Doe", "081234567890", "Regular"));
    QVERIFY(!db->addKonsumen("Jane Doe", "089876543210", "Another"));
}

void ClaudeTest::testRemoveKonsumen()
{
    db->addKonsumen("Remove Me", "081234567890", "Test");
    QVERIFY(db->removeKonsumen("Remove Me"));
}

void ClaudeTest::testRemoveKonsumenWithInvoice()
{
    // Setup
    db->addProduk("Produk H", 10000, 100);
    db->addKonsumen("Konsumen B", "081234567890", "Test");
    
    QStringList produkList = {"Produk H"};
    QList<int> qtyList = {5};
    QList<int> priceList = {10000};
    
    auto penjualanResult = db->addPenjualan(produkList, qtyList, priceList);
    QCOMPARE(penjualanResult.success, true);
    
    auto invoiceResult = db->createInvoice(penjualanResult.penjualanIds, "Konsumen B");
    QCOMPARE(invoiceResult.success, true);
    
    // Test
    QVERIFY(!db->removeKonsumen("Konsumen B"));
}

void ClaudeTest::testSetKonsumenInfo()
{
    db->addKonsumen("Konsumen C", "081234567890", "Old Info");
    QVERIFY(db->setKonsumenInfo("Konsumen C", "New Info"));
}

void ClaudeTest::testSetKonsumenPhone()
{
    db->addKonsumen("Konsumen D", "081234567890", "Test");
    QVERIFY(db->setKonsumenPhone("Konsumen D", "089876543210"));
}

void ClaudeTest::testSetNamaKonsumen()
{
    db->addKonsumen("Konsumen E", "081234567890", "Test");
    QVERIFY(db->setNamaKonsumen("Konsumen E", "Konsumen E Renamed"));
}

// ========== TEST PENJUALAN ==========

void ClaudeTest::testAddPenjualan()
{
    // Setup
    db->addProduk("Produk I", 10000, 100);
    db->addProduk("Produk J", 15000, 50);
    
    QStringList produkList = {"Produk I", "Produk J"};
    QList<int> qtyList = {10, 5};
    QList<int> priceList = {12000, 18000};
    
    auto result = db->addPenjualan(produkList, qtyList, priceList);
    
    QVERIFY(result.success);
    QCOMPARE(result.penjualanIds.count(), 2);
    
    // Verify stock reduction
    QCOMPARE(db->produkStock("Produk I"), (quint64)90);
    QCOMPARE(db->produkStock("Produk J"), (quint64)45);
}

void ClaudeTest::testAddPenjualanInvalidData()
{
    db->addProduk("Produk K", 10000, 100);
    
    QStringList produkList = {"Produk K"};
    QList<int> qtyList = {10, 5}; // Mismatch count
    QList<int> priceList = {12000};
    
    auto result = db->addPenjualan(produkList, qtyList, priceList);
    
    QVERIFY(!result.success);
    QVERIFY(!result.errorMessage.isEmpty());
}

void ClaudeTest::testAddPenjualanBelowPrice()
{
    db->addProduk("Produk L", 10000, 100);
    
    QStringList produkList = {"Produk L"};
    QList<int> qtyList = {10};
    QList<int> priceList = {9000}; // Below base price
    
    auto result = db->addPenjualan(produkList, qtyList, priceList);
    
    QVERIFY(!result.success);
    QVERIFY(result.errorMessage.contains("dibawah harga standar"));
}

void ClaudeTest::testAddPenjualanInsufficientStock()
{
    db->addProduk("Produk M", 10000, 10);
    
    QStringList produkList = {"Produk M"};
    QList<int> qtyList = {20}; // More than available
    QList<int> priceList = {10000};
    
    auto result = db->addPenjualan(produkList, qtyList, priceList);
    
    QVERIFY(!result.success);
    QVERIFY(result.errorMessage.contains("stock"));
}

void ClaudeTest::testSetPenjualanPrice()
{
    // Setup
    db->addProduk("Produk N", 10000, 100);
    QStringList produkList = {"Produk N"};
    QList<int> qtyList = {10};
    QList<int> priceList = {10000};
    
    auto penjualanResult = db->addPenjualan(produkList, qtyList, priceList);
    quint64 pjid = penjualanResult.penjualanIds.first();
    
    // Test
    auto result = db->setPenjualanPrice(pjid, 12000, false);
    QVERIFY(result.success);
}

void ClaudeTest::testSetPenjualanPriceWithInvoice()
{
    // Setup
    db->addProduk("Produk O", 10000, 100);
    db->addKonsumen("Konsumen F", "081234567890", "Test");
    
    QStringList produkList = {"Produk O"};
    QList<int> qtyList = {10};
    QList<int> priceList = {15000};
    
    auto penjualanResult = db->addPenjualan(produkList, qtyList, priceList);
    quint64 pjid = penjualanResult.penjualanIds.first();
    
    db->createInvoice(penjualanResult.penjualanIds, "Konsumen F");
    
    // Test without modifyInvoiced
    auto result1 = db->setPenjualanPrice(pjid, 12000, false);
    QVERIFY(!result1.success);
    QVERIFY(result1.errorMessage.contains("invoice"));
    
    // Test with modifyInvoiced
    auto result2 = db->setPenjualanPrice(pjid, 12000, true);
    QVERIFY(result2.success);
}

void ClaudeTest::testSetPenjualanQty()
{
    // Setup
    db->addProduk("Produk P", 10000, 100);
    QStringList produkList = {"Produk P"};
    QList<int> qtyList = {10};
    QList<int> priceList = {10000};
    
    auto penjualanResult = db->addPenjualan(produkList, qtyList, priceList);
    quint64 pjid = penjualanResult.penjualanIds.first();
    
    // Test increase qty
    auto result = db->setPenjualanQty(pjid, 15, false);
    QVERIFY(result.success);
    QCOMPARE(db->produkStock("Produk P"), (quint64)85);
    
    // Test decrease qty
    auto result2 = db->setPenjualanQty(pjid, 5, false);
    QVERIFY(result2.success);
    QCOMPARE(db->produkStock("Produk P"), (quint64)95);
}

void ClaudeTest::testSetPenjualanQtyInsufficientStock()
{
    // Setup
    db->addProduk("Produk Q", 10000, 20);
    QStringList produkList = {"Produk Q"};
    QList<int> qtyList = {10};
    QList<int> priceList = {10000};
    
    auto penjualanResult = db->addPenjualan(produkList, qtyList, priceList);
    quint64 pjid = penjualanResult.penjualanIds.first();
    
    // Current stock: 10, try to set qty to 50 (needs 40 more)
    auto result = db->setPenjualanQty(pjid, 50, false);
    QVERIFY(!result.success);
    QVERIFY(result.errorMessage.contains("stock"));
}

void ClaudeTest::testRemovePenjualan()
{
    // Setup
    db->addProduk("Produk R", 10000, 100);
    QStringList produkList = {"Produk R"};
    QList<int> qtyList = {10};
    QList<int> priceList = {10000};
    
    auto penjualanResult = db->addPenjualan(produkList, qtyList, priceList);
    quint64 pjid = penjualanResult.penjualanIds.first();
    
    // Test
    auto result = db->removePenjualan(pjid, false);
    QVERIFY(result.success);
}

void ClaudeTest::testRemovePenjualanWithInvoice()
{
    // Setup
    db->addProduk("Produk S", 10000, 100);
    db->addKonsumen("Konsumen G", "081234567890", "Test");
    
    QStringList produkList = {"Produk S"};
    QList<int> qtyList = {10};
    QList<int> priceList = {10000};
    
    auto penjualanResult = db->addPenjualan(produkList, qtyList, priceList);
    quint64 pjid = penjualanResult.penjualanIds.first();
    
    db->createInvoice(penjualanResult.penjualanIds, "Konsumen G");
    
    // Test without force
    auto result1 = db->removePenjualan(pjid, false);
    QVERIFY(!result1.success);
    
    // Test with force
    auto result2 = db->removePenjualan(pjid, true);
    QVERIFY(!result2.success);
}

// ========== TEST INVOICE ==========

void ClaudeTest::testCreateInvoice()
{
    // Setup
    db->addProduk("Produk T", 10000, 100);
    db->addProduk("Produk U", 15000, 50);
    db->addKonsumen("Konsumen H", "081234567890", "Test");
    
    QStringList produkList = {"Produk T", "Produk U"};
    QList<int> qtyList = {10, 5};
    QList<int> priceList = {12000, 18000};
    
    auto penjualanResult = db->addPenjualan(produkList, qtyList, priceList);
    
    // Test
    auto result = db->createInvoice(penjualanResult.penjualanIds, "Konsumen H");
    QVERIFY(result.success);
    QVERIFY(result.invoiceId > 0);
}

void ClaudeTest::testCreateInvoiceInvalidKonsumen()
{
    // Setup
    db->addProduk("Produk V", 10000, 100);
    
    QStringList produkList = {"Produk V"};
    QList<int> qtyList = {10};
    QList<int> priceList = {10000};
    
    auto penjualanResult = db->addPenjualan(produkList, qtyList, priceList);
    
    // Test
    auto result = db->createInvoice(penjualanResult.penjualanIds, "Non Existent Konsumen");
    QVERIFY(!result.success);
    QVERIFY(result.errorMessage.contains("belum terdaftar"));
}

void ClaudeTest::testCreateInvoiceInvalidPenjualan()
{
    // Setup
    db->addKonsumen("Konsumen I", "081234567890", "Test");
    
    QList<quint64> invalidIds = {99999};
    
    // Test
    auto result = db->createInvoice(invalidIds, "Konsumen I");
    QVERIFY(!result.success);
}

QTEST_MAIN(ClaudeTest)
#include "claude_test.moc"