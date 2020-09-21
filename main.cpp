#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char *argv[]) {
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
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
  w.setWindowTitle("DependencyViewer");
  w.show();

  return app.exec();
}
