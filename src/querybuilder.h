#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <QString>
#include <QSqlQuery>

namespace dbup {

class QueryBuilder {
public:
  virtual ~QueryBuilder() { }
  virtual QString createTableNoColumns() = 0;
  virtual QString alterTableAddColumnBasic() = 0;
};

}

#endif // QUERYBUILDER_H
