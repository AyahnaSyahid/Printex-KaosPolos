#include "askbox.h"
#include <QAbstractButton>

AskBox::AskBox(const QString &tt, const QString &mm, QWidget *parent)
: QMessageBox(QMessageBox::Question, tt, mm, QMessageBox::Yes | QMessageBox::No, parent) 
{
  auto yes = button(QMessageBox::Yes);
  auto no = button(QMessageBox::No);
  yes->setText("Ya");
  no->setText("Tidak");
}