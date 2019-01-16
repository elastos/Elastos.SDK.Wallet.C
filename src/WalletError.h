
#ifndef __WALLET_C_ERROR_H__
#define __WALLET_C_ERROR_H__

#define E_WALLET_C_OK                       0
#define E_WALLET_C_INVALID_ARGUMENT         -1
#define E_WALLET_C_SIGN_TX_ERROR            -2
#define E_WALLET_C_NODE_ERROR               -3
#define E_WALLET_C_NOT_IMPLEMENTED          -4
#define E_WALLET_C_OUT_OF_RANGE             -5
#define E_WALLET_C_OUT_OF_MEMORY            -6
#define E_WALLET_C_BALANCE_NOT_ENOUGH       -7


// error from service
#define E_WALLET_C_SERVICE_ERROR            -200
#define E_WALLET_C_BAD_REQUEST              -201
#define E_WALLET_C_NOT_FOUND                -202
#define E_WALLET_C_INTERNAL_ERROR           -203
#define E_WALLET_C_PROCESS_ERROR            -204

#endif //__WALLET_C_ERROR_H__
