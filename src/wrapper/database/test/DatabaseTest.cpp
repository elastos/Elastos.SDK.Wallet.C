
#include "CHistoryDb.h"
#include "SqliteWrapperError.h"
#include "CDidDb.h"

using namespace elastos;

void TestHistory();
void TestDid();

int main(int argc, char** argv)
{
    TestHistory();

    TestDid();

    return 0;
}

void TestHistory() {
    CHistoryDb historyDb("/Users/nathansfile/Elastos.SDK.Wallet.C/obj", "testTable");

    History history;
    history.mTxid = "cd21b8729ca6173862034fb5515d395c25e5c7779330aa3634d7128435ddd4f4";
    history.mAddress = "EbAATdrW7gaomFY3SAy81rokqwqKA3EXbT";
    history.mDirection = "spend";
    history.mAmount = 100;
    history.mTime = 1541755973;
    history.mHeight = 157576;
    history.mFee = 100;
    history.mInputs = "EbAATdrW7gaomFY3SAy81rokqwqKA3EXbT";
    history.mOutputs = "EbAATdrW7gaomFY3SAy81rokqwqKA3EXbT;EbAATdrW7gaomFY3SAy81rokqwqKA3EXbT";

    History history2;
    history2.mTxid = "1e368ad6af41fb626d35f6e2dac238b6b64bfadcf3d3f297919f81029e0027ff";
    history2.mAddress = "EbAATdrW7gaomFY3SAy81rokqwqKA3EXbT";
    history2.mDirection = "spend";
    history2.mAmount = 10100;
    history2.mTime = 1542089418;
    history2.mHeight = 160171;
    history2.mFee = 100;
    history2.mInputs = "EbAATdrW7gaomFY3SAy81rokqwqKA3EXbT";
    history2.mOutputs = "EMHc9JSpxKWbTMf8gQDcWm7Tz1C5nQNA8Z;EbAATdrW7gaomFY3SAy81rokqwqKA3EXbT";

    std::vector<History> histories;
    histories.push_back(history);
    histories.push_back(history2);

    int ret = historyDb.Insert(histories);
    if (ret != E_SQL_WRAPPER_OK) {
        printf("insert failed %d\n", ret);
        return;
    }

    std::vector<std::shared_ptr<History>> query;
    ret = historyDb.Query("EMHc9JSpxKWbTMf8gQDcWm7Tz1C5nQNA8Z", 5, 0, true, &query);
    if (ret != E_SQL_WRAPPER_OK) {
        printf("query failed %d\n", ret);
    }

    query.clear();

    ret = historyDb.Query("EbAATdrW7gaomFY3SAy81rokqwqKA3EXbT", 5, 0, true, &query);
    if (ret != E_SQL_WRAPPER_OK) {
        printf("query failed %d\n", ret);
    }

    for (std::shared_ptr<History> pHistory : query) {
        printf("id: %d\n", pHistory->mId);
        printf("txid: %s\n", pHistory->mTxid.c_str());
    }

    ret = historyDb.Delete("1e368ad6af41fb626d35f6e2dac238b6b64bfadcf3d3f297919f81029e0027ff", "EbAATdrW7gaomFY3SAy81rokqwqKA3EXbT");
    if (ret != E_SQL_WRAPPER_OK) {
        printf("delete failed %d\n", ret);
    }
}

void TestDid()
{
    CDidDb didDb("/Users/nathansfile/Elastos.SDK.Wallet.C/obj");

    DidProperty property1;
    property1.mKey = "clark";
    property1.mProperty = "hello,world";
    property1.mStatus = "normal";
    property1.mBlockTime = 1543902641;
    property1.mTxid = "c39dd1463678146467ea1b43d8905e75ac34e727a4c36824410089a6682b43c8";
    property1.mHeight = 66480;

    DidProperty property2;
    property2.mKey = "phone";
    property2.mProperty = "13678929000";
    property2.mStatus = "normal";
    property2.mBlockTime = 1543902641;
    property2.mTxid = "c39dd1463678146467ea1b43d8905e75ac34e727a4c36824410089a6682b43c8";
    property2.mHeight = 66480;

    std::vector<DidProperty> properties;
    properties.push_back(property1);
    properties.push_back(property2);

    int ret = didDb.InsertProperty("iYnguKQcpeVyrpN6edamSkky1brvQvCWr6", properties);
    if (ret != E_SQL_WRAPPER_OK) {
        printf("insert failed %d\n", ret);
        return;
    }

    DidProperty property;
    ret = didDb.QueryProperty("iYnguKQcpeVyrpN6edamSkky1brvQvCWr6", "alice", &property);
    if (ret == E_SQL_WRAPPER_OK) {
        printf("property: %s, didstatus: %s\n", property.mProperty.c_str(), property.mDidStatus.c_str());
    }
    else if (ret == E_SQL_WRAPPER_NOT_FOUND) {
        printf("query not found\n");
    }
    else {
        printf("query failed %d\n", ret);
    }

    ret = didDb.DeleteProperty("iYnguKQcpeVyrpN6edamSkky1brvQvCWr6", "phone");
    if (ret != E_SQL_WRAPPER_OK) {
        printf("delete property failed ret %d\n", ret);
    }

}

