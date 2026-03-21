#ifndef LAB1_TYPEINFOCOMPLEX_H
#define LAB1_TYPEINFOCOMPLEX_H

#include "typeinfo.h"

typedef struct Complex Complex;

const TypeInfo* GetTypeInfoComplex();

Complex* NewComplex(double re, double im);

#endif //LAB1_TYPEINFOCOMPLEX_H