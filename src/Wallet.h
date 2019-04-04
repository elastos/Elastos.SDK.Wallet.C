
#ifndef __WALLET_H__
#define __WALLET_H__


#include <BlockChainNode.h>
#include <Transaction.h>
#include <string>
#include <vector>

extern "C" {
    extern const int COIN_TYPE_ELA;
}

namespace elastos {

class CHistoryDb;

class Wallet
{
protected:
    Wallet(const std::string& localPath, const std::shared_ptr<BlockChainNode>& node, int coinType)
        : mPath(localPath)
        , mBlockChainNode(node)
        , mCoinType(coinType)
    {}


    int GetCoinType();

    int SyncHistory(const std::string& address, bool* hasHistory = nullptr);

    int GetHistoryAndSave(const std::string& address, int page, CHistoryDb& db, int* total = nullptr);

    std::string GetTableName();

    long GetBalance(const std::string& address);

    int CreateTransaction(const std::vector<Transaction>& transactions,
            const std::vector<std::string>& addresses, const std::string& chain, std::string& txJson);

    int SendRawRxAndInsert(const std::vector<Transaction>& transactions, const std::string& memo,
            const std::string& txJson, const std::string& signedTx, bool singleAddress, std::string& txid);

    int HttpPost(const std::string& api, const std::string& body, std::string& result);

private:
    int SendSignedTx(const std::string& signedTx, std::string& result);

    int InsertSendingTx(const std::vector<Transaction>& transactions,
            const std::string& memo, const std::string& txid, const std::string& tx, bool singleAddress);

protected:
    std::string mPath;
    std::shared_ptr<BlockChainNode> mBlockChainNode;
    int mCoinType = COIN_TYPE_ELA;
};

} // namespace elastos

#endif //__WALLET_H__
