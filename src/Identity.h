
#ifndef __IDENTITY_H__
#define __IDENTITY_H__

#include <string>
#include <vector>
#include "Wallet.h"
#include "HDWallet.h"
#include "Did.h"

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

    std::shared_ptr<Wallet> GetByIndex(int index);

    int DestroyWallet(int index);

    int CreateDid(const std::string& publicKey, std::shared_ptr<Did>* did);

    std::shared_ptr<Did> GetDidByIndex(int index);

    int DestroyDid(int index);

private:
    void SetIndex(int index);

private:
    std::vector<std::shared_ptr<Wallet>> mWallets;
    std::vector<std::shared_ptr<Did>> mDids;
    std::string mLocalPath;
    int mIndex = 0;

    friend class IdentityManager;
};

} // namespace elastos

#endif //__IDENTITY_H__
