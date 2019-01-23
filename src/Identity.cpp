#include "Elastos.Wallet.Utility.h"
#include "common/Utils.h"
#include "WalletError.h"
#include "wrapper/httpclient/HttpClient.hpp"

#include "Identity.h"

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
    *wallet = hdWallet;

    return E_WALLET_C_OK;
}

int Identity::CreateWallet(const std::string& seed, int coinType, std::unique_ptr<BlockChainNode>& node, std::shared_ptr<HDWallet>* wallet)
{
    if (seed.empty() || !wallet) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::shared_ptr<HDWallet> hdWallet = std::make_shared<HDWallet>(mLocalPath, seed, node, coinType);
    *wallet = hdWallet;

    return E_WALLET_C_OK;
}

// int Identity::GetWallet(const std::string& seed, const std::vector<std::string>& publicKeys,
//         const std::string& privateKey, int requiredCount, int coinType,
//         std::shared_ptr<MultiSignWallet>* wallet)
// {
//     return E_WALLET_C_OK;
// }

int Identity::CreateDidManager(const std::string& seed, std::shared_ptr<DidManager>* manager)
{
    if (seed.empty() || !manager) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::shared_ptr<DidManager> didManager = std::make_shared<DidManager>(seed, mLocalPath);
    *manager = didManager;

    return E_WALLET_C_OK;
}

}
