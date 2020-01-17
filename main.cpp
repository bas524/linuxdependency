#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include "qldd.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QApplication::setApplicationName("Qldd");
  QApplication::setApplicationVersion("1.0");

  QCommandLineParser parser;
  parser.setApplicationDescription("Qldd gui over ldd utility");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("filename", QCoreApplication::translate("main", "file analyse."));

  parser.process(app);

  const QStringList args = parser.positionalArguments();

  QString fName;
  if (!args.empty()) {
    fName = args.at(0);
  }

  MainWindow w(fName);

#ifdef __APPLE__
  w.setWindowTitle("UnixDependency");
#else
  w.setWindowTitle("LinuxDependency");
#endif
  w.show();

  return app.exec();
}
