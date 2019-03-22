#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"
#include "common/Log.hpp"
#include "nlohmann/json.hpp"
#include <vector>
#include "WalletError.h"
#include "wrapper/httpclient/HttpClient.hpp"
#include "wrapper/database/CDidDb.h"
#include "wrapper/database/SqliteWrapperError.h"

#include "Did.h"

namespace elastos {

Did::Did(const std::string& publicKey, int index, const std::string& path)
    : mPublicKey(publicKey)
    , mIndex(index)
    , mPath(path)
{
    assert(!publicKey.empty());

    mBlockChainNode = std::make_shared<BlockChainNode>(DID_SERVICE_URL);

    char* did = getDid(publicKey.c_str());
    mDid = did;
    free(did);
}

std::string Did::GetId()
{
    return mDid;
}

std::string Did::SignInfo(const std::string& seed, const std::string& json)
{
    uint8_t* buf;
    int len = GenDidUploadInfo(json, &buf);
    if (len <= 0) {
        Log::E("Did", "generate upload json data failed\n");
        return "";
    }

    std::string signedInfo = SignInfo(seed, buf, len);
    if (signedInfo.empty()) {
        Log::E("Did", "sign did info failed\n");
        free(buf);
        return "";
    }

    nlohmann::json memo = {
        {"msg", Utils::Hex2Str(buf, len).c_str()},
        {"pub", mPublicKey},
        {"sig", signedInfo.c_str()}
    };
    free(buf);

    return memo.dump();
}

std::string Did::SetInfo(const std::string& seed, const std::string& json, const std::shared_ptr<HDWallet>& wallet)
{
    std::string memo = SignInfo(seed, json);
    if (memo.empty()) {
        Log::E("Did", "sign did info and generate memo failed\n");
        return "";
    }

    Transaction tx(DID_SET_PROPERTY_TX_ADDRESS, DID_SET_PROPERTY_FEE);
    std::vector<Transaction> transactions;
    transactions.push_back(tx);
    std::string txid;
    int ret = wallet->SendTransaction(transactions, memo, seed, txid);
    if (ret != E_WALLET_C_OK) {
        Log::E("Did", "send transaction failed: %d\n", ret);
        return "";
    }

    return txid;
}

int Did::SyncInfo()
{
    HttpClient httpClient;
    HttpClient::InitGlobal();

    std::string url = mBlockChainNode->GetUrl();
    url.append("/api/1/didexplorer/did/");
    url.append(mDid);
    url.append("?detailed=true");

    Log::D("Did", "url: %s\n", url.c_str());

    httpClient.Url(url);
    httpClient.SetTimeout(5000);
    httpClient.SetHeader("Content-Type", "application/json");
    int ret = httpClient.SyncGet();
    if (ret != E_WALLET_C_OK) {
        Log::E("Did", "http get did properties failed ret:%d\n", ret);
        return ret;
    }

    ret = httpClient.GetResponseStatus();
    if (ret != 200) {
        Log::E("Did", "http get properties status: %d\n", ret);
        return ret;
    }

    std::string bodyStr;
    ret = httpClient.GetResponseBody(bodyStr);
    if (ret < 0) {
        Log::E("Did", "http get properties response: %d\n", ret);
        return ret;
    }

    Log::D("Did", "response: %s\n", bodyStr.c_str());
    nlohmann::json jRsp = nlohmann::json::parse(bodyStr);
    int status = jRsp["status"];
    if (status != 200) {
        std::string result = jRsp["result"];
        Log::E("Did", "get properties failed: %s\n", result.c_str());
        return Utils::ServiceErr2SdkErr(status);
    }

    std::string result = jRsp["result"];
    if (result.empty()) {
        Log::D("Did", "did is deprecated or no property set\n");
        return E_WALLET_C_OK;
    }

    std::vector<nlohmann::json> jProperties = nlohmann::json::parse(result);
    std::vector<DidProperty> properties;
    for (nlohmann::json property : jProperties) {
        DidProperty dp;
        dp.mKey = property["propertyKey"];
        dp.mProperty = property["propertyValue"];
        dp.mDid = mDid;
        dp.mBlockTime = property["blockTime"];
        dp.mHeight = property["height"];
        dp.mTxid = property["txid"];
        properties.push_back(dp);
    }
    CDidDb db(mPath);
    db.InsertProperty(mDid, properties);

    return jProperties.size();
}

std::string Did::GetInfo(const std::string& key)
{
    CDidDb db(mPath);
    DidProperty property;
    int ret = db.QueryProperty(mDid, key, &property);
    if (ret != E_SQL_WRAPPER_OK) {
        Log::E("Did", "query property failed %d\n", ret);
        return "";
    }

    nlohmann::json jpro = {
        {"did", mDid},
        {"key", key},
        {"value", property.mProperty},
        {"blockTime", property.mBlockTime},
        {"txid", property.mTxid}
    };
    return jpro.dump();
}

std::string Did::SignInfo(const std::string& seed, const uint8_t* message, int len)
{
    uint8_t* seedBuf;
    int seedLen = Utils::Str2Hex(seed, &seedBuf);
    char* privateKey = generateSubPrivateKey(seedBuf, seedLen, COIN_TYPE_ELA, EXTERNAL_CHAIN, mIndex);
    free(seedBuf);

    char* publicKey = getPublicKeyFromPrivateKey(privateKey);
    if (mPublicKey.compare(publicKey)) {
        Log::E("Did", "private key is not match the publick key\n");
        free(privateKey);
        free(publicKey);
        return "";
    }
    free(publicKey);

    void* signedData;
    int signedLen = sign(privateKey, message, len, &signedData);
    free(privateKey);
    if (signedLen <= 0) {
        Log::E("Did", "sign data failed\n");
        return "";
    }

    return Utils::Hex2Str((const uint8_t*)signedData, signedLen);
}

int Did::GenDidUploadInfo(const std::string& json, uint8_t** buf)
{
    std::vector<nlohmann::json> properties = nlohmann::json::parse(json);
    nlohmann::json didInfo;
    didInfo["Tag"] = "DID Property";
    didInfo["Ver"] = "1.0";
    didInfo["Status"] = "Normal";
    // TODO: "Did" is compatible with did service, remove it later.
    didInfo["Did"] = mDid;
    for (int i = 0; i < properties.size(); i++) {
        properties[i]["Status"] = "Normal";
    }
    didInfo["Properties"] = properties;

    std::string info = didInfo.dump();
    Log::D("Did","did info:%s\n", info.c_str());

    int len = info.length();
    uint8_t* binary = (uint8_t*)malloc(len);
    if (!binary) {
        Log::E("Did", "out of memory\n");
        return E_WALLET_C_OUT_OF_MEMORY;
    }

    const char* p = info.c_str();
    for (int i = 0; i < len; i++) {
        binary[i] = p[i];
    }

    *buf = binary;
    return len;
}

void Did::SetNode(const std::shared_ptr<BlockChainNode>& node)
{
    mBlockChainNode.reset();
    mBlockChainNode = node;
}

}
