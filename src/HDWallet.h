
#ifndef __HD_WALLET_H__
#define __HD_WALLET_H__

class HDWallet
{
public:
    HDWallet(const std::string& seed, const BlockChainNode& node, int coinType = CoinType_ELA);

    int GetCoinType();

    int SendTransaction(const std::vector<Transaction>& transactions, const std::string& payPassword);

    std::string GetAddress(int chain, int index);

    std::string GetPublicKey(int chain, int index);

    std::string GetBalance(const std::string& address);

};

#endif // __HD_WALLET_H__
