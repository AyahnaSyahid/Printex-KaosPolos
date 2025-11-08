#ifndef "pembuatnotamodel.h"
#define "pembuatnotamodel.h"

#include <QStandardItemModel>
#include <QSqlQueryModel>

class PembuatNotaModel : public QStandardItemModel
{
  Q_OBJECT

public:
  explicit PembuatNotaModel(QObject *parent=nullptr);
  ~PembuatNotaModel();

  struct DataPenjualan;
  enum DataRole;
  
  QList<DataPenjualan> sales() const;

  Qt::ItemFlags flags(const QModelIndex&);

public slots:
  void setProduk(int row, const QString& nama);

private:
  QSqlQueryModel *qm;
};

struct PembuatNotaModel::DataPenjualan {
  QString namaProduk;
  int qty;
  int price;
  int total() const;
};

enum PembuatNotaModel::DataRole {
  MinPriceRole = 2001,
  MaxQtyRole
};

#endif // "pembuatnotamodel.h"