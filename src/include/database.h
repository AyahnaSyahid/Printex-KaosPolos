// 2025-10-28
#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QDateTime>
#include <QMap>
#include <QString>
#include <QMutex>

class Transaction
{
  public:
    explicit Transaction(QSqlDatabase base = QSqlDatabase());
    ~Transaction();
    bool commit();
    
  private:
    bool m_commited;
    QSqlDatabase dft;
};

struct DatabaseResult
{
  bool success = false;
  QString errorMessage;
};

struct AddPenjualanResult : public DatabaseResult
{
  QList<quint64> penjualanIds;
};

struct CreateInvoiceResult : public DatabaseResult
{
  quint64 invoiceId = 0;
};

struct CreatePaymentResult : public DatabaseResult
{
  quint64 paymentId = 0;
};

class Database
{
  public:
    explicit Database(const QString& dataPath="");
    ~Database();

    void initializeDatabase();

    // CRUD PRODUK
    DatabaseResult addProduk(const QString& uniqueName, int price, int stock=0);
    bool removeProduk(const QString& uniqueName);
    quint64 produkIdFromName(const QString& uniqueName) const;
    bool setProdukPrice(const QString& name, int price);
    bool setProdukName(const QString& from, const QString& to);
    bool addStock(const QString& nama, quint64 stock);
    int produkBasePrice(const QString& name) const;
    int produkStock(const QString& name) const;

    // CRUD Konsumen
    bool addKonsumen(const QString& nama, const QString& phone="", const QString& info="");
    bool removeKonsumen(const QString& nama);
    bool setKonsumenInfo(const QString& nama, const QString& info);
    bool setKonsumenPhone(const QString& nama, const QString& phone);
    bool setNamaKonsumen(const QString& old, const QString& _new);

    // CRUD dan penanganan Penjualan
    const AddPenjualanResult addPenjualan(const QStringList produkList, 
          const QList<int> qtyList, 
          const QList<int> priceList);

    const DatabaseResult setPenjualanPrice(quint64 pjid, int newPrice, bool modifyInvoiced = false);
    const DatabaseResult setPenjualanQty(quint64 pjid, int qty, bool modifyInvoiced = false);
    const DatabaseResult removePenjualan(quint64 pjid, bool force=false);

    // CRUD dan penanganan Invoice
    const CreateInvoiceResult createInvoice(QList<quint64> penjualan, const QString& konsumen);
    
    // CRUD Payment
    const CreatePaymentResult createPayment(quint64 invoice_id, 
          int value,
          const QString& info,
          const QDateTime& pay_time = QDateTime::currentDateTime());

    inline const bool& initialized() const { return _success; }

  private:
    QSqlDatabase db;
    bool _success;
    QMap<QString, QSqlQueryModel*> _queryModel;
    QMap<QString, QSqlTableModel*> _tableModel;
    QString m_dataPath;
};

#endif