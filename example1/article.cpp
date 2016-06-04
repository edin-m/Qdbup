#include "article.h"

Article::Article(dbup::QdbupDatabase* db)
  : QdbupTable(db)
{
}

Author::Author(dbup::QdbupDatabase* db)
  : QdbupTable(db)
{

}

AdsArticle::AdsArticle(dbup::QdbupDatabase* db)
  : Article(db)
{

}

Publisher::Publisher(dbup::QdbupDatabase* db)
  : QdbupTable(db)
{

}

Book::Book(dbup::QdbupDatabase* db)
  : QdbupTable(db)
{

}
