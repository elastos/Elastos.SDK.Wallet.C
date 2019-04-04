
#include "Wallet.h"
#include "Elastos.Wallet.Utility.h"
#include "common/Log.hpp"
#include "common/Utils.h"
#include "WalletError.h"
#include "nlohmann/json.hpp"
#include "wrapper/database/CHistoryDb.h"
#include "wrapper/httpclient/HttpClient.hpp"


#define CLASS_TEXT "Wallet"
#define HISTORY_PAGE_SIZE   10
#define HTTP_TIME_OUT       5000

namespace elastos {

int Wallet::GetCoinType()
{
    return mCoinType;
}

int Wallet::SyncHistory(const std::string& address, bool* hasHistory)
{
    CHistoryDb db(mPath, GetTableName());
    int count = 0;
    int ret = db.GetCount(address, &count);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "SyncHistory get count from db failed ret:%d\n", ret);
        return ret;
    }
    int pageNum = floor(count / HISTORY_PAGE_SIZE) + 1;
    Log::D(CLASS_TEXT, "history count: %d, page: %d\n", count, pageNum);

    int total = 0;
    ret = GetHistoryAndSave(address, pageNum, db, &total);
    if (ret < 0) {
        Log::E(CLASS_TEXT, "get history and save failed ret:%d\n", ret);
        return ret;
    }

    if ((count > 0 || total > 0) && hasHistory) {
        *hasHistory = true;
    }

    if (ret == 0 || total <= pageNum * HISTORY_PAGE_SIZE) {
        return E_WALLET_C_OK;
    }

    double pages = std::ceil((double)total / HISTORY_PAGE_SIZE);
    Log::D(CLASS_TEXT, "history total: %d, pages: %f\n", total, pages);
    while (++pageNum <= pages) {
        ret = GetHistoryAndSave(address, pageNum, db);
        if (ret < 0) {
            Log::E(CLASS_TEXT, "get history and save failed ret:%d\n", ret);
            return ret;
        }
    }

    return E_WALLET_C_OK;
}

int Wallet::GetHistoryAndSave(const std::string& address, int page, CHistoryDb& db, int* total)
{
    HttpClient httpClient;
    HttpClient::InitGlobal();

    std::string url = mBlockChainNode->GetUrl();
    url.append("/api/1/history/");
    url.append(address);
    url.append("?pageSize=");
    url.append(std::to_string(HISTORY_PAGE_SIZE));
    url.append("&pageNum=");
    url.append(std::to_string(page));

    Log::D(CLASS_TEXT, "url: %s\n", url.c_str());

    httpClient.Url(url);
    httpClient.SetConnectTimeout(HTTP_TIME_OUT);
    httpClient.SetHeader("Content-Type", "application/json");
    int ret = httpClient.SyncGet();
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "http get address history failed ret:%d\n", ret);
        return ret;
    }

    ret = httpClient.GetResponseStatus();
    if (ret != 200) {
        Log::E(CLASS_TEXT, "http get history response: %d\n", ret);
        return -ret;
    }

    std::string result;
    httpClient.GetResponseBody(result);
    Log::D(CLASS_TEXT, "response: %s\n", result.c_str());
    nlohmann::json jbody = nlohmann::json::parse(result);
    int status = jbody["status"];
    if (status != 200) {
        Log::E(CLASS_TEXT, "http get history result status: %d\n", status);
        return Utils::ServiceErr2SdkErr(status);
    }

    nlohmann::json jResult = jbody["result"];
    if (total != nullptr) {
        *total = jResult["TotalNum"];
    }

    std::vector<nlohmann::json> jHistories = jResult["History"];
    if (jHistories.empty()) {
        Log::D(CLASS_TEXT, "no history\n");
        return 0;
    }

    int count = 0;
    std::vector<History> histories;
    for(nlohmann::json jHistory : jHistories) {
        History history;
        history.mTxid = jHistory["Txid"];
        history.mAddress = address;
        history.mDirection = jHistory["Type"];
        history.mAmount = jHistory["Value"];
        history.mTime = jHistory["CreateTime"];
        history.mHeight = jHistory["Height"];
        history.mFee = jHistory["Fee"];

        std::vector<std::string> inputs = jHistory["Inputs"];
        std::string inputsStr = "";
        for (int i = 0; i < inputs.size(); i++) {
            inputsStr.append(inputs[i]);
            if (i != inputs.size() - 1) {
                inputsStr.append(";");
            }
        }
        history.mInputs = inputsStr;

        std::vector<std::string> outputs = jHistory["Outputs"];
        std::string outputsStr = "";
        for (int i = 0; i < outputs.size(); i++) {
            outputsStr.append(outputs[i]);
            if (i != outputs.size() - 1) {
                outputsStr.append(";");
            }
        }
        history.mOutputs = outputsStr;
        history.mMemo = jHistory["Memo"];

        histories.push_back(history);
        count++;
    }

    ret = db.Insert(histories);
    if (ret == E_WALLET_C_OK) {
        return count;
    }
    else {
        return ret;
    }
}

std::string Wallet::GetTableName()
{
    std::string url = mBlockChainNode->GetUrl();
    int pos = url.find(':');
    pos = pos == std::string::npos ? 0 : pos + 3;

    return url.substr(pos);
}

long Wallet::GetBalance(const std::string& address)
{
    CHistoryDb db(mPath, GetTableName());

    int count;
    int ret = db.GetCount(address, &count);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "get history count failed\n");
        return 0;
    }

    std::vector<std::shared_ptr<History>> histories;
    ret = db.Query(address, count, 0, true, &histories);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "get history failed\n");
        return 0;
    }

    long balance = 0;
    for (std::shared_ptr<History> history : histories) {
        if (!history->mDirection.compare("income")) {
            balance += history->mAmount;
        }
        else {
            balance -= history->mAmount;
        }
    }

    return balance;
}

int Wallet::CreateTransaction(const std::vector<Transaction>& transactions,
            const std::vector<std::string>& addresses, const std::string& chain, std::string& txJson)
{
    std::stringstream body;
    body << "{" << "\"inputs\":[";
    int index = 0;
    for (std::string address : addresses) {
        if (index > 0) {
            body << ",";
        }
        if (!address.empty()) {
            body << "\"" << address << "\"";
            index++;
        }
    }
    body << "],";
    body << "\"outputs\":[";
    index = 0;
    for (Transaction tx : transactions) {
        if (index > 0) {
            body << ",";
        }
        std::string address = tx.GetAddress();
        if (!address.empty()) {
            body << "{\"addr\":\"" << address << "\",";
            body << "\"amt\":" << tx.GetAmount() << "}";
            index++;
        }
    }
    body << "]}";

    Log::D(CLASS_TEXT, "body: %s\n", body.str().c_str());

    std::string api = chain.empty() ? "/api/1/createTx" : "/api/1/createCrossTx";

    int ret = HttpPost(api, body.str(), txJson);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "create transaction http error: %d\n", ret);
        return ret;
    }

    nlohmann::json jRet = nlohmann::json::parse(txJson);
    int status = jRet["status"].get<int>();
    if (status != 200) {
        Log::E(CLASS_TEXT, "create transaction error: %s\n", jRet["result"].get<std::string>().c_str());
        return E_WALLET_C_INTERNAL_ERROR;
    }

    return E_WALLET_C_OK;
}

int Wallet::SendRawRxAndInsert(const std::vector<Transaction>& transactions, const std::string& memo,
        const std::string& txJson, const std::string& signedTx, bool singleAddress, std::string& txid)
{
    std::string result;
    int ret = SendSignedTx(signedTx, result);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "send transaction failed ret:%d\n", ret);
        return ret;
    }

    nlohmann::json jRet = nlohmann::json::parse(result);
    if (jRet["status"] != 200) {
        std::string result = jRet["result"];
        Log::E(CLASS_TEXT, "send transaction failed: %s\n", result.c_str());
        return E_WALLET_C_NODE_ERROR;
    }

    txid = jRet["result"];

    InsertSendingTx(transactions, memo, txid, txJson, singleAddress);

    return E_WALLET_C_OK;
}

int Wallet::HttpPost(const std::string& api, const std::string& body, std::string& result)
{
    HttpClient httpClient;
    HttpClient::InitGlobal();

    std::string url = mBlockChainNode->GetUrl();
    url.append(api);
    Log::D(CLASS_TEXT, "http post: %s\n", url.c_str());
    int ret = httpClient.Url(url);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "httpClient url ret:%d\n", ret);
        return ret;
    }

    httpClient.SetConnectTimeout(HTTP_TIME_OUT);
    httpClient.SetHeader("Content-Type", "application/json");

    ret = httpClient.SyncPost(body.c_str());
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "httpClient sync post failed ret:%d\n", ret);
        return ret;
    }

    ret = httpClient.GetResponseStatus();
    if (ret != 200) {
        Log::E(CLASS_TEXT, "http response: %d\n", ret);
        return ret;
    }

    ret = httpClient.GetResponseBody(result);
    Log::D(CLASS_TEXT, "response: %s\n", result.c_str());
    if (ret > 0) return 0;
    else return ret;
}

int Wallet::SendSignedTx(const std::string& signedTx, std::string& result)
{
    std::stringstream body;
    body << "{" << "\"data\":\"" << signedTx << "\"}";
    Log::D(CLASS_TEXT, "body: %s\n", body.str().c_str());

    return HttpPost("/api/1/sendRawTx", body.str(), result);
}

int Wallet::InsertSendingTx(const std::vector<Transaction>& transactions,
            const std::string& memo, const std::string& txid, const std::string& tx, bool singleAddress)
{
    long amount = 0l;
    for (Transaction transaction : transactions) {
        amount += transaction.GetAmount();
    }

    nlohmann::json jTx = nlohmann::json::parse(tx);
    std::vector<nlohmann::json> inputsVector = jTx["Transactions"][0]["UTXOInputs"];
    std::string inputs;
    for (int i = 0; i < inputsVector.size(); i++) {
        inputs.append(inputsVector[i]["address"].get<std::string>());
        if (i != inputsVector.size() - 1) {
            inputs.append(";");
        }
    }
    std::vector<nlohmann::json> outputsVector = jTx["Transactions"][0]["Outputs"];
    std::string outputs;
    for (int i = 0; i < outputsVector.size(); i++) {
        if (outputsVector[i]["amount"] == 0) continue;
        outputs.append(outputsVector[i]["address"].get<std::string>());
        if (i != outputsVector.size() - 1) {
            outputs.append(";");
        }
    }

    std::vector<History> historyVector;
    for (nlohmann::json input : inputsVector) {
        History history;
        history.mTxid = txid;
        history.mAddress = input["address"];
        history.mDirection = "spending";
        history.mAmount = singleAddress ? amount : GetBalance(history.mAddress);
        history.mTime = time(0);
        history.mHeight = 0;

        history.mInputs = inputs;
        history.mOutputs = outputs;
        history.mMemo = memo;

        historyVector.push_back(history);
    }

    CHistoryDb db(mPath, GetTableName());
    return db.Insert(historyVector);
}

} // namespace elastos
