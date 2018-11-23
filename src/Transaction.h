
#ifndef __TRANSACTION_H__
#define __TRANSACTION_H__

class Transaction
{
public:
    Transaction(std::string address, int64 amount, std::string memo, int coinType);

    void SetAddress(std::string address, int coinType);

    void SetAmount(int64 amount);

    void SetMemo(std::string memo);

};

#endif // __TRANSACTION_H__
