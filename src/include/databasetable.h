#ifndef DatabaseTable_H
#define DatabaseTable_H

#include <QSqlDatabase>

class DatabaseTable
{
  public:
    DatabaseTable(const QString tableName, QSqlDatabase db = QSqlDatabase::database());

  protected:
    QString _tableName;
    static QString defaultConnectionName;
    QSqlDatabase db;
};

class DatabaseTableHasUniqueID : public DatabaseTable
{
  public:
    DatabaseTableHasUniqueID()
};

#endif