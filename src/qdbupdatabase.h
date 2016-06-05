#ifndef QDBUPDATABASE_H
#define QDBUPDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QVariant>

namespace dbup {

class MetaTable;
class QdbupTable;

class QdbupDatabase : public QObject {
  Q_OBJECT
  friend class QdbupTable;
protected:
  virtual void registerTable(QdbupTable* table) = 0;
  virtual QdbupTable* findById(const QString& className, QVariant id) = 0;

public:
  explicit QdbupDatabase(QObject* parent) : QObject(parent) { }
  virtual void initialize() = 0;
  virtual void setHostname(const QString& hostname) = 0;
  virtual void setPort(int port) = 0;
  virtual void setDatabaseName(const QString& dbName) = 0;
  virtual void setUsername(const QString& username) = 0;
  virtual void setPassword(const QString& password) = 0;
  virtual bool open() = 0;

  virtual QSqlDatabase& database() = 0;

  template<typename className, typename thisName>
  QList<className*> findHasMany(thisName thisPtr) {
    const QMetaObject* meta = thisPtr->metaObject();
    return QList<className*>();
  }

  virtual QVariant save(QdbupTable* item) = 0;
  virtual void remove(QdbupTable* item) = 0;

  template<typename className>
  className* findOne(const QString& classNameParam, QVariant id) {
    return static_cast<className*>(findById(classNameParam, id));
  }

  template<typename className, typename thisName>
  QList<className*> find(thisName* thisParam) {
    return QList<className*>();
  }

};

}

#endif // QDBUPDATABASE_H
