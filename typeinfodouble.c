#include "typeinfodouble.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct Double{
    double value;
} Double;

Double* NewDouble(const double val) {
    Double* d = (Double*)malloc(sizeof(Double));
    if (d == NULL) return NULL;
    d->value = val;
    return d;
}

static void add(const void* arg1, const void* arg2, void* res) {
    const Double* p1 = (const Double*)arg1;
    const Double* p2 = (const Double*)arg2;
    Double* result = (Double*)res;

    result->value = p1->value + p2->value;
}

static void mulp(const void* arg1, const void* arg2, void* res) {
    const Double* p1 = (const Double*)arg1;
    const Double* p2 = (const Double*)arg2;
    Double* result = (Double*)res;

    result->value = p1->value * p2->value;
}

static void set(const void* src, void* dst) {
    const Double* p = (const Double*)src;
    Double* q = (Double*)dst;

    q->value = p->value;
}

static void* alloc() {
    return malloc(sizeof(Double));
}

static int toString(const void* p, char* buf, size_t len) {
    return snprintf(buf, len, "%.5g", ((const Double*)p)->value);
}

static void inc(const void* arg, void* res) {
    const Double* a = (const Double*)arg;
    Double* r = (Double*)res;
    
    r->value = a->value + 1;
}

static const void* zero() {
    static const Double z = {0};
    return &z;
}

static const void* one() {
    static const Double o = {1};
    return &o;
}

static int comp(const void* arg1, const void* arg2) {
    const Double* p1 = (const Double*)arg1;
    const Double* p2 = (const Double*)arg2;

    if (p1->value < p2->value) return -1;
    if (p1->value > p2->value) return 1;
    return 0;
}

static const TypeInfo* TypeInfoDouble = NULL;

const TypeInfo* GetTypeInfoDouble() {
    if (TypeInfoDouble == NULL) {
        TypeInfo* info = malloc(sizeof(TypeInfo));
        info->size = sizeof(Double);
        info->add = add;
        info->multiply = mulp;
        info->alloc = alloc;
        info->increment = inc;
        info->zero = zero;
        info->one = one;
        info->set = set;
        info->compare = comp;
        info->toString = toString;

        TypeInfoDouble = info;
    }
    return TypeInfoDouble;
}
