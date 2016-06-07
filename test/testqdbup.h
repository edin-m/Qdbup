#ifndef TESTQDBUP_H
#define TESTQDBUP_H

#include <QObject>
#include <QVariant>

#include <qdbup/Qdbup>

#include "testmodels.h"

using namespace dbup;

class TestQdbup : public QObject {
  Q_OBJECT
  QdbupDatabase* db = nullptr;
  SimpleName* simpleName;
  DetailedName* detailedName;
  QVariant tmpVar;
private slots:
  void initTestCase();
  void initializeDb();
  void testSaveSimpleName();
  void testFindOneSimpleName();
  void testRemoveSimpleName();
  void testInheritance();
  void testQuerySelect_AllFromSimpleName();
  void testQuerySelect_MultipleTablesColumns();
  void testQuerySelect_Where();
  void cleanupTestCase();
};

#endif // TESTQDBUP_H
