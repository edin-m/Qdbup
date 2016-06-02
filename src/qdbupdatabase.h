#ifndef QDBUPDATABASE_H
#define QDBUPDATABASE_H

#include <QObject>

class MetaTable;

namespace dbup {

class QdbupTable;

class QdbupDatabase : public QObject {
  Q_OBJECT
  friend class QdbupTable;
protected:
  virtual void registerTable(QdbupTable* table) = 0;

public:
  explicit QdbupDatabase(QObject* parent) : QObject(parent) { }
  virtual void initialize() = 0;
  virtual void setHostname(const QString& hostname) = 0;
  virtual void setPort(int port) = 0;
  virtual void setDatabaseName(const QString& dbName) = 0;
  virtual void setUsername(const QString& username) = 0;
  virtual void setPassword(const QString& password) = 0;
  virtual bool open() = 0;

  template<typename className, typename thisName>
  QList<className*> findHasMany(thisName thisPtr) {
    const QMetaObject* meta = thisPtr->metaObject();
    return QList<className*>();
  }

  virtual void update(QdbupTable* item) = 0;
  virtual void save(QdbupTable* item) = 0;

  template<typename className, typename thisName>
  className* findOne(thisName* thisParam) {
    return nullptr;
  }

  template<typename className, typename thisName>
  QList<className*> find(thisName* thisParam) {
    return QList<className*>();
  }
};

}

#endif // QDBUPDATABASE_H
