#ifndef QDBUPTABLE_H
#define QDBUPTABLE_H

#include <QObject>

#include "qdbupdatabase.h"

#define DB_TABLE(className) \
public: \
  QString tableName() const override { return QString(#className).toLower(); } \
  className* findOne() { return m_db->findOne<className>(this); } \
  void update() { return m_db->update(this); } \
  void save() { return m_db->save(this); } \
private:

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
  typeParam* m_##nameParam; \
public: \
  Q_PROPERTY(typeParam* m_##nameParam READ get_##nameParam WRITE set_##nameParam); \
  void set_##nameParam(typeParam* nameParam) { this->m_##nameParam = nameParam; } \
  typeParam* get_##nameParam() const { return this->m_##nameParam; }

#define DB_HAS_MANY(className, nameParam) \
private: \
  Q_INVOKABLE void __registerHasMany##nameParam() { } \
public: \
  QList<className*> get_##nameParam() { return m_db->findHasMany<className>(this); }

namespace dbup {

class QdbupDatabase;

class QdbupTable : public QObject {
  Q_OBJECT
protected:
  dbup::QdbupDatabase* m_db;
public:
  explicit QdbupTable(QdbupDatabase* db);
  virtual QString tableName() const = 0;

//  virtual void save();
};

}

#endif // QDBUPTABLE_H