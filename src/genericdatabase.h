#ifndef GENERICDATABASE_H
#define GENERICDATABASE_H

#include <QObject>
#include <QSqlDatabase>
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

  void initialize() override;

  void setHostname(const QString& hostname) override;
  void setPort(int port) override;
  void setDatabaseName(const QString& dbName) override;
  void setUsername(const QString& username) override;
  void setPassword(const QString& password) override;
  bool open() override;

  void save(QdbupTable* item) override;
  void remove(QdbupTable* item) override;

private:
  virtual QueryBuilder* createQueryBuilder() = 0;
  virtual QString columnDbType(const QdbupTableColumn* col) = 0;
  virtual QString genericDefaultDataType() = 0;

  void saveMetaTableItem(MetaTable* metaTable, QdbupTable* item);
  MetaTable* findMetaTableByClassName(const QString& className);
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
