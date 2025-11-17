#ifndef EDITORINVOICEMODEL_H
#define EDITORINVOICEMODEL_H

#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardItemModel>

class Database;
class EditorInvoiceModel : public QStandardItemModel
{
  Q_OBJECT
public:
  enum ExtRole {
    NumberRole = Qt::UserDataRole + 1,
    DateRole   = NumberRole + 1
  }
  explicit EditorInvoiceModel(Database *b, QObject *parent=nullptr);

protected:
  QList<QSqlRecord> records;
  Database *db;
};

class EditorInvoicePenjualanModel : public EditorInvoiceModel
{
public:
  explicit EditorInvoicePenjualanModel(int invId, Database *b, QObject *parent=nullptr);
};


class EditorInvoicePembayaranModel : public EditorInvoiceModel
{
public:
  explicit EditorInvoicePembayaranModel(int invId, Database *b, QObject *parent=nullptr);
};

#endif