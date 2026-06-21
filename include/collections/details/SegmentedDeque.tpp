#include "../SegmentedDeque.h"
#include <stdexcept>

template <class T>
typename SegmentedDeque<T>::Segment SegmentedDeque<T>::MakeEmptySegment() const {
    Segment segment(segmentLength_);
    int mid = segmentLength_ / 2;
    segment.SetHead(mid);
    segment.SetTail(mid);
    return segment;
}

template <class T>
SegmentedDeque<T>::SegmentedDeque(int segmentLength)
    : segmentLength_(segmentLength), length_(0) {
    if (segmentLength_ < 2)
        throw std::invalid_argument("SegmentedDeque: segmentLength must be at least 2");
    segments_.Append(MakeEmptySegment());
}

template <class T>
const T& SegmentedDeque<T>::GetFirst() const {
    if (IsEmpty())
        throw std::out_of_range("SegmentedDeque::GetFirst: deque is empty");
    return segments_.GetFirst().PeekFirst();
}

template <class T>
const T& SegmentedDeque<T>::GetLast() const {
    if (IsEmpty())
        throw std::out_of_range("SegmentedDeque::GetLast: deque is empty");
    return segments_.GetLast().PeekLast();
}

template <class T>
const T& SegmentedDeque<T>::Get(int index) const {
    if (index < 0 || index >= length_)
        throw std::out_of_range("SegmentedDeque::Get: index out of range");

    IEnumerator<Segment>* segmentIt = segments_.GetEnumerator();
    const T* result = nullptr;
    while (segmentIt->MoveNext()) {
        const Segment& segment = segmentIt->GetCurrent();
        if (index < segment.GetSize()) {
            result = &segment.Get(index);
            break;
        }
        index -= segment.GetSize();
    }
    delete segmentIt;

    if (result == nullptr)
        throw std::out_of_range("SegmentedDeque::Get: index out of range");
    return *result;
}

template <class T>
int SegmentedDeque<T>::GetLength() const {
    return length_;
}

template <class T>
bool SegmentedDeque<T>::IsEmpty() const {
    return length_ == 0;
}

template <class T>
Sequence<T>* SegmentedDeque<T>::GetSubsequence(int startIndex, int endIndex) const {
    if (startIndex < 0 || endIndex >= length_ || startIndex > endIndex)
        throw std::out_of_range("Invalid range");

    auto* result = new SegmentedDeque<T>(segmentLength_);
    IEnumerator<T>* it = this->GetEnumerator();
    int current = 0;
    while (it->MoveNext()) {
        if (current >= startIndex && current <= endIndex)
            result->Append(it->GetCurrent());
        if (current >= endIndex)
            break;
        ++current;
    }
    delete it;
    return result;
}

template <class T>
Sequence<T>* SegmentedDeque<T>::Append(const T& item) {
    Segment& last = segments_.GetLast();
    if (last.IsTailNotAtEnd() || last.IsEmpty()) {
        last.Append(item);
    } else {
        Segment segment(segmentLength_);
        segment.Append(item);
        segments_.Append(segment);
    }
    ++length_;
    return this;
}

template <class T>
Sequence<T>* SegmentedDeque<T>::Prepend(const T& item) {
    Segment& first = segments_.GetFirst();
    if (first.IsHeadNotZero() || first.IsEmpty()) {
        first.Prepend(item);
    } else {
        Segment segment(segmentLength_);
        segment.SetHead(segmentLength_ - 1);
        segment.SetTail(segmentLength_ - 1);
        segment.Prepend(item);
        segments_.Prepend(segment);
    }
    ++length_;
    return this;
}

template <class T>
Sequence<T>* SegmentedDeque<T>::InsertAt(const T& item, int index) {
    if (index < 0 || index > length_)
        throw std::out_of_range("SegmentedDeque::InsertAt: index out of range");

    SegmentedDeque<T> result(segmentLength_);
    IEnumerator<T>* it = this->GetEnumerator();
    int current = 0;
    while (it->MoveNext()) {
        if (current == index)
            result.Append(item);
        result.Append(it->GetCurrent());
        ++current;
    }
    if (index == length_)
        result.Append(item);
    delete it;

    segments_ = result.segments_;
    length_ = result.length_;
    return this;
}

template <class T>
Sequence<T>* SegmentedDeque<T>::Concat(const Sequence<T>& other) const {
    auto* result = new SegmentedDeque<T>(segmentLength_);

    IEnumerator<T>* thisIt = this->GetEnumerator();
    while (thisIt->MoveNext())
        result->Append(thisIt->GetCurrent());
    delete thisIt;

    IEnumerator<T>* otherIt = other.GetEnumerator();
    while (otherIt->MoveNext())
        result->Append(otherIt->GetCurrent());
    delete otherIt;

    return result;
}

template <class T>
T SegmentedDeque<T>::operator[](int index) const {
    return Get(index);
}

template <class T>
Sequence<T>* SegmentedDeque<T>::operator+(const Sequence<T>& other) const {
    return Concat(other);
}

template <class T>
T SegmentedDeque<T>::PopFirst() {
    if (IsEmpty())
        throw std::out_of_range("SegmentedDeque::PopFirst: deque is empty");

    Segment& first = segments_.GetFirst();
    T result = first.PopFirst();
    --length_;

    if (first.IsEmpty()) {
        segments_.Del(0);
        if (segments_.GetLength() == 0)
            segments_.Append(MakeEmptySegment());
    }
    return result;
}

template <class T>
T SegmentedDeque<T>::PopLast() {
    if (IsEmpty())
        throw std::out_of_range("SegmentedDeque::PopLast: deque is empty");

    Segment& last = segments_.GetLast();
    T result = last.PopLast();
    --length_;

    if (last.IsEmpty()) {
        segments_.Del(segments_.GetLength() - 1);
        if (segments_.GetLength() == 0)
            segments_.Append(MakeEmptySegment());
    }
    return result;
}

template <class T>
int SegmentedDeque<T>::FindSubsequence(const Sequence<T>& subsequence) const {
    int subLength = subsequence.GetLength();
    if (subLength == 0)
        return 0;
    if (subLength > length_)
        return -1;

    DynamicArray<T> haystack(length_);
    IEnumerator<T>* it = this->GetEnumerator();
    for (int i = 0; it->MoveNext(); ++i)
        haystack.Set(i, it->GetCurrent());
    delete it;

    DynamicArray<T> needle(subLength);
    IEnumerator<T>* subIt = subsequence.GetEnumerator();
    for (int i = 0; subIt->MoveNext(); ++i)
        needle.Set(i, subIt->GetCurrent());
    delete subIt;

    for (int start = 0; start + subLength <= length_; ++start) {
        bool matched = true;
        for (int i = 0; i < subLength; ++i) {
            if (!(haystack.Get(start + i) == needle.Get(i))) {
                matched = false;
                break;
            }
        }
        if (matched)
            return start;
    }
    return -1;
}
