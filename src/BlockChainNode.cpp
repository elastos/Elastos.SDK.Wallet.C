
#include "BlockChainNode.h"
#include "nlohmann/json.hpp"

namespace elastos {

BlockChainNode::BlockChainNode(const std::string& url)
    : mUrl(url)
{}

void BlockChainNode::SetUrl(const std::string& url)
{
    mUrl = url;
}

std::string BlockChainNode::GetUrl()
{
    return mUrl;
}

void BlockChainNode::SetCertificate(const std::string& cert)
{
    mCert = cert;
}

} // namespace elastos
