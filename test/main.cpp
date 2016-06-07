#include <QCoreApplication>
#include <QtTest/QTest>

#include <QObject>

#include "testqdbup.h"

int main(int argc, char **argv) {
  QCoreApplication app(argc, argv);

  int err = 0;
  {
    TestQdbup testQdbup;
    err = qMax(err, QTest::qExec(&testQdbup, app.arguments()));
  }
  return 0;
}
