
#ifndef __SINGLE_WALLET_H__
#define __SINGLE_WALLET_H__

#include <string>
#include "BlockChainNode.h"
#include "Transaction.h"

namespace elastos {

class SingleWallet
{
public:
    SingleWallet(const std::string& seed, std::shared_ptr<BlockChainNode> node);

    std::string GetPublicKey();

    std::string GetAddress();

    int SendTransaction(const std::vector<Transaction>& transactions, const std::string& seed, std::string& txid);

    long GetBalance();

private:
    int CreateTransaction(const std::vector<Transaction>& transactions, std::string& txJson);

    int SendSignedTx(const std::string& signedTx, std::string& result);

    int HttpPost(const std::string& api, const std::string& body, std::string& result);

private:
    std::shared_ptr<BlockChainNode> mBlockChainNode;
    std::string mPublicKey;
    std::string mAddress;
};

} // namespace elastos

#endif //__SINGLE_WALLET_H__
