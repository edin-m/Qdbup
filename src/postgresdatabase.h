#ifndef POSTGRESDATABASE_H
#define POSTGRESDATABASE_H

#include <QObject>

#include "genericdatabase.h"

namespace dbup {

class PostgresDatabase : public GenericDatabase {
  Q_OBJECT
  QString qTypeToPgType(const QString& qType);
public:
  explicit PostgresDatabase(QObject* parent = 0);

  void initialize() override;
private:
  QueryBuilder* createQueryBuilder() override;
  QString columnDbType(const QdbupTableColumn* col) override;
  QString genericDefaultDataType() override;
  QSqlQuery prepareUpdateQuery(MetaTable* metaTable, QdbupTable* table) override;
  QSqlQuery prepareInsertQuery(MetaTable* metaTable, QdbupTable* table) override;
};

}

#endif // POSTGRESDATABASE_H
