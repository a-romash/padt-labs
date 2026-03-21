#ifndef LAB1_TYPEINFO_H
#define LAB1_TYPEINFO_H
#include <stddef.h>

typedef void (*UnaryOperation)(const void*, void*);
typedef void (*BinaryOperation)(const void*, const void*, void*);
typedef const void* (*GetOperation)();
typedef void (*SetOperation)(const void*, void*);
typedef int (*CompareOperation)(const void*, const void*);
typedef void* (*AllocateOperation)();
typedef int (*ToString) (const void*, char*, size_t);

typedef struct TypeInfo {
    size_t size;
    BinaryOperation add;
    BinaryOperation multiply;
    AllocateOperation alloc;
    UnaryOperation increment;
    GetOperation zero;
    GetOperation one;
    SetOperation set;
    CompareOperation compare;
    ToString toString;
} TypeInfo;

#endif //LAB1_TYPEINFO_H