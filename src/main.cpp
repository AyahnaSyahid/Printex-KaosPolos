#include <QApplication>

#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QTimer>

#include "database.h"

int main(int argc, char **args) {
  QApplication app(argc, args);
  app.setOrganizationName("Custom Soft");
  app.setApplicationName("JualKaosDB");
  QDir appdir(app.applicationDirPath());
  appdir.mkpath("data");
  Database database(appdir.absoluteFilePath("data"));
  QTimer::singleShot(5000, app.quit);
  return app.exec();
}
