
#include <QCoreApplication>
#include <QDebug>
#include <QHash>
#include <QMap>

#include <Qdbup/Qdbup>

#include "genericdatabase.h"
#include "postgresdatabase.h"

#include "article.h"

int main(int argc, char* argv[])
{
  QCoreApplication a(argc, argv);

  dbup::QdbupDatabase* db = new dbup::PostgresDatabase(&a);

  db->setHostname("localhost");
  db->setDatabaseName("test1");
  db->setUsername("postgres");
  db->setPassword("admin");

  qDebug() << "Open" << db->open();

  Author* author = new Author(db);
  Article* article = new Article(db);
  AdsArticle* ad = new AdsArticle(db);
  db->initialize();

  auto ClearAll = [=]() {
    db->database().exec("TRUNCATE TABLE author");
    db->database().exec("TRUNCATE TABLE adsarticle");
    db->database().exec("TRUNCATE TABLE article");
  };

  ClearAll();

  article->set_name("wicked sick");
  article->save();
  QVariant id = article->get_id();
  qDebug() << "article id" << id;

  Article* newart = Article::findOne(db, id);
  qDebug() << newart->get_name();

  QMap<QString, QString> map = {
    { "name", "Jacob" }
  };

  qDebug() << "end";
  qDebug() << "end";
  return 0;
}
