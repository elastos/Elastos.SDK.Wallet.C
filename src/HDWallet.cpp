
#include "HDWallet.h"
#include "common/Log.hpp"
#include "common/Utils.h"
#include "WalletError.h"
#include "nlohmann/json.hpp"
#include "wrapper/httpclient/HttpClient.hpp"


#define CLASS_TEXT "HDWallet"

namespace elastos {

HDWallet::HDWallet(const std::string& seed, std::unique_ptr<BlockChainNode>& node, int coinType, bool singleAddress)
    : mBlockChainNode(std::move(node))
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

HDWallet::HDWallet(const std::string& seed, std::unique_ptr<BlockChainNode>& node, int coinType)
    : HDWallet(seed, node, coinType, false)
{}

HDWallet::HDWallet(const std::string& seed, std::unique_ptr<BlockChainNode>& node, bool singleAddress)
    : HDWallet(seed, node, COIN_TYPE_ELA, singleAddress)
{}

HDWallet::HDWallet(const std::string& seed, std::unique_ptr<BlockChainNode>& node)
    : HDWallet(seed, node, COIN_TYPE_ELA, false)
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

int HDWallet::GetIndex()
{
    return mIndex;
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

void HDWallet::SetIndex(int index)
{
    assert(index >= 0);
    mIndex = index;
}

} // namespace elastos
