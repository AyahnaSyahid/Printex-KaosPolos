#include "konsumenmodel.h"
#include <QSqlQueryModel>
#include <QSqlQuery>

KonsumenModel::KonsumenModel(QObject *parent)
  : src(new QSqlQueryModel(this)), QSortFilterProxyModel(parent)
{
  src->setQuery("SELECT * FROM Konsumen");
  setSourceModel(src);
}

KonsumenModel::~KonsumenModel(){}

void KonsumenModel::refresh() { src->setQuery(src->query().lastQuery()); }
