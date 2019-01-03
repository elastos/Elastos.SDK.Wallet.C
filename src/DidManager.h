
#ifndef __DID_MANAGER_H__
#define __DID_MANAGER_H__

#include <string>
#include <vector>
#include "Did.h"
#include "Elastos.Wallet.Utility.h"

namespace elastos{

class DidManager {
public:
    DidManager(const std::string& seed, const std::string& localPath);

    int CreateDid(int index, std::shared_ptr<Did>* did);

    std::shared_ptr<Did> GetByPosition(int pos);

    int DestroyDid(int pos);

private:
    std::string mLocalPath;
    std::unique_ptr<MasterPublicKey> mMasterPublicKey;
    std::vector<std::shared_ptr<Did>> mDids;
};

}

#endif //__DID_MANAGER_H__
