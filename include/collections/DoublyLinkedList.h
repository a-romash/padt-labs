#pragma once

#include "../utils/IEnumerator.h"

template <class T>
class DoublyLinkedList {
private:
    struct Node;
    Node* head_;
    Node* tail_;
    int length_;

    Node* GetNode(int index) const;
    void CheckIndex(int index) const;
    void Clear();

public:
    DoublyLinkedList();
    DoublyLinkedList(const T* items, int count);
    DoublyLinkedList(const DoublyLinkedList<T>& other);
    DoublyLinkedList<T>& operator=(const DoublyLinkedList<T>& other);
    ~DoublyLinkedList();

    const T& GetFirst() const;
    const T& GetLast() const;
    const T& Get(int index) const;

    T& GetFirst();
    T& GetLast();
    T& Get(int index);

    int GetLength() const;
    void Append(const T& item);
    void Prepend(const T& item);
    void InsertAt(const T& item, int index);
    void Del(int index);

    DoublyLinkedList<T> Concat(const DoublyLinkedList<T>& list) const;
    DoublyLinkedList<T> GetSubList(int startIndex, int endIndex) const;

    T operator[](int index) const;

    class Enumerator : public IEnumerator<T> {
    private:
        const Node* head_;
        const Node* current_;
        const Node* next_;

    public:
        Enumerator(const Node* head)
            : head_(head), current_(nullptr), next_(head) {
        }

        bool MoveNext() override {
            if (next_ == nullptr) {
                current_ = nullptr;
                return false;
            }

            current_ = next_;
            next_ = next_->next;

            return true;
        }

        const T& GetCurrent() const override {
            if (current_ == nullptr) {
                throw std::out_of_range("Enumerator is out of range");
            }

            return current_->data;
        }

        void Reset() override {
            current_ = nullptr;
            next_ = head_;
        }
    };

    IEnumerator<T>* GetEnumerator() const {
        return new Enumerator(head_);
    }
};

#include "details/DoublyLinkedList.tpp"
