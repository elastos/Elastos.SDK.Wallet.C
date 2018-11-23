
#ifndef __MULTISIGN_WALLET_H__
#define __MULTISIGN_WALLET_H__

class MultiSignWallet
{
public:
    MultiSignWallet(std::vector<std::string> publicKeys,
            std::string privateKey, int requireCount, BlockChainNode node, int coinType = CoinType_ELA);

    int GetCoinType();

    std::string GetAddress();

    std::string GetPublicKey();

    std::string GetBalance();

    // create a new transaction
    std::string SignTransaction(std::vector<Transaction> transactions, std::string seed);

    // sign the transaction from cosigner
    std::string SignTransaction(std::string json, std::string seed);

    // send the signed transaction
    int SendTransaction(std::string json);

};

#endif // __MULTISIGN_WALLET_H__
