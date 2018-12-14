
#include "BlockChainNode.h"
#include "nlohmann/json.hpp"

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

std::string BlockChainNode::ToJson()
{
    nlohmann::json json;
    json["Url"] = mUrl;
    json["Cert"] = mCert;

    return json.dump();
}

void BlockChainNode::FromJson(const std::string& json)
{
    nlohmann::json jNode = nlohmann::json::parse(json);
    mUrl = jNode["Url"];
    mCert = jNode["Cert"];
}
