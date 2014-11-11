#include "qldd.h"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <QTreeWidgetItem>
#include <QListWidgetItem>

#define MAXBUFFER 512

using namespace std;

QLdd::QLdd(const QString &fileName, const QString &lddDirPath) :
  _fileName(fileName),
  _link(false),
  _lddDirPath(lddDirPath) {
  memset(&_fstat, 0, sizeof(_fstat));
  int rez = stat64(_fileName.toLocal8Bit().data(), &_fstat);
  if (rez) {
    throw "Error stat file";
  }
  _link = S_ISLNK(_fstat.st_mode);
#ifdef __APPLE__
  _tmStatus = std::ctime((const time_t *)&_fstat.st_ctimespec.tv_sec);
  _tmAccess = std::ctime((const time_t *)&_fstat.st_atimespec.tv_sec);
  _tmModify = std::ctime((const time_t *)&_fstat.st_mtimespec.tv_sec);
#else
  _tmStatus = std::ctime(&_fstat.st_ctim.tv_sec);
  _tmAccess = std::ctime(&_fstat.st_atim.tv_sec);
  _tmModify = std::ctime(&_fstat.st_mtim.tv_sec);
#endif

  _ownerMod.read = S_IRUSR & _fstat.st_mode;
  _ownerMod.write = S_IWUSR & _fstat.st_mode;
  _ownerMod.execute = S_IXUSR & _fstat.st_mode;

  _groupMod.read = S_IRGRP & _fstat.st_mode;
  _groupMod.write = S_IWGRP & _fstat.st_mode;
  _groupMod.execute = S_IXGRP & _fstat.st_mode;

  _otherMod.read = S_IROTH & _fstat.st_mode;
  _otherMod.write = S_IWOTH & _fstat.st_mode;
  _otherMod.execute = S_IXOTH & _fstat.st_mode;

  struct passwd *pw = getpwuid(_fstat.st_uid);
  _ownerName.append(pw->pw_name);
  struct group  *gr = getgrgid(_fstat.st_gid);
  _groupName.append(gr->gr_name);
}

QLdd::~QLdd() {
}

size_t QLdd::getFileSize() {
  return _fstat.st_size;
}

const QString &QLdd::getStringFileSize() {
  double size = static_cast<double>(_fstat.st_size);
  int i = 0;
  const char *units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
  while (size > 1024) {
    size /= 1024;
    i++;
  }
  char buf[512] = {0};
  sprintf(buf, "%.*f %s", i, size, units[i]);
  _fileSize.clear();
  _fileSize.append(buf);
  return _fileSize;

}

void QLdd::fillDependency(QTreeWidget &treeWidget) {
  treeWidget.clear();
  QTreeWidgetItem *item = NULL;

  char buffer[MAXBUFFER] = {0};
  stringstream ss;
  QString path = getPathOfBinary();

  chdir(path.toStdString().c_str());
#ifdef __APPLE__
  ss << "otool -L " << _fileName.toStdString();
#else
  ss << "ldd " << _fileName.toStdString();
#endif
  FILE *stream = popen(ss.str().c_str(), "r");
  QString buf;
  QStringList sl;
#ifdef __APPLE__
  bool flag = false;
#endif
//  QString substr;
  while (fgets(buffer, MAXBUFFER, stream) != NULL) {
    buf.clear();
    buf = QString::fromLocal8Bit(buffer).trimmed();
    if (!buf.contains("=>") && buf.contains("(0x")) {
      sl = buf.split("(");
      sl.removeLast();
    }
    else {
#ifdef __APPLE__
      sl = buf.split(":");
#else
      sl = buf.split("=>");
#endif
    }
    int i = 0;
    foreach(QString v, sl) {
      if (v.contains("(0x")) {
        QStringList slTmp = v.split("(");
        if (slTmp.size() > 1) {
          slTmp.removeLast();
          QString vt = slTmp.first().trimmed();
          if (!vt.isEmpty()) {
            sl.replace(i, slTmp.first());
          }
          else {
            sl.removeOne(v);
          }
        }
        else {
          sl.removeOne(v);
        }
      }
      i++;
    }
    item = new QTreeWidgetItem();
#ifdef __APPLE__
    if (!flag) {
      flag = true;
      item->setTextColor(0, Qt::magenta);
      item->setText(0, sl.first());
    }
    else {
      item->setText(0, "# "+sl.first());
    }
#else
    item->setText(0, sl.first());
#endif
    item->setToolTip(0, sl.first());

    treeWidget.addTopLevelItem(item);
    sl.removeFirst();
    QTreeWidgetItem *tmp = item;
    QColor redC("red");
    foreach(QString v, sl) {
      if (!v.trimmed().isEmpty()) {
        if (v.contains("not found")) {
          tmp->setTextColor(0, redC);
          tmp->setText(0, tmp->text(0) + " " + v);
          tmp->setToolTip(0, tmp->text(0));
        }
        else {
          QTreeWidgetItem *nitm = new QTreeWidgetItem(tmp);
          nitm->setText(0, v);
          nitm->setToolTip(0, v);
          tmp = nitm;
        }
      }
    }

    memset(&buffer, 0, sizeof(buffer));
  }
  pclose(stream);
  chdir(_lddDirPath.toStdString().c_str());
}

void QLdd::fillExportTable(QListWidget &listWidget) {
  listWidget.clear();


  char buffer[MAXBUFFER] = {0};
  stringstream ss;
#ifdef __APPLE__
  ss << "nm -g "
#else
  ss << "nm -D "
#endif
     << _fileName.toStdString() << " | grep \\ T\\ ";
  FILE *stream = popen(ss.str().c_str(), "r");
  QString buf;
  QStringList sl;

  while (fgets(buffer, MAXBUFFER, stream) != NULL) {
    buf.clear();
    buf = QString::fromLocal8Bit(buffer).trimmed();

    listWidget.addItem(new QListWidgetItem(buf));

    memset(&buffer, 0, sizeof(buffer));
  }
  pclose(stream);
}

QString QLdd::getPathOfBinary() {
  string fullPath = _fileName.toStdString();
  size_t cwdLen = 0;
  for (size_t i = fullPath.size(); i > 0; i--) {
    if ((fullPath.c_str()[i] == '/') || (fullPath.c_str()[i] == '\\')) {
      cwdLen = i;
      fullPath = fullPath.substr(0, cwdLen);
      break;
    }
  }
  return QString::fromStdString(fullPath);
}

QString QLdd::getBinaryName() {
  string fullPath = _fileName.toStdString();
  size_t cwdLen = 0;
  for (size_t i = fullPath.size(); i > 0; i--) {
    if ((fullPath.c_str()[i] == '/') || (fullPath.c_str()[i] == '\\')) {
      cwdLen = i;
      fullPath = fullPath.substr(cwdLen + 1, fullPath.size());
      break;
    }
  }
  return QString::fromStdString(fullPath);
}

const QString &QLdd::getStatusTime() {
  return _tmStatus;
}

const QString &QLdd::getModifyTime() {
  return _tmModify;
}

QString QLdd::getInfo() {
  char buffer[MAXBUFFER] = {0};
  stringstream ss;
  ss << "file " << _fileName.toStdString();
  FILE *stream = popen(ss.str().c_str(), "r");
  QString buf;
  while (fgets(buffer, MAXBUFFER, stream) != NULL) {
    buf.append(QString::fromLocal8Bit(buffer));
    memset(&buffer, 0, sizeof(buffer));
  }
  pclose(stream);
  QStringList slTmp = buf.split(",");
  buf.clear();
  foreach(QString v, slTmp) {
    buf.append(v.trimmed()).append("\n");
  }
  return buf;
}
const QMOD &QLdd::getOwnerMod() const {
  return _ownerMod;
}

void QLdd::setOwnerMod(const QMOD &ownerMod) {
  _ownerMod = ownerMod;
}
const QMOD &QLdd::getGroupMod() const {
  return _groupMod;
}

void QLdd::setGroupMod(const QMOD &groupMod) {
  _groupMod = groupMod;
}
const QMOD &QLdd::getOtherMod() const {
  return _otherMod;
}

void QLdd::setOtherMod(const QMOD &otherMod) {
  _otherMod = otherMod;
}
const QString &QLdd::getOwnerName() const {
  return _ownerName;
}

void QLdd::setOwnerName(const QString &ownerName) {
  _ownerName = ownerName;
}

const QString &QLdd::getGroupName() const {
  return _groupName;
}

void QLdd::setGroupName(const QString &groupName) {
  _groupName = groupName;
}

const QString &QLdd::getAccessTime() {
  return _tmAccess;
}
