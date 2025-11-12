#include "konsumenmodel.h"
#include <QSqlQueryModel>
#include <QSqlQuery>

KonsumenModel::KonsumenModel(QObject *parent)
  : src(new QSqlQueryModel(this)), QSortFilterProxyModel(parent)
{
  src->setQuery("SELECT * FROM Konsumen");
  setSourceModel(src);
  setHeaderData(1, Qt::Horizontal, "Nama");
  setHeaderData(2, Qt::Horizontal, "No. HP");
  setHeaderData(3, Qt::Horizontal, "Info");
}

KonsumenModel::~KonsumenModel(){}

void KonsumenModel::refresh() { src->setQuery(src->query().lastQuery()); }
