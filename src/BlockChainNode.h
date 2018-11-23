
#ifndef __BLOCK_CHAIN_NODE_H__
#define __BLOCK_CHAIN_NODE_H__

class BlockChainNode
{
public:
    BlockChainNode(std::string url);

    void SetUrl(std::string url);

    std::string GetUrl();

    void SetCertificate(std::string cert);
};

#endif //__BLOCK_CHAIN_NODE_H__
