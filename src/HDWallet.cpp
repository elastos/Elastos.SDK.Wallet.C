
#include "HDWallet.h"
#include "common/Log.hpp"
#include "common/Utils.h"
#include "WalletError.h"
#include "nlohmann/json.hpp"
#include "wrapper/httpclient/HttpClient.hpp"


#define CLASS_TEXT "HDWallet"
#define HISTORY_PAGE_SIZE   10
#define HTTP_TIME_OUT       10000

namespace elastos {

HDWallet::HDWallet(const std::string& localPath, const std::string& seed, std::unique_ptr<BlockChainNode>& node, int coinType, bool singleAddress)
    : mPath(localPath)
    , mBlockChainNode(std::move(node))
    , mCoinType(coinType)
    , mSingleAddress(singleAddress)
{
    uint8_t* seedBuf;
    int seedLen = Utils::Str2Hex(seed, &seedBuf);
    if (seedLen == 0) {
        Log::E(CLASS_TEXT, "seed is empty\n");
        return;
    }

    MasterPublicKey* masterPublicKey = getMasterPublicKey(seedBuf, seedLen, mCoinType);
    if (!masterPublicKey) {
        Log::E(CLASS_TEXT, "get master public key failed\n");
        return;
    }

    mMasterPublicKey.reset(masterPublicKey);
}

HDWallet::HDWallet(const std::string& localPath, const std::string& seed, std::unique_ptr<BlockChainNode>& node, int coinType)
    : HDWallet(localPath, seed, node, coinType, false)
{}

HDWallet::HDWallet(const std::string& localPath, const std::string& seed, std::unique_ptr<BlockChainNode>& node, bool singleAddress)
    : HDWallet(localPath, seed, node, COIN_TYPE_ELA, singleAddress)
{}

HDWallet::HDWallet(const std::string& localPath, const std::string& seed, std::unique_ptr<BlockChainNode>& node)
    : HDWallet(localPath, seed, node, COIN_TYPE_ELA, false)
{}

int HDWallet::GetCoinType()
{
    return mCoinType;
}

int HDWallet::SendTransaction(const std::vector<Transaction>& transactions,
        const std::string& memo, const std::string& seed, std::string& txid, const std::string& chain)
{
    if (transactions.empty() || seed.empty()) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::string txJson;
    int ret;
    if (mSingleAddress) {
        ret = SingleAddressCreateTx(transactions, memo, seed, chain, txJson);
    }
    else {
        ret = HDCreateTx(transactions, memo, seed, chain, txJson);
    }
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "create transaction failed ret:%d\n", ret);
        return ret;
    }

    char* signedData = generateRawTransaction(txJson.c_str());
    if (!signedData) {
        Log::E(CLASS_TEXT, "sign transaction error\n");
        return E_WALLET_C_SIGN_TX_ERROR;
    }

    std::string result;
    ret = SendSignedTx(signedData, result);
    free(signedData);
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

    InsertSendingTx(transactions, memo, txid, txJson);

    return E_WALLET_C_OK;
}

std::string HDWallet::GetAddress(int chain, int index)
{
    std::string publicKey = GetPublicKey(chain, index);
    char* address = getAddress(publicKey.c_str());
    std::string ret = address;
    free(address);
    return address;
}

std::string HDWallet::GetPublicKey(int chain, int index)
{
    assert(mMasterPublicKey && chain >= 0 && index >= 0);
    int param1 = mSingleAddress ? 0 : chain;
    int param2 = mSingleAddress ? 0 : index;
    char* publicKey = generateSubPublicKey(mMasterPublicKey.get(), param1, param2);
    std::string ret = publicKey;
    free(publicKey);
    return ret;
}

long HDWallet::GetBalance(const std::string& address)
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

int HDWallet::GetPosition()
{
    return mPosition;
}

int HDWallet::SyncHistory()
{
    if (mSingleAddress) {
        return SyncHistory(GetAddress(0, 0));
    }
    else {
        return SyncMultiHistory();
    }
}

int HDWallet::GetHistoryCount(const std::string& address)
{
    if (address.empty()) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    CHistoryDb db(mPath, GetTableName());
    int count;
    int ret = db.GetCount(address, &count);
    if (ret != E_WALLET_C_OK) {
        return 0;
    }

    return count;
}

int HDWallet::GetHistory(const std::string& address, int pageSize, int page, bool ascending, std::string& histories)
{
    if (address.empty() || pageSize <= 0 || page < 0) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    CHistoryDb db(mPath, GetTableName());
    std::vector<std::shared_ptr<History>> historyVector;
    int ret = db.Query(address, pageSize, page, ascending, &historyVector);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "query %s history failed ret:%d\n", address.c_str(), ret);
        return ret;
    }

    if (historyVector.empty()) {
        Log::D(CLASS_TEXT, "%s no history\n", address.c_str());
        histories.clear();
        return E_WALLET_C_OK;
    }

    nlohmann::json jHistories;
    std::vector<nlohmann::json> jHistoryVector;
    for (std::shared_ptr<History> history : historyVector) {
        nlohmann::json jHistory = {
            {"txid", history->mTxid},
            {"direction", history->mDirection},
            {"value", history->mAmount},
            {"time", history->mTime},
            {"height", history->mHeight},
            {"fee", history->mFee},
            {"inputs", history->mInputs},
            {"outputs", history->mOutputs},
            {"memo", history->mMemo}
        };

        jHistoryVector.push_back(jHistory);
    }
    jHistories = jHistoryVector;
    histories = jHistories.dump();

    return E_WALLET_C_OK;
}

int HDWallet::SingleAddressCreateTx(const std::vector<Transaction>& transactions,
        const std::string& memo, const std::string& seed, const std::string& chain, std::string& txJson)
{
    std::vector<std::string> addresses;
    std::string address = GetAddress(0, 0);
    addresses.push_back(address);

    std::string json;
    int ret = CreateTransaction(transactions, addresses, chain, json);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "create transaction failed ret:%d\n", ret);
        return ret;
    }

    nlohmann::json jRet = nlohmann::json::parse(json);
    nlohmann::json jResult = jRet["result"];

    uint8_t* seedBuf;
    int seedLen = Utils::Str2Hex(seed, &seedBuf);
    if (seedLen == 0) {
        Log::E(CLASS_TEXT, "seed is empty\n");
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    char* privateKey = generateSubPrivateKey(seedBuf, seedLen, mCoinType, 0, 0);
    free(seedBuf);

    jResult["Transactions"][0]["UTXOInputs"][0]["privateKey"] = privateKey;
    if (!memo.empty()) {
        jResult["Transactions"][0]["Memo"] = memo;
    }

    Log::D(CLASS_TEXT, "transaction: %s\n", jResult.dump().c_str());

    txJson = jResult.dump();
    free(privateKey);

    return E_WALLET_C_OK;
}

int HDWallet::HDCreateTx(const std::vector<Transaction>& transactions,
        const std::string& memo, const std::string& seed, const std::string& chain, std::string& txJson)
{
    return E_WALLET_C_NOT_IMPLEMENTED;
}

int HDWallet::CreateTransaction(const std::vector<Transaction>& transactions,
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

    return HttpPost(api, body.str(), txJson);
}

int HDWallet::SendSignedTx(const std::string& signedTx, std::string& result)
{
    std::stringstream body;
    body << "{" << "\"data\":\"" << signedTx << "\"}";
    Log::D(CLASS_TEXT, "body: %s\n", body.str().c_str());

    return HttpPost("/api/1/sendRawTx", body.str(), result);
}

int HDWallet::HttpPost(const std::string& api, const std::string& body, std::string& result)
{
    HttpClient httpClient;
    HttpClient::InitGlobal();

    std::string url = mBlockChainNode->GetUrl();
    url.append(api);
    int ret = httpClient.Url(url);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "httpClient url ret:%d\n", ret);
        return ret;
    }

    httpClient.SetTimeout(HTTP_TIME_OUT);
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

void HDWallet::SetPosition(int pos)
{
    assert(pos >= 0);
    mPosition = pos;
}

int HDWallet::SyncHistory(const std::string& address)
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

int HDWallet::GetHistoryAndSave(const std::string& address, int page, CHistoryDb& db, int* total)
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
    httpClient.SetTimeout(HTTP_TIME_OUT);
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

int HDWallet::SyncMultiHistory()
{
    return E_WALLET_C_NOT_IMPLEMENTED;
}

std::string HDWallet::GetTableName()
{
    std::string url = mBlockChainNode->GetUrl();
    int pos = url.find(':');
    pos = pos == std::string::npos ? 0 : pos + 3;

    return url.substr(pos);
}

int HDWallet::InsertSendingTx(const std::vector<Transaction>& transactions, const std::string& memo, const std::string& txid, const std::string& tx)
{
    if (!mSingleAddress) {
        return E_WALLET_C_NOT_IMPLEMENTED;
    }

    long amount = 0l;
    for (Transaction transaction : transactions) {
        amount += transaction.GetAmount();
    }

    History history;
    history.mTxid = txid;
    history.mAddress = GetAddress(0, 0);
    history.mDirection = "spending";
    history.mAmount = amount;
    history.mTime = time(0);
    history.mHeight = 0;

    nlohmann::json jTx = nlohmann::json::parse(tx);
    std::vector<nlohmann::json> inputsVector = jTx["Transactions"][0]["UTXOInputs"];
    std::string inputs;
    for (int i = 0; i < inputsVector.size(); i++) {
        inputs.append(inputsVector[i]["address"].get<std::string>());
        if (i != inputsVector.size() - 1) {
            inputs.append(";");
        }
    }
    history.mInputs = inputs;

    std::vector<nlohmann::json> outputsVector = jTx["Transactions"][0]["Outputs"];
    std::string outputs;
    for (int i = 0; i < outputsVector.size(); i++) {
        outputs.append(outputsVector[i]["address"].get<std::string>());
        if (i != outputsVector.size() - 1) {
            outputs.append(";");
        }
    }
    history.mOutputs = outputs;
    history.mMemo = memo;

    std::vector<History> historyVector;
    historyVector.push_back(history);
    CHistoryDb db(mPath, GetTableName());

    return db.Insert(historyVector);
}

} // namespace elastos
