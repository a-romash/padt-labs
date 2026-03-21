#include "typeinfocomplex.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct Complex{
    double re;
    double im;
} Complex;

Complex* NewComplex(const double re, const double im) {
    Complex* d = (Complex*)malloc(sizeof(Complex));
    if (d == NULL) return NULL;
    d->re = re;
    d->im = im;
    return d;
}

static double squaredLen(const Complex* p) {
    return p->re * p->re + p->im * p->im;
}

static void add(const void* arg1, const void* arg2, void* res) {
    const Complex* p1 = (const Complex*)arg1;
    const Complex* p2 = (const Complex*)arg2;
    Complex* result = (Complex*)res;

    result->re = p1->re + p2->re;
    result->im = p1->im + p2->im;
}

static void mulp(const void* arg1, const void* arg2, void* res) {
    const Complex* p1 = (const Complex*)arg1;
    const Complex* p2 = (const Complex*)arg2;
    Complex* result = (Complex*)res;

    double re = p1->re * p2->re - p1->im * p2->im;
    double im = p1->re * p2->im + p1->im * p2->re;

    result->re = re;
    result->im = im;
}

static void set(const void* src, void* dst) {
    const Complex* p = (const Complex*)src;
    Complex* q = (Complex*)dst;

    q->re = p->re;
    q->im = p->im;
}

static void* alloc() {
    return malloc(sizeof(Complex));
}

static int comp(const void* arg1, const void* arg2) {
    const Complex* p1 = (const Complex*)arg1;
    const Complex* p2 = (const Complex*)arg2;

    if (squaredLen(p1) < squaredLen(p2)) return -1;
    if (squaredLen(p1) > squaredLen(p2)) return 1;
    return 0;
}

static int toString(const void* p, char* buf, size_t len) {
    const Complex* c = (const Complex*)p;
    if (c->im > 0) {
        return snprintf(buf, len, "(%.5g+%.5gi)", c->re, c->im);
    } if (c->im == 0) {
        if (c->re == 0) return snprintf(buf, len, "0");
        return snprintf(buf, len, "(%.5g)", c->re);
    }

    return snprintf(buf, len, "(%.5g%.5gi)", c->re, c->im);
}

static void inc(const void* arg, void* res) {
    const Complex* a = (const Complex*)arg;
    Complex* r = (Complex*)res;
    
    r->re = a->re + 1;
    r->im = a->im;
}

static const void* zero() {
    static const Complex z = {0, 0};
    return &z;
}

static const void* one() {
    static const Complex o = {1, 0};
    return &o;
}

static const TypeInfo* TypeInfoComplex = NULL;

const TypeInfo* GetTypeInfoComplex() {
    if (TypeInfoComplex == NULL) {
        TypeInfo* info = malloc(sizeof(TypeInfo));
        info->size = sizeof(Complex);
        info->add = add;
        info->multiply = mulp;
        info->alloc = alloc;
        info->increment = inc;
        info->zero = zero;
        info->one = one;
        info->set = set;
        info->compare = comp;
        info->toString = toString;

        TypeInfoComplex = info;
    }
    return TypeInfoComplex;
}
