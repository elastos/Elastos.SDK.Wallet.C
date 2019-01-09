
#include "Identity.h"
#include "common/Utils.h"
#include "WalletError.h"

namespace elastos {

Identity::Identity(const std::string& localPath)
    : mLocalPath(localPath)
{}

int Identity::CreateSingleAddressWallet(const std::string& seed, std::unique_ptr<BlockChainNode>& node, std::shared_ptr<HDWallet>* wallet)
{
    if (seed.empty() || !wallet) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::shared_ptr<HDWallet> hdWallet = std::make_shared<HDWallet>(mLocalPath, seed, node, true);
    mWallets.push_back(hdWallet);
    int pos = mWallets.size() - 1;
    hdWallet->SetPosition(pos);

    *wallet = hdWallet;

    return pos;
}

int Identity::CreateWallet(const std::string& seed, int coinType, std::unique_ptr<BlockChainNode>& node, std::shared_ptr<HDWallet>* wallet)
{
    if (seed.empty() || !wallet) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::shared_ptr<HDWallet> hdWallet = std::make_shared<HDWallet>(mLocalPath, seed, node, coinType);
    mWallets.push_back(hdWallet);
    int pos = mWallets.size() - 1;
    hdWallet->SetPosition(pos);

    *wallet = hdWallet;

    return pos;
}

// int Identity::GetWallet(const std::string& seed, const std::vector<std::string>& publicKeys,
//         const std::string& privateKey, int requiredCount, int coinType,
//         std::shared_ptr<MultiSignWallet>* wallet)
// {
//     return E_WALLET_C_OK;
// }

// int Identity::GetDid(const std::string& seed, int index, std::shared_ptr<Did>* did)
// {
//     return E_WALLET_C_OK;
// }

// int Identity::CreateDid(const std::string& seed, std::shared_ptr<Did>* did)
// {
//     return E_WALLET_C_OK;
// }

std::shared_ptr<Wallet> Identity::GetByPosition(int pos)
{
    if (pos < 0 || pos >= mWallets.size()) {
        return nullptr;
    }
    return mWallets.at(pos);
}

int Identity::DestroyWallet(int pos)
{
    if (pos < 0 || pos >= mWallets.size()) {
        return E_WALLET_C_OUT_OF_RANGE;
    }

    std::shared_ptr<Wallet> wallet = mWallets.at(pos);
    if (wallet == nullptr) {
        return E_WALLET_C_OK;
    }

    wallet.reset();
    mWallets[pos] = nullptr;

    return E_WALLET_C_OK;
}

int Identity::CreateDidManager(const std::string& seed, std::shared_ptr<DidManager>* manager)
{
    if (seed.empty() || !manager) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::shared_ptr<DidManager> mDidManager = std::make_shared<DidManager>(seed, mLocalPath);
    *manager = mDidManager;

    return E_WALLET_C_OK;
}

std::shared_ptr<DidManager> Identity::GetDidManager()
{
    return mDidManager;
}

int Identity::DestroyDidManager()
{
    if (mDidManager == nullptr) {
        return E_WALLET_C_OK;
    }

    mDidManager.reset();
    mDidManager = nullptr;

    return E_WALLET_C_OK;
}

void Identity::SetPosition(int pos)
{
    assert(pos >= 0);
    mPosition = pos;
}

}
