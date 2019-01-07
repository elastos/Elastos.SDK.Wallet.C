
#include "HDWallet.h"
#include "common/Log.hpp"
#include "common/Utils.h"
#include "WalletError.h"
#include "nlohmann/json.hpp"
#include "wrapper/httpclient/HttpClient.hpp"


#define CLASS_TEXT "HDWallet"

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

int HDWallet::SendTransaction(const std::vector<Transaction>& transactions, const std::string& seed, std::string& txid)
{
    if (transactions.empty() || seed.empty()) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::string txJson;
    int ret;
    if (mSingleAddress) {
        ret = SingleAddressCreateTx(transactions, seed, txJson);
    }
    else {
        ret = HDCreateTx(transactions, seed, txJson);
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

int HDWallet::GetBalance(const std::string& address)
{
    HttpClient httpClient;
    HttpClient::InitGlobal();

    std::string url = mBlockChainNode->GetUrl();
    url.append("/api/1/balance/");
    url.append(address);

    Log::D(CLASS_TEXT, "url: %s\n", url.c_str());

    httpClient.Url(url);
    httpClient.SetTimeout(5000);
    httpClient.SetHeader("Accept-Encoding", "identity");
    int ret = httpClient.SyncGet();
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "http get balance failed ret:%d\n", ret);
        return 0;
    }

    ret = httpClient.GetResponseStatus();
    if (ret != 200) {
        Log::E(CLASS_TEXT, "http get balance status: %d\n", ret);
        return 0;
    }

    std::string bodyStr;
    ret = httpClient.GetResponseBody(bodyStr);
    if (ret < 0) {
        Log::E(CLASS_TEXT, "http get balance response: %d\n", ret);
        return 0;
    }

    Log::D(CLASS_TEXT, "response: %s\n", bodyStr.c_str());
    nlohmann::json jReslut = nlohmann::json::parse(bodyStr);
    if (jReslut["status"] != 200) {
        std::string result = jReslut["result"];
        Log::E(CLASS_TEXT, "get balance failed: %s\n", result.c_str());
        return 0;
    }

    std::string result = jReslut["result"];
    double balance = std::stod(result);

    return balance * 100000000;
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

int HDWallet::SingleAddressCreateTx(const std::vector<Transaction>& transactions, const std::string& seed, std::string& txJson)
{
    std::vector<std::string> addresses;
    std::string address = GetAddress(0, 0);
    addresses.push_back(address);

    std::string json;
    int ret = CreateTransaction(transactions, addresses, json);
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

    Log::D(CLASS_TEXT, "transaction: %s\n", jResult.dump().c_str());

    txJson = jResult.dump();
    free(privateKey);

    return E_WALLET_C_OK;
}

int HDWallet::HDCreateTx(const std::vector<Transaction>& transactions, const std::string& seed, std::string& txJson)
{
    return E_WALLET_C_NOT_IMPLEMENTED;
}

int HDWallet::CreateTransaction(const std::vector<Transaction>& transactions,
        const std::vector<std::string>& addresses, std::string& txJson)
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

    return HttpPost("/api/1/createTx", body.str(), txJson);
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

    httpClient.SetTimeout(10000);
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
    //remove the protocol
    std::string url = mBlockChainNode->GetUrl();
    int pos = url.find(':');
    pos = pos == std::string::npos ? 0 : pos + 3;

    CHistoryDb db(mPath, url.substr(pos));
    int count = 0;
    int ret = db.GetCount(address, &count);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "SyncHistory get count from db failed ret:%d\n", ret);
        return ret;
    }
    int pageNum = floor(count / 20) + 1;
    Log::D(CLASS_TEXT, "history count: %d, page: %d\n", count, pageNum);

    int total = 0;
    ret = GetHistroyAndSave(address, pageNum, db, &total);
    if (ret < 0) {
        Log::E(CLASS_TEXT, "get histroy and save failed ret:%d\n", ret);
        return ret;
    }

    if (ret == 0 || total <= pageNum * 20) {
        return E_WALLET_C_OK;
    }

    int pages = ceil(total / 20);
    Log::D(CLASS_TEXT, "history total: %d, pages: %d\n", total, pages);
    while (++pageNum <= pages) {
        ret = GetHistroyAndSave(address, pageNum, db);
        if (ret < 0) {
            Log::E(CLASS_TEXT, "get histroy and save failed ret:%d\n", ret);
            return ret;
        }
    }

    return E_WALLET_C_OK;
}

int HDWallet::GetHistroyAndSave(const std::string& address, int page, CHistoryDb& db, int* total)
{
    HttpClient httpClient;
    HttpClient::InitGlobal();

    std::string url = TEST_NET ? TEST_NET_WALLET_SERVICE_URL : WALLET_SERVICE_URL;
    url.append("/api/1/history/");
    url.append(address);
    url.append("?pageSize=20&pageNum=");
    url.append(std::to_string(page));

    Log::D(CLASS_TEXT, "url: %s\n", url.c_str());

    httpClient.Url(url);
    httpClient.SetTimeout(5000);
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

} // namespace elastos
