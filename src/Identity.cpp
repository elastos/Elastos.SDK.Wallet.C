
#include "Identity.h"
#include "common/Utils.h"
#include "WalletError.h"

namespace elastos {

Identity::Identity(const std::string& localPath)
    : mLocalPath(localPath)
    , mIndex(0)
{}

// int Identity::GetWallet(const std::string& seed, int coinType, std::shared_ptr<HDWallet>* wallet)
// {
//     return E_WALLET_C_OK;
// }

int Identity::CreateWallet(const std::string& seed, std::shared_ptr<SingleWallet>* wallet)
{
    if (seed.empty() || !wallet) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::string url(TEST_NET ? TEST_NET_WALLET_SERVICE_URL : WALLET_SERVICE_URL);

    std::unique_ptr<BlockChainNode> node = std::make_unique<BlockChainNode>(url);
    std::shared_ptr<SingleWallet> singleWallet = std::make_shared<SingleWallet>(seed, std::move(node));
    mWallets.push_back(singleWallet);
    int index = mWallets.size() - 1;

    *wallet = singleWallet;

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
    return nullptr;
}

void Identity::SetIndex(int index)
{
    mIndex = index;
}

}
