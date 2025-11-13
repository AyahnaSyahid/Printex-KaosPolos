#ifndef WIDGETINVOICE_H
#define WIDGETINVOICE_H

#include <QWidget>
#include <QSortFilterProxyModel>

namespace Ui {
  class WidgetInvoice;
}

class WidgetInvoice : public QWidget
{
  Q_OBJECT

public:
  class UnpaidModel;
  explicit WidgetInvoice(QWidget *parent=nullptr);
  ~WidgetInvoice();

public slots:
  void refreshData();

private:
  Ui::WidgetInvoice* ui;
};

class WidgetInvoice::UnpaidModel : public QSortFilterProxyModel
{
public:
  explicit UnpaidModel(QObject *parent=nullptr);
  QVariant data(const QModelIndex&, int role) const override;
};

#endif