#pragma once

#include "SegmentedDeque.h"
#include "../utils/IEnumerator.h"
#include <iosfwd>
#include <stdexcept>

template <class T> class Stack;
template <class T>
std::ostream& operator<<(std::ostream& os, const Stack<T>& stack);

template <class T>
class Stack {
private:
    SegmentedDeque<T> storage_;
    int segmentLength_;

public:
    explicit Stack(int segmentLength = 16);
    ~Stack() = default;

    void Push(const T& item);
    T Pop();
    const T& Peek() const;

    const T& Get(int index) const;
    int GetLength() const;
    bool IsEmpty() const;
    void Clear();

    Stack<T> Concat(const Stack<T>& other) const;

    IEnumerator<T>* GetEnumerator() const;

    friend std::ostream& operator<< <T>(std::ostream& os, const Stack<T>& stack);
};

#include "details/Stack.tpp"
