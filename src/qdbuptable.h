#ifndef QDBUPTABLE_H
#define QDBUPTABLE_H

#include <QObject>
#include <QVariant>

#include "qdbupdatabase.h"

//  className* findOne() { return m_db->findOne<className>(this); } \
//    return db->findOne<className>(QString(#className), id); \

#define DB_TABLE(className) \
public: \
  QString tableName() const override { return QString(#className).toLower(); } \
  static className* findOne(dbup::QdbupDatabase* db, QVariant id) { \
    return db->findOne<className>(id); \
  } \
  QVariant save() { return m_db->save(this); } \
  void remove() { return m_db->remove(this); } \
  Q_INVOKABLE className* __dbupConstructor() { return new className(m_db); } \
private: \
  bool m_existsInDb_##nameParam = false; \
  Q_PROPERTY(bool m_existsInDb_##nameParam READ get_existsInDb_##nameParam WRITE set_existsInDb_##nameParam); \
  void set_existsInDb_##nameParam(bool value) { this->m_existsInDb_##nameParam = value; } \
  bool get_existsInDb_##nameParam() { return this->m_existsInDb_##nameParam; }

#define DB_COLUMN(typeParam, nameParam, ...) \
private: \
  Q_INVOKABLE typeParam __registerColumn##nameParam() { return typeParam(); } \
  Q_INVOKABLE QString __registerConstraints##nameParam() { return QString(#__VA_ARGS__); } \
protected: \
  typeParam m_##nameParam; \
public: \
  Q_PROPERTY(typeParam m_##nameParam READ get_##nameParam WRITE set_##nameParam); \
  void set_##nameParam(typeParam nameParam) { this->m_##nameParam = nameParam; } \
  typeParam get_##nameParam() const { return this->m_##nameParam; }

#define DB_BELONGS_TO(typeParam, nameParam) \
private: \
  Q_INVOKABLE typeParam* __registerBelongsTo##nameParam() { return nullptr; } \
protected: \
  typeParam* m_##nameParam = nullptr; \
public: \
  Q_PROPERTY(typeParam* m_##nameParam READ get_##nameParam WRITE set_##nameParam); \
  void set_##nameParam(typeParam* nameParam) { this->m_##nameParam = nameParam; } \
  typeParam* get_##nameParam() const { return this->m_##nameParam; }

#define DB_HAS_MANY(className, nameParam) \
private: \
  Q_INVOKABLE void __registerHasMany##nameParam() { } \
public: \
  QList<className*> get_##nameParam() { return m_db->findMany<className>(this); }

namespace dbup {

class QdbupDatabase;
class GenericDatabase;
class QdbupTableColumn;
class QueryBuilder;

class QdbupTable : public QObject {
  Q_OBJECT
  friend class dbup::QdbupDatabase;
  friend class dbup::GenericDatabase;
  friend class dbup::QdbupTableColumn;
  friend class dbup::QueryBuilder;
public:
  Q_INVOKABLE void setExistsInDb(MetaTable* metaTable, bool value);
  Q_INVOKABLE bool existsInDb(MetaTable* metaTable);
  Q_INVOKABLE QVariant primaryKeyValue(MetaTable* metaTable);
  Q_INVOKABLE void setPrimaryKeyValue(MetaTable* metaTable, QVariant value);
  Q_INVOKABLE QVariant columnValue(QdbupTableColumn* tableColumn);
  Q_INVOKABLE void setColumnValue(QdbupTableColumn* tableColumn, QVariant value);
protected:
  dbup::QdbupDatabase* m_db;
public:
  explicit QdbupTable(QdbupDatabase* db);
  virtual QString tableName() const = 0;
};

}

#endif // QDBUPTABLE_H
