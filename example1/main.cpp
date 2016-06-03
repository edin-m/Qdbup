
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

  if (testAuthor) {
    author->set_name("new author");
    author->save();
  }

  if (testAuthor) {
    article->set_author(author);
  }

  article->set_name("wicked article");
  article->save();
  qDebug() << article->get_id();




  QMap<QString, QString> map = {
    { "name", "Jacob" }
  };

  qDebug() << "end";
  return 0;
}
