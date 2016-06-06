#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QString>
#include <QDebug>
#include <QSqlQuery>

#include <vector>
#include <tuple>

#include "metatable.h"

namespace dbup {

class QueryBuilder;
class QuerySelect;

/**
 * @brief The QuerySelect class
 */
class QuerySelect {
  QdbupDatabase* m_db;
  QString m_selectPart = "SELECT ";
  QString m_fromPart = "FROM ";
  QString m_wherePart = "";
  MetaTable* m_selectTable = nullptr;
public:
  QuerySelect(QdbupDatabase* db) : m_db(db) { }
  QuerySelect(const QuerySelect& builder)
    : m_db(builder.m_db),
      m_selectPart(builder.m_selectPart),
      m_fromPart(builder.m_fromPart),
      m_wherePart(builder.m_wherePart),
      m_selectTable(builder.m_selectTable)
  { }

  QString queryStr() {
    auto lastIsComma = [](const QString& str) -> bool {
      return str.length() > 0 && str.lastIndexOf(',') == str.length() - 1;
    };
    m_selectPart = lastIsComma(m_selectPart.trimmed()) ? m_selectPart.left(m_selectPart.length() - 2) : m_selectPart;
    m_fromPart = lastIsComma(m_fromPart.trimmed()) ? m_fromPart.left(m_fromPart.length() - 2) : m_fromPart;
    m_wherePart = lastIsComma(m_wherePart.trimmed()) ? m_wherePart.left(m_wherePart.length() - 2) : m_wherePart;
    return QString(m_selectPart.trimmed() + " " + m_fromPart.trimmed() + " " + m_wherePart.trimmed()).trimmed();
  }

  template<typename A>
  QuerySelect& test() {
    qDebug() << __FUNCTION__ << A::staticMetaObject.className();
    return *this;
  }

  template<typename A, typename B, typename... C>
  QuerySelect& test() {
    test<A>();
    test<B, C...>();
    return *this;
  }

  template<typename param1>
  QuerySelect& select() {
    const QMetaObject* metaObject = &param1::staticMetaObject;
    if (MetaTable* metaTable = m_db->findMetaTableByMetaObject(metaObject)) {
      m_selectTable = metaTable;
      foreach (QdbupTableColumn* column, metaTable->columns) {
        m_selectPart += QString("%1.%2 as %3, ")
            .arg(metaTable->tableName)
            .arg(column->name())
            .arg(metaTable->tableName + "_" + column->name());
      }
      m_fromPart += metaTable->tableName;
    }
    return *this;
  }

  template<typename First>
  void selectInternal() {
    qDebug() << __FUNCTION__ << First::staticMetaObject.className();
  }

  template<typename First, typename Second, typename... Others>
  void selectInternal() {
    selectInternal<First>();
    selectInternal<Second, Others...>();
  }

  template<class... Types, class... Args>
  QuerySelect& select(Args... args) {
    qDebug() << sizeof...(Types);
    qDebug() << sizeof...(Args);
    selectInternal<Types...>();
    std::vector<QString> str = { args... };
    for (int i = 0; i < str.size(); i++) {
      qDebug() << str.at(i);
    }
    return *this;
  }

//  template<typename First, typename Second, typename... Others>
//  QuerySelect& select() {
//    select<First>();
//    select<Second, Others...>();
//    return *this;
//  }

  template<typename param1>
  QuerySelect& leftJoin() {
    return leftJoin(QuerySelect(m_db).select<param1>());
  }

  QuerySelect& leftJoin(QuerySelect query) {
    static const QString leftJoin(" LEFT JOIN (%1) %2");
    m_selectPart += query.m_selectTable->tableName + ".*";
    m_fromPart += leftJoin.arg(query.queryStr(), query.m_selectTable->tableName);
    return *this;
  }

  template<typename param1>
  QuerySelect& innerJoin() {
    return innerJoin(QuerySelect(m_db).select<param1>());
  }

  QuerySelect& innerJoin(QuerySelect query) {
    static const QString innerJoin(" INNER JOIN (%1) %2");
    m_selectPart += query.m_selectTable->tableName + ".*";
    m_fromPart += innerJoin.arg(query.queryStr(), query.m_selectTable->tableName);
    return *this;
  }

  template<typename param1, typename param2>
  QuerySelect& on(const QString leftPart, const QString op, const QString rightPart) {
    static const QString ON(" ON %1.%2%3%4.%5");
    MetaTable* metaTableLeft = m_db->findMetaTableByMetaObject(&param1::staticMetaObject);
    MetaTable* metaTableRight = m_db->findMetaTableByMetaObject(&param2::staticMetaObject);
    if (metaTableLeft && metaTableRight) {
      QString leftPrefix = metaTableLeft->tableName + "_";
      QString rightPrefix = metaTableRight->tableName + "_";
      m_fromPart += ON.arg(metaTableLeft->tableName).arg(leftPart)
          .arg(op).arg(metaTableRight->tableName).arg(rightPrefix + rightPart);
    }
    return *this;
  }

  template<typename param1>
  QuerySelect& where(const QString name, const QString op, const QVariant value) {
    static const QString WHERE("%1%2%3");
    m_wherePart += m_wherePart.length() == 0 ? " WHERE " : "";
    QString prefix = "";
    if (MetaTable* metaTable = m_db->findMetaTableByMetaObject(&param1::staticMetaObject)) {
      prefix = metaTable->tableName + ".";
    }
    m_wherePart += WHERE.arg(prefix + name, op, value.toString());
    return *this;
  }
};

/**
 * @brief The QueryBuilder class
 */
class QueryBuilder {
protected:
  virtual QVariant findValueForForeignKey(QdbupTable* item, QdbupTableColumn* column);
  void bindColumnToQuery(MetaTable* metaTable,
                         QdbupTable* item,
                         QdbupTableColumn* column,
                         QSqlQuery& query);
public:
  virtual ~QueryBuilder() { }
  virtual QString createTableNoColumns() = 0;
  virtual QString alterTableAddColumnBasic() = 0;
  virtual QSqlQuery insertQuery(QSqlDatabase& db,
                                MetaTable* metaTable,
                                QdbupTable* item,
                                QList<QdbupTableColumn*> columns);
  virtual QSqlQuery updateQuery(QSqlDatabase& db,
                                MetaTable* metaTable,
                                QdbupTable* item,
                                QList<QdbupTableColumn*> columns);
  virtual QSqlQuery deleteQuery(QSqlQuery& query,
                                MetaTable* metaTable,
                                QdbupTable* item);
  virtual QSqlQuery selectByIdQuery(QSqlDatabase& db,
                                    MetaTable* metaTable,
                                    QVariant id);
};

}

#endif // QUERYBUILDER_H
