#include "polyerr.h"

const char* PolyError_description(const PolyError err) {
    if (1 < err && err < 1000) return strerror(err); // системные
    switch (err) {
        case POLY_OK:              return "OK";
        case POLY_ERR_TYPE:        return "Different types of polynomial coefficients";
        case POLY_ERR_SIZE:        return "Different size of given coeffs and polynomial degree";
        case POLY_ERR_COEFF_COUNT: return "Invalid coeff count";
        default:                   return "Unknown error";
    }
}