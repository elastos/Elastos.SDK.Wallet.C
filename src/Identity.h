
#ifndef __IDENTITY_H__
#define __IDENTITY_H__

#include <string>
#include <vector>
#include "Wallet.h"
#include "HDWallet.h"
#include "Did.h"
#include "DidManager.h"

namespace elastos {

class Identity
{
public:

    Identity(const std::string& localPath);

    int CreateSingleAddressWallet(const std::string& seed, std::shared_ptr<HDWallet>* wallet);

    int CreateWallet(const std::string& seed, int coinType, std::shared_ptr<HDWallet>* wallet);

    // int GetWallet(const std::string& seed, const std::vector<std::string>& publicKeys,
    //         const std::string& privateKey, int requiredCount, int coinType,
    //         std::shared_ptr<MultiSignWallet>* wallet);

    std::shared_ptr<Wallet> GetByPosition(int pos);

    int DestroyWallet(int pos);

    int CreateDidManager(const std::string& seed, std::shared_ptr<DidManager>* manager);

    std::shared_ptr<DidManager> GetDidManager();

    int DestroyDidManager();

private:
    void SetPosition(int pos);

private:
    std::vector<std::shared_ptr<Wallet>> mWallets;
    std::shared_ptr<DidManager> mDidManager;
    std::string mLocalPath;
    int mPosition = 0;

    friend class IdentityManager;
};

} // namespace elastos

#endif //__IDENTITY_H__
