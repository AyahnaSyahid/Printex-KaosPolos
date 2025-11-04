#ifndef KONSUMENINFO_H
#define KONSUMENINFO_H

#include <QDialog>

namespace Ui {
  class KonsumenInfo;
}

class KonsumenInfo : public QDialog
{
  Q_OBJECT
public:
  explicit KonsumenInfo(const QString& nama, QWidget *parent);
  ~KonsumenInfo();
  
  const QString& updateNama() const { return u_nama; }
  const QString& updatePhone() const { return u_phone; }
  const QString& updateInfo() const { return u_info; }
  const QString& nama() const { return m_nama; }
  bool namaChanged() const { return (u_nama != m_nama && u_nama != ""); }
  bool phoneChanged() const { return (u_phone != m_phone && u_phone != ""); }
  bool infoChanged() const { return (u_info != m_info && u_info != ""); }

private slots:
  void on_konsumenNotFound();
  void on_phoneEditButton_clicked();
  void on_namaEditButton_clicked();
  void on_infoEditButton_clicked();
  void on_simpanButton_clicked();

signals:
  void finishEditing();

private:
  Ui::KonsumenInfo *ui;
  QString m_nama, u_nama;
  QString m_phone, u_phone;
  QString m_info, u_info;
};

#endif