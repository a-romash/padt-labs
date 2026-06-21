#pragma once

#include <functional>

#include "Streams.h"
#include "../BinaryHeap.h"  // NOLINT: нужен для details/StreamSort.tpp (BinaryHeap используется там, не в этом заголовке)

// Сортировка потока данных через бинарную кучу.
// Полностью вычитывает input в кучу, затем выгружает элементы в output
// в порядке приоритета (по умолчанию — по возрастанию).
// Открывает и закрывает оба потока самостоятельно.
// Источник должен быть конечным.
template <class T>
void SortStream(ReadOnlyStream<T>& input, WriteOnlyStream<T>& output,
                std::function<bool(const T&, const T&)> comparator);

template <class T>
void SortStream(ReadOnlyStream<T>& input, WriteOnlyStream<T>& output);

#include "details/StreamSort.tpp"
