
#include "CHistoryDb.h"
#include "SqliteWrapperError.h"
#include "common/Utils.h"
#include "common/Log.hpp"
#include <sstream>
#include <sqlite3.h>
#include "CDbHelper.h"

namespace elastos {

CHistoryDb::CHistoryDb(const std::string& filePath, const std::string& tableName)
    : mDb(nullptr)
    , mTableName(tableName)
{
    std::string dbFile(filePath);
    dbFile += "/";
    dbFile += TEST_NET ? TEST_DB_FILE : DB_FILE;
    int ret = sqlite3_open(dbFile.c_str(), &mDb);
    if (ret !=  SQLITE_OK) {
        CloseDb();
        return;
    }

    if (CDbHelper::TableExist(mDb, tableName)) return;

    char sql[512];
    sprintf(sql, "CREATE TABLE %s(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, txid TEXT NOT NULL, "
        "address TEXT NOT NULL, direction TEXT NOT NULL, amount INTEGER NOT NULL, "
        "time INTEGER NOT NULL, height INTEGER NOT NULL, fee INTEGER NOT NULL,"
        "inputs TEXT NOT NULL, outputs TEXT NOT NULL);", tableName.c_str());

    Log::D("Database", "%s\n", sql);

    char* errMsg;
    ret = sqlite3_exec(mDb, sql, NULL, NULL, &errMsg);
    if (ret !=  SQLITE_OK) {
        Log::E("Database", "create table failed ret %d, %s\n", ret, errMsg);
        sqlite3_free(errMsg);
        CloseDb();
    }
}

CHistoryDb::~CHistoryDb()
{
    CloseDb();
}

int CHistoryDb::Insert(const std::vector<History>& histories)
{
    if (!mDb) {
        return E_SQL_WRAPPER_DB_OPEN;
    }
    if (histories.empty()) {
        return E_SQL_WRAPPER_INVALID_ARGUMENT;
    }

    char* errMsg;
    int ret = sqlite3_exec(mDb, "BEGIN;", NULL, NULL, &errMsg);
    if (ret != SQLITE_OK) {
        Log::E("Database", "insert begin transaction failed ret %d, %s\n", ret, errMsg);
        sqlite3_free(errMsg);
        return ret + E_SQL_WRAPPER_BASE;
    }

    bool succeeded = true;
    for (History history : histories) {
        std::string sql = CreateInsertSql(history);
        Log::D("Database", "insert sql: %s\n", sql.c_str());
        ret = sqlite3_exec(mDb, sql.c_str(), NULL, NULL, &errMsg);
        if (ret != SQLITE_OK) {
            Log::E("Database", "insert history failed ret %d, %s\n", ret, errMsg);
            sqlite3_free(errMsg);
            succeeded = false;
            break;
        }
    }

    if (succeeded) {
        sqlite3_exec(mDb, "COMMIT;", NULL, NULL, NULL);
        return E_SQL_WRAPPER_OK;
    }
    else {
        sqlite3_exec(mDb, "ROLLBACK;", NULL, NULL, NULL);
        return ret + E_SQL_WRAPPER_BASE;
    }
}

int CHistoryDb::Delete(const std::string& txid, const std::string& address)
{
    if (!mDb) {
        return E_SQL_WRAPPER_DB_OPEN;
    }
    if (txid.empty() && address.empty()) {
        return E_SQL_WRAPPER_INVALID_ARGUMENT;
    }

    char* errMsg;
    char sql[512];
    sprintf(sql, "DELETE FROM %s WHERE", mTableName.c_str());
    if (!txid.empty()) {
        strcat(sql, " txid='");
        strcat(sql, txid.c_str());
        strcat(sql, "'");
    }
    if (!txid.empty() && !address.empty()) {
        strcat(sql, " AND");
    }
    if (!address.empty()) {
        strcat(sql, " address='");
        strcat(sql, address.c_str());
        strcat(sql, "'");
    }
    strcat(sql, ";");

    int ret = sqlite3_exec(mDb, sql, NULL, NULL, &errMsg);
    if (ret != SQLITE_OK) {
        Log::E("Database", "delete history failed ret %d, %s\n", ret, errMsg);
        sqlite3_free(errMsg);
        return ret + E_SQL_WRAPPER_BASE;
    }

    return E_SQL_WRAPPER_OK;
}

int CHistoryDb::Query(const std::string& address, std::vector<History*>* pHistories)
{
    if (!mDb) {
        return E_SQL_WRAPPER_DB_OPEN;
    }
    if (address.empty() || !pHistories) {
        return E_SQL_WRAPPER_INVALID_ARGUMENT;
    }

    char sql[512];
    sqlite3_stmt* pStmt = nullptr;
    int ret = E_SQL_WRAPPER_OK;
    sprintf(sql, "SELECT * FROM %s WHERE address='%s'", mTableName.c_str(), address.c_str());

    ret = sqlite3_prepare_v2(mDb, sql, -1, &pStmt, NULL);
    if (ret != SQLITE_OK) {
        ret += E_SQL_WRAPPER_BASE;
        goto exit;
    }

    while(SQLITE_ROW == sqlite3_step(pStmt)) {
        History* pHistory = new History();
        if (!pHistory) {
            ret = E_SQL_WRAPPER_OUT_OF_MEMORY;
            goto exit;
        }

        pHistory->mId = sqlite3_column_int(pStmt, 0);

        char* txid = (char*)sqlite3_column_text(pStmt, 1);
        pHistory->mTxid.assign(txid, strlen(txid));
        char* address = (char*)sqlite3_column_text(pStmt, 2);
        pHistory->mAddress.assign(address, strlen(address));
        char* direction = (char*)sqlite3_column_text(pStmt, 3);
        pHistory->mDirection.assign(direction, strlen(direction));

        pHistory->mAmount = sqlite3_column_int64(pStmt, 4);
        pHistory->mTime = sqlite3_column_int64(pStmt, 5);
        pHistory->mHeight = sqlite3_column_int64(pStmt, 6);
        pHistory->mFee = sqlite3_column_int(pStmt, 7);

        char* inputs = (char*)sqlite3_column_text(pStmt, 8);
        pHistory->mInputs.assign(inputs, strlen(inputs));
        char* outputs = (char*)sqlite3_column_text(pStmt, 9);
        pHistory->mOutputs.assign(outputs, strlen(outputs));

        pHistories->push_back(pHistory);
    }

exit:
    if (pStmt) {
        sqlite3_finalize(pStmt);
    }
    return ret;
}

void CHistoryDb::CloseDb()
{
    if (!mDb) return;

    sqlite3_close(mDb);
    mDb = nullptr;
}

std::string CHistoryDb::CreateInsertSql(const History& history)
{
    std::stringstream stream;
    stream << "INSERT OR REPLACE INTO " << mTableName;
    stream << "(id,txid,address,direction,amount,time,height,fee,inputs,outputs) VALUES (";
    stream << "(SELECT id FROM " << mTableName << " WHERE txid='" << history.mTxid << "' AND";
    stream << " address='" << history.mAddress << "'),'";
    stream << history.mTxid << "','" << history.mAddress << "','" << history.mDirection << "',";
    stream << history.mAmount << "," << history.mTime << "," << history.mHeight << ",";
    stream << history.mFee << ",'" << history.mInputs << "','" << history.mOutputs << "');";
    return stream.str();
}

} // namespace elastos
