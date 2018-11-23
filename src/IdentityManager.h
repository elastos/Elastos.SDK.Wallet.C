
#ifndef __IDENTITY_MANAGER_H__
#define __IDENTITY_MANAGER_H__

class IdentityManager
{
public:
    Identity ImportFromFile(std::string filePath);

    void ExportToFile(Identity identity, std::string filePath);

    std::string GetMnmonic(std::string lanaguage);

    std::string GetSeed(std::string mnmonic, std::string mnmonicPassword);

    Identity CreateIdentity(std::string seed, std::string localPath);
};

#endif //__IDENTITY_MANAGER_H__
