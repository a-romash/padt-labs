#pragma once

template <class T, class U>
struct Pair {
    T first;
    U second;

    Pair() = default;

    Pair(const T& first, const U& second)
        : first(first), second(second) {
    }
};
