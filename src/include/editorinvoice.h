#ifndef EDITORINVOICE_H
#define EDITORINVOICE_H

#include <QDialog>
#include <QIdentityProxyModel>

namespace Ui
{
  class EditorInvoice; 
} // namespace Ui

class KaosPolosWindow;
class Database;
class EditorInvoice : public QDialog
{
  Q_OBJECT
public:
  class ModelProxy;
  explicit EditorInvoice(int invoiceId, KaosPolosWindow *k, QWidget *parent=nullptr);
  ~EditorInvoice();

private slots:
  void invoiceNotFound();

signals:
  void produkUpdated();
  void paymentUpdated();

private:
  Ui::EditorInvoice *ui;
  Database *db;
  KaosPolosWindow *kpw;
};

class EditorInvoice::ModelProxy : public QIdentityProxyModel
{
  public:
    explicit ModelProxy(QObject *parent) : QIdentityProxyModel(parent) {}
    virtual QVariant data(const QModelIndex &mi, int role=Qt::DisplayRole) const = 0;
    virtual Qt::ItemFlags flags(const QModelIndex &mi) const = 0;
};

#endif