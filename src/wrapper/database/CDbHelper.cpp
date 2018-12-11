
#include "CDbHelper.h"
#include <sqlite3.h>

bool CDbHelper::TableExist(sqlite3* db, std::string tableName)
{
    char sql[128];
    sprintf(sql, "SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='%s'", tableName.c_str());
    sqlite3_stmt* pStmt = nullptr;
    bool exist = false;
    int count;
    int ret = sqlite3_prepare_v2(db, sql, -1, &pStmt, NULL);
    if (ret != SQLITE_OK) {
        goto exit;
    }

    ret = sqlite3_step(pStmt);
    if (SQLITE_OK != ret && SQLITE_DONE != ret && SQLITE_ROW != ret) {
        goto exit;
    }

    count = sqlite3_column_int(pStmt, 0);
    exist = count > 0;

exit:
    if (pStmt) {
        sqlite3_finalize(pStmt);
    }
    return exist;
}
