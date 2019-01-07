
#ifndef __HD_WALLET_H__
#define __HD_WALLET_H__

#include <string>
#include "Elastos.Wallet.Utility.h"
#include "BlockChainNode.h"
#include "Transaction.h"
#include "Wallet.h"
#include "wrapper/database/CHistoryDb.h"

namespace elastos {

class HDWallet : public Wallet
{
public:

    HDWallet(const std::string& localPath, const std::string& seed, std::unique_ptr<BlockChainNode>& node, int coinType, bool singleAddress);

    HDWallet(const std::string& localPath, const std::string& seed, std::unique_ptr<BlockChainNode>& node, int coinType);

    HDWallet(const std::string& localPath, const std::string& seed, std::unique_ptr<BlockChainNode>& node, bool singleAddress);

    HDWallet(const std::string& localPath, const std::string& seed, std::unique_ptr<BlockChainNode>& node);

    int GetCoinType();

    int SendTransaction(const std::vector<Transaction>& transactions, const std::string& seed, std::string& txid);

    std::string GetAddress(int chain, int index);

    std::string GetPublicKey(int chain, int index);

    int GetBalance(const std::string& address);

    int GetPosition();

    int SyncHistory();

private:
    int SingleAddressCreateTx(const std::vector<Transaction>& transactions, const std::string& seed, std::string& txJson);

    int HDCreateTx(const std::vector<Transaction>& transactions, const std::string& seed, std::string& txJson);

    int CreateTransaction(const std::vector<Transaction>& transactions,
            const std::vector<std::string>& addresses, std::string& txJson);

    int SendSignedTx(const std::string& signedTx, std::string& result);

    int HttpPost(const std::string& api, const std::string& body, std::string& result);

    void SetPosition(int pos);

    int SyncHistory(const std::string& address);

    int GetHistroyAndSave(const std::string& address, int page, CHistoryDb& db, int* total = nullptr);

    int SyncMultiHistory();

private:
    std::string mPath;
    std::unique_ptr<BlockChainNode> mBlockChainNode;
    int mPosition = 0;
    int mCoinType = COIN_TYPE_ELA;
    bool mSingleAddress {false};
    std::unique_ptr<MasterPublicKey> mMasterPublicKey;

    friend class Identity;
};

} // namespace elastos

#endif // __HD_WALLET_H__
