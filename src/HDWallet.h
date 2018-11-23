
#ifndef __HD_WALLET_H__
#define __HD_WALLET_H__

class HDWallet
{
public:
    HDWallet(std::string seed, BlockChainNode node, int coinType = CoinType_ELA);

    int GetCoinType();

    int SendTransaction(std::vector<Transaction> transactions, std::string payPassword);

    std::string GetAddress(int chain, int index);

    std::string GetPublicKey(int chain, int index);

    std::string GetBalance(std::string address);

};

#endif // __HD_WALLET_H__
