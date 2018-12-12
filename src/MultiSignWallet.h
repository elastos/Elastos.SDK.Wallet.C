
#ifndef __MULTISIGN_WALLET_H__
#define __MULTISIGN_WALLET_H__

class MultiSignWallet
{
public:
    MultiSignWallet(const std::vector<std::string>& publicKeys,
            const std::string& privateKey, int requireCount, const BlockChainNode& node, int coinType = CoinType_ELA);

    int GetCoinType();

    std::string GetAddress();

    std::string GetPublicKey();

    std::string GetBalance();

    // create a new transaction
    std::string SignTransaction(const std::vector<Transaction>& transactions, const std::string& seed);

    // sign the transaction from cosigner
    std::string SignTransaction(const std::string& json, const std::string& seed);

    // send the signed transaction
    int SendTransaction(const std::string& json);

};

#endif // __MULTISIGN_WALLET_H__
