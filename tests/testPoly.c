#include "testPoly.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../polynomial_errors/polyerr.h"
#include "../typeinfo.h"
#include "../polynomial.h"

// mocks

static void mockAdd(const void* a, const void* b, void* res) {
    *(int*)res = *(const int*)a + *(const int*)b;
}

static void mockMult(const void* a, const void* b, void* res) {
    *(int*)res = *(const int*)a * *(const int*)b;
}

static void* mockAlloc() {
    return malloc(sizeof(int));
}

static void mockInc(const void* arg, void* res) {
    *(int*)res = *(const int*)arg + 1;
}

static const void* mockZero() {
    static const int z = 0;
    return &z;
}

static const void* mockOne() {
    static const int z = 1;
    return &z;
}

static void mockSet(const void* src, void* dst) {
    *(int*)dst = *(const int*)src;
}

static int mockCompare(const void* a, const void* b) {
    if (*(const int*)a < *(const int*)b) return -1;
    if (*(const int*)a > *(const int*)b) return 1;
    return 0;
}

static int mockToString(const void* a, char* buf, size_t len) {
    return snprintf(buf, len, "%d", *(const int*)a);
}


static TypeInfo mock_ti = {
    sizeof(int),
    mockAdd,
    mockMult,
    mockAlloc,
    mockInc,
    mockZero,
    mockOne,
    mockSet,
    mockCompare,
    mockToString
};

static Polynomial* makePoly(size_t n, int* vals) {
    Polynomial* poly = malloc(sizeof(Polynomial));
    newPolynomial(n, &mock_ti, poly);

    fillPolynomial(poly, vals, n);

    return poly;
}

// end mocks

void testNewPoly() {
    Polynomial* poly = malloc(sizeof(Polynomial));

    // ok
    PolyError err = newPolynomial(10, &mock_ti, poly);
    assert(err == POLY_OK);
    for (size_t i = 0; i < 10; i++) {
        assert(*(int*)(poly->coeffs + i*mock_ti.size) == 0);
    }
    free(poly->coeffs);

    // no memory
    err = newPolynomial(-1, &mock_ti, poly);
    assert(err == POLY_ERR_NOMEM);

    // null args
    err = newPolynomial(10, NULL, poly);
    assert(err == POLY_ERR_NULLARG);

    err = newPolynomial(10, &mock_ti, NULL);
    assert(err == POLY_ERR_NULLARG);

    // invalid coeff count
    err = newPolynomial(0, &mock_ti, poly);
    assert(err == POLY_ERR_COEFF_COUNT);

    freePolynomial(&poly);

    printf("\t[OK] testNewPoly\n");
}

void testFillPoly() {
    Polynomial* poly = malloc(sizeof(Polynomial));
    PolyError err = newPolynomial(5, &mock_ti, poly);
    assert(err == POLY_OK);

    // ok
    int vals[5] = {1, 2, 3, 4, 5};
    err = fillPolynomial(poly, vals, 5);
    assert(err == POLY_OK);

    // null args
    err = fillPolynomial(NULL, vals, 5);
    assert(err == POLY_ERR_NULLARG);

    err = fillPolynomial(poly, NULL, 5);
    assert(err == POLY_ERR_NULLARG);

    // invalid size
    err = fillPolynomial(poly, vals, 1);
    assert(err == POLY_ERR_SIZE);

    // poly->coeff == NULL
    // ok
    free(poly->coeffs);
    poly->coeffs = NULL;
    err = fillPolynomial(poly, vals, 5);
    assert(err == POLY_OK);

    // no mem
    free(poly->coeffs);
    poly->coeffs = NULL;
    poly->coeffCount = -1;
    err = fillPolynomial(poly, vals, -1);
    assert(err == POLY_ERR_NOMEM);

    freePolynomial(&poly);
    printf("\t[OK] testFillPoly\n");
}

void testSumPoly() {
    Polynomial* poly1 = makePoly(3, (int[]){1, 2, 3});
    Polynomial* poly2 = makePoly(3, (int[]){4, 5, 6});
    Polynomial* poly3 = makePoly(2, (int[]){7, 8});
    Polynomial* res = malloc(sizeof(Polynomial));
    newPolynomial(1, &mock_ti, res);

    // ok with same degrees
    PolyError err = sumPolynomial(poly1, poly2, res);
    assert(err == POLY_OK);
    assert(res->coeffCount == 3);
    assert(*(int*)(res->coeffs + 0*mock_ti.size) == 5);
    assert(*(int*)(res->coeffs + 1*mock_ti.size) == 7);
    assert(*(int*)(res->coeffs + 2*mock_ti.size) == 9);

    // ok with diff degrees
    free(res->coeffs);
    res->coeffs = NULL;
    newPolynomial(1, &mock_ti, res);

    err = sumPolynomial(poly1, poly3, res);
    assert(err == POLY_OK);
    assert(res->coeffCount == 3);
    assert(*(int*)(res->coeffs + 0*mock_ti.size) == 8);
    assert(*(int*)(res->coeffs + 1*mock_ti.size) == 10);

    err = sumPolynomial(poly1, poly3, poly3);
    assert(err == POLY_OK);
    assert(poly3->coeffCount == 3);
    assert(*(int*)(poly3->coeffs + 0*mock_ti.size) == 8);
    assert(*(int*)(poly3->coeffs + 1*mock_ti.size) == 10);

    // null args
    err = sumPolynomial(NULL, poly2, res);
    assert(err == POLY_ERR_NULLARG);

    err = sumPolynomial(poly1, NULL, res);
    assert(err == POLY_ERR_NULLARG);

    err = sumPolynomial(poly1, poly2, NULL);
    assert(err == POLY_ERR_NULLARG);

    // no mem
    poly1->coeffCount = -1;
    err = sumPolynomial(poly1, poly2, res);
    assert(err == POLY_ERR_NOMEM);
    poly1->coeffCount = 3;

    // invalid type
    TypeInfo* mock_ti2 = malloc(sizeof(TypeInfo));
    poly2->type = mock_ti2;
    err = sumPolynomial(poly1, poly2, res);
    assert(err == POLY_ERR_TYPE);
    poly2->type = &mock_ti;

    freePolynomial(&poly1);
    freePolynomial(&poly2);
    freePolynomial(&poly3);
    freePolynomial(&res);
    free(mock_ti2);
    printf("\t[OK] testSumPoly\n");
}

void testMultPoly() {
    Polynomial* poly1 = makePoly(3, (int[]){1, 2, 3});
    Polynomial* poly2 = makePoly(2, (int[]){4, 5});
    Polynomial* res = malloc(sizeof(Polynomial));
    newPolynomial(1, &mock_ti, res);


    // ok
    PolyError err = multPolynomial(poly1, poly2, res);
    assert(err == POLY_OK);
    assert(res->coeffCount == 4);
    assert(*(int*)(res->coeffs + 0*mock_ti.size) == 4);
    assert(*(int*)(res->coeffs + 1*mock_ti.size) == 13);
    assert(*(int*)(res->coeffs + 2*mock_ti.size) == 22);
    assert(*(int*)(res->coeffs + 3*mock_ti.size) == 15);

    err = multPolynomial(poly1, poly2, poly1);
    assert(err == POLY_OK);
    assert(poly1->coeffCount == 4);
    assert(*(int*)(poly1->coeffs + 0*mock_ti.size) == 4);
    assert(*(int*)(poly1->coeffs + 1*mock_ti.size) == 13);
    assert(*(int*)(poly1->coeffs + 2*mock_ti.size) == 22);
    assert(*(int*)(poly1->coeffs + 3*mock_ti.size) == 15);

    // null args
    err = multPolynomial(NULL, poly2, res);
    assert(err == POLY_ERR_NULLARG);

    err = multPolynomial(poly1, NULL, res);
    assert(err == POLY_ERR_NULLARG);

    err = multPolynomial(poly1, poly2, NULL);
    assert(err == POLY_ERR_NULLARG);

    // diff types
    TypeInfo* mock_ti2 = malloc(sizeof(TypeInfo));
    poly2->type = mock_ti2;
    err = multPolynomial(poly1, poly2, res);
    assert(err == POLY_ERR_TYPE);
    poly2->type = &mock_ti;

    // no mem
    mock_ti.size = -1;
    poly1->type = &mock_ti;
    err = multPolynomial(poly1, poly2, res);
    assert(err == POLY_ERR_NOMEM);
    mock_ti.size = sizeof(int);
    poly1->type = &mock_ti;

    poly1->coeffCount = -1;
    err = multPolynomial(poly1, poly2, res);
    assert(err != POLY_OK);
    poly1->coeffCount = 3;

    freePolynomial(&poly1);
    freePolynomial(&poly2);
    freePolynomial(&res);
    free(mock_ti2);
    printf("\t[OK] testMultPoly\n");
}

void testScalarMultPoly() {
    Polynomial* poly = makePoly(3, (int[]){1, 2, 3});
    int val = 2;

    // ok
    PolyError err = multPolynomialByScalar(poly, &val);
    assert(err == POLY_OK);
    assert(*(int*)(poly->coeffs + 0*mock_ti.size) == 2);
    assert(*(int*)(poly->coeffs + 1*mock_ti.size) == 4);
    assert(*(int*)(poly->coeffs + 2*mock_ti.size) == 6);
    
    // null args
    err = multPolynomialByScalar(NULL, &val);
    assert(err == POLY_ERR_NULLARG);

    err = multPolynomialByScalar(poly, NULL);
    assert(err == POLY_ERR_NULLARG);

    freePolynomial(&poly);
    printf("\t[OK] testScalarMultPoly\n");
}

void testSolvePoly() {
    Polynomial* poly = makePoly(3, (int[]){1, 2, 3});
    int val = 1;
    int res = 0;

    // ok
    PolyError err = computePolynomial(poly, &val, &res);
    assert(err == POLY_OK);
    assert(res == 6);

    val = 0;
    err = computePolynomial(poly, &val, &res);
    assert(err == POLY_OK);
    assert(res == 1);

    // null args
    err = computePolynomial(NULL, &val, &res);
    assert(err == POLY_ERR_NULLARG);

    err = computePolynomial(poly, NULL, &res);
    assert(err == POLY_ERR_NULLARG);

    err = computePolynomial(poly, &val, NULL);
    assert(err == POLY_ERR_NULLARG);

    freePolynomial(&poly);
    printf("\t[OK] testSolvePoly\n");
}

void testPolyToString() {
    Polynomial* poly = makePoly(3, (int[]){1, -2, 3});
    Polynomial* poly2 = makePoly(3, (int[]){1, 0, 3});
    Polynomial* poly3 = makePoly(3, (int[]){0, 0, 0});

    // ok
    char* str = polynomialToString(poly);
    assert(strcmp(str, "1-2x+3x^2") == 0);
    free(str);

    str = polynomialToString(poly2);
    assert(strcmp(str, "1+3x^2") == 0);
    free(str);

    str = polynomialToString(poly3);
    assert(strcmp(str, "0") == 0);
    free(str);

    // null args
    str = polynomialToString(NULL);
    assert(str == NULL);

    freePolynomial(&poly);
    freePolynomial(&poly2);
    freePolynomial(&poly3);
    printf("\t[OK] testPolyToString\n");
}

void testPolyAll() {
    printf("=== Test Polynomial ===\n");
    testNewPoly();
    testFillPoly();
    testSumPoly();
    testMultPoly();
    testScalarMultPoly();
    testSolvePoly();
    testPolyToString();
    printf("=======================\n");
}
