
#include "MultiSignWallet.h"
#include "Elastos.Wallet.Utility.h"
#include "WalletError.h"
#include "common/Log.hpp"
#include "common/Utils.h"
#include "nlohmann/json.hpp"


#define CLASS_TEXT "MultiSignWallet"

namespace elastos {

MultiSignWallet::MultiSignWallet(const std::string& localPath, const std::vector<std::string>& publicKeys,
            int requireCount, const std::shared_ptr<BlockChainNode>& node, int coinType)
    : Wallet(localPath, node, coinType)
    , mRequireCount(requireCount)
{
    mPublicKeyLen = publicKeys.size();
    mPublicKeys = (char**)malloc(mPublicKeyLen * sizeof(char*));
    if (!mPublicKeys) return;
    for (int i = 0; i < mPublicKeyLen; i++) {
        mPublicKeys[i] = (char*)malloc(publicKeys[i].length() + 1);
        strcpy(mPublicKeys[i], publicKeys[i].c_str());
    }

    char* address = getMultiSignAddress(mPublicKeys, mPublicKeyLen, requireCount);
    if (address) {
        mAddress = address;
        free(address);
    }
}

int MultiSignWallet::GetCoinType()
{
    return Wallet::GetCoinType();
}

std::string MultiSignWallet::GetAddress()
{
    return mAddress;
}

long MultiSignWallet::GetBalance()
{
    return Wallet::GetBalance(mAddress);
}

int MultiSignWallet::SyncHistory()
{
    return Wallet::SyncHistory(mAddress);
}

std::string MultiSignWallet::SignTransaction(const std::vector<Transaction>& transactions, const std::string& seed, int chain, int index)
{
    if (transactions.size() == 0 || seed.empty() || chain < 0 || index < 0 || mPublicKeys == NULL) {
        Log::E(CLASS_TEXT, "SignTransaction invalid argument\n");
        return "";
    }
    std::vector<std::string> addresses;
    addresses.push_back(mAddress);

    std::string json;
    int ret = CreateTransaction(transactions, addresses, "", json);
    if (ret != E_WALLET_C_OK) {
        Log::E(CLASS_TEXT, "create transaction failed ret:%d\n", ret);
        return "";
    }

    nlohmann::json jRet = nlohmann::json::parse(json);
    nlohmann::json jResult = jRet["result"];

    return SignTransaction(jResult.dump(), seed, chain, index);
}

std::string MultiSignWallet::SignTransaction(const std::string& json, const std::string& seed, int chain, int index)
{
    if (json.empty() || seed.empty() || chain < 0 || index < 0 || mPublicKeys == NULL) {
        Log::E(CLASS_TEXT, "SignTransaction invalid argument\n");
        return "";
    }

    std::string privateKey = Wallet::GetPrivateKey(seed, chain, index);

    char* signedTx = multiSignTransaction(privateKey.c_str(), mPublicKeys, mPublicKeyLen, mRequireCount, json.c_str());
    if (signedTx == NULL) {
        Log::E(CLASS_TEXT, "sign transaction failed");
        return "";
    }
    std::string ret = signedTx;
    free(signedTx);
    return ret;
}

int MultiSignWallet::SendTransaction(const std::string& json, std::string& txid)
{
    if (json.empty()) {
        Log::E(CLASS_TEXT, "SignTransaction invalid argument\n");
        return E_WALLET_C_INVALID_ARGUMENT;
    }

    std::vector<Transaction> transactions;

    nlohmann::json jTxs = nlohmann::json::parse(json);
    std::vector<nlohmann::json> txs = jTxs["Transactions"];
    std::vector<nlohmann::json> outputs = txs[0]["Outputs"];

    Transaction tx(outputs[0]["address"].get<std::string>(), outputs[0]["amount"].get<long>(), mCoinType);
    transactions.push_back(tx);

    std::string memo;
    auto jMemo = txs[0].find("Memo");
    if (jMemo != txs[0].end()) {
        memo = txs[0]["Memo"].get<std::string>();
    }

    char* serialized = serializeMultiSignTransaction(json.c_str());
    int ret = SendRawRxAndInsert(transactions, memo, json, serialized, true, txid);
    free(serialized);
    return ret;
}

std::vector<std::string> MultiSignWallet::GetSignedSigners(const std::string& json)
{
    std::vector<std::string> singedSingers;
    return singedSingers;
}

} //namespace elastos
