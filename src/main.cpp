#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QLocale>
#include <QStandardPaths>
#include <QTimer>

#include "database.h"
#include "kaospoloswindow.h"

void cleanUp() {
  auto base = QSqlDatabase::database();
  if (base.isOpen()) base.close();
  QDir appDir(qApp->applicationDirPath());
  QDir tbr(appDir.absoluteFilePath("data"));
  tbr.removeRecursively();
}

void initDummyData(Database *);

int main(int argc, char **args) {
  QApplication app(argc, args);
  app.setOrganizationName("Custom Soft");
  app.setApplicationName("JualKaosDB");
  QLocale locale(QLocale::Indonesian, QLocale::Indonesia);
  QLocale::setDefault(locale);

  QDir appdir(app.applicationDirPath());

  appdir.mkpath("data");
  Database database(appdir.absoluteFilePath("data"));
  
  initDummyData(&database);
  
  KaosPolosWindow kp(&database);
  kp.show();

#ifdef ENABLE_DUMMY_DATA
  app.connect(&app, &QApplication::aboutToQuit, cleanUp);
#endif
  return app.exec();
}
