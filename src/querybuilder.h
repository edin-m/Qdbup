#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QString>
#include <QSqlQuery>

#include "metatable.h"

namespace dbup {

class QueryBuilder {
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
};

}

#endif // QUERYBUILDER_H
