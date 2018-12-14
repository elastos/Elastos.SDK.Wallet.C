
#ifndef __BLOCK_CHAIN_NODE_H__
#define __BLOCK_CHAIN_NODE_H__

#include <string>

class BlockChainNode
{
public:
    BlockChainNode()
    {}

    BlockChainNode(const std::string& url);

    void SetUrl(const std::string& url);

    std::string GetUrl();

    void SetCertificate(const std::string& cert);

    std::string ToJson();

    void FromJson(const std::string& json);

private:
    std::string mUrl;
    std::string mCert;
};

#endif //__BLOCK_CHAIN_NODE_H__
