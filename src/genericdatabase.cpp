#include "genericdatabase.h"

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlField>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlQuery>
#include <QPair>
#include <QMetaProperty>
#include <QMetaMethod>
#include <QMetaClassInfo>
#include <QMetaObject>
#include <QDebug>
#include <QScopedPointer>

#include "qdbupdatabase.h"

static const char* REGISTER_COLUMN_DELIMITER = "__registerColumn";
static const char* REGISTER_COLUMN_CONSTRAINTS_DELIMITER = "__registerConstraints";
static const char* REGISTER_BELONGS_TO_DELIMITER = "__registerBelongsTo";
static const char* REGISTER_HAS_MANY_DELIMITER = "__registerHasMany";

namespace dbup {

GenericDatabase::GenericDatabase(QObject* parent)
  : QdbupDatabase(parent)
{
}

void GenericDatabase::clearDoubleTableEntries() {
  QStringList list;
  QList<int> toRemove;
  for (int i = 0; i < m_tables.length(); i++) {
    if (!list.contains(m_tables.at(i)->tableName())) {
      list << m_tables.at(i)->tableName();
    } else {
      toRemove << i;
    }
  }
  foreach (int i, toRemove) {
    m_tables.removeAt(i);
  }
}

GenericDatabase::~GenericDatabase() {
  qDeleteAll(m_metaTables);
}

void GenericDatabase::registerTable(QdbupTable* table) {
  m_tables.append(table);
}

void GenericDatabase::initialize() {
  m_queryBuilder = QSharedPointer<QueryBuilder>(createQueryBuilder());
  clearDoubleTableEntries();

  registerTables();
  registerColumns();

  qDebug() << "Loaded tables";
  foreach (MetaTable* metaTable, m_metaTables) {
    qDebug() << "Table:" << metaTable->tableName;
    foreach (QdbupTableColumn* col, metaTable->columns) {
      qDebug() << "\tColumn:" << col->name() << col->qType() << col->constraints();
    }
  }

  if (m_db.isOpen()) {
    ensureTablesAreCreated();
    addColumnsToTables();
  } else {
    qWarning() << "Database is not open!";
  }
}

void GenericDatabase::setHostname(const QString& hostname) {
  m_db.setHostName(hostname);
}

void GenericDatabase::setPort(int port) {
  m_db.setPort(port);
}

void GenericDatabase::setDatabaseName(const QString& dbName) {
  m_db.setDatabaseName(dbName);
}

void GenericDatabase::setUsername(const QString& username) {
  m_db.setUserName(username);
}

void GenericDatabase::setPassword(const QString& password) {
  m_db.setPassword(password);
}

bool GenericDatabase::open() {
  return m_db.open();
}

/**
 * DEPRECATE
 */
void GenericDatabase::update(QdbupTable* item) {
  if (MetaTable* metaTable = findMetaTable(item)) {
//    QSqlQuery updateQuery = prepareUpdateQuery(metaTable, item);
  } else {
    qWarning() << "Could not find meta table!" << item->metaObject()->className();
  }
}

void GenericDatabase::save(QdbupTable* item) {
  if (MetaTable* metaTable = findMetaTable(item)) {
    QList<QdbupTableColumn*> columns;
    foreach (QdbupTableColumn* column, metaTable->columns) {
      if (!column->isPrimaryKey()) {
        columns.append(column);
      }
    }
    QSqlQuery query;
    bool isInsertQuery = !item->m_existsInDb;
    if (!isInsertQuery) {
      query = m_queryBuilder->updateQuery(m_db, metaTable, item, columns);
    } else {
      query = m_queryBuilder->insertQuery(m_db, metaTable, item, columns);
    }
    bool result = query.exec();
    if (!result) {
      // TODO: emit error
      qDebug() << query.lastError().databaseText() << query.lastError().driverText();
    } else {
      // update id if insert
      if (isInsertQuery) {
        QVariant lastInsertId = query.lastInsertId();
        qDebug() << metaTable->primaryKey()->propertyName();
        item->setProperty(metaTable->primaryKey()->propertyName().toUtf8().data(), lastInsertId);
        item->m_existsInDb = true;
      }
    }
  } else {
    qWarning() << "Could not find meta table!" << item->metaObject()->className();
  }
}

/**
 * Find meta table containing metaObject->className() same as className
 */
MetaTable* GenericDatabase::findMetaTableByClassName(const QString& className) {
  QString classNameLocal(className);
  if (classNameLocal.right(1) == "*") {
    classNameLocal = classNameLocal.left(classNameLocal.lastIndexOf('*'));
  }
  foreach (MetaTable* metaTable, m_metaTables) {
    if (metaTable->metaObject->className() == classNameLocal) {
      return metaTable;
    }
  }
  return nullptr;
}

MetaTable* GenericDatabase::findMetaTable(QdbupTable* table) {
  const QMetaObject* metaObject = table->metaObject();
  foreach (MetaTable* metaTable, m_metaTables) {
    if (metaTable->metaObject == metaObject) {
      return metaTable;
    }
  }
  return nullptr;
}

/**
 * Return qType of a primary key column of a className table
 */
QString GenericDatabase::findClassNameColumnType(const QString& className) {
  QString columnType = className;
  // if advanced type take its primary key as a type for db
  if (columnType.right(1) == "*") {
    columnType = columnType.left(columnType.lastIndexOf('*'));
  }
  foreach (MetaTable* metaTableIn, m_metaTables) {
    if (metaTableIn->metaObject->className() == columnType) {
      if (QdbupTableColumn* primaryKeyColumn = metaTableIn->primaryKey()) {
        return primaryKeyColumn->qType();
      }
    }
  }
  qWarning() << "Could not" << __FUNCTION__ << "returning same type" << className;
  return className;
}

void GenericDatabase::registerTables() {
  // register tables
  foreach (QdbupTable* table, m_tables) {
    MetaTable* metaTable = new MetaTable;
    m_metaTables.append(metaTable);
//    metaTable->table = table;
    metaTable->tableName = table->tableName();
    metaTable->metaObject = table->metaObject();
  }
}

void GenericDatabase::registerColumns() {
  foreach (MetaTable* metaTable, m_metaTables) {
    registerSuperTable(metaTable);
    registerColumn(metaTable);
    registerConstraints(metaTable);
    registerSubclassRelationship(metaTable);
    registerBelongsToRelationship(metaTable);
    registerHasManyRelationship(metaTable);
  }
}

void GenericDatabase::registerSuperTable(MetaTable* metaTable) {
  const QMetaObject* meta = metaTable->metaObject;
  // register super table
  if (!QString(meta->superClass()->className()).contains("QdbupTable")) {
    const char* superClassName = meta->superClass()->className();
    foreach (MetaTable* metaTable2, m_metaTables) {
      if (metaTable != metaTable2 &&
          superClassName == metaTable2->metaObject->className()) {
        metaTable->parentTable = metaTable2;
      }
    }
  }
}

void GenericDatabase::registerColumn(MetaTable* metaTable) {
  const QMetaObject* meta = metaTable->metaObject;
  int method_count = meta->methodCount();
  // register columns
  for (int i = 0; i < method_count; ++i) {
    QMetaMethod method = meta->method(i);
    QString name = QString(method.name());
    if (name.contains(REGISTER_COLUMN_DELIMITER)) {
      QString columnName = name.right(name.length() - QString(REGISTER_COLUMN_DELIMITER).length());
      QString columnType = method.typeName();
      if (metaTable->parentTable && metaTable->parentTable->findColumn(columnName)) {
        continue;
      }
      QdbupTableColumn* column = new QdbupTableColumn(columnType, columnName);
      // first field with id suffix is primary key
      if (metaTable->columns.count() == 0 && columnName.right(2).toLower() == "id") {
        column->addConstraints("PRIMARY_KEY, AUTOINCREMENT, NOT_NULL");
      }
      metaTable->columns.append(column);
    }
  }
}

void GenericDatabase::registerConstraints(MetaTable* metaTable) {
  const QMetaObject* meta = metaTable->metaObject;
  int method_count = meta->methodCount();
  // register constraints
  for (int i = 0; i < method_count; ++i) {
    QMetaMethod method = meta->method(i);
    QString name = QString(method.name());
    if (name.contains(REGISTER_COLUMN_CONSTRAINTS_DELIMITER)) {
      QString columnName = name.right(name.length() - QString(REGISTER_COLUMN_CONSTRAINTS_DELIMITER).length());
      if (QdbupTableColumn* column = metaTable->findColumn(columnName)) {
        QString constraints;
//        method.invoke(qobject_cast<QObject*>(const_cast<QdbupTable*>(table)), Qt::DirectConnection, Q_RETURN_ARG(QString, constraints));
        QdbupTable* table2 = static_cast<QdbupTable*>(metaTable->metaObject->newInstance(Q_ARG(QdbupDatabase*, this)));
        method.invoke(qobject_cast<QObject*>(const_cast<QdbupTable*>(table2)), Qt::DirectConnection, Q_RETURN_ARG(QString, constraints));
        column->addConstraints(constraints);
        delete table2;
      }
    }
  }
}

void GenericDatabase::registerSubclassRelationship(MetaTable* metaTable) {
  const QMetaObject* meta = metaTable->metaObject;
  // register subclass relationship
  if (metaTable->parentTable) {
    QString columnType = metaTable->parentTable->metaObject->className();
    QString columnName = columnType.toLower() + "_id";
    columnType = findClassNameColumnType(columnType);
    QdbupTableColumn* column = new QdbupTableColumn(columnType, columnName);
//    column->setForeignKey(true);
//    column->setForeignKeyTable(metaTable->parentTable);
    column->setSubclass(true);
    metaTable->columns.append(column);
  }
}

void GenericDatabase::registerBelongsToRelationship(MetaTable* metaTable) {
  const QMetaObject* meta = metaTable->metaObject;
  int method_count = meta->methodCount();
  // register belongs to relationship
  for (int i = 0; i < method_count; ++i) {
    QMetaMethod method = meta->method(i);
    QString name = QString(method.name());
    if (name.contains(REGISTER_BELONGS_TO_DELIMITER)) {
      QString origColumnName = name.right(name.length() - QString(REGISTER_BELONGS_TO_DELIMITER).length());
      QString columnName = origColumnName + "_id";
      QString columnType = method.typeName();
      if (metaTable->parentTable && metaTable->parentTable->findColumn(columnName)) {
        continue;
      }
      columnType = findClassNameColumnType(columnType);
      QdbupTableColumn* column = new QdbupTableColumn(columnType, columnName);
      column->setPropertyName(origColumnName);
      column->setForeignKey(true);
      if (MetaTable* foreignMetaTable = findMetaTableByClassName(QString(method.typeName()))) {
        column->setForeignKeyTable(foreignMetaTable);
      } else {
        qWarning() << "Could not find foreign meta table!" << method.typeName();
      }
      metaTable->columns.append(column);
    }
  }
}

void GenericDatabase::registerHasManyRelationship(MetaTable* metaTable) {
  const QMetaObject* meta = metaTable->metaObject;
  int method_count = meta->methodCount();
  // register has many relationship
  for (int i = 0; i < method_count; ++i) {
    QMetaMethod method = meta->method(i);
    QString name = QString(method.name());
    if (name.contains(REGISTER_HAS_MANY_DELIMITER)) {
      QString columnName = name.right(name.length() - QString(REGISTER_HAS_MANY_DELIMITER).length());
      columnName = columnName + "_id";
      QString columnType = method.typeName();
    }
  }
}

void GenericDatabase::ensureTablesAreCreated() {
  QStringList tables = m_db.driver()->tables(QSql::AllTables);
  // create tables
  QList<MetaTable*> tablesToCreate;
  foreach (MetaTable* metaTable, m_metaTables) {
    if (!tables.contains(metaTable->tableName)) {
      tablesToCreate.append(metaTable);
    }
  }
  foreach (MetaTable* metaTable, tablesToCreate) {
    QString queryString = m_queryBuilder->createTableNoColumns().arg(metaTable->tableName);
    QSqlQuery query = m_db.exec(queryString);
      qDebug() << query.lastQuery();
    if (!query.isValid()) {
      // TODO: emit error
      qDebug() << m_db.lastError().databaseText() << m_db.lastError().driverText();
    }
  }
}

void GenericDatabase::addColumnsToTables() {
  // add columns
  foreach (MetaTable* table, m_metaTables) {
    QSqlRecord record = m_db.driver()->record(table->tableName);
    QStringList columnsExist;
    int count = record.count();
    for (int i = 0; i < count; i++) {
      columnsExist << record.fieldName(i);
    }
    QList<QdbupTableColumn*> columnsToAdd;
    foreach (QdbupTableColumn* col, table->columns) {
      if (!columnsExist.contains(col->name())) {
        columnsToAdd.append(col);
      }
    }
    // alter table add columns
    foreach (QdbupTableColumn* col, columnsToAdd) {
      QString queryString = m_queryBuilder->alterTableAddColumnBasic();
      QString dbType = columnDbType(col);
      QSqlQuery query = m_db.exec(queryString.arg(table->tableName, col->name(), dbType));
      qDebug() << query.lastQuery();
      if (!query.isValid()) {
        // TODO: emit error
        qDebug() << m_db.lastError().databaseText() << m_db.lastError().driverText();
      }
    }
  }
}

}
