#include "testqdbup.h"

#include <QDebug>
#include <QTest>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDriver>
#include <QSqlResult>
#include <QSqlRecord>

#include "postgresdatabase.h"
#include "querybuilder.h"

static const QString testString1 = "wicked sick 1!";
static const QString testString2 = "wicked sick 2!";
static const QString testString3 = "wicked sick 3!";
static const QString testString4 = "wicked sick 3!";

void TestQdbup::initTestCase()
{
  // TODO: use list of databases and foreach database in tests
  // to test all databases
  db = new PostgresDatabase(this);
  db->setHostname("localhost");
  db->setUsername("postgres");
  db->setPassword("admin");
  db->setDatabaseName("postgres");
  Q_ASSERT(db->open());
  // prepare database
  QSqlQuery drop(db->database());
  if (!drop.exec("DROP DATABASE IF EXISTS test_qdbup")) {
    qDebug() << drop.lastError().databaseText() << drop.lastError().driverText();
  }
  if (!drop.exec("CREATE DATABASE test_qdbup")) {
    qDebug() << drop.lastError().databaseText() << drop.lastError().driverText();
  }
  db->setDatabaseName("test_qdbup");
  Q_ASSERT(db->open());
}

void TestQdbup::initializeDb()
{
  simpleName = new SimpleName(db);
  detailedName = new DetailedName(db);
  db->initialize();
}

void TestQdbup::testSaveSimpleName()
{
  simpleName->set_name(testString1);
  tmpVar = simpleName->save();
  Q_ASSERT(tmpVar.isValid());
  Q_ASSERT(tmpVar.toInt() == 1);
  Q_ASSERT(true);
}

void TestQdbup::testFindOneSimpleName()
{
  SimpleName* simpleName = SimpleName::findOne(db, tmpVar);
  Q_ASSERT(simpleName->get_name() == testString1);
  delete simpleName;
}

void TestQdbup::testRemoveSimpleName()
{
  simpleName->remove();
  Q_ASSERT(simpleName->get_id() == 0);
}

void TestQdbup::testInheritance()
{
  detailedName->set_name(testString3);
  detailedName->set_detail(testString4);
  QVariant id = detailedName->save();
  Q_ASSERT(id.isValid());
  DetailedName* detailed = DetailedName::findOne(db, id);
  Q_ASSERT(detailed->get_name() == detailedName->get_name());
  Q_ASSERT(detailed->get_detail() == detailedName->get_detail());
  delete detailed;
}

void TestQdbup::testQuerySelect_AllFromSimpleName()
{
  QuerySelect query = QuerySelect(db).select<SimpleName>();
  Q_ASSERT(query.queryStr() == "SELECT simplename.id as simplename_id, simplename.name as simplename_name FROM simplename");
}

void TestQdbup::cleanupTestCase()
{
  db->database().close();
  db->setDatabaseName("postgres");
  Q_ASSERT(db->open());
  QSqlQuery drop(db->database());
  if (!drop.exec("DROP DATABASE IF EXISTS test_qdbup")) {
    qDebug() << drop.lastError().databaseText() << drop.lastError().driverText();
  }
}
