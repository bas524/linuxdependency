#include "qldd.h"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <utility>
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include <QFileInfo>
#include <QDateTime>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <cxxabi.h>

#ifdef __APPLE__
#define CMD_LDD "otool -L"
#define NM "nm -g"
#define DEPEND_SPLITTER ":"
#define INFO_SPLITTER "\n"
#else
#define CMD_LDD "ldd"
#define NM "nm -D"
#define DEPEND_SPLITTER "=>"
#define INFO_SPLITTER ","
#endif

QLdd::QLdd(QString fileName, QString lddDirPath, RulesMap demangleRules)
    : _fileName(std::move(fileName)),
      _fileInfo(_fileName),
      _link(false),
      _lddDirPath(std::move(lddDirPath)),
      _demangleRules(std::move(demangleRules)) {
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
  ss << CMD_LDD << " \"" << _fileName.toStdString() << "\"";

  treeWidget.setUpdatesEnabled(false);
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
    for (const QString &v : sl) {
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
      for (const QString &v : sl) {
        if (!v.trimmed().isEmpty()) {
          if (v.contains("not found")) {
            tmp->setForeground(0, QBrush(Qt::red));
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
  treeWidget.setUpdatesEnabled(true);

  QDir::setCurrent(_lddDirPath);
}

void QLdd::fillExportTable(QListWidget &listWidget, const QString &filter) {
  listWidget.clear();
  std::mutex mutex;
  std::stringstream ss;
  ss << NM << " \"" << _fileName.toStdString() << "\" | grep \\ T\\ ";

  // Collect demangled results from worker threads; never touch the widget from them.
  std::vector<std::pair<QString, QString>> results;

  execAndDoOnEveryLine(
      ss.str(),
      [&mutex, &results, &filter, this](const QString &line) {
        QStringList info = line.split(" ");
        if (info.size() < 3) return;

        int status = 0;
        QString demangled(info.at(2));
        char *realname = abi::__cxa_demangle(info.at(2).toUtf8().constData(), nullptr, nullptr, &status);
        if (realname) {
          demangled = QString::fromLocal8Bit(realname);
          ::free(realname);
          for (const auto &rule : _demangleRules) {
            demangled.replace(rule.first, rule.second);
          }
        }

        if (!filter.isEmpty() && !demangled.contains(filter, Qt::CaseInsensitive)) {
          return;
        }

        std::unique_lock<std::mutex> lock(mutex);
        results.emplace_back(info.at(0), std::move(demangled));
      },
      Exec::ASYNC);

  // All async work is done — populate the widget on the main thread.
  listWidget.setUpdatesEnabled(false);
  for (const auto &r : results) {
    auto *item = new QListWidgetItem(r.first + " " + r.second);
    item->setToolTip(r.second);
    listWidget.addItem(item);
  }
  listWidget.setUpdatesEnabled(true);
}

QString QLdd::getPathOfBinary() { return _fileInfo.absolutePath(); }

QString QLdd::getBinaryName() { return _fileInfo.fileName(); }

const QString &QLdd::getCreatedTime() { return _tmCreate; }

const QString &QLdd::getModifyTime() { return _tmModify; }

QString QLdd::getInfo() {
  std::stringstream ss;
  ss << "file \"" << _fileName.toStdString() << "\"";
  QString buf;
  execAndDoOnEveryLine(ss.str(), [&buf](const QString &line) {
    for (const QString &part : line.split(INFO_SPLITTER)) {
      buf.append(part.trimmed()).append("\n");
    }
  });
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
