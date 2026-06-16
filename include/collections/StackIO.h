#pragma once

#include <iostream>
#include "Stack.h"
#include "SegmentedDequeIO.h"

template <class T>
std::ostream& operator<<(std::ostream& os, const Stack<T>& stack) {
    os << stack.storage_;
    return os;
}
