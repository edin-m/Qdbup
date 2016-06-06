#ifndef GENERICDATABASE_H
#define GENERICDATABASE_H

#include <QObject>
#include <QSharedPointer>
#include <QSqlQuery>

#include "qdbupdatabase.h"
#include "metatable.h"
#include "querybuilder.h"

namespace dbup {


class GenericDatabase : public QdbupDatabase {
  Q_OBJECT
protected:
  QSqlDatabase m_db;
  QList<QdbupTable*> m_tables;
  QList<MetaTable*> m_metaTables;
  QSharedPointer<QueryBuilder> m_queryBuilder;
public:
  explicit GenericDatabase(QObject* parent = 0);
  ~GenericDatabase();

  QSqlDatabase& database() override { return m_db; }

  void initialize() override;

  void setHostname(const QString& hostname) override;
  void setPort(int port) override;
  void setDatabaseName(const QString& dbName) override;
  void setUsername(const QString& username) override;
  void setPassword(const QString& password) override;
  bool open() override;
  QList<MetaTable*> metaTables() override;

  QVariant save(QdbupTable* item) override;
  void remove(QdbupTable* item) override;
  MetaTable* findMetaTableByMetaObject(const QMetaObject* metaObject) override;
  MetaTable* findMetaTableByTableName(const QString tableName) override;

protected:
//  virtual QdbupTable* findById(const QString& className, QVariant id) override;
  virtual QdbupTable* findById(const QMetaObject* metaObject, QVariant id) override;
  virtual QList<QdbupTable*> findMany(const QMetaObject* meta) override;

private:
  virtual QueryBuilder* createQueryBuilder() = 0;
  virtual QString columnDbType(const QdbupTableColumn* col) = 0;
  virtual QString genericDefaultDataType() = 0;

  void populateItem(const  QSqlRecord& record, QdbupTable* item, MetaTable* metaTable);
  void saveOrUpdateMetaTableItem(MetaTable* metaTable, QdbupTable* item);
  void removeMetaTableItem(MetaTable* metaTable, QdbupTable* item);
  MetaTable* findMetaTableByClassName(const QString& className) ;
  MetaTable* findMetaTable(QdbupTable* table);
  void clearDoubleTableEntries();
  void registerTable(QdbupTable* table) override;
  QString findClassNameColumnType(const QString& className);
  void registerTables();
  void registerColumns();
  void registerSuperTable(MetaTable* table);
  void registerColumn(MetaTable* table);
  void registerConstraints(MetaTable* table);
  void registerSubclassRelationship(MetaTable* table);
  void registerBelongsToRelationship(MetaTable* table);
  void registerHasManyRelationship(MetaTable* table);
  void ensureTablesAreCreated();
  void addColumnsToTables();
};

}

#endif // GENERICDATABASE_H
