#ifndef LAB1_TYPEINFODOUBLE_H
#define LAB1_TYPEINFODOUBLE_H

#include "typeinfo.h"

typedef struct Double Double;

const TypeInfo* GetTypeInfoDouble();

Double* NewDouble(double val);

#endif //LAB1_TYPEINFODOUBLE_H