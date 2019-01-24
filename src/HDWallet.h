
#ifndef __HD_WALLET_H__
#define __HD_WALLET_H__

#include <BlockChainNode.h>
#include <Transaction.h>
#include <pthread.h>
#include <set>
#include <string>
#include <vector>
#include <memory>

extern "C" {
extern const int COIN_TYPE_ELA;
struct MasterPublicKey;
}

namespace elastos {

class CHistoryDb;

class HDWallet
{
public:

    HDWallet(const std::string& localPath, const std::string& seed, const std::shared_ptr<BlockChainNode>& node, int coinType, bool singleAddress);

    HDWallet(const std::string& localPath, const std::string& seed, const std::shared_ptr<BlockChainNode>& node, int coinType);

    HDWallet(const std::string& localPath, const std::string& seed, const std::shared_ptr<BlockChainNode>& node, bool singleAddress);

    HDWallet(const std::string& localPath, const std::string& seed, const std::shared_ptr<BlockChainNode>& node);

    ~HDWallet();

    int GetCoinType();

    int SendTransaction(const std::vector<Transaction>& transactions,
            const std::string& memo, const std::string& seed, std::string& txid, const std::string& chain = "");

    std::string GetAddress(int chain, int index);

    std::string GetPublicKey(int chain, int index);

    long GetBalance(const std::string& address);

    long GetBalance();

    int SyncHistory();

    int GetHistoryCount(const std::string& address);

    int GetHistory(const std::string& address, int pageSize, int page, bool ascending, std::string& histories);

    std::vector<std::string> GetUsedAddresses();

    std::vector<std::string> GetUnUsedAddresses(unsigned int count);

    int Recover();

private:
    int SingleAddressCreateTx(const std::vector<Transaction>& transactions,
            const std::string& memo, const std::string& seed, const std::string& chain, std::string& txJson);

    int HDCreateTx(const std::vector<Transaction>& transactions,
            const std::string& memo, const std::string& seed, const std::string& chain, std::string& txJson);

    int CreateTransaction(const std::vector<Transaction>& transactions,
            const std::vector<std::string>& addresses, const std::string& chain, std::string& txJson);

    int SendSignedTx(const std::string& signedTx, std::string& result);

    int HttpPost(const std::string& api, const std::string& body, std::string& result);

    int SyncHistory(const std::string& address, bool* hasHistory = nullptr);

    int GetHistoryAndSave(const std::string& address, int page, CHistoryDb& db, int* total = nullptr);

    int SyncMultiHistory(int gap);

    int SyncMultiHistory(int gap, int chain, bool generate = false);

    std::string GetTableName();

    int InsertSendingTx(const std::vector<Transaction>& transactions, const std::string& memo, const std::string& txid, const std::string& tx);

    void Init();

    void Init(int chain);

    std::vector<std::string> GetUnUsedAddresses(unsigned int count, int chain);

    int GetAddressIndex(const std::string& address, int* chain);

private:
    std::string mPath;
    std::shared_ptr<BlockChainNode> mBlockChainNode;
    int mCoinType = COIN_TYPE_ELA;
    bool mSingleAddress {false};
    std::unique_ptr<MasterPublicKey> mMasterPublicKey;

    // addresses
    std::vector<std::string> mInternalAddrs;
    std::vector<std::string> mExternalAddrs;
    std::set<std::string> mUsedAddrs;

    // mutex
    pthread_mutex_t mAddrLock;

private:
    struct Address
    {
        Address(const std::string& addr, long balance)
            : mAddress(addr)
            , mBalance(balance)
        {}
        std::string mAddress;
        long mBalance;
    };
};

} // namespace elastos

#endif // __HD_WALLET_H__
