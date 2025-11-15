#ifndef ASKBOX_H
#define ASKBOX_H

#include <QMessageBox>

class AskBox : public QMessageBox
{
public:
    AskBox(const QString &tt, const QString &msg, QWidget* parent);
};

#endif