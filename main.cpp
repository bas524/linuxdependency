#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include "qldd.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  app.setApplicationName("Qldd");
  app.setApplicationVersion("1.0");

  QCommandLineParser parser;
  parser.setApplicationDescription("Qldd gui over ldd utility");
  parser.addHelpOption();
  parser.addVersionOption();
  parser.addPositionalArgument("filename", QCoreApplication::translate("main", "file analyse."));

  // A boolean option with multiple names (-f, --force)
//  QCommandLineOption archOption(QStringList() << "a" << "arch",
//                                 QCoreApplication::translate("main", "Show architecture."));
//  parser.addOption(archOption);

  // Process the actual command line arguments given by the user
  parser.process(app);

  const QStringList args = parser.positionalArguments();
  // source is args.at(0), destination is args.at(1)
  QString fName;
  if (args.size() > 0) {
    fName = args.at(0);
  }


  MainWindow w(fName);

//  if (parser.isSet(archOption)) {
//    QLdd qldd(fName, app.applicationDirPath());
//    size_t fsz = qldd.getFileSize();
//  }

  w.setWindowTitle("LinuxDependency");

  w.show();

  return app.exec();
}
