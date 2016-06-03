#ifndef METATABLE_H
#define METATABLE_H

#include <QMetaObject>
#include <QMetaProperty>
#include <QString>
#include <QSet>
#include <QList>

#include "qdbuptable.h"

namespace dbup {

class MetaTable;

/**
 * @brief The QdbupTableColumn class
 */
class QdbupTableColumn {
  enum ColumnRole {
    PrimaryKeyRole,
    DataRole,
    ForeignKeyRole,
    SubclassRole
  };
  QString m_name; // column name and property name setProperty to use different property name
  QString m_qType;
  QSet<QString> m_constraints;
  QSet<ColumnRole> m_roles;
  MetaTable* m_foreignKeyTo = nullptr; // pointer to relation pointing table
  QString m_propertyName; // property name (after ctor same as name)
public:
  static const char* const PRIMARY_KEY;
  static const char* const AUTOINCREMENT;
  QdbupTableColumn(const QString& qType, const QString& name)
    : m_qType(qType), m_name(name), m_propertyName(name) { }
  QString name() const { return m_name; }
  void setName(const QString& name) { m_name = name; }
  QString qType() const { return m_qType; }
  void addConstraints(QString constraints) {
    QStringList list = constraints.split(",");
    foreach (QString item, list) {
      QString trimmed = item.trimmed();
      if (trimmed.length() != 0) {
        m_constraints.insert(trimmed);
      }
    }
    if (m_constraints.contains(PRIMARY_KEY)) {
      m_roles.insert(PrimaryKeyRole);
    }
  }
  void setForeignKey(bool value) {
    if (value) {
      m_roles.insert(ForeignKeyRole);
    } else {
      m_roles.remove(ForeignKeyRole);
    }
  }
  void setSubclass(bool value) {
    if (value) {
      m_roles.insert(SubclassRole);
    } else {
      m_roles.remove(SubclassRole);
    }
  }
  QString propertyName() {
    return "m_" + m_propertyName;
  }
  void setPropertyName(const QString& propertyName) {
    m_propertyName = propertyName;
  }
  bool isForeignKey() const { return m_roles.contains(ForeignKeyRole); }
  bool isPrimaryKey() const { return m_roles.contains(PrimaryKeyRole); }
  bool isDataOnly() const { return !isPrimaryKey() && !isForeignKey(); }
  bool isSubclass() const { return m_roles.contains(SubclassRole); }
  const QSet<QString>& constraints() const { return m_constraints; }
  MetaTable* foreignKeyTable() const { return m_foreignKeyTo; }
  void setForeignKeyTable(MetaTable* metaTable) { m_foreignKeyTo = metaTable; }
};

/**
 * @brief The MetaTable class
 */
class MetaTable {
public:
  ~MetaTable() {
    qDeleteAll(columns);
  }

  MetaTable* parentTable = nullptr;
  const QMetaObject* metaObject;
//  QdbupTable* table;
  QString tableName;
  QList<QdbupTableColumn*> columns;

  QdbupTableColumn* findColumn(QString columnName);
  QdbupTableColumn* primaryKey();
};

}

#endif // METATABLE_H
