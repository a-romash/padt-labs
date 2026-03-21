#include "polynomial.h"
#include "typeinfodouble.h"
#include "utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

PolyError newPolynomial(size_t coeffCount, const TypeInfo* type, Polynomial* result) {
    if (type == NULL || result == NULL) return POLY_ERR_NULLARG;
    if (coeffCount == 0) return POLY_ERR_COEFF_COUNT;
    if (coeffCount > SIZE_MAX / type->size) return POLY_ERR_NOMEM;

    result->coeffCount = coeffCount;
    result->type = type;
    result->coeffs = malloc(type->size * coeffCount);
    if (result->coeffs == NULL) return POLY_ERR_NOMEM;

    for (size_t i = 0; i < result->coeffCount; i++) type->set(type->zero(), (char*)result->coeffs + i*(type->size));

    return 0;
}

void freePolynomial(Polynomial** polynomial) {
    if (polynomial == NULL || *polynomial == NULL) return;
    if ((*polynomial)->coeffs != NULL) free((*polynomial)->coeffs);
    free(*polynomial);

    *polynomial = NULL;
}

PolyError fillPolynomial(Polynomial* polynomial, const void* coeffs, size_t size) {
    if (coeffs == NULL || polynomial == NULL) return POLY_ERR_NULLARG;
    if (size != polynomial->coeffCount) {
        return POLY_ERR_SIZE;
    }

    if (polynomial->coeffs == NULL) {
        polynomial->coeffs = malloc(polynomial->type->size * polynomial->coeffCount);
        if (polynomial->coeffs == NULL) return POLY_ERR_NOMEM;
    }
    memcpy(polynomial->coeffs, coeffs, polynomial->type->size * polynomial->coeffCount);

    return 0;
}

PolyError sumPolynomial(const Polynomial* polynomial, const Polynomial* other, Polynomial* result) {
    if (polynomial == NULL || other == NULL || result == NULL) return POLY_ERR_NULLARG;
    if (polynomial->coeffs == NULL || other->coeffs == NULL) return POLY_ERR_NULLARG;
    if (polynomial->type != other->type) {
        return POLY_ERR_TYPE;
    }
    const TypeInfo* type = polynomial->type;
    size_t size = type->size;

    Polynomial* res = malloc(sizeof(Polynomial));
    if (res == NULL) {
        return POLY_ERR_NOMEM;
    }
    PolyError err = newPolynomial(max(polynomial->coeffCount, other->coeffCount), polynomial->type, res);
    if (err != POLY_OK) {
        freePolynomial(&res);
        return err;
    }

    memcpy(res->coeffs, polynomial->coeffs, polynomial->type->size * polynomial->coeffCount);
    for (size_t i = 0; i < other->coeffCount; i++) {
        type->add((char*)res->coeffs + i*size, (char*)other->coeffs + i*size, (char*)res->coeffs + i*size);
    }

    if (result->coeffs != NULL) free(result->coeffs);
    result->coeffs = res->coeffs;
    result->coeffCount = res->coeffCount;
    result->type = res->type;
    res->coeffs = NULL;
    freePolynomial(&res);

    return 0;
}

PolyError multPolynomial(const Polynomial* polynomial, const Polynomial* other, Polynomial* result) {
    if (polynomial == NULL || other == NULL || result == NULL) return POLY_ERR_NULLARG;
    if (polynomial->type != other->type) {
        return POLY_ERR_TYPE;
    }

    const TypeInfo* type = polynomial->type;
    size_t size = type->size;

    Polynomial* res = malloc(sizeof(Polynomial));
    if (res == NULL) {
        return POLY_ERR_NOMEM;
    }

    if (polynomial->coeffCount > SIZE_MAX - other->coeffCount + 1) return POLY_ERR_NOMEM;
    PolyError err = newPolynomial(polynomial->coeffCount + other->coeffCount - 1, polynomial->type, res);
    if (err != POLY_OK) {
        freePolynomial(&res);
        return err;
    }

    void* tmp = malloc(size);
    if (tmp == NULL) {
        freePolynomial(&res);
        return POLY_ERR_NOMEM;
    }
    
    for (size_t i = 0; i < polynomial->coeffCount; i++) {
        if (type->compare((char*)polynomial->coeffs + i*size, type->zero()) == 0) continue;

        for (size_t j = 0; j < other->coeffCount; j++) {
            if (type->compare((char*)other->coeffs + j*size, type->zero()) == 0) continue;

            type->multiply((char*)polynomial->coeffs + i*size, (char*)other->coeffs + j*size, tmp);
            type->add((char*)res->coeffs + (i+j)*size, tmp, (char*)res->coeffs + (i+j)*size);

        }
    }

    free(tmp);

    if (result->coeffs != NULL) free(result->coeffs);
    result->coeffs = res->coeffs;
    result->coeffCount = res->coeffCount;
    result->type = res->type;
    res->coeffs = NULL;
    freePolynomial(&res);

    return 0;
}

PolyError multPolynomialByScalar(Polynomial* polynomial, const void* scalar) {
    if (polynomial == NULL || scalar == NULL) return POLY_ERR_NULLARG;
    size_t size = polynomial->type->size;

    for (size_t i = 0; i < polynomial->coeffCount; i++) {
        polynomial->type->multiply((char*)polynomial->coeffs + i*size, scalar, (char*)polynomial->coeffs + i*size);
    }

    return 0;
}

PolyError computePolynomial(const Polynomial* polynomial, const void* value, void* result) {
    if (polynomial == NULL || value == NULL || result == NULL || polynomial->coeffs == NULL) return POLY_ERR_NULLARG;
    const TypeInfo* type = polynomial->type;
    size_t size = type->size;

    if (type->compare(value, type->zero()) == 0) { type->set(polynomial->coeffs, result); return 0; }

    void* x = malloc(size);
    if (x == NULL) return POLY_ERR_NOMEM;
    type->set(type->one(), x);

    void* tmp = malloc(size);
    if (tmp == NULL) { free(x); return POLY_ERR_NOMEM; }
    type->set(type->zero(), tmp);
    type->set(type->zero(), result);

    for (size_t i = 0; i < polynomial->coeffCount; i++) {
        type->multiply((char*)polynomial->coeffs + i*size, x, tmp);
        type->add(tmp, result, result);
        type->multiply(x, value, x);
    }

    free(tmp);
    free(x);

    return 0;
}

char* polynomialToString(const Polynomial* polynomial) {
    if (polynomial == NULL) return NULL;
    const TypeInfo* type = polynomial->type;
    size_t size = type->size;

    size_t buf_size = 32;
    size_t offset = 0;
    char* buf = malloc(buf_size);
    if (buf == NULL) return NULL;

    for (size_t i = 0; i < polynomial->coeffCount; i++) {
        void* coeff = (char*)polynomial->coeffs + i*size;
        int cmp = type->compare(coeff, type->zero());

        int written = 0;

        // evaluate needed amount of mem for temp buffer
        int tmp_buf_size = type->toString(coeff, NULL, 0);
        tmp_buf_size += snprintf(NULL, 0, "^%zu", i)*(i > 1) + (i != 0) + (i != 0 && cmp > 0) + 1;
        char* tmp = malloc(tmp_buf_size);
        if (tmp == NULL) { free(buf); return NULL; }

        // write coeff to temp buffer
        if (cmp != 0) {
            if (i != 0 && cmp > 0) {
                written += snprintf(tmp + written, tmp_buf_size - written, "+");
            }
            written += type->toString(coeff, tmp + written, tmp_buf_size - written);
            if (i != 0) written += snprintf(tmp + written, tmp_buf_size - written, "x");
            if (i > 1)  written += snprintf(tmp + written, tmp_buf_size - written, "^%zu", i);
        }

        // realloc main buffer if needed
        while (offset + written + 1 > buf_size) {
            buf_size *= 2;
            char* new_buf = realloc(buf, buf_size);
            if (new_buf == NULL) { free(buf); free(tmp); return NULL; }
            buf = new_buf;
        }

        memcpy(buf + offset, tmp, written);
        offset += written;
        free(tmp);
    }

    if (offset == 0) {
        memcpy(buf, "0", 1);
        offset = 1;
    }

    buf[offset] = '\0';
    return buf;
}
