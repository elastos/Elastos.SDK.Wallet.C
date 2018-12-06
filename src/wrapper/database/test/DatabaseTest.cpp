
#include "CHistoryDb.h"
#include "SqliteWrapperError.h"

using namespace elastos;

int main(int argc, char** argv)
{
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
        return ret;
    }

    std::vector<History*> query;
    ret = historyDb.Query("EMHc9JSpxKWbTMf8gQDcWm7Tz1C5nQNA8Z", &query);
    if (ret != E_SQL_WRAPPER_OK) {
        printf("query failed %d\n", ret);
    }

    for (History* pHistory : query) {
        delete pHistory;
    }
    query.clear();

    ret = historyDb.Query("EbAATdrW7gaomFY3SAy81rokqwqKA3EXbT", &query);
    if (ret != E_SQL_WRAPPER_OK) {
        printf("query failed %d\n", ret);
    }

    for (History* pHistory : query) {
        printf("id: %d\n", pHistory->mId);
        printf("txid: %s\n", pHistory->mTxid.c_str());
        delete pHistory;
    }

    ret = historyDb.Delete("1e368ad6af41fb626d35f6e2dac238b6b64bfadcf3d3f297919f81029e0027ff", "EbAATdrW7gaomFY3SAy81rokqwqKA3EXbT");
    if (ret != E_SQL_WRAPPER_OK) {
        printf("delete failed %d\n", ret);
    }

    return 0;
}
