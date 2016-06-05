#ifndef TESTMODELS_H
#define TESTMODELS_H

#include <QObject>
#include <QString>

#include <qdbup/Qdbup>

class SimpleName : public dbup::QdbupTable {
  Q_OBJECT
  DB_TABLE(SimpleName)
  DB_COLUMN(long, id)
  DB_COLUMN(QString, name)
public:
  Q_INVOKABLE SimpleName(dbup::QdbupDatabase* db)
    : dbup::QdbupTable(db)
  { }
};

class DetailedName : public SimpleName {
  Q_OBJECT
  DB_TABLE(DetailedName)
  DB_COLUMN(QString, detail)
public:
  Q_INVOKABLE DetailedName(dbup::QdbupDatabase* db)
    : SimpleName(db)
  { }
};

#endif // TESTMODELS_H
