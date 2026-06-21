#include <stdexcept>
#include <utility>

#include "../LazySequenceGenerators.h"
#include "../LazySequence.h"

inline Ordinal CalcRangeLength(const Ordinal endIndex, const Ordinal startIndex, const bool includeEndIndex) {
    if (endIndex < startIndex)
        throw std::out_of_range("Invalid ordinal subtraction");

    Ordinal result = Ordinal::Finite(0);

    if (startIndex.IsFinite()) {
        if (endIndex.IsFinite())
            result = Ordinal::Finite(endIndex.FinitePart() - startIndex.FinitePart());
        else
            result = endIndex;
    } else if (endIndex.OmegaCoefficient() == startIndex.OmegaCoefficient()) {
        result = Ordinal::Finite(endIndex.FinitePart() - startIndex.FinitePart());
    } else {
        result = Ordinal::FromParts(
            endIndex.OmegaCoefficient() - startIndex.OmegaCoefficient(),
            endIndex.FinitePart()
        );
    }

    return includeEndIndex
               ? Ordinal::FromParts(result.OmegaCoefficient(), result.FinitePart() + 1)
               : result;
}


template <class T>
Ordinal EmptyGenerator<T>::GetLength() const {
    return Ordinal::Finite(0);
}

template <class T>
bool EmptyGenerator<T>::HasNext(Ordinal) const {
    return false;
}

template <class T>
T EmptyGenerator<T>::Get(Ordinal) {
    throw std::out_of_range("LazySequence is empty");
}

template <class T>
std::unique_ptr<Generator<T>> EmptyGenerator<T>::Clone() const {
    return std::unique_ptr<Generator<T>>(new EmptyGenerator<T>(*this));
}


template <class T>
SequenceGenerator<T>::SequenceGenerator(const T* items, const std::size_t count)
    : data_() {
    if (items == nullptr && count > 0)
        throw std::invalid_argument("LazySequence source array is null");

    for (std::size_t i = 0; i < count; ++i)
        data_.Append(items[i]);
}

template <class T>
SequenceGenerator<T>::SequenceGenerator(const Sequence<T>& source)
    : data_() {
    IEnumerator<T>* enumerator = source.GetEnumerator();
    while (enumerator->MoveNext())
        data_.Append(enumerator->GetCurrent());
    delete enumerator;
}

template <class T>
Ordinal SequenceGenerator<T>::GetLength() const {
    return Ordinal::Finite(data_.GetLength());
}

template <class T>
bool SequenceGenerator<T>::HasNext(Ordinal nextIndex) const {
    return nextIndex < Ordinal::Finite(data_.GetLength());
}

template <class T>
T SequenceGenerator<T>::Get(Ordinal nextIndex) {
    if (!HasNext(nextIndex))
        throw std::out_of_range("Index out of range");
    return data_.Get(nextIndex.FinitePart());
}

template <class T>
std::unique_ptr<Generator<T>> SequenceGenerator<T>::Clone() const {
    return std::unique_ptr<Generator<T>>(new SequenceGenerator<T>(*this));
}


template <class T>
RecurrenceGenerator<T>::RecurrenceGenerator(std::function<T(Sequence<T>*)> rule, Sequence<T>* firstItems,
                                            const Ordinal length)
    : generatedItems_(), rule_(std::move(rule)), length_(length) {
    if (!rule_)
        throw std::invalid_argument("Recurrence rule is empty");
    if (firstItems == nullptr)
        throw std::invalid_argument("Recurrence seed sequence is null");
    if (length_.IsFinite() && length_.FinitePart() < static_cast<std::size_t>(firstItems->GetLength()))
        throw std::invalid_argument("Finite length is less than seed count");

    IEnumerator<T>* enumerator = firstItems->GetEnumerator();
    while (enumerator->MoveNext())
        generatedItems_.Append(enumerator->GetCurrent());
    delete enumerator;
}

template <class T>
Ordinal RecurrenceGenerator<T>::GetLength() const {
    return length_;
}

template <class T>
bool RecurrenceGenerator<T>::HasNext(Ordinal nextIndex) const {
    return nextIndex < length_;
}

template <class T>
T RecurrenceGenerator<T>::Get(Ordinal nextIndex) {
    if (!HasNext(nextIndex))
        throw std::out_of_range("Index out of range");
    if (!nextIndex.IsFinite())
        throw std::logic_error("Recurrence generator supports only finite indexes");

    std::size_t finiteIndex = nextIndex.FinitePart();
    while (generatedItems_.GetLength() <= static_cast<int>(finiteIndex))
        generatedItems_.Append(rule_(&generatedItems_));
    return generatedItems_.Get(finiteIndex);
}

template <class T>
std::unique_ptr<Generator<T>> RecurrenceGenerator<T>::Clone() const {
    return std::unique_ptr<Generator<T>>(new RecurrenceGenerator<T>(*this));
}


template <class T>
FunctionGenerator<T>::FunctionGenerator(std::function<T(std::size_t)> rule, const Ordinal length)
    : rule_(std::move(rule)), length_(length) {
    if (!rule_)
        throw std::invalid_argument("Index function rule is empty");
}

template <class T>
Ordinal FunctionGenerator<T>::GetLength() const {
    return length_;
}

template <class T>
bool FunctionGenerator<T>::HasNext(Ordinal nextIndex) const {
    return nextIndex < length_;
}

template <class T>
T FunctionGenerator<T>::Get(Ordinal nextIndex) {
    if (!HasNext(nextIndex))
        throw std::out_of_range("Index out of range");
    if (!nextIndex.IsFinite())
        throw std::logic_error("Index function generator supports only finite indexes");
    return rule_(nextIndex.FinitePart());
}

template <class T>
std::unique_ptr<Generator<T>> FunctionGenerator<T>::Clone() const {
    return std::unique_ptr<Generator<T>>(new FunctionGenerator<T>(*this));
}


template <class T>
PrependGenerator<T>::PrependGenerator(std::shared_ptr<LazySequence<T>> source,
                                      std::shared_ptr<LazySequence<T>> prepended)
    : source_(std::move(source)), prepended_(std::move(prepended)) {
}

template <class T>
Ordinal PrependGenerator<T>::GetLength() const {
    return prepended_->GetLength() + source_->GetLength();
}

template <class T>
bool PrependGenerator<T>::HasNext(Ordinal nextIndex) const {
    return nextIndex < GetLength();
}

template <class T>
T PrependGenerator<T>::Get(Ordinal nextIndex) {
    if (!HasNext(nextIndex))
        throw std::out_of_range("Index out of range");

    Ordinal prependedLength = prepended_->GetLength();
    if (nextIndex < prependedLength)
        return prepended_->Get(nextIndex);
    return source_->Get(CalcRangeLength(nextIndex, prependedLength));
}

template <class T>
std::unique_ptr<Generator<T>> PrependGenerator<T>::Clone() const {
    return std::unique_ptr<Generator<T>>(new PrependGenerator<T>(*this));
}


template <class T>
AppendGenerator<T>::AppendGenerator(std::shared_ptr<LazySequence<T>> source,
                                    std::shared_ptr<LazySequence<T>> appended)
    : source_(std::move(source)), appended_(std::move(appended)) {
}

template <class T>
Ordinal AppendGenerator<T>::GetLength() const {
    return source_->GetLength() + appended_->GetLength();
}

template <class T>
bool AppendGenerator<T>::HasNext(Ordinal nextIndex) const {
    return nextIndex < GetLength();
}

template <class T>
T AppendGenerator<T>::Get(Ordinal nextIndex) {
    if (!HasNext(nextIndex))
        throw std::out_of_range("Index out of range");

    Ordinal length = source_->GetLength();
    if (nextIndex < length)
        return source_->Get(nextIndex);
    return appended_->Get(CalcRangeLength(nextIndex, length));
}

template <class T>
std::unique_ptr<Generator<T>> AppendGenerator<T>::Clone() const {
    return std::unique_ptr<Generator<T>>(new AppendGenerator<T>(*this));
}


template <class T>
InsertGenerator<T>::InsertGenerator(std::shared_ptr<LazySequence<T>> source,
                                    std::shared_ptr<LazySequence<T>> inserted,
                                    const Ordinal index)
    : source_(std::move(source)), inserted_(std::move(inserted)), index_(index) {
    if (!source_)
        throw std::invalid_argument("Insert source is null");
    if (!inserted_)
        throw std::invalid_argument("Inserted sequence is null");

    Ordinal length = source_->GetLength();
    if (!(index_ < length))
        throw std::out_of_range("Insert index out of range");
}

template <class T>
Ordinal InsertGenerator<T>::GetLength() const {
    Ordinal prefixLength = index_;
    Ordinal insertedLength = inserted_->GetLength();
    Ordinal suffixLength = CalcRangeLength(source_->GetLength(), prefixLength);
    return prefixLength + insertedLength + suffixLength;
}

template <class T>
bool InsertGenerator<T>::HasNext(Ordinal nextIndex) const {
    return nextIndex < GetLength();
}

template <class T>
T InsertGenerator<T>::Get(Ordinal nextIndex) {
    if (!HasNext(nextIndex))
        throw std::out_of_range("Index out of range");
    if (nextIndex < index_)
        return source_->Get(nextIndex);

    Ordinal localInsertedIndex = CalcRangeLength(nextIndex, index_);
    Ordinal insertedLength = inserted_->GetLength();
    if (localInsertedIndex < insertedLength)
        return inserted_->Get(localInsertedIndex);

    Ordinal sourceSuffixIndex = CalcRangeLength(localInsertedIndex, insertedLength);
    return source_->Get(index_ + sourceSuffixIndex);
}

template <class T>
std::unique_ptr<Generator<T>> InsertGenerator<T>::Clone() const {
    return std::unique_ptr<Generator<T>>(new InsertGenerator<T>(*this));
}


template <class T>
ConcatGenerator<T>::ConcatGenerator(std::shared_ptr<LazySequence<T>> first,
                                    std::shared_ptr<LazySequence<T>> second)
    : first_(std::move(first)), second_(std::move(second)) {
}

template <class T>
Ordinal ConcatGenerator<T>::GetLength() const {
    return first_->GetLength() + second_->GetLength();
}

template <class T>
bool ConcatGenerator<T>::HasNext(Ordinal nextIndex) const {
    return nextIndex < GetLength();
}

template <class T>
T ConcatGenerator<T>::Get(Ordinal nextIndex) {
    if (!HasNext(nextIndex))
        throw std::out_of_range("Index out of range");
    Ordinal firstLength = first_->GetLength();
    if (nextIndex < firstLength)
        return first_->Get(nextIndex);
    return second_->Get(CalcRangeLength(nextIndex, firstLength));
}

template <class T>
std::unique_ptr<Generator<T>> ConcatGenerator<T>::Clone() const {
    return std::unique_ptr<Generator<T>>(new ConcatGenerator<T>(*this));
}


template <class T>
SubsequenceGenerator<T>::SubsequenceGenerator(std::shared_ptr<LazySequence<T>> source,
                                              const Ordinal start, const Ordinal length)
    : source_(std::move(source)), start_(start), length_(length) {
}

template <class T>
Ordinal SubsequenceGenerator<T>::GetLength() const {
    return length_;
}

template <class T>
bool SubsequenceGenerator<T>::HasNext(Ordinal nextIndex) const {
    return nextIndex < length_;
}

template <class T>
T SubsequenceGenerator<T>::Get(Ordinal nextIndex) {
    if (!HasNext(nextIndex))
        throw std::out_of_range("Index out of range");
    Ordinal sourceIndex = start_ + nextIndex;
    return source_->Get(sourceIndex);
}

template <class T>
std::unique_ptr<Generator<T>> SubsequenceGenerator<T>::Clone() const {
    return std::unique_ptr<Generator<T>>(new SubsequenceGenerator<T>(*this));
}


template <class T, class SourceT>
MapGeneratorFrom<T, SourceT>::MapGeneratorFrom(std::shared_ptr<LazySequence<SourceT>> source,
                                               std::function<T(SourceT)> mapper)
    : source_(std::move(source)), mapper_(std::move(mapper)) {
}

template <class T, class SourceT>
Ordinal MapGeneratorFrom<T, SourceT>::GetLength() const {
    return source_->GetLength();
}

template <class T, class SourceT>
bool MapGeneratorFrom<T, SourceT>::HasNext(Ordinal nextIndex) const {
    return nextIndex < source_->GetLength();
}

template <class T, class SourceT>
T MapGeneratorFrom<T, SourceT>::Get(Ordinal nextIndex) {
    return mapper_(source_->Get(nextIndex));
}

template <class T, class SourceT>
std::unique_ptr<Generator<T>> MapGeneratorFrom<T, SourceT>::Clone() const {
    return std::unique_ptr<Generator<T>>(new MapGeneratorFrom<T, SourceT>(*this));
}


template <class T>
WhereGenerator<T>::WhereGenerator(std::shared_ptr<LazySequence<T>> source, std::function<bool(T)> predicate)
    : source_(std::move(source)), predicate_(std::move(predicate)), sourceIndex_(0) {
}

template <class T>
Ordinal WhereGenerator<T>::GetLength() const {
    Ordinal sourceLength = source_->GetLength();
    if (sourceLength.IsInfinite())
        return sourceLength;

    std::size_t count = 0;
    for (std::size_t i = 0; i < sourceLength.FinitePart(); ++i) {
        if (predicate_(source_->Get(Ordinal::Finite(i))))
            ++count;
    }
    return Ordinal::Finite(count);
}

template <class T>
bool WhereGenerator<T>::HasNext(Ordinal nextIndex) const {
    return nextIndex < GetLength();
}

template <class T>
T WhereGenerator<T>::Get(Ordinal nextIndex) {
    if (!nextIndex.IsFinite())
        throw std::logic_error("Where generator supports only finite indexes");
    Ordinal sourceLength = source_->GetLength();
    while (sourceLength.IsInfinite() || sourceIndex_ < sourceLength.FinitePart()) {
        T current = source_->Get(Ordinal::Finite(sourceIndex_));
        ++sourceIndex_;
        if (predicate_(current))
            return current;
    }
    throw std::out_of_range("No next element satisfies predicate");
}

template <class T>
std::unique_ptr<Generator<T>> WhereGenerator<T>::Clone() const {
    return std::unique_ptr<Generator<T>>(new WhereGenerator<T>(*this));
}
