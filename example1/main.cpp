
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

  bool testAuthor = true;
  bool testArticle = false;
  bool testAd = true;

  author->set_name("new author");
  if (testAuthor) {
    author->save();
    qDebug() << author->get_id();
  }
  if (testAuthor) {
    article->set_author(author);
    ad->set_author(author);
  }
  article->set_name("wicked article");
  if (testArticle) {
    article->save();
    qDebug() << article->get_id();
  }

  ad->set_name("Ad article 1");
  ad->set_expires("7 days");
  if (testAd) {
    ad->save();
    qDebug() << ad->get_id();
  }





  QMap<QString, QString> map = {
    { "name", "Jacob" }
  };

  qDebug() << "end";
  qDebug() << "end";
  return 0;
}
