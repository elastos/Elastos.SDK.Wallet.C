
#include "Transaction.h"

namespace elastos {

Transaction::Transaction(const std::string& address, long amount, const std::string& memo, int coinType)
    : mAddress(address)
    , mAmount(amount)
    , mMemo(memo)
    , mCoinType(coinType)
{}

void Transaction::SetAddress(const std::string& address, int coinType)
{
    mAddress = address;
    mCoinType = coinType;
}

void Transaction::SetAmount(long amount)
{
    mAmount = amount;
}

void Transaction::SetMemo(const std::string& memo)
{
    mMemo = memo;
}

int Transaction::GetCoinType()
{
    return mCoinType;
}

std::string Transaction::GetAddress()
{
    return mAddress;
}

std::string Transaction::GetMemo()
{
    return mMemo;
}

long Transaction::GetAmount()
{
    return mAmount;
}

} //namespace elastos
