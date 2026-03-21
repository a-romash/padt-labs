#include "testTypeInfoComplex.h"
#include "../typeinfocomplex.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Complex* c(const double re, const double im) {
    return NewComplex(re, im);
}

static const TypeInfo* ti = NULL;

void testComplexZeroOne() {
    assert(ti->compare(ti->zero(), c(0.0, 0.0)) == 0);
    assert(ti->compare(ti->one(), c(1.0, 0.0)) == 0);

    printf("\t[OK] testComplexZeroOne\n");
}

void testComplexSet() {
    Complex* src = c(3.0, 4.0);
    Complex* dst = c(0.0, 0.0);

    ti->set(src, dst);
    assert(ti->compare(src, dst) == 0);

    // set zero
    ti->set(ti->zero(), dst);
    assert(ti->compare(ti->zero(), dst) == 0);

    free(src);
    free(dst);
    printf("\t[OK] testComplexSet\n");
}

void testComplexAdd() {
    Complex* a = c(1.0, 2.0);
    Complex* b = c(3.0, 4.0);
    Complex* res = c(0.0, 0.0);

    // ok
    ti->add(a, b, res);
    assert(ti->compare(res, c(4.0, 6.0)) == 0);

    // add zero
    ti->add(a, ti->zero(), res);
    assert(ti->compare(res, c(1.0, 2.0)) == 0);

    // aliasing: add(a, b, a)
    ti->add(a, b, a);
    assert(ti->compare(a, c(4.0, 6.0)) == 0);

    free(a);
    free(b);
    free(res);
    printf("\t[OK] testComplexAdd\n");
}

void testComplexMultiply() {
    Complex* a = c(1.0, 2.0);
    Complex* b = c(3.0, 4.0);
    Complex* res = c(0.0, 0.0);

    // ok: (1+2i)(3+4i) = 3+4i+6i+8i^2 = (3-8)+(4+6)i = -5+10i
    ti->multiply(a, b, res);
    assert(ti->compare(res, c(-5.0, 10.0)) == 0);

    // multiply by zero
    ti->multiply(a, ti->zero(), res);
    assert(ti->compare(res, ti->zero()) == 0);

    // multiply by one
    ti->multiply(a, ti->one(), res);
    assert(ti->compare(res, a) == 0);

    // aliasing: multiply(a, b, a)
    ti->multiply(a, b, a);
    assert(ti->compare(a, c(-5.0, 10.0)) == 0);

    free(a);
    free(b);
    free(res);
    printf("\t[OK] testComplexMultiply\n");
}

void testComplexCompare() {
    // compare (by norm)
    Complex* a = c(3.0, 4.0);
    Complex* b = c(1.0, 1.0);
    Complex* c_eq = c(0.0, 5.0);

    assert(ti->compare(b, a) < 0);
    assert(ti->compare(a, b) > 0);
    assert(ti->compare(a, c_eq) == 0);

    // compare with zero
    assert(ti->compare(ti->zero(), ti->zero()) == 0);
    assert(ti->compare(a, ti->zero()) > 0);

    free(a);
    free(b);
    free(c_eq);
    printf("\t[OK] testComplexCompare\n");
}

void testComplexIncrement() {
    Complex* a = c(2.0, 3.0);
    Complex* res = c(0.0, 0.0);

    // increment re
    ti->increment(a, res);
    assert(ti->compare(res, c(3.0, 3.0)) == 0);

    // increment zero
    ti->increment(ti->zero(), res);
    assert(ti->compare(res, c(1.0, 0.0)) == 0);

    free(a);
    free(res);
    printf("\t[OK] testComplexIncrement\n");
}

void testComplexAlloc() {
    void* p = ti->alloc();
    assert(p != NULL);

    free(p);
    printf("\t[OK] testComplexAlloc\n");
}

void testComplexToString() {
    char buf[64];

    Complex* a = c(1.0, 2.0);   // positive im
    Complex* b = c(1.0, -2.0);  // negative im
    Complex* zero = c(0.0, 0.0);

    // positive imaginary: (re+imi)
    ti->toString(a, buf, sizeof(buf));
    assert(strcmp(buf, "(1+2i)") == 0);

    // negative imaginary: (re-imi)
    ti->toString(b, buf, sizeof(buf));
    assert(strcmp(buf, "(1-2i)") == 0);


    // zero
    ti->toString(zero, buf, sizeof(buf));
    assert(strcmp(buf, "0") == 0);

    // zero imaginary: (re)
    ti->increment(zero, zero);
    ti->toString(zero, buf, sizeof(buf));
    assert(strcmp(buf, "(1)") == 0);

    // small buffer
    ti->toString(a, buf, 3);

    free(a);
    free(b);
    free(zero);
    printf("\t[OK] testComplexToString\n");
}

void testTypeInfoComplexAll() {
    printf("=== Test TypeInfoComplex ===\n");
    ti = GetTypeInfoComplex();

    testComplexZeroOne();
    testComplexSet();
    testComplexAdd();
    testComplexMultiply();
    testComplexCompare();
    testComplexIncrement();
    testComplexAlloc();
    testComplexToString();
    printf("============================\n");
}