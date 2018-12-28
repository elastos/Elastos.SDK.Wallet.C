
#ifndef __DID_H__
#define __DID_H__

#include <string>
#include "HDWallet.h"

namespace elastos {

class Did
{
public:
    Did(const std::string& publicKey);

    std::string GetId();

    // return signed info
    std::string SignInfo(const std::string& seed, int index, const std::string& json);

    // return txid
    std::string SetInfo(const std::string& seed, int index, const std::string& json, const std::shared_ptr<HDWallet>& wallet);

    std::string GetInfo();

private:
    int GenDidUploadInfo(const std::string& json, uint8_t** buf);

    std::string SignInfo(const std::string& seed, int index, const uint8_t* message, int len);

private:
    std::string mPublicKey;
    std::string mDid;
};

}

#endif // __DID_H__
