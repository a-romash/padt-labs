#pragma once

#include <cstddef>
#include <functional>
#include <memory>

#include "Generator.h"
#include "Ordinal.h"
#include "../ArraySequence.h"
#include "../Sequence.h"

template <class T>
class LazySequence;

Ordinal CalcRangeLength(Ordinal endIndex, Ordinal startIndex, bool includeEndIndex = false);

template <class T>
class EmptyGenerator : public Generator<T> {
public:
    Ordinal GetLength() const override;
    bool HasNext(Ordinal nextIndex) const override;
    T Get(Ordinal nextIndex) override;
    std::unique_ptr<Generator<T>> Clone() const override;
};

template <class T>
class SequenceGenerator : public Generator<T> {
private:
    MutableArraySequence<T> data_;

public:
    SequenceGenerator(const T* items, std::size_t count);
    explicit SequenceGenerator(const Sequence<T>& source);

    Ordinal GetLength() const override;
    bool HasNext(Ordinal nextIndex) const override;
    T Get(Ordinal nextIndex) override;
    std::unique_ptr<Generator<T>> Clone() const override;
};

template <class T>
class RecurrenceGenerator : public Generator<T> {
private:
    MutableArraySequence<T> generatedItems_;
    std::function<T(Sequence<T>*)> rule_;
    Ordinal length_;

public:
    RecurrenceGenerator(std::function<T(Sequence<T>*)> rule, Sequence<T>* firstItems, Ordinal length);

    Ordinal GetLength() const override;
    bool HasNext(Ordinal nextIndex) const override;
    T Get(Ordinal nextIndex) override;
    std::unique_ptr<Generator<T>> Clone() const override;
};

template <class T>
class FunctionGenerator : public Generator<T> {
private:
    std::function<T(std::size_t)> rule_;
    Ordinal length_;

public:
    FunctionGenerator(std::function<T(std::size_t)> rule, Ordinal length);

    Ordinal GetLength() const override;
    bool HasNext(Ordinal nextIndex) const override;
    T Get(Ordinal nextIndex) override;
    std::unique_ptr<Generator<T>> Clone() const override;
};

template <class T>
class PrependGenerator : public Generator<T> {
private:
    std::shared_ptr<LazySequence<T>> source_;
    std::shared_ptr<LazySequence<T>> prepended_;

public:
    PrependGenerator(std::shared_ptr<LazySequence<T>> source, std::shared_ptr<LazySequence<T>> prepended);

    Ordinal GetLength() const override;
    bool HasNext(Ordinal nextIndex) const override;
    T Get(Ordinal nextIndex) override;
    std::unique_ptr<Generator<T>> Clone() const override;
};

template <class T>
class AppendGenerator : public Generator<T> {
private:
    std::shared_ptr<LazySequence<T>> source_;
    std::shared_ptr<LazySequence<T>> appended_;

public:
    AppendGenerator(std::shared_ptr<LazySequence<T>> source, std::shared_ptr<LazySequence<T>> appended);

    Ordinal GetLength() const override;
    bool HasNext(Ordinal nextIndex) const override;
    T Get(Ordinal nextIndex) override;
    std::unique_ptr<Generator<T>> Clone() const override;
};

template <class T>
class InsertGenerator : public Generator<T> {
private:
    std::shared_ptr<LazySequence<T>> source_;
    std::shared_ptr<LazySequence<T>> inserted_;
    Ordinal index_;

public:
    InsertGenerator(std::shared_ptr<LazySequence<T>> source,
                    std::shared_ptr<LazySequence<T>> inserted,
                    Ordinal index);

    Ordinal GetLength() const override;
    bool HasNext(Ordinal nextIndex) const override;
    T Get(Ordinal nextIndex) override;
    std::unique_ptr<Generator<T>> Clone() const override;
};

template <class T>
class ConcatGenerator : public Generator<T> {
private:
    std::shared_ptr<LazySequence<T>> first_;
    std::shared_ptr<LazySequence<T>> second_;

public:
    ConcatGenerator(std::shared_ptr<LazySequence<T>> first, std::shared_ptr<LazySequence<T>> second);

    Ordinal GetLength() const override;
    bool HasNext(Ordinal nextIndex) const override;
    T Get(Ordinal nextIndex) override;
    std::unique_ptr<Generator<T>> Clone() const override;
};

template <class T>
class SubsequenceGenerator : public Generator<T> {
private:
    std::shared_ptr<LazySequence<T>> source_;
    Ordinal start_;
    Ordinal length_;

public:
    SubsequenceGenerator(std::shared_ptr<LazySequence<T>> source, Ordinal start, Ordinal length);

    Ordinal GetLength() const override;
    bool HasNext(Ordinal nextIndex) const override;
    T Get(Ordinal nextIndex) override;
    std::unique_ptr<Generator<T>> Clone() const override;
};

template <class T, class SourceT>
class MapGeneratorFrom : public Generator<T> {
private:
    std::shared_ptr<LazySequence<SourceT>> source_;
    std::function<T(SourceT)> mapper_;

public:
    MapGeneratorFrom(std::shared_ptr<LazySequence<SourceT>> source, std::function<T(SourceT)> mapper);

    Ordinal GetLength() const override;
    bool HasNext(Ordinal nextIndex) const override;
    T Get(Ordinal nextIndex) override;
    std::unique_ptr<Generator<T>> Clone() const override;
};

template <class T>
class WhereGenerator : public Generator<T> {
private:
    std::shared_ptr<LazySequence<T>> source_;
    std::function<bool(T)> predicate_;
    std::size_t sourceIndex_;

public:
    WhereGenerator(std::shared_ptr<LazySequence<T>> source, std::function<bool(T)> predicate);

    Ordinal GetLength() const override;
    bool HasNext(Ordinal nextIndex) const override;
    T Get(Ordinal nextIndex) override;
    std::unique_ptr<Generator<T>> Clone() const override;
};

#include "details/LazySequenceGenerators.tpp"
