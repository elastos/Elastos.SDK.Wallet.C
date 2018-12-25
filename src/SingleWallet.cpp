
#include "SingleWallet.h"
#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"
#include "common/Log.hpp"
#include "wrapper/httpclient/HttpClient.hpp"
#include "WalletError.h"
#include "nlohmann/json.hpp"
#include <sstream>

namespace elastos {

#define CLASS_TEXT  "SingleWallet"

SingleWallet::SingleWallet(const std::string& seed, std::unique_ptr<BlockChainNode> node)
    : mBlockChainNode(std::move(node))
{
    uint8_t* seedBuf;
    int seedLen = Utils::Str2Hex(seed, &seedBuf);
    if (seedLen == 0) {
        Log::E(CLASS_TEXT, "seed is empty\n");
        return;
    }

    char* publicKey = getSinglePublicKey(seedBuf, seedLen);
    free(seedBuf);
    if (!publicKey) return;
    mPublicKey = publicKey;
    free(publicKey);

    char* address = getAddress(mPublicKey.c_str());
    mAddress = address;
    free(address);
}

std::string SingleWallet::GetPublicKey()
{
    return mPublicKey;
}

std::string SingleWallet::GetAddress()
{
    return mAddress;
}

int SingleWallet::SendTransaction(const std::vector<Transaction>& transactions, const std::string& seed, std::string& txid)
{
    if (transactions.empty() || seed.empty()) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::string txJson;
    int ret = CreateTransaction(transactions, txJson);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "create transaction failed ret:%d\n", ret);
        return ret;
    }

    nlohmann::json jRet = nlohmann::json::parse(txJson);
    nlohmann::json jResult = jRet["result"];

    uint8_t* seedBuf;
    int seedLen = Utils::Str2Hex(seed, &seedBuf);
    if (seedLen == 0) {
        Log::E(CLASS_TEXT, "seed is empty\n");
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    char* privateKey = getSinglePrivateKey(seedBuf, seedLen);
    free(seedBuf);

    jResult["Transactions"][0]["UTXOInputs"][0]["privateKey"] = privateKey;

    Log::D(CLASS_TEXT, "transaction: %s\n", jResult.dump().c_str());

    char* signedData = generateRawTransaction(jResult.dump().c_str());
    free(privateKey);
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

    jRet = nlohmann::json::parse(result);
    if (jRet["status"] != 200) {
        std::string result = jRet["result"];
        Log::E(CLASS_TEXT, "send transaction failed: %s\n", result.c_str());
        return E_WALLET_C_NODE_ERROR;
    }

    txid = jRet["result"];

    return E_WALLET_C_OK;
}

long SingleWallet::GetBalance()
{
    HttpClient httpClient;
    HttpClient::InitGlobal();

    std::string url = mBlockChainNode->GetUrl();
    url.append("/api/1/balance/");
    url.append(mAddress);

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

int SingleWallet::GetIndex()
{
    return mIndex;
}

int SingleWallet::CreateTransaction(const std::vector<Transaction>& transactions, std::string& txJson)
{
    std::stringstream body;
    body << "{" << "\"inputs\":[\"" << mAddress << "\"],";
    body << "\"outputs\":[";
    int index = 0;
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

int SingleWallet::SendSignedTx(const std::string& signedTx, std::string& result)
{
    std::stringstream body;
    body << "{" << "\"data\":\"" << signedTx << "\"}";
    Log::D(CLASS_TEXT, "body: %s\n", body.str().c_str());

    return HttpPost("/api/1/sendRawTx", body.str(), result);
}

int SingleWallet::HttpPost(const std::string& api, const std::string& body, std::string& result)
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

void SingleWallet::SetIndex(int index)
{
    mIndex = index;
}

} // namespace elastos
