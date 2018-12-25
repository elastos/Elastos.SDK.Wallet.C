
#ifndef __DID_H__
#define __DID_H__

#include <string>

namespace elastos {

class Did
{
public:
    Did(const std::string& publicKey);

    std::string GetId();

    // return signed info
    std::string SetInfo(const std::string& seed, int index, const std::string& json);

    std::string GetInfo();

private:
    std::string mPublicKey;
    std::string mDid;
};

}

#endif // __DID_H__
