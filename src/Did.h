
#ifndef __DID_H__
#define __DID_H__

#include <string>

class Did
{
public:
    Did(const std::string& seed, int index);

    std::string GetId();

    // return signed info
    std::string SetInfo(const std::string& json);

    std::string GetInfo();

};

#endif // __DID_H__
