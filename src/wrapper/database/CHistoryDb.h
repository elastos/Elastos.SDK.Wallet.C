
#ifndef __CHISTORY_DATABASE_H__
#define __CHISTORY_DATABASE_H__

#include <string>
#include <vector>
#include <memory>

struct sqlite3;

namespace elastos {

struct History
{
    int mId;
    std::string mTxid;
    std::string mAddress;
    std::string mDirection;
    long mAmount;
    long mTime;
    long mHeight = 0;
    int mFee = 100;
    std::string mInputs;
    std::string mOutputs;
    std::string mMemo;
};

class CHistoryDb
{
public:
    CHistoryDb(const std::string& filePath, const std::string& tableName);

    ~CHistoryDb();

    int Insert(const std::vector<History>& histories);

    int Delete(const std::string& txid, const std::string& address);

    int Query(const std::string& address, int pageSize, int page, bool ascending, std::vector<std::shared_ptr<History>>* pHistories);

    int GetCount(const std::string& address, int* count);

private:
    void CloseDb();

    std::string CreateInsertSql(const History& history);

private:
    sqlite3* mDb;
    std::string mTableName;
};

} // namespace elastos

#endif //__CHISTORY_DATABASE_H__
