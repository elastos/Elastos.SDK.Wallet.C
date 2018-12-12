
#ifndef __BLOCK_CHAIN_NODE_H__
#define __BLOCK_CHAIN_NODE_H__

class BlockChainNode
{
public:
    BlockChainNode(const std::string& url);

    void SetUrl(const std::string& url);

    std::string GetUrl();

    void SetCertificate(const std::string& cert);
};

#endif //__BLOCK_CHAIN_NODE_H__
