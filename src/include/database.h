// 2025-10-28

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
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
  quint64 invoiceId;
};


class Database
{
  public:
    explicit Database(const QString& dataPath="");
    ~Database();

    void initializeDatabase();

    // CRUD PRODUK
    bool addProduk(const QString& uniqueName, int price, int stock=0);
    bool removeProduk(const QString& uniqueName);
    quint64 produkIdFromName(const QString& uniqueName) const;
    bool setProdukPrice(const QString& name, int price);
    bool setProdukName(const QString& from, const QString& to);
    bool addStock(const QString& nama, quint64 stock);
    quint64 produkBasePrice(const QString& name) const;
    quint64 produkStock(const QString& name) const;

    // CRUD Konsumen
    bool addKonsumen(const QString& nama, const QString& phone="", const QString& info="");
    bool removeKonsumen(const QString& nama);
    bool setKonsumenInfo(const QString& nama, const QString& info);
    bool setKonsumenPhone(const QString& nama, const QString& phone);
    bool setNamaKonsumen(const QString& old, const QString& _new);
    
    // CRUD dan penanganan Penjualan
    const AddPenjualanResult addPenjualan(const QStringList produkList, 
          const QList<int> qtyList, 
          const QList<int> priceList, bool* ok = nullptr);
    
    const DatabaseResult setPenjualanPrice(quint64 pjid, int newPrice, bool modifyInvoiced = false);
    const DatabaseResult setPenjualanQty(quint64 pjid, int qty, bool modifyInvoiced = false);
    const DatabaseResult removePenjualan(quint64 pjid, bool force=false);
    
    // CRUD dan penanganan Invoice
    const CreateInvoiceResult createInvoice(QList<quint64> penjualan, const QString& konsumen, bool* ok = nullptr);
    

    inline bool hasError() const { return !_lastError.isEmpty(); }
    inline const bool& initialized() const { return _success; }
    inline const QString& lastError() const { return _lastError; }
    inline void resetError() { _lastError = ""; }

  private:
    QString _lastError;
    QSqlDatabase db;
    bool _success;
    QMap<QString, QSqlQueryModel*> _queryModel;
    QMap<QString, QSqlTableModel*> _tableModel;
    QString m_dataPath;
};