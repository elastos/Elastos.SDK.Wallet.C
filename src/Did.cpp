#include "Did.h"
#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"
#include "common/Log.hpp"
#include "nlohmann/json.hpp"
#include <vector>
#include "WalletError.h"

namespace elastos {

Did::Did(const std::string& publicKey)
    : mPublicKey(publicKey)
{
    assert(!publicKey.empty());
    char* did = getDid(publicKey.c_str());
    mDid = did;
    free(did);
}

std::string Did::GetId()
{
    return mDid;
}

std::string Did::SignInfo(const std::string& seed, int index, const std::string& json)
{
    uint8_t* buf;
    int len = GenDidUploadInfo(json, &buf);
    if (len <= 0) {
        Log::E("Did", "generate upload json data failed\n");
        return "";
    }

    std::string signedInfo = SignInfo(seed, index, buf, len);
    if (signedInfo.empty()) {
        Log::E("Did", "sign did info failed\n");
        free(buf);
        return "";
    }

    nlohmann::json memo;
    memo["msg"] = Utils::Hex2Str(buf, len);
    memo["pub"] = mPublicKey;
    memo["sig"] = signedInfo;
    free(buf);

    return memo.dump();
}

std::string Did::SetInfo(const std::string& seed, int index, const std::string& json, const std::shared_ptr<HDWallet>& wallet)
{
    std::string memo = SignInfo(seed, index, json);
    if (memo.empty()) {
        Log::E("Did", "sign did info and generate memo failed\n");
        return "";
    }

    Transaction tx(wallet->GetAddress(0, 0), 100, memo);
    std::vector<Transaction> transactions;
    transactions.push_back(tx);
    std::string txid;
    int ret = wallet->SendTransaction(transactions, seed, txid);
    if (ret != E_WALLET_C_OK) {
        Log::E("Did", "send transaction failed: %d\n", ret);
        return "";
    }

    return txid;
}

std::string Did::GetInfo()
{
    return "";
}

std::string Did::SignInfo(const std::string& seed, int index, const uint8_t* message, int len)
{
    uint8_t* seedBuf;
    int seedLen = Utils::Str2Hex(seed, &seedBuf);
    char* privateKey = generateSubPrivateKey(seedBuf, seedLen, COIN_TYPE_ELA, 0, index);
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

}
