#ifndef QUERYSELECT_H
#define QUERYSELECT_H

#include <QList>
#include <QString>
#include <QVariant>

#include <vector>

#include "metatable.h"
#include "qdbupdatabase.h"
#include "qdbuptable.h"

namespace dbup {

class QuerySelect {
  QdbupDatabase* m_db;
  QList<MetaTable*> m_selectTables;
  QList<MetaTable*> m_fromTables;
  QString m_selectPart = "SELECT";
  QString m_fromPart = "FROM";
  QString m_wherePart = "";
  std::vector<QString> m_columns;

  template<typename First>
  inline void selectInternal() {
    select<First>();
  }

  template<typename First, typename Second, typename... Others>
  inline void selectInternal() {
    selectInternal<First>();
    selectInternal<Second, Others...>();
  }

  template<typename First>
  inline void fromInternal() {
    from<First>();
  }

  template<typename First, typename Second, typename... Others>
  inline void fromInternal() {
    fromInternal<First>();
    fromInternal<Second, Others...>();
  }

  QString normalize(const QString str) {
    QString s = str.trimmed();
    if (s.at(s.length() - 1) == ',') {
      s = s.left(s.length() - 1);
    }
    return s;
  }

public:
  QuerySelect(QdbupDatabase* db)
    : m_db (db) { }
  QuerySelect(const QuerySelect& query)
    : m_db(query.m_db),
      m_selectPart(query.m_selectPart),
      m_fromPart(query.m_fromPart),
      m_wherePart(query.m_wherePart),
      m_selectTables(query.m_selectTables),
      m_fromTables(query.m_fromTables),
      m_columns(query.m_columns)
  { }

  template<class... Types, class... Args>
//  template<typename... Types>
  QuerySelect& select(Args... args) {
//  QuerySelect& select(std::vector<QString> args = {}) {
    m_columns = { args... };
//    m_columns = args;
    selectInternal<Types...>();
    return *this;
  }

  template<class... Types, class... Args>
  QuerySelect& from(Args... args) {
    fromInternal<Types...>();
    return *this;
  }

  template<typename TableParam>
  QuerySelect& select(std::vector<QString> v = { }) {
    const QMetaObject* metaObject = &TableParam::staticMetaObject;
    if (MetaTable* metaTable = m_db->findMetaTableByMetaObject(metaObject)) {
      m_selectTables.append(metaTable);
      m_fromTables.append(metaTable);
      m_fromPart += " " + metaTable->tableName + ", ";
      int index = m_selectTables.length() - 1;
      QList<QdbupTableColumn*> columns;
      if (m_columns.size() == 0) {
        m_columns = v;
      }
      if (index < m_columns.size()) {
        // add specified columns and primary key
        foreach (QString rawColumn, m_columns.at(m_selectTables.length() - 1).split(",")) {
          columns << metaTable->findColumn(rawColumn.trimmed());
        }
        if (!columns.contains(metaTable->primaryKey())) {
          columns.prepend(metaTable->primaryKey());
        }
      } else {
        // add all columns
        foreach (QdbupTableColumn* column, metaTable->columns) {
          columns << column;
        }
      }
      // add columns
      foreach (QdbupTableColumn* column, columns) {
        m_selectPart += " " + QString("%1.%2 as %3,")
            .arg(metaTable->tableName, column->name())
            .arg(metaTable->tableName + "_" + column->name());
      }
    }
    return *this;
  }

  template<typename Table>
  QuerySelect& leftJoin(QuerySelect query) {
    const QMetaObject* metaObject = &Table::staticMetaObject;
    MetaTable* metaTable = m_db->findMetaTableByMetaObject(metaObject);
    m_fromTables.append(metaTable);
    m_fromPart = normalize(m_fromPart);
    m_selectPart = normalize(m_selectPart);
    m_selectPart += ", " + metaTable->tableName + ".*";
    m_fromPart += " LEFT JOIN " + QString("(%1) %2").arg(query.queryStr(), metaTable->tableName);
    return *this;
  }

  template<typename Table>
  QuerySelect& leftJoin(const QString leftPart, const QString op, const QString rightPart) {
    leftJoin<Table>(QuerySelect(m_db).select<Table>());
    on(leftPart, op, rightPart);
    return *this;
  }

  template<typename Table>
  QuerySelect& innerJoin(const QString leftPart, const QString op, const QString rightPart) {
    innerJoin<Table>(QuerySelect(m_db).select<Table>());
    on(leftPart, op, rightPart);
    return *this;
  }

  template<typename Table>
  QuerySelect& innerJoin(QuerySelect query) {
    const QMetaObject* metaObject = &Table::staticMetaObject;
    MetaTable* metaTable = m_db->findMetaTableByMetaObject(metaObject);
    m_fromTables.append(metaTable);
    m_fromPart = normalize(m_fromPart);
    m_selectPart = normalize(m_selectPart);
    m_selectPart += ", " + metaTable->tableName + ".*";
    m_fromPart += " INNER JOIN " + QString("(%1) %2").arg(query.queryStr(), metaTable->tableName);
    return *this;
  }

  QuerySelect& on(const QString leftPart, const QString op, const QString rightPart) {
    MetaTable* leftTable = m_fromTables[m_fromTables.length() - 2];
    MetaTable* rightTable = m_fromTables[m_fromTables.length() - 1];
    m_fromPart += QString(" ON %1.%2%3%4.%5")
        .arg(leftTable->tableName)
        .arg(leftPart)
        .arg(op)
        .arg(rightTable->tableName)
        .arg(rightTable->tableName + "_" + rightPart);
    return *this;
  }

  QuerySelect& where(MetaTable* leftTable, MetaTable* rightTable,
                     const QString leftPart, const QString op, const QVariant val) {
    m_wherePart += m_wherePart.length() == 0 ? "WHERE " : "";
    m_wherePart += leftTable->tableName + "." + leftPart + op + rightTable->tableName + "." + val.toString();
    return *this;
  }

  QuerySelect& where(MetaTable* leftTable, const QString leftPart, const QString op, const QVariant val) {
    m_wherePart += m_wherePart.length() == 0 ? "WHERE " : "";
    m_wherePart += leftTable->tableName + "." + leftPart + op + val.toString();
    return *this;
  }

  QuerySelect& where(const QString leftPart, const QString op, const QVariant val) {
    MetaTable* lastTable = m_fromTables[m_fromTables.length() - 1];
    return where(lastTable, leftPart, op, val);
  }

  template<typename RightTable>
  QuerySelect& where(const QString leftPart, const QString op, const QVariant val) {
    MetaTable* lastTable = m_fromTables[m_fromTables.length() - 1];
    MetaTable* rightTable = m_db->findMetaTableByMetaObject(&RightTable::staticMetaObject);
    return where(lastTable, rightTable, lastTable, leftPart, op, val);
  }

  template<typename LeftTable, typename RightTable>
  QuerySelect& where(const QString leftPart, const QString op, const QVariant val) {
    MetaTable* lastTable = m_db->findMetaTableByMetaObject(&LeftTable::staticMetaObject);
    MetaTable* rightTable = m_db->findMetaTableByMetaObject(&RightTable::staticMetaObject);
    return where(lastTable, rightTable, leftPart, op, val);
  }

//  template<typename TableParam>
//  QuerySelect& where(const QString leftPart, const QString op, const QVariant val) {
//    return *this;
//  }

//  template<typename LeftTable, typename RightTable>
//  QuerySelect& where(const QString leftPart, const QString op, const QVariant val) {
//    m_wherePart += m_wherePart.length() == 0 ? "WHERE " : "";
//    m_wherePart += leftPart + " " + op + " " + val.toString();
//    return *this;
//  }

//  template<typename TableParam>
//  QuerySelect& andWhere(const QString leftPart, const QString op, const QVariant val) {
//    m_wherePart += " AND ";
//    return where<TableParam>(leftPart, op, val);
//  }

//  template<typename LeftTable, typename RightTable>
//  QuerySelect& andWhere(const QString leftPart, const QString op, const QVariant val) {
//    m_wherePart += " AND ";
//    return where<LeftTable, RightTable>(leftPart, op, val);
//  }

//  template<typename TableParam>
//  QuerySelect& orWhere(const QString leftPart, const QString op, const QVariant val) {
//    m_wherePart += " OR ";
//    return where<TableParam>(leftPart, op, val);
//  }

//  template<typename LeftTable, typename RightTable>
//  QuerySelect& orWhere(const QString leftPart, const QString op, const QVariant val) {
//    m_wherePart += " OR ";
//    return where<LeftTable, RightTable>(leftPart, op, val);
//  }

  QString queryStr() {
    m_selectPart = normalize(m_selectPart);
    m_fromPart = normalize(m_fromPart);
    return m_selectPart + " " + m_fromPart + " " + m_wherePart;
  }
};

}

#endif // QUERYSELECT_H
