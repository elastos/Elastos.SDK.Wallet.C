
#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__

class Transaction
{
public:
    Transaction(const std::string& address, int64 amount, const std::string& memo, int coinType);

    void SetAddress(const std::string& address, int coinType);

    void SetAmount(int64 amount);

    void SetMemo(const std::string& memo);

};

#endif // __TRANSACTION_H__
