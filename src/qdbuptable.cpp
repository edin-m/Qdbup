#include "qdbuptable.h"

#include "qdbupdatabase.h"

namespace dbup {

QdbupTable::QdbupTable(QdbupDatabase* db)
  : QObject(db), m_db(db)
{
  m_db->registerTable(this);
}

}

