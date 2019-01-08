
#include "Transaction.h"

namespace elastos {

Transaction::Transaction(const std::string& address, long amount, int coinType)
    : mAddress(address)
    , mAmount(amount)
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

int Transaction::GetCoinType()
{
    return mCoinType;
}

std::string Transaction::GetAddress()
{
    return mAddress;
}

long Transaction::GetAmount()
{
    return mAmount;
}

} //namespace elastos
