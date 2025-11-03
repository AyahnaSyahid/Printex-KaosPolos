#ifndef KONSUMENMODEL_H
#define KONSUMENMODEL_H

#include <QSqlQueryModel>
#include <QSortFilterProxyModel>

class KonsumenModel : public QSortFilterProxyModel
{
  Q_OBJECT
public:
  explicit KonsumenModel(QObject *parent=nullptr);
  ~KonsumenModel();

public slots:
  void refresh();

private:
  QSqlQueryModel *src;
};

#endif