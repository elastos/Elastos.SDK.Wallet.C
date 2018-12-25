
#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__

#include <string>
#include "Elastos.Wallet.Utility.h"

namespace elastos {

class Transaction
{
public:
    Transaction(const std::string& address, long amount, const std::string& memo, int coinType = COIN_TYPE_ELA);

    void SetAddress(const std::string& address, int coinType);

    void SetAmount(long amount);

    void SetMemo(const std::string& memo);

    int GetCoinType();

    std::string GetAddress();

    std::string GetMemo();

    long GetAmount();

private:
    std::string mAddress;
    long mAmount = 0;
    std::string mMemo;
    int mCoinType = 0;
};

} //namespace elastos

#endif // __TRANSACTION_H__
