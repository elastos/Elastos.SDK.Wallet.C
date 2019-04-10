
#include "Elastos.Wallet.Utility.h"
#include "common/Log.hpp"
#include "common/Utils.h"
#include "WalletError.h"
#include "nlohmann/json.hpp"
#include "wrapper/database/CHistoryDb.h"
#include "wrapper/httpclient/HttpClient.hpp"
#include  <algorithm>

#include "HDWallet.h"

#define CLASS_TEXT "HDWallet"
#define HISTORY_PAGE_SIZE   10
#define HTTP_TIME_OUT       5000

#define ADDRESS_CHECK_INTERVAL  20


namespace elastos {

HDWallet::HDWallet(const std::string& localPath, const std::string& seed, const std::shared_ptr<BlockChainNode>& node, int coinType, bool singleAddress)
    : Wallet(localPath, node, coinType)
    , mPath(localPath)
    , mBlockChainNode(node)
    , mCoinType(coinType)
    , mSingleAddress(singleAddress)
{
    // mutex init
    pthread_mutexattr_t recursiveAttr;
    pthread_mutexattr_init(&recursiveAttr);
    pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mAddrLock, &recursiveAttr);
    pthread_mutexattr_destroy(&recursiveAttr);

    uint8_t* seedBuf;
    int seedLen = Utils::Str2Hex(seed, &seedBuf);
    if (seedLen == 0) {
        Log::E(CLASS_TEXT, "seed is empty\n");
        return;
    }

    MasterPublicKey* masterPublicKey = getMasterPublicKey(seedBuf, seedLen, mCoinType);
    if (!masterPublicKey) {
        Log::E(CLASS_TEXT, "get master public key failed\n");
        return;
    }

    mMasterPublicKey.reset(masterPublicKey);

    if (!mSingleAddress) {
        Init();
    }
}

HDWallet::HDWallet(const std::string& localPath, const std::string& seed, const std::shared_ptr<BlockChainNode>& node, int coinType)
    : HDWallet(localPath, seed, node, coinType, false)
{}

HDWallet::HDWallet(const std::string& localPath, const std::string& seed, const std::shared_ptr<BlockChainNode>& node, bool singleAddress)
    : HDWallet(localPath, seed, node, COIN_TYPE_ELA, singleAddress)
{}

HDWallet::HDWallet(const std::string& localPath, const std::string& seed, const std::shared_ptr<BlockChainNode>& node)
    : HDWallet(localPath, seed, node, COIN_TYPE_ELA, false)
{}

HDWallet::~HDWallet()
{
    pthread_mutex_destroy(&mAddrLock);
}

int HDWallet::GetCoinType()
{
    return Wallet::GetCoinType();
}

int HDWallet::SendTransaction(const std::vector<Transaction>& transactions,
        const std::string& memo, const std::string& seed, std::string& txid, const std::string& chain)
{
    if (transactions.empty() || seed.empty()) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::string txJson;
    int ret;
    if (mSingleAddress) {
        ret = SingleAddressCreateTx(transactions, memo, seed, chain, txJson);
    }
    else {
        ret = HDCreateTx(transactions, memo, seed, chain, txJson);
    }
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "create transaction failed ret:%d\n", ret);
        return ret;
    }

    char* signedData = generateRawTransaction(txJson.c_str());
    if (!signedData) {
        Log::E(CLASS_TEXT, "sign transaction error\n");
        return E_WALLET_C_SIGN_TX_ERROR;
    }

    ret = SendRawRxAndInsert(transactions, memo, txJson, signedData, mSingleAddress,txid);
    free(signedData);
    return ret;
}

std::string HDWallet::GetAddress(int chain, int index)
{
    std::string publicKey = GetPublicKey(chain, index);
    char* address = getAddress(publicKey.c_str());
    std::string ret = address;
    free(address);
    return ret;
}

std::string HDWallet::GetPublicKey(int chain, int index)
{
    assert(mMasterPublicKey && chain >= 0 && index >= 0);
    int param1 = mSingleAddress ? EXTERNAL_CHAIN : chain;
    int param2 = mSingleAddress ? 0 : index;
    char* publicKey = generateSubPublicKey(mMasterPublicKey.get(), param1, param2);
    std::string ret = publicKey;
    free(publicKey);
    return ret;
}

std::string HDWallet::GetPrivateKey(const std::string& seed, int chain, int index)
{
    if (seed.empty() || chain < 0 || index < 0) {
        Log::E(CLASS_TEXT, "GetPrivateKey invalid argument\n");
        return "";
    }

    int param1 = mSingleAddress ? EXTERNAL_CHAIN : chain;
    int param2 = mSingleAddress ? 0 : index;

    return Wallet::GetPrivateKey(seed, param1, param2);
}

long HDWallet::GetBalance(const std::string& address)
{
    return Wallet::GetBalance(address);
}

long HDWallet::GetBalance()
{
    if (mSingleAddress) {
        return Wallet::GetBalance(GetAddress(0, 0));
    }

    long total = 0;
    std::vector<std::string> used = GetUsedAddresses();
    for (std::string addr : used) {
        total += Wallet::GetBalance(addr);
    }

    return total;
}

int HDWallet::SyncHistory()
{
    if (mSingleAddress) {
        return Wallet::SyncHistory(GetAddress(EXTERNAL_CHAIN, 0));
    }
    else {
        return SyncMultiHistory(5);
    }
}

int HDWallet::GetHistoryCount(const std::string& address)
{
    if (address.empty()) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    CHistoryDb db(mPath, GetTableName());
    int count;
    int ret = db.GetCount(address, &count);
    if (ret != E_WALLET_C_OK) {
        return 0;
    }

    return count;
}

int HDWallet::GetHistory(const std::string& address, int pageSize, int page, bool ascending, std::string& histories)
{
    if (address.empty() || pageSize <= 0 || page < 0) {
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    CHistoryDb db(mPath, GetTableName());
    std::vector<std::shared_ptr<History>> historyVector;
    int ret = db.Query(address, pageSize, page, ascending, &historyVector);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "query %s history failed ret:%d\n", address.c_str(), ret);
        return ret;
    }

    if (historyVector.empty()) {
        Log::D(CLASS_TEXT, "%s no history\n", address.c_str());
        histories.clear();
        return E_WALLET_C_OK;
    }

    nlohmann::json jHistories;
    std::vector<nlohmann::json> jHistoryVector;
    for (std::shared_ptr<History> history : historyVector) {
        nlohmann::json jHistory = {
            {"txid", history->mTxid},
            {"direction", history->mDirection},
            {"value", history->mAmount},
            {"time", history->mTime},
            {"height", history->mHeight},
            {"fee", history->mFee},
            {"inputs", history->mInputs},
            {"outputs", history->mOutputs},
            {"memo", history->mMemo}
        };

        jHistoryVector.push_back(jHistory);
    }
    jHistories = jHistoryVector;
    histories = jHistories.dump();

    return E_WALLET_C_OK;
}

std::vector<std::string> HDWallet::GetUsedAddresses()
{
    std::vector<std::string> used(mUsedAddrs.begin(), mUsedAddrs.end());
    return used;
}

std::vector<std::string> HDWallet::GetUnUsedAddresses(unsigned int count)
{
    assert(count > 0);
    return GetUnUsedAddresses(count, EXTERNAL_CHAIN);
}

int HDWallet::Recover()
{
    if (mSingleAddress) {
        return SyncHistory();
    }

    int ret = SyncMultiHistory(ADDRESS_CHECK_INTERVAL, INTERNAL_CHAIN, true);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "recover internal chain failed: %d\n", ret);
        return ret;
    }

    ret = SyncMultiHistory(ADDRESS_CHECK_INTERVAL, EXTERNAL_CHAIN, true);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "recover external chain failed: %d\n", ret);
        return ret;
    }

    Log::D(CLASS_TEXT, "internal chain size: %d\n", mInternalAddrs.size());
    Log::D(CLASS_TEXT, "external chain size: %d\n", mExternalAddrs.size());
    Log::D(CLASS_TEXT, "used addr size: %d\n", mUsedAddrs.size());

    return ret;
}

int HDWallet::SingleAddressCreateTx(const std::vector<Transaction>& transactions,
        const std::string& memo, const std::string& seed, const std::string& chain, std::string& txJson)
{
    std::vector<std::string> addresses;
    std::string address = GetAddress(EXTERNAL_CHAIN, 0);
    addresses.push_back(address);

    std::string json;
    int ret = CreateTransaction(transactions, addresses, chain, json);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "create transaction failed ret:%d\n", ret);
        return ret;
    }

    nlohmann::json jRet = nlohmann::json::parse(json);
    nlohmann::json jResult = jRet["result"];

    uint8_t* seedBuf;
    int seedLen = Utils::Str2Hex(seed, &seedBuf);
    if (seedLen == 0) {
        Log::E(CLASS_TEXT, "seed is empty\n");
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    char* privateKey = generateSubPrivateKey(seedBuf, seedLen, mCoinType, 0, 0);
    free(seedBuf);

    jResult["Transactions"][0]["UTXOInputs"][0]["privateKey"] = privateKey;
    if (!memo.empty()) {
        jResult["Transactions"][0]["Memo"] = memo;
    }

    Log::D(CLASS_TEXT, "transaction: %s\n", jResult.dump().c_str());

    txJson = jResult.dump();
    free(privateKey);

    return E_WALLET_C_OK;
}

int HDWallet::HDCreateTx(const std::vector<Transaction>& transactions,
        const std::string& memo, const std::string& seed, const std::string& chain, std::string& txJson)
{
    long amount = 0;
    for (Transaction tx : transactions) {
        amount += tx.GetAmount();
    }

    std::vector<Address> addresses;
    for (std::string addr : mUsedAddrs) {
        long balance = Wallet::GetBalance(addr);
        if (balance == 0) continue;
        Address address(addr, balance);
        addresses.push_back(address);
    }

    std::sort(addresses.begin(), addresses.end(),
            [](const Address& a, const Address& b)
            {
                return a.mBalance <= b.mBalance;
            });

    long balance = 0;
    long fee = Utils::GetFee();
    std::vector<std::string> inputAddrs;
    for (Address addr : addresses) {
        balance += addr.mBalance;
        inputAddrs.push_back(addr.mAddress);
        if (balance >= amount + fee) break;
    }

    if (balance < amount + fee) return E_WALLET_C_BALANCE_NOT_ENOUGH;

    std::vector<std::string> unused = GetUnUsedAddresses(1, INTERNAL_CHAIN);
    printf("unused address: %s\n", unused[0].c_str());
    Transaction tx(unused[0], balance - amount - fee, mCoinType);
    std::vector<Transaction> finalTxes(transactions.begin(), transactions.end());
    finalTxes.push_back(tx);

    std::string json;
    int ret = CreateTransaction(finalTxes, inputAddrs, chain, json);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "create transaction failed ret:%d\n", ret);
        return ret;
    }

    nlohmann::json jRet = nlohmann::json::parse(json);
    nlohmann::json jResult = jRet["result"];

    uint8_t* seedBuf;
    int seedLen = Utils::Str2Hex(seed, &seedBuf);
    if (seedLen == 0) {
        Log::E(CLASS_TEXT, "seed is empty\n");
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::vector<nlohmann::json> inputs = jResult["Transactions"][0]["UTXOInputs"];
    for (int i = 0; i < inputs.size(); i++) {
        int chain;
        int index = GetAddressIndex(inputs[i]["address"], &chain);
        char* privateKey = generateSubPrivateKey(seedBuf, seedLen, mCoinType, chain, index);
        jResult["Transactions"][0]["UTXOInputs"][i]["privateKey"] = privateKey;
        free(privateKey);
    }
    free(seedBuf);

    if (!memo.empty()) {
        jResult["Transactions"][0]["Memo"] = memo;
    }

    Log::D(CLASS_TEXT, "transaction: %s\n", jResult.dump().c_str());

    txJson = jResult.dump();

    return E_WALLET_C_OK;
}

int HDWallet::SyncMultiHistory(int gap)
{
    int ret = SyncMultiHistory(gap, INTERNAL_CHAIN);
    if (ret != E_WALLET_C_OK) return ret;

    return SyncMultiHistory(gap, EXTERNAL_CHAIN);
}

int HDWallet::SyncMultiHistory(int gap, int chain, bool generate)
{
    int ret;
    int count = 0, start = 0;
    std::vector<std::string>& addrs = chain == EXTERNAL_CHAIN ? mExternalAddrs : mInternalAddrs;

check:
    for (int i = start; i < addrs.size(); i++) {
        bool hasHistory = false;
        ret = Wallet::SyncHistory(addrs[i], &hasHistory);
        if (ret != E_WALLET_C_OK) return ret;

        if (hasHistory) {
            // set address used.
            mUsedAddrs.insert(addrs[i]);
            count = 0;
        }
        else if (++count >= gap) break;
    }

    if (!generate || count >= gap) return E_WALLET_C_OK;

    std::vector<std::string> unused = GetUnUsedAddresses(gap - count, chain);
    start = addrs.size();
    addrs.insert(addrs.end(), unused.begin(), unused.end());
    goto check;
}

void HDWallet::Init()
{
    Init(EXTERNAL_CHAIN);
    Init(INTERNAL_CHAIN);
}

void HDWallet::Init(int chain)
{
    pthread_mutex_lock(&mAddrLock);
    std::vector<std::string>& addrs = chain == EXTERNAL_CHAIN ? mExternalAddrs : mInternalAddrs;
    int interval = ADDRESS_CHECK_INTERVAL;
    for (int i = 0; i < interval; i++) {
        std::string address = GetAddress(chain, i);
        addrs.push_back(address);
        int count = GetHistoryCount(address);
        if (count > 0) {
            mUsedAddrs.insert(address);
            interval++;
        }
    }
    pthread_mutex_unlock(&mAddrLock);
}

std::vector<std::string> HDWallet::GetUnUsedAddresses(unsigned int count, int chain)
{
    assert(count > 0);

    pthread_mutex_lock(&mAddrLock);
    std::vector<std::string>& addrs = chain == EXTERNAL_CHAIN ? mExternalAddrs : mInternalAddrs;
    int gen = 0, used = 0;

    std::vector<std::string> unusedAddrs;
    for (std::string addr : addrs) {
        if (mUsedAddrs.count(addr)) continue;
        unusedAddrs.push_back(addr);
        if (unusedAddrs.size() == count) break;
    }

    int diff = count - unusedAddrs.size();
    if (diff <= 0) {
        pthread_mutex_unlock(&mAddrLock);
        return unusedAddrs;
    }

    int size = addrs.size();
    for (int i = 0; i < diff; i++) {
        std::string address = GetAddress(chain, i + size);
        addrs.push_back(address);
        unusedAddrs.push_back(address);
    }
    pthread_mutex_unlock(&mAddrLock);

    return unusedAddrs;
}

int HDWallet::GetAddressIndex(const std::string& address, int* chain)
{
    int dis = std::distance(mInternalAddrs.begin(),
            find(mInternalAddrs.begin(), mInternalAddrs.end(), address));
    if (dis < mInternalAddrs.size()) {
        *chain = INTERNAL_CHAIN;
        return dis;
    }

    dis =  std::distance(mExternalAddrs.begin(),
            find(mExternalAddrs.begin(), mExternalAddrs.end(), address));
    *chain = EXTERNAL_CHAIN;
    return dis;
}

} // namespace elastos
