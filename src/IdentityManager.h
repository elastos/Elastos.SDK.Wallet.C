
#ifndef __IDENTITY_MANAGER_H__
#define __IDENTITY_MANAGER_H__

#include <string>
#include <vector>
#include "Identity.h"

namespace elastos {

class IdentityManager
{
public:
    static int ImportFromFile(const std::string& filePath, std::shared_ptr<Identity>* identity);

    static void ExportToFile(const std::shared_ptr<Identity>& identity, const std::string& filePath);

    static std::string GetMnemonic(const std::string& lanaguage, const std::string& words);

    static std::string GetSeed(const std::string& mnemonic, const std::string& lanaguage, const std::string& words, const std::string& mnmonicPassword);

    static int CreateIdentity(const std::string& localPath, std::shared_ptr<Identity>* identity);
};

}

#endif //__IDENTITY_MANAGER_H__
