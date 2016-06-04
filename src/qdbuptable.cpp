#include "qdbuptable.h"

#include <QDebug>

#include "qdbupdatabase.h"
#include "metatable.h"

namespace dbup {

void QdbupTable::setExistsInDb(MetaTable* metaTable, bool value) {
  setProperty(metaTable->existsInDbPropertyName().toUtf8().constData(), value);
}

bool QdbupTable::existsInDb(MetaTable* metaTable) {
  return property(metaTable->existsInDbPropertyName().toUtf8().constData()).toBool();
}

QVariant QdbupTable::primaryKeyValue(MetaTable* metaTable) {
  return property(metaTable->primaryKey()->propertyName().toUtf8().constData());
}

void QdbupTable::setPrimaryKeyValue(MetaTable* metaTable, QVariant value) {
  setProperty(metaTable->primaryKey()->propertyName().toUtf8().data(), value);
}

QVariant QdbupTable::columnValue(QdbupTableColumn* tableColumn) {
  return property(tableColumn->propertyName().toUtf8().constData());
}

void QdbupTable::setColumnValue(QdbupTableColumn* tableColumn, QVariant value) {
  setProperty(tableColumn->propertyName().toUtf8().constData(), value);
}

QdbupTable::QdbupTable(QdbupDatabase* db)
  : QObject(db), m_db(db)
{
  m_db->registerTable(this);
}

}

