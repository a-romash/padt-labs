#ifndef LAB1_POLYNOMIAL_H
#define LAB1_POLYNOMIAL_H
#include "typeinfo.h"
#include "polynomial_errors/polyerr.h"

typedef struct {
    size_t coeffCount;
    void* coeffs;
    const TypeInfo* type;
} Polynomial;

PolyError newPolynomial(size_t coeffCount, const TypeInfo* type, Polynomial* result);
void freePolynomial(Polynomial** polynomial);
PolyError fillPolynomial(Polynomial* polynomial, const void* coeffs, size_t size);
PolyError sumPolynomial(const Polynomial* polynomial, const Polynomial* other, Polynomial* result);
PolyError multPolynomial(const Polynomial* polynomial, const Polynomial* other, Polynomial* result);
PolyError multPolynomialByScalar(Polynomial* polynomial, const void* scalar);
PolyError computePolynomial(const Polynomial* polynomial, const void* value, void* result);
//todo: (допзадача) корни полинома
char* polynomialToString(const Polynomial* polynomial);

#endif //LAB1_POLYNOMIAL_H
