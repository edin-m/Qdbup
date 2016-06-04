#include "querybuilder.h"

#include <QDebug>

#include "metatable.h"
#include "qdbuptable.h"

namespace dbup {

QVariant QueryBuilder::findValueForForeignKey(QdbupTable* item, QdbupTableColumn* column) {
  QObject* ptr = item->property(column->propertyName().toUtf8().constData()).value<QObject*>();
  if (!ptr) {
    return QVariant::String;
  }
  MetaTable* foreignKeyTable = column->foreignKeyTable();
  if (!static_cast<QdbupTable*>(ptr)->existsInDb(foreignKeyTable)) {
    return QVariant::String;
  }
  QString primaryKeyPropertyName = foreignKeyTable->primaryKey()->propertyName();
  QVariant var = ptr->property(primaryKeyPropertyName.toUtf8().constData());
  return var;
}

void QueryBuilder::bindColumnToQuery(MetaTable* metaTable,
                                     QdbupTable* item,
                                     QdbupTableColumn* column,
                                     QSqlQuery& query) {
  QVariant val = item->columnValue(column);
  // data columns
  if (column->isDataOnly()) {
    query.addBindValue(val);
  }
  // foreign keys
  if (column->isForeignKey()) {
    // foreign key must exist in database if we are to fetch its value
    QVariant foreignVal = findValueForForeignKey(item, column);
    query.addBindValue(foreignVal);
  }
  // one-to-one subclass
  if (metaTable->parentTable && column->isSubclassColumn()) {
    QVariant primaryKeyVal = item->property(metaTable->parentTable->primaryKey()->propertyName().toUtf8().constData());
    query.addBindValue(primaryKeyVal);
  }
}

QSqlQuery QueryBuilder::insertQuery(QSqlDatabase& db,
                                    MetaTable* metaTable,
                                    QdbupTable* item,
                                    QList<QdbupTableColumn*> columns) {
  QString commaColumns = "";
  QString preparedColumns = "";
  foreach (QdbupTableColumn* column, columns) {
    commaColumns = commaColumns + column->name() + ", ";
    preparedColumns = preparedColumns + "?, ";
  }
  commaColumns = commaColumns.left(commaColumns.length() - 2);
  preparedColumns = preparedColumns.left(preparedColumns.length() - 2);

  QSqlQuery insertQuery(db);
  QString queryStr = "INSERT INTO %1 (%2) VALUES (%3)";
  queryStr = queryStr.arg(metaTable->tableName);
  queryStr = queryStr.arg(commaColumns);
  queryStr = queryStr.arg(preparedColumns);
  insertQuery.prepare(queryStr);
  foreach (QdbupTableColumn* column, columns) {
    bindColumnToQuery(metaTable, item, column, insertQuery);
  }
  return insertQuery;
}

QSqlQuery QueryBuilder::updateQuery(QSqlDatabase& db,
                                    MetaTable* metaTable,
                                    QdbupTable* item,
                                    QList<QdbupTableColumn*> columns) {
  QSqlQuery updateQuery(db);
  QString queryStr = "UPDATE " + metaTable->tableName + " SET ";
  foreach (QdbupTableColumn* column, columns) {
    queryStr += column->name() + "=?, ";
  }
  queryStr = queryStr.left(queryStr.length() - 2);
  queryStr += " WHERE " + metaTable->primaryKey()->name()
      + "=" + item->primaryKeyValue(metaTable).toString();
  updateQuery.prepare(queryStr);
  foreach (QdbupTableColumn* column, columns) {
    bindColumnToQuery(metaTable, item, column, updateQuery);
  }
  return updateQuery;
}

QSqlQuery QueryBuilder::deleteQuery(QSqlQuery& query,
                                    MetaTable* metaTable,
                                    QdbupTable* item) {
  QString queryStr = QString("DELETE FROM %1 WHERE %2=?")
      .arg(metaTable->tableName).arg(metaTable->primaryKey()->name());
  QVariant primaryKeyValue = item->primaryKeyValue(metaTable);
  query.prepare(queryStr);
  query.addBindValue(primaryKeyValue);
  return query;
}

QSqlQuery QueryBuilder::selectByIdQuery(QSqlDatabase& db, MetaTable* metaTable, QVariant id) {
  QString queryStr = QString("SELECT * FROM %1 WHERE %2=?")
      .arg(metaTable->tableName).arg(metaTable->primaryKey()->name());
  QSqlQuery selectQuery(db);
  selectQuery.prepare(queryStr);
  selectQuery.addBindValue(id);
  return selectQuery;
}

}
