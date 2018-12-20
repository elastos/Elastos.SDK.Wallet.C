
#ifndef __IDENTITY_H__
#define __IDENTITY_H__

#include <string>
#include <vector>
#include "SingleWallet.h"
#include "Wallet.h"

namespace elastos {

class Identity
{
public:

    Identity(const std::string& localPath);

    // int GetWallet(const std::string& seed, int coinType, std::shared_ptr<HDWallet>* wallet);

    int CreateWallet(const std::string& seed, std::shared_ptr<SingleWallet>* wallet);

    // int GetWallet(const std::string& seed, const std::vector<std::string>& publicKeys,
    //         const std::string& privateKey, int requiredCount, int coinType,
    //         std::shared_ptr<MultiSignWallet>* wallet);

    // int GetDid(const std::string& seed, int index, std::shared_ptr<Did>* did);

    // int CreateDid(const std::string& seed, std::shared_ptr<Did>* did);

    std::shared_ptr<Wallet> GetByIndex(int index);

private:
    void SetIndex(int index);

private:
    std::vector<std::shared_ptr<Wallet>> mWallets;
    std::string mLocalPath;
    int mIndex;

    friend class IdentityManager;
};

} // namespace elastos

#endif //__IDENTITY_H__
