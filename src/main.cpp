#include <QApplication>


#include <QTimer>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

#include "database.h"

bool initAppData() {
  auto sp = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir dir;
  qDebug() << "writableLocation =" << sp;
  if(!dir.mkpath(sp)) {
    qDebug() << qApp->applicationName() << " not exists";
    return false;
  }
  qApp->setProperty("appDataPath", sp);
  qInfo() << "AppData dir initialized";
  return true;
};

int main(int argc, char** args) {
  QApplication app(argc, args);
  app.setApplicationName("KaosPolosDB");
  if(!initAppData()) {
    qWarning() << "Unable to initialize appDataPath";
    app.quit();
    return 1;
  }
  Database database;
  QTimer::singleShot(5000, app.quit);
  return app.exec();
}