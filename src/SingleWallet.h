
#ifndef __SINGLE_WALLET_H__
#define __SINGLE_WALLET_H__

class SingleWallet
{
public:
    SingleWallet(std::string seed, BlockChainNode node);

    std::string GetPublicKey();

    std::string GetAddress();

    int SendTransaction(std::vector<Transaction> transactions, std::string seed);

};

#endif //__SINGLE_WALLET_H__
