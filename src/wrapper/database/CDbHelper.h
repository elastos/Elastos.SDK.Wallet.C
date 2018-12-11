
#ifndef __CDATABASE_HELPER_H__
#define __CDATABASE_HELPER_H__

#include <string>

struct sqlite3;

class CDbHelper
{
public:
    static bool TableExist(sqlite3* db, std::string tableName);
};

#endif // __CDATABASE_HELPER_H__
