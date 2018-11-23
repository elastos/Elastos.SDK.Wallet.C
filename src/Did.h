
#ifndef __DID_H__
#define __DID_H__

class Did
{
public:
    Did(std::string seed, int index);

    std::string GetId();

    // return signed info
    std::string SetInfo(std::string json);

    std::string GetInfo();

};

#endif // __DID_H__
