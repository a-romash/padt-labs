#include <stdexcept>
#include "../DoublyLinkedList.h"

template <class T>
struct DoublyLinkedList<T>::Node {
    T data;
    Node* prev;
    Node* next;
    Node(const T& value, Node* prevNode = nullptr, Node* nextNode = nullptr)
        : data(value), prev(prevNode), next(nextNode) {
    }
};

template <class T>
DoublyLinkedList<T>::DoublyLinkedList()
    : head_(nullptr), tail_(nullptr), length_(0) {
}

template <class T>
DoublyLinkedList<T>::DoublyLinkedList(const T* items, const int count)
    : head_(nullptr), tail_(nullptr), length_(0) {
    if (count < 0)
        throw std::invalid_argument("Count cannot be negative");
    if (items == nullptr && count > 0)
        throw std::invalid_argument("Items cannot be null");
    for (int i = 0; i < count; ++i)
        Append(items[i]);
}

template <class T>
DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList<T>& other)
    : head_(nullptr), tail_(nullptr), length_(0) {
    Node* current = other.head_;
    while (current != nullptr) {
        Append(current->data);
        current = current->next;
    }
}

template <class T>
DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(const DoublyLinkedList<T>& other) {
    if (this != &other) {
        Clear();
        Node* current = other.head_;
        while (current != nullptr) {
            Append(current->data);
            current = current->next;
        }
    }
    return *this;
}

template <class T>
DoublyLinkedList<T>::~DoublyLinkedList() {
    Clear();
}

template <class T>
void DoublyLinkedList<T>::CheckIndex(int index) const {
    if (index < 0 || index >= length_)
        throw std::out_of_range("Index is out of range");
}

template <class T>
typename DoublyLinkedList<T>::Node* DoublyLinkedList<T>::GetNode(int index) const {
    CheckIndex(index);
    Node* current = head_;
    for (int i = 0; i < index; ++i)
        current = current->next;
    return current;
}

template <class T>
void DoublyLinkedList<T>::Clear() {
    Node* current = head_;
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    head_ = nullptr;
    tail_ = nullptr;
    length_ = 0;
}

template <class T>
const T& DoublyLinkedList<T>::GetFirst() const {
    if (length_ == 0)
        throw std::out_of_range("List is empty");
    return head_->data;
}

template <class T>
const T& DoublyLinkedList<T>::GetLast() const {
    if (length_ == 0)
        throw std::out_of_range("List is empty");
    return tail_->data;
}

template <class T>
const T& DoublyLinkedList<T>::Get(const int index) const {
    return GetNode(index)->data;
}

template <class T>
T& DoublyLinkedList<T>::GetFirst() {
    if (length_ == 0)
        throw std::out_of_range("List is empty");
    return head_->data;
}

template <class T>
T& DoublyLinkedList<T>::GetLast() {
    if (length_ == 0)
        throw std::out_of_range("List is empty");
    return tail_->data;
}

template <class T>
T& DoublyLinkedList<T>::Get(const int index) {
    return GetNode(index)->data;
}

template <class T>
DoublyLinkedList<T> DoublyLinkedList<T>::GetSubList(const int startIndex, const int endIndex) const {
    if (startIndex < 0 || endIndex < 0 ||
        endIndex >= length_ || startIndex > endIndex)
        throw std::out_of_range("Invalid range");
    DoublyLinkedList<T> result;
    Node* current = GetNode(startIndex);
    for (int i = startIndex; i <= endIndex; ++i) {
        result.Append(current->data);
        current = current->next;
    }
    return result;
}

template <class T>
int DoublyLinkedList<T>::GetLength() const {
    return length_;
}

template <class T>
void DoublyLinkedList<T>::Append(const T& item) {
    Node* newNode = new Node(item, tail_, nullptr);
    if (length_ == 0) {
        head_ = tail_ = newNode;
    }
    else {
        tail_->next = newNode;
        tail_ = newNode;
    }
    ++length_;
}

template <class T>
void DoublyLinkedList<T>::Prepend(const T& item) {
    Node* newNode = new Node(item, nullptr, head_);
    if (length_ == 0) {
        head_ = tail_ = newNode;
    }
    else {
        head_->prev = newNode;
        head_ = newNode;
    }
    ++length_;
}

template <class T>
void DoublyLinkedList<T>::InsertAt(const T& item, int index) {
    if (index < 0 || index > length_)
        throw std::out_of_range("Index is out of range");

    if (index == 0) {
        Prepend(item);
        return;
    }
    if (index == length_) {
        Append(item);
        return;
    }

    Node* next = GetNode(index);
    Node* prev = next->prev;
    Node* newNode = new Node(item, prev, next);
    prev->next = newNode;
    next->prev = newNode;

    ++length_;
}

template <class T>
void DoublyLinkedList<T>::Del(int index) {
    Node* node = GetNode(index);

    if (node->prev != nullptr)
        node->prev->next = node->next;
    else
        head_ = node->next;

    if (node->next != nullptr)
        node->next->prev = node->prev;
    else
        tail_ = node->prev;

    delete node;
    --length_;
}

template <class T>
DoublyLinkedList<T> DoublyLinkedList<T>::Concat(const DoublyLinkedList<T>& list) const {
    DoublyLinkedList<T> result(*this);
    Node* current = list.head_;
    while (current != nullptr) {
        result.Append(current->data);
        current = current->next;
    }
    return result;
}

template <class T>
T DoublyLinkedList<T>::operator[](const int index) const {
    return Get(index);
}
