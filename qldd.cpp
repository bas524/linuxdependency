#include "qldd.h"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QDir>
#include <cxxabi.h>

#ifdef __APPLE__
#define CMD_LDD "otool -L"
#define NM "nm -g"
#define DEPEND_SPLITTER ":"
#else
#define CMD_LDD "ldd"
#define NM "nm -D"
#define DEPEND_SPLITTER "=>"
#endif

template <typename Action>
void execAndDoOnEveryLine(const std::string &execString, const Action &action) {
  std::unique_ptr<FILE, std::function<int(FILE *)>> stream(popen(execString.c_str(), "r"), pclose);

  QTextStream nmOutStream(stream.get());
  QString line;
  do {
    line = nmOutStream.readLine();
    if (line.isNull()) {
      break;
    }
    line = line.trimmed();
    if (line.isEmpty()) {
      break;
    }
    action(line);
  } while (!line.isNull());
}

QLdd::QLdd(QString fileName, QString lddDirPath)
    : _fileName(std::move(fileName)), _fileInfo(_fileName), _link(false), _lddDirPath(std::move(lddDirPath)) {
  _ownerMod.read = _fileInfo.permission(QFile::ReadOwner);
  _ownerMod.write = _fileInfo.permission(QFile::WriteOwner);
  _ownerMod.execute = _fileInfo.permission(QFile::ExeOwner);

  _groupMod.read = _fileInfo.permission(QFile::ReadGroup);
  _groupMod.write = _fileInfo.permission(QFile::WriteGroup);
  _groupMod.execute = _fileInfo.permission(QFile::ExeGroup);

  _otherMod.read = _fileInfo.permission(QFile::ReadOther);
  _otherMod.write = _fileInfo.permission(QFile::WriteOther);
  _otherMod.execute = _fileInfo.permission(QFile::ExeOther);

  _tmCreate = _fileInfo.birthTime().toString(Qt::RFC2822Date);
  _tmAccess = _fileInfo.lastRead().toString(Qt::RFC2822Date);
  _tmModify = _fileInfo.lastModified().toString(Qt::RFC2822Date);

  _link = _fileInfo.isSymLink();

  _ownerName.append(_fileInfo.owner());
  _groupName.append(_fileInfo.group());

  _fileSize = getHumanReadableDataSize();
}
QString QLdd::getHumanReadableDataSize() const {
  auto size = static_cast<double>(_fileInfo.size());
  int i = 0;
  const char *units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
  while (size > 1024) {
    size /= 1024;
    i++;
  }
  char buf[512] = {0};
  snprintf(buf, sizeof(buf), "%.*f %s", i, size, units[i]);
  return QString::fromLocal8Bit(buf);
}

QLdd::~QLdd() = default;

int64_t QLdd::getFileSize() const { return _fileInfo.size(); }

const QString &QLdd::getStringFileSize() const { return _fileSize; }

void QLdd::fillDependency(QTreeWidget &treeWidget) {
  treeWidget.clear();

  std::stringstream ss;

  QDir::setCurrent(getPathOfBinary());
  ss << CMD_LDD << " " << _fileName.toStdString();

  execAndDoOnEveryLine(ss.str(), [this, &treeWidget](const QString &line) {
    QTreeWidgetItem *item = nullptr;
    QStringList sl;
    if (!line.contains("=>") && line.contains("(0x")) {
      sl = line.split("(");
      sl.removeLast();
    } else {
      sl = line.split(DEPEND_SPLITTER);
    }
    int i = 0;
    for (const QString &v : qAsConst(sl)) {
      if (v.contains("(0x") || v.contains("(compatibility")) {
        QStringList slTmp = v.split("(");
        if (slTmp.size() > 1) {
          slTmp.removeLast();
          QString vt = slTmp.first().trimmed();
          if (!vt.isEmpty()) {
            sl.replace(i, slTmp.first());
          } else {
            sl.removeOne(v);
          }
        } else {
          sl.removeOne(v);
        }
      }
      i++;
    }
    if (sl.size() == 1) {
      QFileInfo qfinf(sl.first());
      sl.push_front(qfinf.fileName());
    }
    if (sl.first() != _fileName) {
      item = new QTreeWidgetItem();
      item->setText(0, sl.first());
      item->setToolTip(0, sl.first());

      treeWidget.addTopLevelItem(item);
      sl.removeFirst();
      QTreeWidgetItem *tmp = item;
      QColor redC("red");
      for (const QString &v : qAsConst(sl)) {
        if (!v.trimmed().isEmpty()) {
          if (v.contains("not found")) {
            tmp->setForeground(0, QBrush(redC));
            tmp->setText(0, tmp->text(0) + " " + v);
            tmp->setToolTip(0, tmp->text(0));
          } else {
            auto *nitm = new QTreeWidgetItem(tmp);
            nitm->setText(0, v);
            nitm->setToolTip(0, v);
            tmp = nitm;
          }
        }
      }
    }
  });

  QDir::setCurrent(_lddDirPath);
}

void QLdd::fillExportTable(QListWidget &listWidget, const QString &filter) {
  listWidget.clear();
  int status = 0;
  std::stringstream ss;
  ss << NM << " " << _fileName.toStdString() << " | grep \\ T\\ ";

  execAndDoOnEveryLine(ss.str(), [&status, &listWidget, &filter](const QString &line) {
    QStringList info = line.split(" ");
    QString demangled(info.at(2));
    char *realname = abi::__cxa_demangle(info.at(2).toStdString().c_str(), nullptr, nullptr, &status);
    if (realname) {
      demangled = QString::fromLocal8Bit(realname);
      ::free(realname);
      demangled.replace(":__1:", "");
      demangled.replace(":__cxx11:", "");
      demangled.replace("std::basic_string<char, std::char_traits<char>, std::allocator<char> >", "std::string");
      demangled.replace("__gnu_cxx::__normal_iterator<char const*, std::string >", "std::string::const_iterator");
      demangled.replace("std::__wrap_iter<char const*>", "std::string::const_iterator");
      demangled.replace("std::basic_istream<char, std::char_traits<char> >", "std::istream");
      demangled.replace("std::basic_ostream<char, std::char_traits<char> >", "std::ostream");
    }
    std::unique_ptr<QListWidgetItem> item(new QListWidgetItem(info.at(0) + " " + demangled));
    item->setToolTip(demangled);
    if (!filter.isEmpty() && demangled.contains(filter, Qt::CaseInsensitive)) {
      listWidget.addItem(item.release());
    } else if (filter.isEmpty()) {
      listWidget.addItem(item.release());
    }
  });
}

QString QLdd::getPathOfBinary() { return _fileInfo.absolutePath(); }

QString QLdd::getBinaryName() { return _fileInfo.fileName(); }

const QString &QLdd::getCreatedTime() { return _tmCreate; }

const QString &QLdd::getModifyTime() { return _tmModify; }

QString QLdd::getInfo() {
  std::stringstream ss;
  ss << "file " << _fileName.toStdString();
  QString buf;
  execAndDoOnEveryLine(ss.str(), [&buf](const QString &line) { buf.append(line); });
  QStringList slTmp = buf.split(",");
  buf.clear();
  for (const QString &v : qAsConst(slTmp)) {
    buf.append(v.trimmed()).append("\n");
  }
  return buf;
}
const QMOD &QLdd::getOwnerMod() const { return _ownerMod; }

void QLdd::setOwnerMod(const QMOD &ownerMod) { _ownerMod = ownerMod; }
const QMOD &QLdd::getGroupMod() const { return _groupMod; }

void QLdd::setGroupMod(const QMOD &groupMod) { _groupMod = groupMod; }
const QMOD &QLdd::getOtherMod() const { return _otherMod; }

void QLdd::setOtherMod(const QMOD &otherMod) { _otherMod = otherMod; }
const QString &QLdd::getOwnerName() const { return _ownerName; }

void QLdd::setOwnerName(const QString &ownerName) { _ownerName = ownerName; }

const QString &QLdd::getGroupName() const { return _groupName; }

void QLdd::setGroupName(const QString &groupName) { _groupName = groupName; }

const QString &QLdd::getAccessTime() { return _tmAccess; }
