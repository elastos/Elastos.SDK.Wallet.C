
#include "CDidDb.h"
#include "SqliteWrapperError.h"
#include "CDbHelper.h"
#include "common/Utils.h"
#include "common/Log.hpp"
#include <sstream>
#include <string.h>

#define DID_TABLE_NAME     "DID"
#define DID_PROPERTIES_TABLE_NAME     "DIDProperties"

namespace elastos {

CDidDb::CDidDb(const std::string& filePath)
    : mDb(nullptr)
{
    std::string dbFile(filePath);
    dbFile += "/";
    dbFile += TEST_NET ? TEST_DB_FILE : DB_FILE;
    int ret = sqlite3_open(dbFile.c_str(), &mDb);
    if (ret !=  SQLITE_OK) {
        CloseDb();
        return;
    }

    if (!CDbHelper::TableExist(mDb, DID_TABLE_NAME)) {
        char sql[512];
        sprintf(sql, "CREATE TABLE %s(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, "
                "did TEXT UNIQUE NOT NULL, didStatus TEXT NOT NULL);", DID_TABLE_NAME);

        Log::D("Database", "%s\n", sql);

        char* errMsg;
        ret = sqlite3_exec(mDb, sql, NULL, NULL, &errMsg);
        if (ret !=  SQLITE_OK) {
            Log::E("Database", "create Did table failed ret %d, %s\n", ret, errMsg);
            sqlite3_free(errMsg);
            CloseDb();
            return;
        }
    }

    if (!CDbHelper::TableExist(mDb, DID_PROPERTIES_TABLE_NAME)) {
        char sql[512];
        sprintf(sql, "CREATE TABLE %s(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, did TEXT NOT NULL, "
            "key TEXT NOT NULL, value TEXT NOT NULL, status TEXT NOT NULL, "
            "blockTime INTEGER NOT NULL, txid TEXT NOT NULL, "
            "height INTEGER NOT NULL);", DID_PROPERTIES_TABLE_NAME);

        Log::D("Database", "%s\n", sql);

        char* errMsg;
        ret = sqlite3_exec(mDb, sql, NULL, NULL, &errMsg);
        if (ret !=  SQLITE_OK) {
            Log::E("Database", "create properties table failed ret %d, %s\n", ret, errMsg);
            sqlite3_free(errMsg);
            CloseDb();
        }
    }
}

CDidDb::~CDidDb()
{
    CloseDb();
}

int CDidDb::InsertProperty(const std::string& did, const std::vector<DidProperty>& properties)
{
    if (!mDb) {
        return E_SQL_WRAPPER_DB_OPEN;
    }
    if (did.empty() || properties.empty()) {
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
    std::string status;
    ret = GetDidStatus(did, status);
    if (ret != E_SQL_WRAPPER_OK) {
        Log::E("Database", "get did %s status failed ret %d\n", did.c_str(), ret);
        succeeded = false;
        goto exit;
    }

    if (status.empty()) {
        ret = InsertDid(did);
        if (ret != SQLITE_OK) {
            succeeded = false;
            goto exit;
        }
    }

    if (!status.compare("deprecated")) {
        Log::E("Database", "did %s deprecated\n", did.c_str());
        ret = E_SQL_WRAPPER_DID_DEPRECATED;
        succeeded = false;
        goto exit;
    }

    for (DidProperty property : properties) {
        std::string sql = CreateInsertSql(did, property);
        Log::D("Database", "insert sql: %s\n", sql.c_str());
        ret = sqlite3_exec(mDb, sql.c_str(), NULL, NULL, &errMsg);
        if (ret != SQLITE_OK) {
            Log::E("Database", "insert did property failed ret %d, %s\n", ret, errMsg);
            sqlite3_free(errMsg);
            succeeded = false;
            break;
        }
    }

exit:
    if (succeeded) {
        sqlite3_exec(mDb, "COMMIT;", NULL, NULL, NULL);
        return E_SQL_WRAPPER_OK;
    }
    else {
        sqlite3_exec(mDb, "ROLLBACK;", NULL, NULL, NULL);
        return ret + E_SQL_WRAPPER_BASE;
    }
}

int CDidDb::DeleteProperty(const std::string& did, const std::string& key)
{
    if (!mDb) {
        return E_SQL_WRAPPER_DB_OPEN;
    }
    if (did.empty()) {
        return E_SQL_WRAPPER_INVALID_ARGUMENT;
    }

    char* errMsg;
    char sql[512];
    sprintf(sql, "DELETE FROM %s WHERE did='%s'", DID_PROPERTIES_TABLE_NAME, did.c_str());
    if (!key.empty()) {
        strcat(sql, " AND key='");
        strcat(sql, key.c_str());
        strcat(sql, "'");
    }
    strcat(sql, ";");

    int ret = sqlite3_exec(mDb, sql, NULL, NULL, &errMsg);
    if (ret != SQLITE_OK) {
        Log::E("Database", "delete property failed ret %d, %s\n", ret, errMsg);
        sqlite3_free(errMsg);
        return ret + E_SQL_WRAPPER_BASE;
    }

    return E_SQL_WRAPPER_OK;
}

int CDidDb::QueryProperty(const std::string& did, const std::string& key, DidProperty* property)
{
    if (!mDb) {
        return E_SQL_WRAPPER_DB_OPEN;
    }
    if (did.empty() || key.empty() || !property) {
        return E_SQL_WRAPPER_INVALID_ARGUMENT;
    }

    char sql[512];
    sqlite3_stmt* pStmt = nullptr;
    int ret = E_SQL_WRAPPER_OK;
    sprintf(sql, "SELECT * FROM %s AS p INNER JOIN %s AS d ON p.did=d.did"
            " WHERE p.did='%s' AND p.key='%s';", DID_PROPERTIES_TABLE_NAME,
            DID_TABLE_NAME, did.c_str(), key.c_str());
    Log::D("Database", "query: %s\n", sql);

    ret = sqlite3_prepare_v2(mDb, sql, -1, &pStmt, NULL);
    if (ret != SQLITE_OK) {
        ret += E_SQL_WRAPPER_BASE;
        goto exit;
    }

    ret = sqlite3_step(pStmt);
    Log::D("Database", "query did key step ret: %d\n", ret);
    if (ret == SQLITE_ROW) {
        property->mId = sqlite3_column_int(pStmt, 0);

        char* did = (char*)sqlite3_column_text(pStmt, 1);
        property->mDid.assign(did, strlen(did));
        char* key = (char*)sqlite3_column_text(pStmt, 2);
        property->mKey.assign(key, strlen(key));
        char* value = (char*)sqlite3_column_text(pStmt, 3);
        property->mProperty.assign(value, strlen(value));
        char* status = (char*)sqlite3_column_text(pStmt, 4);
        property->mStatus.assign(status, strlen(status));

        property->mBlockTime = sqlite3_column_int64(pStmt, 5);

        char* txid = (char*)sqlite3_column_text(pStmt, 6);
        property->mTxid.assign(txid, strlen(txid));
        property->mHeight = sqlite3_column_int64(pStmt, 7);

        status = (char*)sqlite3_column_text(pStmt, 10);
        property->mDidStatus.assign(status, strlen(status));

        ret = E_SQL_WRAPPER_OK;
    }

exit:
    if (pStmt) {
        sqlite3_finalize(pStmt);
    }
    return ret;
}


void CDidDb::CloseDb()
{
    if (!mDb) return;

    sqlite3_close(mDb);
    mDb = nullptr;
}

int CDidDb::GetDidStatus(const std::string& did, std::string& status)
{
    char sql[512];
    sprintf(sql, "SELECT didStatus FROM %s WHERE did='%s';", DID_TABLE_NAME, did.c_str());
    sqlite3_stmt* pStmt = nullptr;
    int ret = SQLITE_OK;
    char* cStatus;

    ret = sqlite3_prepare_v2(mDb, sql, -1, &pStmt, NULL);
    if (ret != SQLITE_OK) {
        ret += E_SQL_WRAPPER_BASE;
        goto exit;
    }

    ret = sqlite3_step(pStmt);
    Log::D("Database", "get did status step ret: %d\n", ret);
    if (SQLITE_OK != ret && SQLITE_DONE != ret && SQLITE_ROW != ret) {
        goto exit;
    }
    ret = E_SQL_WRAPPER_OK;

    cStatus = (char*)sqlite3_column_text(pStmt, 0);
    if (cStatus != NULL) {
        status.assign(cStatus, strlen(cStatus));
    }

exit:
    if (pStmt) {
        sqlite3_finalize(pStmt);
    }
    return ret;
}

int CDidDb::InsertDid(const std::string& did)
{
    char sql[512];
    sprintf(sql, "INSERT INTO %s(did,didStatus) VALUES ('%s', '%s');",
            DID_TABLE_NAME, did.c_str(), "normal");
    char* errMsg;
    int ret = sqlite3_exec(mDb, sql, NULL, NULL, &errMsg);
    if (ret != SQLITE_OK) {
        Log::E("Database", "insert did failed ret %d, %s\n", ret, errMsg);
        sqlite3_free(errMsg);
        return ret + E_SQL_WRAPPER_BASE;
    }

    return E_SQL_WRAPPER_OK;
}

std::string CDidDb::CreateInsertSql(const std::string& did, const DidProperty& property)
{
    std::stringstream stream;
    stream << "INSERT OR REPLACE INTO " << DID_PROPERTIES_TABLE_NAME;
    stream << "(id,did,key,value,status,blockTime,txid,height) VALUES (";
    stream << "(SELECT id FROM " << DID_PROPERTIES_TABLE_NAME << " WHERE did='" << did << "' AND";
    stream << " key='" << property.mKey << "'),'" << did << "','";
    stream << property.mKey << "','" << property.mProperty << "','" << property.mStatus << "',";
    stream << property.mBlockTime << ",'" << property.mTxid << "'," << property.mHeight << ");";
    return stream.str();
}

} // namespace elastos
