
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

  author->set_name("author name");
  author->save();

  ad->set_author(author);
  ad->set_name("ad article");
  ad->set_expires("never");
  ad->save();
  QVariant id = ad->get_id();
  qDebug() << id;

//  Author::findOne(db, id);
//  Article::findOne(db, id);
  AdsArticle* ad2 = AdsArticle::findOne(db, id);
  qDebug() << ad2->get_id();
  qDebug() << ad2->get_expires();
  qDebug() << ad2->get_name();
  qDebug() << ad2->get_author()->get_name();
  qDebug() << ad2->get_author()->get_hair();

//  article->set_name("wicked sick");
//  article->save();
//  QVariant id = article->get_id();
//  qDebug() << "article id" << id;

//  Article* newart = Article::findOne(db, id);
//  qDebug() << newart->get_name();

  QMap<QString, QString> map = {
    { "name", "Jacob" }
  };

  qDebug() << "end";
  qDebug() << "end";
  return 0;
}
