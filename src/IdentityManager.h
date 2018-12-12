
#ifndef __IDENTITY_MANAGER_H__
#define __IDENTITY_MANAGER_H__

class IdentityManager
{
public:
    Identity ImportFromFile(const std::string& filePath);

    void ExportToFile(const Identity& identity, const std::string& filePath);

    std::string GetMnmonic(const std::string& lanaguage);

    std::string GetSeed(const std::string& mnmonic, const std::string& mnmonicPassword);

    Identity CreateIdentity(const std::string& seed, const std::string& localPath);
};

#endif //__IDENTITY_MANAGER_H__
