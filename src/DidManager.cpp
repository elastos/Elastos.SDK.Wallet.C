
#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"
#include "common/Log.hpp"
#include "WalletError.h"

#include "DidManager.h"

namespace elastos{

#define CLASS_TEXT  "DidManager"

DidManager::DidManager(const std::string& seed, const std::string& localPath)
    : mLocalPath(localPath)
{
    uint8_t* seedBuf;
    int seedLen = Utils::Str2Hex(seed, &seedBuf);
    if (seedLen == 0) {
        Log::E(CLASS_TEXT, "seed is empty\n");
        return;
    }

    MasterPublicKey* masterPublicKey = getMasterPublicKey(seedBuf, seedLen, COIN_TYPE_ELA);
    if (!masterPublicKey) {
        Log::E(CLASS_TEXT, "get master public key failed\n");
        return;
    }

    mMasterPublicKey.reset(masterPublicKey);
}

int DidManager::CreateDid(int index, std::shared_ptr<Did>* did)
{
    if (index < 0 || !did) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    char* publicKey = generateSubPublicKey(mMasterPublicKey.get(), 0, index);
    std::shared_ptr<Did> temp = std::make_shared<Did>(publicKey, index, mLocalPath);
    free(publicKey);

    *did = temp;

    return E_WALLET_C_OK;
}

}
