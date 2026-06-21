#pragma once

#include <functional>
#include "DynamicArray.h"

template <class T>
class BinaryHeap {
private:
    DynamicArray<T> data_;
    int size_;
    int capacity_;
    std::function<bool(const T&, const T&)> less_;

    void EnsureCapacity(int required);
    void SiftUp(int index);
    void SiftDown(int index);
    void SwapAt(int i, int j);

public:
    BinaryHeap();
    explicit BinaryHeap(std::function<bool(const T&, const T&)> comparator);

    void Push(const T& item);
    const T& Peek() const;
    T Pop();

    int GetSize() const;
    bool IsEmpty() const;
};

#include "details/BinaryHeap.tpp"
