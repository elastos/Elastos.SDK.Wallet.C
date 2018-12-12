
#ifndef __IDENTITY_H__
#define __IDENTITY_H__

class Identity
{
public:

    Identity(const std::string& localPath);

    HDWallet GetWallet(const std::string& seed, int coinType = CoinType_ELA);

    SingleWallet GetWallet(const std::string& seed);

    MultiSignWallet GetWallet(const std::string& seed, const std::vector<std::string>& publicKeys,
            const std::string& privateKey, int requiredCount, int coinType = CoinType_ELA);

    Did GetDid(const std::string& seed, int index);

    Did CreateDid(const std::string& seed);
};

#endif //__IDENTITY_H__
