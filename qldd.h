#ifndef QLDD_H
#define QLDD_H

#include <QString>
#include <QTreeWidget>
#include <QListWidget>
#include <QFileInfo>
#include <QFont>
#include <QTextStream>
#include "customtypes.h"

struct QMOD {
  bool read;
  bool write;
  bool execute;
};

class QLdd {
 public:
  QLdd(QString fileName, QString lddDirPath, RulesMap demangleRules);
  QLdd(const QLdd &) = delete;
  QLdd(QLdd &&) = delete;
  QLdd operator=(const QLdd &) = delete;
  QLdd operator=(QLdd &&) = delete;
  virtual ~QLdd();

  int64_t getFileSize() const;
  const QString &getStringFileSize() const;

  void fillDependency(QTreeWidget &treeWidget);

  void fillExportTable(QListWidget &listWidget, const QString &filter);

  QString getPathOfBinary();
  QString getBinaryName();

  const QString &getCreatedTime();
  const QString &getAccessTime();
  const QString &getModifyTime();

  QString getInfo();

  const QMOD &getOwnerMod() const;
  void setOwnerMod(const QMOD &ownerMod);

  const QMOD &getGroupMod() const;
  void setGroupMod(const QMOD &groupMod);

  const QMOD &getOtherMod() const;
  void setOtherMod(const QMOD &otherMod);

  const QString &getOwnerName() const;
  void setOwnerName(const QString &ownerName);

  const QString &getGroupName() const;
  void setGroupName(const QString &groupName);

 private:
  QString _fileName;
  QFileInfo _fileInfo;
  bool _link;
  QString _tmCreate;
  QString _tmAccess;
  QString _tmModify;
  QString _lddDirPath;
  QString _fileSize;
  QMOD _ownerMod{};
  QMOD _groupMod{};
  QMOD _otherMod{};
  QString _ownerName;
  QString _groupName;
  RulesMap _demangleRules;
  QString getHumanReadableDataSize() const;
};

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

#endif  // QLDD_H
