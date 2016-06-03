#ifndef POSTGRESQUERYBUILDER_H
#define POSTGRESQUERYBUILDER_H

#include <QSqlQuery>

#include "querybuilder.h"

namespace dbup {

class PostgresQueryBuilder : public QueryBuilder {
public:
  explicit PostgresQueryBuilder() { }
  QString createTableNoColumns() override;
  QString alterTableAddColumnBasic() override;
};

}

#endif // POSTGRESQUERYBUILDER_H
