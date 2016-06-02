#ifndef METATABLE_H
#define METATABLE_H

#include <QMetaObject>
#include <QString>
#include <QSet>
#include <QList>

#include "qdbuptable.h"

namespace dbup {

/**
 * @brief The QdbupTableColumn class
 */
class QdbupTableColumn {
  QString m_name;
  QString m_qType;
  QSet<QString> m_constraints;
public:
  static const char* const PRIMARY_KEY;
  static const char* const AUTOINCREMENT;
  QdbupTableColumn(const QString& qType, const QString& name) : m_qType(qType), m_name(name) { }
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
  }
  const QSet<QString>& constraints() const { return m_constraints; }
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
  QdbupTable* table;
  QString tableName;
  QList<QdbupTableColumn*> columns;

  QdbupTableColumn* findColumn(QString columnName);
  QdbupTableColumn* primaryKey();
};

}

#endif // METATABLE_H
