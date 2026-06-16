#include "../Stack.h"
#include <ostream>
#include <stdexcept>

template <class T>
Stack<T>::Stack(int segmentLength)
    : storage_(segmentLength), segmentLength_(segmentLength) {
}

template <class T>
void Stack<T>::Push(const T& item) {
    storage_.Append(item);
}

template <class T>
T Stack<T>::Pop() {
    if (IsEmpty())
        throw std::out_of_range("Stack::Pop: stack is empty");
    return storage_.PopLast();
}

template <class T>
const T& Stack<T>::Peek() const {
    if (IsEmpty())
        throw std::out_of_range("Stack::Peek: stack is empty");
    return storage_.GetLast();
}

template <class T>
const T& Stack<T>::Get(int index) const {
    return storage_.Get(index);
}

template <class T>
int Stack<T>::GetLength() const {
    return storage_.GetLength();
}

template <class T>
bool Stack<T>::IsEmpty() const {
    return storage_.IsEmpty();
}

template <class T>
void Stack<T>::Clear() {
    storage_ = SegmentedDeque<T>(segmentLength_);
}

template <class T>
Stack<T> Stack<T>::Concat(const Stack<T>& other) const {
    Stack<T> result(segmentLength_);

    IEnumerator<T>* thisIt = this->GetEnumerator();
    while (thisIt->MoveNext())
        result.Push(thisIt->GetCurrent());
    delete thisIt;

    IEnumerator<T>* otherIt = other.GetEnumerator();
    while (otherIt->MoveNext())
        result.Push(otherIt->GetCurrent());
    delete otherIt;

    return result;
}

template <class T>
IEnumerator<T>* Stack<T>::GetEnumerator() const {
    return storage_.GetEnumerator();
}
