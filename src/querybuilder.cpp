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
  QString primaryKeyPropertyName = column->foreignKeyTable()->primaryKey()->propertyName();
  QVariant var = ptr->property(primaryKeyPropertyName.toUtf8().constData());
  return var;
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
//    QVariant val = item->property(column->propertyName().toUtf8().data());
    QVariant val = item->columnValue(column);
    // data columns
    if (column->isDataOnly()) {
      insertQuery.addBindValue(val);
    }
    // foreign keys
    if (column->isForeignKey()) {
      QVariant foreignVal = findValueForForeignKey(item, column);
      insertQuery.addBindValue(foreignVal);
    }
    // one-to-one subclass
    if (metaTable->parentTable && column->isSubclassColumn()) {
      QVariant primaryKeyVal = item->property(metaTable->parentTable->primaryKey()->propertyName().toUtf8().constData());
      insertQuery.addBindValue(primaryKeyVal);
    }
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
//  queryStr += " WHERE " + metaTable->primaryKey()->name()
//      + "=" + item->property(metaTable->primaryKey()->propertyName().toUtf8().constData()).toString();
  queryStr += " WHERE " + metaTable->primaryKey()->name()
      + "=" + item->primaryKeyValue(metaTable).toString();
  qDebug() << queryStr;
  updateQuery.prepare(queryStr);
  foreach (QdbupTableColumn* column, columns) {
    if (column->isDataOnly()) {
//      QVariant val = item->property(column->propertyName().toUtf8().constData());
      QVariant val = item->columnValue(column);
      updateQuery.addBindValue(val);
    }
    if (column->isForeignKey()) {
      QVariant foreignKeyVal = findValueForForeignKey(item, column);
      updateQuery.addBindValue(foreignKeyVal);
    }
    if (metaTable->parentTable && column->isSubclassColumn()) {
//      QVariant primaryKeyVal = item->property(metaTable->parentTable->primaryKey()->propertyName().toUtf8().constData());
      QVariant primaryKeyVal = item->primaryKeyValue(metaTable->parentTable);
      updateQuery.addBindValue(primaryKeyVal);
    }
  }
  return updateQuery;
}

QSqlQuery QueryBuilder::deleteQuery(QSqlQuery& query,
                                    MetaTable* metaTable,
                                    QdbupTable* item) {
  QString queryStr = QString("DELETE FROM %1 WHERE id=?").arg(metaTable->tableName);
//  QVariant primaryKeyValue = item->property(metaTable->primaryKey()->propertyName().toUtf8().constData());
  QVariant primaryKeyValue = item->primaryKeyValue(metaTable);
  query.prepare(queryStr);
  query.addBindValue(primaryKeyValue);
  return query;
}

}
