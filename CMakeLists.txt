cmake_minimum_required(VERSION 4.0)
project(lab1 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab1 main.c
        typeinfo.h
        typeinfodouble.c
        typeinfodouble.h
        typeinfocomplex.c
        typeinfocomplex.h
        polynomial.c
        polynomial.h
        utils.h
        polynomial_errors/polyerr.c
        polynomial_errors/polyerr.h)
add_executable(test typeinfo.h
        typeinfodouble.c
        typeinfodouble.h
        typeinfocomplex.c
        typeinfocomplex.h
        polynomial.c
        polynomial.h
        utils.h
        polynomial_errors/polyerr.c
        polynomial_errors/polyerr.h
        tests/test.c
        tests/testPoly.c
        tests/testPoly.h
        tests/testTypeInfoDouble.c
        tests/testTypeInfoDouble.h
        tests/testTypeInfoComplex.c
        tests/testTypeInfoComplex.h
)

#target_compile_options(test PRIVATE -fsanitize=address -g)
#target_link_options(test PRIVATE -fsanitize=address)