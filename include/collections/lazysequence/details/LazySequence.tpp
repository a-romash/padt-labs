#include <stdexcept>
#include <utility>

#include "../LazySequence.h"

template <class T>
void LazySequence<T>::EnsureMaterialized(std::size_t index) const {
    while (static_cast<std::size_t>(cache_.GetLength()) <= index) {
        std::size_t nextIndex = cache_.GetLength();
        Ordinal ordinalIndex = Ordinal::Finite(nextIndex);
        if (!generator_->HasNext(ordinalIndex))
            throw std::out_of_range("Index out of range");
        T value = generator_->Get(ordinalIndex);
        cache_.Append(value);
    }
}

template <class T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::SharedCopy() const {
    return std::make_shared<LazySequence<T>>(*this);
}

template <class T>
std::shared_ptr<LazySequence<T>> LazySequence<T>::CreateSeqFromElem(const T& item) {
    T items[] = {item};
    return std::make_shared<LazySequence<T>>(items, 1);
}

template <class T>
LazySequence<T>::LazySequence(std::unique_ptr<Generator<T>> generatorPtr)
    : generator_(std::move(generatorPtr)), cache_() {
}

template <class T>
LazySequence<T>::LazySequence()
    : generator_(std::unique_ptr<Generator<T>>(new EmptyGenerator<T>())), cache_() {
}

template <class T>
LazySequence<T>::LazySequence(const T* items, std::size_t count)
    : generator_(std::unique_ptr<Generator<T>>(new SequenceGenerator<T>(items, count))), cache_() {
}

template <class T>
LazySequence<T>::LazySequence(const Sequence<T>& sequence)
    : generator_(std::unique_ptr<Generator<T>>(new SequenceGenerator<T>(sequence))), cache_() {
}

template <class T>
LazySequence<T>::LazySequence(Sequence<T>* sequence) {
    if (sequence == nullptr)
        throw std::invalid_argument("LazySequence source sequence is null");
    generator_ = std::unique_ptr<Generator<T>>(new SequenceGenerator<T>(*sequence));
    cache_ = MutableArraySequence<T>();
}

template <class T>
LazySequence<T>::LazySequence(std::function<T(Sequence<T>*)> recurrenceRule, Sequence<T>* firstItems,
                              Ordinal length)
    : generator_(std::unique_ptr<Generator<T>>(
          new RecurrenceGenerator<T>(std::move(recurrenceRule), firstItems, length))),
      cache_() {
}

template <class T>
LazySequence<T>::LazySequence(T (*recurrenceRule)(Sequence<T>*), Sequence<T>* firstItems, Ordinal length)
    : LazySequence(std::function<T(Sequence<T>*)>(recurrenceRule), firstItems, length) {
}

template <class T>
LazySequence<T>::LazySequence(std::function<T(std::size_t)> indexRule, Ordinal length)
    : generator_(std::unique_ptr<Generator<T>>(new FunctionGenerator<T>(std::move(indexRule), length))),
      cache_() {
}

template <class T>
LazySequence<T>::LazySequence(const LazySequence& other)
    : generator_(other.generator_->Clone()), cache_(other.cache_) {
}

template <class T>
LazySequence<T>& LazySequence<T>::operator=(const LazySequence& other) {
    if (this != &other) {
        generator_ = other.generator_->Clone();
        cache_ = other.cache_;
    }
    return *this;
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::FromIndexFunction(std::function<T(std::size_t)> indexRule,
                                                                    Ordinal length) {
    return std::unique_ptr<LazySequence<T>>(new LazySequence<T>(std::move(indexRule), length));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::Infinite(std::function<T(std::size_t)> indexRule) {
    return std::unique_ptr<LazySequence<T>>(new LazySequence<T>(std::move(indexRule), Ordinal::Omega()));
}

template <class T>
T LazySequence<T>::GetFirst() const {
    return Get(Ordinal::Finite(0));
}

template <class T>
T LazySequence<T>::GetLast() const {
    Ordinal length = GetLength();
    if (length.IsInfinite() && length.FinitePart() != 0)
        return Get(Ordinal(length.OmegaCoefficient(), length.FinitePart() - 1));
    if (length.IsInfinite())
        throw std::logic_error("Cannot get last element of omega-length LazySequence");
    if (length.FinitePart() == 0)
        throw std::out_of_range("LazySequence is empty");
    return Get(Ordinal::Finite(length.FinitePart() - 1));
}

template <class T>
T LazySequence<T>::Get(Ordinal index) const {
    if (!generator_->HasNext(index))
        throw std::out_of_range("Index out of range");
    if (!index.IsFinite())
        return generator_->Get(index);

    std::size_t finiteIndex = index.FinitePart();
    EnsureMaterialized(finiteIndex);
    return cache_.Get(finiteIndex);
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::GetSubsequence(Ordinal startIndex, Ordinal endIndex) const {
    Ordinal sourceLength = GetLength();
    if (endIndex < startIndex)
        throw std::out_of_range("Invalid subsequence bounds");
    if (sourceLength.IsInfinite() && endIndex == sourceLength) {
        if (!(startIndex < sourceLength))
            throw std::out_of_range("Subsequence index out of range");
        Ordinal count = CalcRangeLength(sourceLength, startIndex);
        return std::unique_ptr<LazySequence<T>>(new LazySequence<T>(
            std::unique_ptr<Generator<T>>(new SubsequenceGenerator<T>(SharedCopy(), startIndex, count))));
    }
    if (!(startIndex < sourceLength) || !(endIndex < sourceLength))
        throw std::out_of_range("Subsequence index out of range");

    Ordinal count = CalcRangeLength(endIndex, startIndex, true);
    return std::unique_ptr<LazySequence<T>>(new LazySequence<T>(
        std::unique_ptr<Generator<T>>(new SubsequenceGenerator<T>(SharedCopy(), startIndex, count))));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::GetSubsequence(std::size_t startIndex,
                                                                 std::size_t endIndex) const {
    return GetSubsequence(Ordinal::Finite(startIndex), Ordinal::Finite(endIndex));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex < 0)
        throw std::out_of_range("Invalid subsequence bounds");
    return GetSubsequence(static_cast<std::size_t>(startIndex), static_cast<std::size_t>(endIndex));
}

template <class T>
Ordinal LazySequence<T>::GetLength() const {
    return generator_->GetLength();
}

template <class T>
std::size_t LazySequence<T>::GetMaterializedCount() const {
    return cache_.GetLength();
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::Append(const T& item) const {
    return Append(*CreateSeqFromElem(item));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::Prepend(const T& item) const {
    return Prepend(*CreateSeqFromElem(item));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::Append(const Sequence<T>& items) const {
    return Append(LazySequence<T>(items));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::Append(const LazySequence<T>& items) const {
    return std::unique_ptr<LazySequence<T>>(new LazySequence<T>(std::unique_ptr<Generator<T>>(
        new AppendGenerator<T>(SharedCopy(), std::make_shared<LazySequence<T>>(items)))));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::Prepend(const Sequence<T>& items) const {
    return Prepend(LazySequence<T>(items));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::Prepend(const LazySequence<T>& items) const {
    return std::unique_ptr<LazySequence<T>>(new LazySequence<T>(std::unique_ptr<Generator<T>>(
        new PrependGenerator<T>(SharedCopy(), std::make_shared<LazySequence<T>>(items)))));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::InsertAt(const T& item, std::size_t index) const {
    return InsertAt(*CreateSeqFromElem(item), Ordinal::Finite(index));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::InsertAt(const T& item, int index) const {
    if (index < 0)
        throw std::out_of_range("Negative index");
    return InsertAt(item, static_cast<std::size_t>(index));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::InsertAt(const Sequence<T>& items, Ordinal index) const {
    return InsertAt(LazySequence<T>(items), index);
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::InsertAt(const LazySequence<T>& items, Ordinal index) const {
    return std::unique_ptr<LazySequence<T>>(new LazySequence<T>(std::unique_ptr<Generator<T>>(
        new InsertGenerator<T>(SharedCopy(), std::make_shared<LazySequence<T>>(items), index))));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::Concat(const LazySequence<T>& other) const {
    return std::unique_ptr<LazySequence<T>>(new LazySequence<T>(std::unique_ptr<Generator<T>>(
        new ConcatGenerator<T>(SharedCopy(), std::make_shared<LazySequence<T>>(other)))));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::Concat(const LazySequence<T>* other) const {
    if (other == nullptr)
        throw std::invalid_argument("Concat argument is null");
    return Concat(*other);
}

template <class T>
template <class T2>
std::unique_ptr<LazySequence<T2>> LazySequence<T>::Map(std::function<T2(T)> mapper) const {
    if (!mapper)
        throw std::invalid_argument("Map function is empty");

    std::unique_ptr<Generator<T2>> generator(new MapGeneratorFrom<T2, T>(SharedCopy(), std::move(mapper)));
    return std::unique_ptr<LazySequence<T2>>(new LazySequence<T2>(std::move(generator)));
}

template <class T>
std::unique_ptr<LazySequence<T>> LazySequence<T>::Where(std::function<bool(T)> predicate) const {
    if (!predicate)
        throw std::invalid_argument("Where predicate is empty");

    return std::unique_ptr<LazySequence<T>>(new LazySequence<T>(
        std::unique_ptr<Generator<T>>(new WhereGenerator<T>(SharedCopy(), std::move(predicate)))));
}

template <class T>
template <class TResult>
TResult LazySequence<T>::ReduceFirstN(std::size_t count, TResult initialValue,
                                      std::function<TResult(TResult, T)> reducer) const {
    if (!reducer)
        throw std::invalid_argument("Reduce function is empty");

    TResult result = initialValue;
    for (std::size_t i = 0; i < count; ++i)
        result = reducer(result, Get(Ordinal::Finite(i)));
    return result;
}

template <class T>
template <class TResult>
TResult LazySequence<T>::Reduce(TResult initialValue, std::function<TResult(TResult, T)> reducer) const {
    Ordinal length = GetLength();
    if (length.IsInfinite())
        throw std::logic_error("Cannot fully reduce omega-length LazySequence. Use ReduceFirstN instead.");
    return ReduceFirstN(length.FinitePart(), initialValue, std::move(reducer));
}

template <class T>
std::unique_ptr<Sequence<T>> LazySequence<T>::Take(std::size_t count) const {
    std::unique_ptr<Sequence<T>> result(new MutableArraySequence<T>());
    for (std::size_t i = 0; i < count; ++i)
        result->Append(Get(Ordinal::Finite(i)));
    return result;
}
