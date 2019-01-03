
#ifndef __WALLET_H__
#define __WALLET_H__

namespace elastos {

class Wallet
{
public:
    virtual int GetPosition() = 0;
};

} // namespace elastos

#endif // __WALLET_H__
