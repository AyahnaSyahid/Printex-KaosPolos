#ifndef EDITORPEMBAYARAN_H
#define EDITORPEMBAYARAN_H

#include <QDialog>
#include <QSqlRecord>

namespace Ui {
  class EditorPembayaran;
}

class KaosPolosWindow;
class EditorPembayaran:public QDialog {
  Q_OBJECT

public:
  explicit EditorPembayaran(int pid, KaosPolosWindow *kpw);
  ~EditorPembayaran();
  inline bool isValid() const { return validId; }
  bool hasModified() const;

public slots:
  void reject() override;

private slots:
  void on_simpanButton_clicked();

signals:
  void pembayaranUpdated();

private:
  bool validId;
  int pembayaran_id;
  QSqlRecord record;
  KaosPolosWindow *kpw;
  Ui::EditorPembayaran *ui;
};

#endif
