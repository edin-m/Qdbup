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
//  virtual QdbupTable* findById(const QString& className, QVariant id) = 0;
  virtual QdbupTable* findById(const QMetaObject* metaObject, QVariant id) = 0;
  virtual QList<QdbupTable*> findMany(const QMetaObject* meta) = 0;

public:
  explicit QdbupDatabase(QObject* parent) : QObject(parent) { }
  virtual void initialize() = 0;
  virtual void setHostname(const QString& hostname) = 0;
  virtual void setPort(int port) = 0;
  virtual void setDatabaseName(const QString& dbName) = 0;
  virtual void setUsername(const QString& username) = 0;
  virtual void setPassword(const QString& password) = 0;
  virtual bool open() = 0;

  virtual QList<MetaTable*> metaTables() = 0;
  virtual MetaTable* findMetaTableByMetaObject(const QMetaObject* metaObject) = 0;
  virtual MetaTable* findMetaTableByTableName(const QString tableName) = 0;

  virtual QSqlDatabase& database() = 0;

  template<typename className, typename thisName>
  QList<className*> findMany(thisName thisPtr) {
    const QMetaObject* meta = thisPtr->metaObject();
    QList<className*> list;
    QList<QdbupTable*> items = findMany(meta);
    foreach (QdbupTable* item, items) {
      list.append(qobject_cast<className*>(item));
    }
    return list;
  }

  virtual QVariant save(QdbupTable* item) = 0;
  virtual void remove(QdbupTable* item) = 0;

  template<typename className>
  className* findOne(const QString& classNameParam, QVariant id) {
    return qobject_cast<className*>(findById(classNameParam, id));
  }

  template<typename className>
  className* findOne(QVariant id) {
    const QMetaObject* metaObject = &className::staticMetaObject;
    return qobject_cast<className*>(findById(metaObject, id));
  }

  template<typename className, typename thisName>
  QList<className*> find(thisName* thisParam) {
    return QList<className*>();
  }

};

}

#endif // QDBUPDATABASE_H
