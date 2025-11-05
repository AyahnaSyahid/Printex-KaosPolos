#ifndef PEMBUATNOTADELEGATE_H
#define PEMBUATNOTADELEGATE_H

#include <QStyledItemDelegate>

class PembuatNotaDelegate : public QStyledItemDelegate
{
  public:
    PembuatNotaDelegate(QObject *parent=nullptr);
    ~PembuatNotaDelegate();
  
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex&) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex&) const override;
};

#endif // PEMBUATNOTADELEGATE_H