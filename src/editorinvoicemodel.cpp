#include "editorinvoicemodel.h"
#include "database.h"

#include <QSqlQuery>

EditorInvoiceModel::EditorInvoiceModel(Database *b, QObject *parent): db(b), StandardItemModel(parent) {}

EditorInvoicePenjualanModel::EditorInvoicePenjualanModel(int invId, Database *b, QObject *parent)
  : EditorInvoiceModel(b, parent)
{
  QSqlQuery q;
  q.prepare("SELECT * FROM Penjualan WHERE invoice_id = ?");
  q.addBindValue(invId);
  q.exec();
  
}