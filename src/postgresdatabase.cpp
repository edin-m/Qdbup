#include "postgresdatabase.h"

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlIndex>
#include <QSqlResult>
#include <QDebug>

#include "postgresquerybuilder.h"

namespace dbup {

namespace PgTypes {
  static const char* const SERIAL    = "serial";
  static const char* const BIGSERIAL = "bigserial";
  static const char* const SMALLINT  = "smallint";
  static const char* const INTEGER   = "integer";
  static const char* const BIGINT    = "bigint";
  static const char* const TEXT      = "text";
}

QString PostgresDatabase::qTypeToPgType(const QString& qType) {
  if (qType == "QString") {
    return PgTypes::TEXT;
  }
  if (qType == "short") {
    return PgTypes::SMALLINT;
  }
  if (qType == "int") {
    return PgTypes::INTEGER;
  }
  if (qType == "long") {
    return PgTypes::BIGINT;
  }
  if (qType == "QDateTime") {
    return PgTypes::TEXT;
  }
  return PgTypes::TEXT;
}

PostgresDatabase::PostgresDatabase(QObject *parent)
  : GenericDatabase(parent)
{
  m_db = QSqlDatabase::addDatabase("QPSQL");
}

void PostgresDatabase::initialize() {
  GenericDatabase::initialize();
}

QueryBuilder* PostgresDatabase::createQueryBuilder() {
  return new PostgresQueryBuilder();
}

QString PostgresDatabase::columnDbType(const QdbupTableColumn* col) {
  if (col->constraints().contains(QdbupTableColumn::AUTOINCREMENT)) {
    if (col->qType() == "int") {
      return PgTypes::SERIAL;
    } else {
      return PgTypes::BIGSERIAL;
    }
  }
  return qTypeToPgType(col->qType());
}

QString PostgresDatabase::genericDefaultDataType() {
  return "text";
}

}

