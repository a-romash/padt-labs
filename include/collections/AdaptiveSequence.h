#pragma once

#include "Sequence.h"
#include "ArraySequence.h"
#include "ListSequence.h"

template <class T>
class AdaptiveSequence : public Sequence<T> {
public:
    AdaptiveSequence();
    ~AdaptiveSequence();

    const T& GetFirst() const override;
    const T& GetLast() const override;
    const T& Get(int index) const override;
    int GetLength() const override;

    Sequence<T>* GetSubsequence(int start, int end) const override;
    Sequence<T>* Append(const T& item) override;
    Sequence<T>* Prepend(const T& item) override;
    Sequence<T>* InsertAt(const T& item, int index) override;
    Sequence<T>* Concat(const Sequence<T>& other) const override;

    T operator[](int index) const override;
    Sequence<T>* operator+(const Sequence<T>& other) const override;

    IEnumerator<T>* GetEnumerator() const override;

    bool IsArray() const;

private:
    mutable int index_ops_;
    mutable int insert_ops_;
    mutable bool is_array_;
    mutable Sequence<T>* inner_;

    static const int THRESHOLD = 10;

    void MaybeSwitch();
};

#include "details/AdaptiveSequence.tpp"

