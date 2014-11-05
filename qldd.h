#ifndef QLDD_H
#define QLDD_H

#include <QString>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <QTreeWidget>
typedef struct _QMOD {
  bool read;
  bool write;
  bool execute;
} QMOD;

class QLdd {
public:
  QLdd(const QString &fileName, const QString &lddDirPath);
  virtual ~QLdd();
  size_t getFileSize();
  const QString &getStringFileSize();
  void fillDependency(QTreeWidget &treeWidget);
  QString getPathOfBinary();
  QString getBinaryName();
  const QString &getStatusTime();
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
  struct stat64 _fstat;
  bool _link;
  QString _tmStatus;
  QString _tmAccess;
  QString _tmModify;
  QString _lddDirPath;
  QString _fileSize;
  QMOD    _ownerMod;
  QMOD    _groupMod;
  QMOD    _otherMod;
  QString _ownerName;
  QString _groupName;
};

#endif // QLDD_H
