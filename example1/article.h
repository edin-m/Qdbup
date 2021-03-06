#ifndef ARTICLE_H
#define ARTICLE_H

#include <QObject>

#include "qdbuptable.h"

class Article;
class Book;

class Author : public dbup::QdbupTable {
  Q_OBJECT
  DB_TABLE(Author)
  DB_COLUMN(long, id, AUTOINCREMENT)
  DB_COLUMN(QString, name)
  DB_COLUMN(QString, hair)
  DB_HAS_MANY(Article, articles)
public:
  Q_INVOKABLE explicit Author(dbup::QdbupDatabase* db);
};

class Publisher : public dbup::QdbupTable {
  Q_OBJECT
  DB_TABLE(Publisher)
  DB_COLUMN(long, id)
  DB_COLUMN(QString, name)
  DB_HAS_MANY(Book, books)
public:
  Q_INVOKABLE explicit Publisher(dbup::QdbupDatabase* db);
};

class Book : public dbup::QdbupTable {
  Q_OBJECT
  DB_TABLE(Book)
  DB_COLUMN(long, id)
  DB_COLUMN(QString, title)
public:
   Q_INVOKABLE explicit Book(dbup::QdbupDatabase* db);
};

class Article : public dbup::QdbupTable {
  Q_OBJECT
  DB_TABLE(Article)
  DB_COLUMN(long, id)
  DB_COLUMN(QString, name)
  DB_BELONGS_TO(Author, author)
public:
  Q_INVOKABLE explicit Article(dbup::QdbupDatabase* db);
};

class AdsArticle : public Article {
  Q_OBJECT
  DB_TABLE(AdsArticle)
//  DB_COLUMN(long, id)
  DB_COLUMN(QString, expires)
public:
  Q_INVOKABLE explicit AdsArticle(dbup::QdbupDatabase* db);
};

#endif // ARTICLE_H
