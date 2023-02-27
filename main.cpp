#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QFileInfo>

int main(int argc, char *argv[]) {
  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication app(argc, argv);

  QApplication::setApplicationName("Qldd");
  QApplication::setApplicationVersion("1.0.0");

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
  QString title = "DependencyViewer";
  if (!fName.isEmpty()) {
    QFileInfo fi(fName);
    auto s = QString(" - %1").arg(fi.fileName());
    title.append(s);
  }
  w.setWindowTitle(title);
  w.show();

  return app.exec();
}
