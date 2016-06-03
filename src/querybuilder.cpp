#include "querybuilder.h"

#include <QDebug>

#include "metatable.h"
#include "qdbuptable.h"

namespace dbup {

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
    QVariant val = item->property(column->propertyName().toUtf8().data());
    if (column->isDataOnly()) {
      insertQuery.addBindValue(val);
    }
    // TODO: add saving foreign keys
    if (column->isForeignKey()) {
      qDebug() << column->propertyName();
      qDebug() << item->property(column->propertyName().toUtf8().constData());
      QObject* ptr = item->property(column->propertyName().toUtf8().constData()).value<QObject*>();
      if (ptr) {
        QString primaryKeyPropertyName = column->foreignKeyTable()->primaryKey()->propertyName();
        QVariant var = ptr->property(primaryKeyPropertyName.toUtf8().constData());
        insertQuery.addBindValue(var);
      } else {
        insertQuery.addBindValue(QVariant::String);
      }
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
  queryStr += " WHERE " + metaTable->primaryKey()->name()
      + "=" + item->property(metaTable->primaryKey()->propertyName().toUtf8().constData()).toString();
  qDebug() << queryStr;
  updateQuery.prepare(queryStr);
  foreach (QdbupTableColumn* column, columns) {
    if (column->isDataOnly()) {
      QVariant val = item->property(column->propertyName().toUtf8().constData());
      updateQuery.addBindValue(val);
    }
    if (column->isForeignKey()) {
      QString primaryKeyOfForeignTable = column->foreignKeyTable()->primaryKey()->propertyName();
//      QVariant foreignKeyVal = item->property(column->propertyName());
//      QObject* ptr = foreignKeyVal.value<QObject*>();
//      QVariant foreignKey = ptr->property(primaryKeyOfForeignTable);
//      qDebug() << foreignKey;
    }
  }
  return updateQuery;
}

}
