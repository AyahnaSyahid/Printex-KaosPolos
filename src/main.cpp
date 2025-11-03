#include <QApplication>

#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QTimer>

#include "database.h"
#include "kaospoloswindow.h"

void cleanUp() {
  auto base = QSqlDatabase::database();
  if (base.isOpen())
    base.close();
  QDir appDir(qApp->applicationDirPath());
  QDir tbr(appDir.absoluteFilePath("data"));
  tbr.removeRecursively();
}

int main(int argc, char **args) {
  QApplication app(argc, args);
  app.setOrganizationName("Custom Soft");
  app.setApplicationName("JualKaosDB");
  QDir appdir(app.applicationDirPath());
  appdir.mkpath("data");
  Database database(appdir.absoluteFilePath("data"));
  KaosPolosWindow kp(&database);
  kp.show();
  // QTimer::singleShot(5000, app.quit);
  app.connect(&app, &QApplication::aboutToQuit, cleanUp);
  return app.exec();
}
