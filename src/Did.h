
#ifndef __DID_H__
#define __DID_H__

#include <string>
#include <memory>
#include "HDWallet.h"

namespace elastos {

class Did
{
public:
    Did(const std::string& publicKey, int index, const std::string& path);

    std::string GetId();

    // return signed info
    std::string SignInfo(const std::string& seed, const std::string& json, bool encrypt = false);

    // return txid
    std::string SetInfo(const std::string& seed, const std::string& json, const std::shared_ptr<HDWallet>& wallet, bool encrypt = false);

    // sync the did info
    int SyncInfo();

    std::string GetInfo(const std::string& key, bool encrypt = false, const std::string& seed = "");

    void SetNode(const std::shared_ptr<BlockChainNode>& node);

private:
    int GenDidUploadInfo(const std::string& json, bool encrypt, uint8_t** buf);

    std::string SignInfo(const std::string& seed, const uint8_t* message, int len);

    std::string GetPrivateKey(const std::string& seed);

private:
    std::shared_ptr<BlockChainNode> mBlockChainNode;
    std::string mPublicKey;
    std::string mDid;
    std::string mPath;
    int mIndex = 0;

};

}

#endif // __DID_H__
