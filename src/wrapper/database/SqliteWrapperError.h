
#ifndef __SQLITE_WRAPPER_ERROR_H__
#define __SQLITE_WRAPPER_ERROR_H__

#include "sqlite3.h"

#define E_SQL_WRAPPER_OK                    0
#define E_SQL_WRAPPER_INVALID_ARGUMENT      -1
#define E_SQL_WRAPPER_DB_OPEN               -2
#define E_SQL_WRAPPER_OUT_OF_MEMORY         -3
#define E_SQL_WRAPPER_BASE                  -1000

#define E_SQL_WRAPPER_DID_DEPRECATED        -1100

#define E_SQL_WRAPPER_NOT_FOUND             SQLITE_DONE

#endif //__SQLITE_WRAPPER_ERROR_H__
