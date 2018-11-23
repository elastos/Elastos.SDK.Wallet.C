
#ifndef __IDENTITY_H__
#define __IDENTITY_H__

class Identity
{
public:

    Identity(std::string localPath);

    HDWallet GetWallet(std::string seed, int coinType = CoinType_ELA);

    SingleWallet GetWallet(std::string seed);

    MultiSignWallet GetWallet(std::string seed, std::vector<std::string> publicKeys,
            std::string privateKey, int requiredCount, int coinType = CoinType_ELA);

    Did GetDid(std::string seed, int index);

    Did CreateDid(std::string seed);
};

#endif //__IDENTITY_H__
