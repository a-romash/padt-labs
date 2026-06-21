#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>

#include "Ordinal.h"
#include "Generator.h"
#include "LazySequenceGenerators.h"
#include "../ArraySequence.h"
#include "../Sequence.h"
#include "../../utils/IEnumerator.h"

template <class T>
class LazySequence {
private:
    std::unique_ptr<Generator<T>> generator_;
    mutable MutableArraySequence<T> cache_;

    void EnsureMaterialized(std::size_t index) const;

    std::shared_ptr<LazySequence<T>> SharedCopy() const;

    static std::shared_ptr<LazySequence<T>> CreateSeqFromElem(const T& item);

    explicit LazySequence(std::unique_ptr<Generator<T>> generatorPtr);

    template <class U>
    friend class LazySequence;

    class LazyEnumerator : public IEnumerator<T> {
    private:
        const LazySequence<T>* sequence_;
        std::size_t index_;
        bool currentValid_;
        T current_;

    public:
        explicit LazyEnumerator(const LazySequence<T>* sequence)
            : sequence_(sequence), index_(0), currentValid_(false), current_() {}

        bool MoveNext() override {
            if (!(Ordinal::Finite(index_) < sequence_->GetLength())) {
                currentValid_ = false;
                return false;
            }
            current_ = sequence_->Get(Ordinal::Finite(index_));
            ++index_;
            currentValid_ = true;
            return true;
        }

        const T& GetCurrent() const override {
            if (!currentValid_)
                throw std::out_of_range("Enumerator out of range");
            return current_;
        }

        void Reset() override {
            index_ = 0;
            currentValid_ = false;
        }
    };

public:
    LazySequence();
    LazySequence(const T* items, std::size_t count);
    explicit LazySequence(const Sequence<T>& sequence);
    explicit LazySequence(Sequence<T>* sequence);
    LazySequence(std::function<T(Sequence<T>*)> recurrenceRule, Sequence<T>* firstItems,
                 Ordinal length = Ordinal::Omega());
    LazySequence(T (*recurrenceRule)(Sequence<T>*), Sequence<T>* firstItems,
                 Ordinal length = Ordinal::Omega());
    LazySequence(std::function<T(std::size_t)> indexRule, Ordinal length);

    LazySequence(const LazySequence& other);
    LazySequence(LazySequence&& other) noexcept = default;

    LazySequence& operator=(const LazySequence& other);
    LazySequence& operator=(LazySequence&& other) noexcept = default;

    static std::unique_ptr<LazySequence<T>> FromIndexFunction(std::function<T(std::size_t)> indexRule,
                                                              Ordinal length);
    static std::unique_ptr<LazySequence<T>> Infinite(std::function<T(std::size_t)> indexRule);

    T GetFirst() const;
    T GetLast() const;
    T Get(Ordinal index) const;

    std::unique_ptr<LazySequence<T>> GetSubsequence(Ordinal startIndex, Ordinal endIndex) const;
    std::unique_ptr<LazySequence<T>> GetSubsequence(std::size_t startIndex, std::size_t endIndex) const;
    std::unique_ptr<LazySequence<T>> GetSubsequence(int startIndex, int endIndex) const;

    Ordinal GetLength() const;
    std::size_t GetMaterializedCount() const;

    std::unique_ptr<LazySequence<T>> Append(const T& item) const;
    std::unique_ptr<LazySequence<T>> Prepend(const T& item) const;
    std::unique_ptr<LazySequence<T>> Append(const Sequence<T>& items) const;
    std::unique_ptr<LazySequence<T>> Append(const LazySequence<T>& items) const;
    std::unique_ptr<LazySequence<T>> Prepend(const Sequence<T>& items) const;
    std::unique_ptr<LazySequence<T>> Prepend(const LazySequence<T>& items) const;

    std::unique_ptr<LazySequence<T>> InsertAt(const T& item, std::size_t index) const;
    std::unique_ptr<LazySequence<T>> InsertAt(const T& item, int index) const;
    std::unique_ptr<LazySequence<T>> InsertAt(const Sequence<T>& items, Ordinal index) const;
    std::unique_ptr<LazySequence<T>> InsertAt(const LazySequence<T>& items, Ordinal index) const;

    std::unique_ptr<LazySequence<T>> Concat(const LazySequence<T>& other) const;
    std::unique_ptr<LazySequence<T>> Concat(const LazySequence<T>* other) const;

    template <class T2>
    std::unique_ptr<LazySequence<T2>> Map(std::function<T2(T)> mapper) const;

    std::unique_ptr<LazySequence<T>> Where(std::function<bool(T)> predicate) const;

    template <class TResult>
    TResult ReduceFirstN(std::size_t count, TResult initialValue,
                         std::function<TResult(TResult, T)> reducer) const;

    template <class TResult>
    TResult Reduce(TResult initialValue, std::function<TResult(TResult, T)> reducer) const;

    std::unique_ptr<IEnumerator<T>> GetEnumerator() const {
        return std::unique_ptr<IEnumerator<T>>(new LazyEnumerator(this));
    }

    std::unique_ptr<Sequence<T>> Take(std::size_t count) const;
};

#include "details/LazySequence.tpp"
