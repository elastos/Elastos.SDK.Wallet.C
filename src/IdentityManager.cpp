
#include "IdentityManager.h"
#include "WalletError.h"
#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"

namespace elastos {

std::vector<std::shared_ptr<Identity>> IdentityManager::sIdentities;

int IdentityManager::ImportFromFile(const std::string& filePath, std::shared_ptr<Identity>* identity)
{
    return E_WALLET_C_OK;
}

void IdentityManager::ExportToFile(const std::shared_ptr<Identity>& identity, const std::string& filePath)
{}

std::string IdentityManager::GetMnemonic(const std::string& lanaguage, const std::string& words)
{
    if (lanaguage.empty() || (lanaguage.compare("english") && words.empty())) {
        return "";
    }
    char* mnemonic = generateMnemonic(lanaguage.c_str(), words.c_str());
    std::string str = mnemonic;
    free(mnemonic);
    return str;
}

std::string IdentityManager::GetSeed(const std::string& mnemonic,
        const std::string& lanaguage, const std::string& words, const std::string& mnmonicPassword)
{
    if (mnemonic.empty() || lanaguage.empty() || (lanaguage.compare("english") && words.empty())) {
        return "";
    }

    void* seed;
    int seedLen = getSeedFromMnemonic(&seed, mnemonic.c_str(),
            lanaguage.c_str(), words.c_str(), mnmonicPassword.c_str());
    if (seedLen == 0) {
        return "";
    }

    std::string str = Utils::Hex2Str((uint8_t*)seed, seedLen);
    free(seed);
    return str;
}

int IdentityManager::CreateIdentity(const std::string& localPath, std::shared_ptr<Identity>* identity)
{
    if (localPath.empty() || !identity) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::shared_ptr<Identity> identityPtr = std::make_shared<Identity>(localPath);
    *identity = identityPtr;

    sIdentities.push_back(identityPtr);
    int pos = sIdentities.size() - 1;
    identityPtr->SetPosition(pos);

    return pos;
}

int IdentityManager::DestroyIdentity(int pos)
{
    if (pos < 0 || pos >= sIdentities.size()) {
        return E_WALLET_C_OUT_OF_RANGE;
    }

    std::shared_ptr<Identity> identity = sIdentities.at(pos);
    identity.reset();
    sIdentities[pos] = nullptr;

    return E_WALLET_C_OK;
}

std::shared_ptr<Identity> IdentityManager::GetByPosition(int pos)
{
    if (pos < 0 || pos >= sIdentities.size()) {
        return nullptr;
    }
    return sIdentities.at(pos);
}

}
