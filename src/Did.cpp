#include "Did.h"
#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"
#include "common/Log.hpp"

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

std::string Did::SetInfo(const std::string& seed, int index, const std::string& json)
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

    int len = json.size();
    uint8_t* buf = (uint8_t*)malloc(len);
    if (!buf) {
        Log::E("Did", "out of memory\n");
        free(privateKey);
        return "";
    }

    const char* p = json.c_str();
    for (int i = 0; i < len; i++) {
        buf[i] = p[i];
    }

    void* signedData;
    int signedLen = sign(privateKey, buf, len, &signedData);
    free(privateKey);
    if (signedLen <= 0) {
        Log::E("Did", "sign data failed\n");
        return "";
    }

    return Utils::Hex2Str((const uint8_t*)signedData, signedLen);
}

std::string Did::GetInfo()
{
    return "";
}

}
