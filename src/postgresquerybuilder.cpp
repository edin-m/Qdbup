#include "postgresquerybuilder.h"

namespace dbup {

QString PostgresQueryBuilder::createTableNoColumns() {
  return "CREATE TABLE %1();";
}

QString PostgresQueryBuilder::alterTableAddColumnBasic() {
  return "ALTER TABLE %1 ADD COLUMN %2 %3";
}

}

