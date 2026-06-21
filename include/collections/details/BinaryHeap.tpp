#include <stdexcept>
#include <utility>

#include "../BinaryHeap.h"

template <class T>
BinaryHeap<T>::BinaryHeap()
    : data_(0), size_(0), capacity_(0),
      less_([](const T& a, const T& b) { return a < b; }) {
}

template <class T>
BinaryHeap<T>::BinaryHeap(std::function<bool(const T&, const T&)> comparator)
    : data_(0), size_(0), capacity_(0), less_(std::move(comparator)) {
    if (!less_)
        throw std::invalid_argument("Comparator is empty");
}

template <class T>
void BinaryHeap<T>::EnsureCapacity(int required) {
    if (required <= capacity_) return;
    int newCapacity = (capacity_ == 0) ? 1 : capacity_;
    while (newCapacity < required)
        newCapacity *= 2;
    data_.Resize(newCapacity);
    capacity_ = newCapacity;
}

template <class T>
void BinaryHeap<T>::SwapAt(int i, int j) {
    T tmp = data_.Get(i);
    data_.Set(i, data_.Get(j));
    data_.Set(j, tmp);
}

template <class T>
void BinaryHeap<T>::SiftUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (!less_(data_.Get(index), data_.Get(parent)))
            break;
        SwapAt(index, parent);
        index = parent;
    }
}

template <class T>
void BinaryHeap<T>::SiftDown(int index) {
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int best = index;

        if (left < size_ && less_(data_.Get(left), data_.Get(best)))
            best = left;
        if (right < size_ && less_(data_.Get(right), data_.Get(best)))
            best = right;
        if (best == index)
            break;

        SwapAt(index, best);
        index = best;
    }
}

template <class T>
void BinaryHeap<T>::Push(const T& item) {
    EnsureCapacity(size_ + 1);
    data_.Set(size_, item);
    ++size_;
    SiftUp(size_ - 1);
}

template <class T>
const T& BinaryHeap<T>::Peek() const {
    if (size_ == 0)
        throw std::out_of_range("Heap is empty");
    return data_.Get(0);
}

template <class T>
T BinaryHeap<T>::Pop() {
    if (size_ == 0)
        throw std::out_of_range("Heap is empty");
    T top = data_.Get(0);
    data_.Set(0, data_.Get(size_ - 1));
    --size_;
    if (size_ > 0)
        SiftDown(0);
    return top;
}

template <class T>
int BinaryHeap<T>::GetSize() const {
    return size_;
}

template <class T>
bool BinaryHeap<T>::IsEmpty() const {
    return size_ == 0;
}
