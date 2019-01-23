
#ifndef __DID_MANAGER_H__
#define __DID_MANAGER_H__

#include <string>
#include <vector>
#include "Did.h"

namespace elastos{

class DidManager {
public:
    DidManager(const std::string& seed, const std::string& localPath);

    int CreateDid(int index, std::shared_ptr<Did>* did);

private:
    std::string mLocalPath;
    std::unique_ptr<MasterPublicKey> mMasterPublicKey;

};

}

#endif //__DID_MANAGER_H__
