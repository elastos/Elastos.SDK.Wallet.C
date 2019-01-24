
#ifndef __IDENTITY_H__
#define __IDENTITY_H__

#include <string>
#include <vector>
#include "HDWallet.h"
#include "Did.h"
#include "DidManager.h"

namespace elastos {

class Identity
{
public:

    Identity(const std::string& localPath);

    int CreateSingleAddressWallet(const std::string& seed, const std::shared_ptr<BlockChainNode>& node, std::shared_ptr<HDWallet>* wallet);

    int CreateWallet(const std::string& seed, int coinType, const std::shared_ptr<BlockChainNode>& node, std::shared_ptr<HDWallet>* wallet);

    // int GetWallet(const std::string& seed, const std::vector<std::string>& publicKeys,
    //         const std::string& privateKey, int requiredCount, int coinType,
    //         std::shared_ptr<MultiSignWallet>* wallet);

    int CreateDidManager(const std::string& seed, std::shared_ptr<DidManager>* manager);

private:
    std::string mLocalPath;

};

} // namespace elastos

#endif //__IDENTITY_H__
