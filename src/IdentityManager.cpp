
#include "IdentityManager.h"
#include "WalletError.h"
#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"

namespace elastos {

int IdentityManager::ImportFromFile(const std::string& filePath, std::shared_ptr<Identity>* identity)
{
    return E_WALLET_C_OK;
}

void IdentityManager::ExportToFile(const std::shared_ptr<Identity>& identity, const std::string& filePath)
{}

std::string IdentityManager::GetMnemonic(const std::string& language, const std::string& words)
{
    if (language.empty() || (language.compare("english") && words.empty())) {
        return "";
    }
    char* mnemonic = generateMnemonic(language.c_str(), words.c_str());
    std::string str = mnemonic;
    free(mnemonic);
    return str;
}

std::string IdentityManager::GetSeed(const std::string& mnemonic,
        const std::string& language, const std::string& words, const std::string& mnemonicPassword)
{
    if (mnemonic.empty() || language.empty() || (language.compare("english") && words.empty())) {
        return "";
    }

    void* seed;
    int seedLen = getSeedFromMnemonic(&seed, mnemonic.c_str(),
            language.c_str(), words.c_str(), mnemonicPassword.c_str());
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

    return E_WALLET_C_OK;
}

}
