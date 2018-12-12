
#ifndef __SINGLE_WALLET_H__
#define __SINGLE_WALLET_H__

class SingleWallet
{
public:
    SingleWallet(const std::string& seed, const BlockChainNode& node);

    std::string GetPublicKey();

    std::string GetAddress();

    int SendTransaction(const std::vector<Transaction>& transactions, const std::string& seed);

};

#endif //__SINGLE_WALLET_H__
