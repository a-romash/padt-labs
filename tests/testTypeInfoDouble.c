#include "testTypeInfoDouble.h"
#include "../typeinfodouble.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static Double* d(const double val) {
    return NewDouble(val);
}

static const TypeInfo* ti = NULL;

void testDoubleZeroOne() {
    assert(ti->compare(ti->zero(), d(0.0)) == 0);
    assert(ti->compare(ti->one(), d(1.0)) == 0);

    printf("\t[OK] testDoubleZeroOne\n");
}

void testDoubleSet() {
    Double* src = d(3.14);
    Double* dst = d(0.0);

    ti->set(src, dst);
    assert(ti->compare(src, dst) == 0);

    // set zero
    ti->set(ti->zero(), dst);
    assert(ti->compare(ti->zero(), dst) == 0);

    free(src);
    free(dst);
    printf("\t[OK] testDoubleSet\n");
}

void testDoubleAdd() {
    Double* a = d(1.0);
    Double* b = d(2.0);
    Double* res = d(0.0);

    // ok
    ti->add(a, b, res);
    assert(ti->compare(res, d(3.0)) == 0);

    // add zero
    ti->add(a, ti->zero(), res);
    assert(ti->compare(res, d(1.0)) == 0);

    // aliasing: add(a, b, a)
    ti->add(a, b, a);
    assert(ti->compare(a, d(3.0)) == 0);

    free(a);
    free(b);
    free(res);
    printf("\t[OK] testDoubleAdd\n");
}

void testDoubleMultiply() {
    Double* a = d(3.0);
    Double* b = d(4.0);
    Double* res = d(0.0);

    // ok
    ti->multiply(a, b, res);
    assert(ti->compare(res, d(12.0)) == 0);

    // multiply by zero
    ti->multiply(a, ti->zero(), res);
    assert(ti->compare(res, ti->zero()) == 0);

    // multiply by one
    ti->multiply(a, ti->one(), res);
    assert(ti->compare(res, d(3.0)) == 0);

    // aliasing: multiply(a, b, a)
    ti->multiply(a, b, a);
    assert(ti->compare(a, d(12.0)) == 0);

    free(a);
    free(b);
    free(res);
    printf("\t[OK] testDoubleMultiply\n");
}

void testDoubleCompare() {
    Double* a = d(1.0);
    Double* b = d(2.0);
    Double* c = d(1.0);

    assert(ti->compare(a, b) < 0);
    assert(ti->compare(b, a) > 0);
    assert(ti->compare(a, c) == 0);

    // compare with zero
    assert(ti->compare(ti->zero(), ti->zero()) == 0);
    assert(ti->compare(a, ti->zero()) > 0);

    // negative
    Double* neg = d(-1.0);
    assert(ti->compare(neg, ti->zero()) < 0);

    free(a);
    free(b);
    free(c);
    free(neg);
    printf("\t[OK] testDoubleCompare\n");
}

void testDoubleIncrement() {
    Double* a = d(2.0);
    Double* res = d(0.0);

    ti->increment(a, res);
    assert(ti->compare(res, d(3.0)) == 0);

    // increment zero
    ti->increment(ti->zero(), res);
    assert(ti->compare(res, d(1.0)) == 0);

    free(a);
    free(res);
    printf("\t[OK] testDoubleIncrement\n");
}

void testDoubleAlloc() {
    void* p = ti->alloc();
    assert(p != NULL);
    free(p);

    printf("\t[OK] testDoubleAlloc\n");
}

void testDoubleToString() {
    char buf[64];

    Double* a = d(3.14);
    Double* b = d(-2.5);
    Double* c = d(0.0);

    // ok
    ti->toString(a, buf, sizeof(buf));
    assert(strcmp(buf, "3.14") == 0);

    ti->toString(b, buf, sizeof(buf));
    assert(strcmp(buf, "-2.5") == 0);

    ti->toString(c, buf, sizeof(buf));
    assert(strcmp(buf, "0") == 0);

    ti->toString(a, buf, 3);

    free(a);
    free(b);
    free(c);
    printf("\t[OK] testDoubleToString\n");
}

void testTypeInfoDoubleAll() {
    printf("=== Test TypeInfoDouble ===\n");
    ti = GetTypeInfoDouble();

    testDoubleZeroOne();
    testDoubleSet();
    testDoubleAdd();
    testDoubleMultiply();
    testDoubleCompare();
    testDoubleIncrement();
    testDoubleAlloc();
    testDoubleToString();
    printf("===========================\n");
}