#include "metatable.h"

namespace dbup {

const char* const QdbupTableColumn::PRIMARY_KEY = "PRIMARY_KEY";
const char* const QdbupTableColumn::AUTOINCREMENT = "AUTOINCREMENT";

QdbupTableColumn* MetaTable::findColumn(QString columnName) {
  foreach (QdbupTableColumn* column, columns) {
    if (column->name() == columnName) {
      return column;
    }
  }
  return nullptr;
}

QdbupTableColumn* MetaTable::primaryKey() {
  foreach (QdbupTableColumn* column, columns) {
    if (column->isPrimaryKey()) {
      return column;
    }
  }
  return nullptr;
}

}


