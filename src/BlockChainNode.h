
#ifndef __BLOCK_CHAIN_NODE_H__
#define __BLOCK_CHAIN_NODE_H__

#include <string>

namespace elastos{

#define TEST_NET_WALLET_SERVICE_URL         "https://api-wallet-ela-testnet.elastos.org"
#define WALLET_SERVICE_URL                  "https://api-wallet-ela.elastos.org"

#define TEST_NET_DID_SERVICE_URL            "https://api-wallet-did-testnet.elastos.org"
#define DID_SERVICE_URL                     "https://api-wallet-did.elastos.org"

class BlockChainNode
{
public:
    BlockChainNode()
    {}

    BlockChainNode(const std::string& url);

    void SetUrl(const std::string& url);

    std::string GetUrl();

    void SetCertificate(const std::string& cert);

private:
    std::string mUrl;
    std::string mCert;
};

} // namespace elastos

#endif //__BLOCK_CHAIN_NODE_H__
