
#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

namespace elastos {

#define TEST_NET    true

#define TEST_DB_FILE    "ElastosWalletTest.db"
#define DB_FILE         "ElastosWallet.db"

// TODO: read the address from config file
#define DID_SET_PROPERTY_TX_ADDRESS         "EZAw49S3is7bqbhwdJPfEDGd9CgbY9HBmj"
#define DID_SET_PROPERTY_FEE                10000

class Utils {

public:
    static std::string Hex2Str(const uint8_t* buff, int len);

    static int Str2Hex(const std::string& str, uint8_t** buf);

    static int ServiceErr2SdkErr(int error);

    static long GetFee();
};

} // namespace elastos

#endif //__UTILS_H__
