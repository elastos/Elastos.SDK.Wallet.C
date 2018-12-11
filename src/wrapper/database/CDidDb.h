
#ifndef __CDID_DATABASE_H__
#define __CDID_DATABASE_H__

#include <string>
#include <vector>
#include <sqlite3.h>

struct sqlite3;

namespace elastos {

struct DidProperty
{
    int mId;
    std::string mDid;
    std::string mDidStatus;
    std::string mKey;
    std::string mProperty;
    std::string mStatus;
    long mBlockTime;
    std::string mTxid;
    long mHeight;
};

class CDidDb
{
public:
    CDidDb(const std::string& filePath);

    ~CDidDb();

    int InsertProperty(const std::string& did, const std::vector<DidProperty>& properties);

    int DeleteProperty(const std::string& did, const std::string& key);

    int QueryProperty(const std::string& did, const std::string& key, DidProperty* property);

private:
    void CloseDb();

    int GetDidStatus(const std::string& did, std::string& status);

    int InsertDid(const std::string& did);

    std::string CreateInsertSql(const std::string& did, const DidProperty& property);

private:
    sqlite3* mDb;
};

} // namespace elastos

#endif // __CDID_DATABASE_H__
