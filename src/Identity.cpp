
#include "Identity.h"
#include "common/Utils.h"
#include "WalletError.h"

namespace elastos {

Identity::Identity(const std::string& localPath)
    : mLocalPath(localPath)
{}

int Identity::CreateSingleAddressWallet(const std::string& seed, std::shared_ptr<HDWallet>* wallet)
{
    if (seed.empty() || !wallet) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::string url(TEST_NET ? TEST_NET_WALLET_SERVICE_URL : WALLET_SERVICE_URL);

    std::unique_ptr<BlockChainNode> node = std::make_unique<BlockChainNode>(url);
    std::shared_ptr<HDWallet> hdWallet = std::make_shared<HDWallet>(seed, node, true);
    mWallets.push_back(hdWallet);
    int index = mWallets.size() - 1;

    *wallet = hdWallet;

    return index;
}

int Identity::CreateWallet(const std::string& seed, int coinType, std::shared_ptr<HDWallet>* wallet)
{
    if (seed.empty() || !wallet) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::string url(TEST_NET ? TEST_NET_WALLET_SERVICE_URL : WALLET_SERVICE_URL);

    std::unique_ptr<BlockChainNode> node = std::make_unique<BlockChainNode>(url);
    std::shared_ptr<HDWallet> hdWallet = std::make_shared<HDWallet>(seed, node, coinType);
    mWallets.push_back(hdWallet);
    int index = mWallets.size() - 1;

    *wallet = hdWallet;

    return index;
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

std::shared_ptr<Wallet> Identity::GetByIndex(int index)
{
    if (index < 0 || index >= mWallets.size()) {
        return nullptr;
    }
    return mWallets.at(index);
}

int Identity::DestroyWallet(int index)
{
    if (index < 0 || index >= mWallets.size()) {
        return E_WALLET_C_OUT_OF_RANGE;
    }

    std::shared_ptr<Wallet> wallet = mWallets.at(index);
    wallet.reset();
    mWallets[index] = nullptr;

    return E_WALLET_C_OK;
}

int Identity::CreateDid(const std::string& publicKey, std::shared_ptr<Did>* did)
{
    if (publicKey.empty() || !did) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::shared_ptr<Did> temp = std::make_shared<Did>(publicKey, mLocalPath);
    mDids.push_back(temp);
    int index = mDids.size() - 1;
    *did = temp;

    return index;
}

std::shared_ptr<Did> Identity::GetDidByIndex(int index)
{
    if (index < 0 || index >= mDids.size()) {
        return nullptr;
    }
    return mDids.at(index);
}

int Identity::DestroyDid(int index)
{
    if (index < 0 || index >= mDids.size()) {
        return E_WALLET_C_OUT_OF_RANGE;
    }

    std::shared_ptr<Did> did = mDids.at(index);
    did.reset();
    mDids[index] = nullptr;

    return E_WALLET_C_OK;
}

void Identity::SetIndex(int index)
{
    assert(index >= 0);
    mIndex = index;
}

}
