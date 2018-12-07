
#ifndef __CHISTORY_DATABASE_H__
#define __CHISTORY_DATABASE_H__

#include <string>
#include <vector>

struct sqlite3;

namespace elastos{

struct History
{
    int mId;
    std::string mTxid;
    std::string mAddress;
    std::string mDirection;
    long mAmount;
    long mTime;
    long mHeight;
    int mFee;
    std::string mInputs;
    std::string mOutputs;
};

class CHistoryDb
{
public:
    CHistoryDb(std::string filePath, std::string tableName);

    ~CHistoryDb();

    int Insert(const std::vector<History>& histories);

    int Delete(std::string txid, std::string address);

    int Query(std::string address, std::vector<History*>* pHistories);

private:
    void CloseDb();

    bool TableExist(std::string tableName);

    std::string CreateInsertSql(const History& history);

private:
    sqlite3* mDb;
    std::string mTableName;
};

} // namespace elastos

#endif //__CHISTORY_DATABASE_H__
