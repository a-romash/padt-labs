#ifndef LAB1_POLYERR_H
#define LAB1_POLYERR_H
#include <string.h>
#include <sys/errno.h>

typedef enum {
    POLY_OK                = 0,
    POLY_ERR_NOMEM         = ENOMEM,
    POLY_ERR_NULLARG       = EINVAL,
    POLY_ERR_TYPE          = 1000,
    POLY_ERR_SIZE          = 1001,
    POLY_ERR_COEFF_COUNT   = 1002,
} PolyError;


const char* PolyError_description(PolyError err);

#endif //LAB1_POLYERR_H