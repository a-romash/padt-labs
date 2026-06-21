#pragma once

#include "Sequence.h"
#include "DynamicArray.h"
#include "DoublyLinkedList.h"
#include "../utils/IEnumerator.h"
#include <iosfwd>
#include <stdexcept>

template <class T> class SegmentedDeque;
template <class T>
std::ostream& operator<<(std::ostream& os, const SegmentedDeque<T>& deque);

template <class T>
class SegmentedDeque : public Sequence<T> {
private:
    class Segment {
    private:
        DynamicArray<T> data_;
        int head_;
        int tail_;
        int size_; // todo: исправить

        class SegmentEnumerator : public IEnumerator<T> {
        private:
            const Segment& segment_;
            int offset_;

        public:
            explicit SegmentEnumerator(const Segment& segment)
                : segment_(segment), offset_(-1) {
            }

            bool MoveNext() override {
                ++offset_;
                return offset_ < segment_.size_;
            }

            const T& GetCurrent() const override {
                if (offset_ < 0 || offset_ >= segment_.size_)
                    throw std::out_of_range("Enumerator is out of range");
                return segment_.data_.Get(segment_.head_ + offset_);
            }

            void Reset() override {
                offset_ = -1;
            }
        };

    public:
        explicit Segment(int capacity)
            : data_(capacity), head_(0), tail_(0), size_(0) {
        }

        int GetHead() const { return head_; }
        int GetTail() const { return tail_; }
        int GetSize() const { return size_; }

        bool IsEmpty() const { return size_ == 0; }
        bool IsHeadNotZero() const { return head_ != 0; }
        bool IsTailNotAtEnd() const { return tail_ != data_.GetSize() - 1; }

        void SetHead(int newHead) {
            if (newHead < 0 || newHead >= data_.GetSize())
                throw std::out_of_range("Segment::SetHead: index out of range");
            head_ = newHead;
        }

        void SetTail(int newTail) {
            if (newTail < 0 || newTail >= data_.GetSize())
                throw std::out_of_range("Segment::SetTail: index out of range");
            tail_ = newTail;
        }

        const T& Get(int offset) const { return data_.Get(head_ + offset); }
        const T& PeekFirst() const { return data_.Get(head_); }
        const T& PeekLast() const { return data_.Get(tail_); }

        bool Append(const T& value) {
            if (tail_ >= data_.GetSize())
                return false;
            if (size_ == 0) {
                data_.Set(tail_, value);
                ++size_;
                return true;
            }
            ++tail_;
            ++size_;
            data_.Set(tail_, value);
            return true;
        }

        bool Prepend(const T& value) {
            if (head_ == 0)
                return false;
            if (size_ == 0) {
                data_.Set(head_, value);
                ++size_;
                return true;
            }
            --head_;
            ++size_;
            data_.Set(head_, value);
            return true;
        }

        T PopFirst() {
            if (IsEmpty())
                throw std::out_of_range("Segment::PopFirst: segment is empty");
            T result = data_.Get(head_);
            ++head_;
            --size_;
            return result;
        }

        T PopLast() {
            if (IsEmpty())
                throw std::out_of_range("Segment::PopLast: segment is empty");
            T result = data_.Get(tail_);
            --tail_;
            --size_;
            return result;
        }

        IEnumerator<T>* GetEnumerator() const {
            return new SegmentEnumerator(*this);
        }
    };

    class SegmentedDequeIterator : public IEnumerator<T> {
    private:
        const DoublyLinkedList<Segment>& segments_;
        int segmentIndex_;
        IEnumerator<T>* segmentEnum_;

    public:
        explicit SegmentedDequeIterator(const DoublyLinkedList<Segment>& segments)
            : segments_(segments), segmentIndex_(0), segmentEnum_(nullptr) {
        }

        ~SegmentedDequeIterator() override {
            delete segmentEnum_;
        }

        bool MoveNext() override {
            if (segmentEnum_ != nullptr && segmentEnum_->MoveNext())
                return true;

            delete segmentEnum_;
            segmentEnum_ = nullptr;

            while (segmentIndex_ < segments_.GetLength()) {
                segmentEnum_ = segments_.Get(segmentIndex_++).GetEnumerator();
                if (segmentEnum_->MoveNext())
                    return true;
                delete segmentEnum_;
                segmentEnum_ = nullptr;
            }
            return false;
        }

        const T& GetCurrent() const override {
            if (segmentEnum_ == nullptr)
                throw std::out_of_range("Enumerator is out of range");
            return segmentEnum_->GetCurrent();
        }

        void Reset() override {
            delete segmentEnum_;
            segmentEnum_ = nullptr;
            segmentIndex_ = 0;
        }
    };

    DoublyLinkedList<Segment> segments_;
    int segmentLength_;
    int length_;

    Segment MakeEmptySegment() const;

public:
    explicit SegmentedDeque(int segmentLength);
    ~SegmentedDeque() override = default;

    const T& GetFirst() const override;
    const T& GetLast() const override;
    const T& Get(int index) const override;
    int GetLength() const override;

    bool IsEmpty() const;

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override;
    Sequence<T>* Append(const T& item) override;
    Sequence<T>* Prepend(const T& item) override;
    Sequence<T>* InsertAt(const T& item, int index) override;
    Sequence<T>* Concat(const Sequence<T>& other) const override;

    T operator[](int index) const override;
    Sequence<T>* operator+(const Sequence<T>& other) const override;

    IEnumerator<T>* GetEnumerator() const override {
        return new SegmentedDequeIterator(segments_);
    }

    T PopFirst();
    T PopLast();

    int FindSubsequence(const Sequence<T>& subsequence) const;

    friend std::ostream& operator<< <T>(std::ostream& os, const SegmentedDeque<T>& deque);
};

#include "details/SegmentedDeque.tpp"
