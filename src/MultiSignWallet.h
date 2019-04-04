
#ifndef __MULTISIGN_WALLET_H__
#define __MULTISIGN_WALLET_H__

#include <Transaction.h>
#include <BlockChainNode.h>
#include <vector>
#include "Wallet.h"

extern "C" {
    extern const int COIN_TYPE_ELA;
}

namespace elastos {

class MultiSignWallet : public Wallet
{
public:
    MultiSignWallet(const std::string& localPath, const std::vector<std::string>& publicKeys,
            int requireCount, const std::shared_ptr<BlockChainNode>& node, int coinType = COIN_TYPE_ELA);

    ~MultiSignWallet()
    {
        if (mPublicKeys == NULL) return;

        for (int i = 0; i < mPublicKeyLen; i++) {
            if (mPublicKeys[i] != NULL) free(mPublicKeys[i]);
        }
        free(mPublicKeys);
    }

    int GetCoinType();

    std::string GetAddress();

    long GetBalance();

    int SyncHistory();

    // create a new transaction
    std::string SignTransaction(const std::vector<Transaction>& transactions, const std::string& privateKey);

    // sign the transaction from cosigner
    std::string SignTransaction(const std::string& json, const std::string& privateKey);

    // send the signed transaction
    int SendTransaction(const std::string& json, std::string& txid);

    std::vector<std::string> GetSignedSigners(const std::string& json);

private:
    std::string mAddress;
    int mRequireCount;
    int mCoinType;
    char** mPublicKeys = NULL;
    int mPublicKeyLen = 0;
};

} //namespace elastos

#endif // __MULTISIGN_WALLET_H__
