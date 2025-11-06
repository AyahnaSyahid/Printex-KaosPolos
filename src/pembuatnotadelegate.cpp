#include "pembuatnotadelegate.h"

PembuatNotaDelegate::PembuatNotaDelegate(QObject *parent) : QStyledItemDelegate(parent) {}
PembuatNotaDelegate::~PembuatNotaDelegate();

void PembuatNotaDelegate::createEditor( 
          QWidget* parent, 
          const QStyledItemDelegate &opt, 
          const QModelIndex& ix )
{
  switch (ix.column()) {
    case 1: {
      
    }
    case 2: {}
    case 3: {}
  }
}