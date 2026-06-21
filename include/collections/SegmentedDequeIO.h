#pragma once

#include <iostream>
#include "SegmentedDeque.h"

template <class T>
std::ostream& operator<<(std::ostream& os, const SegmentedDeque<T>& deque) {
    os << "{ ";
    for (int i = 0; i < deque.segments_.GetLength(); ++i) {
        const auto& segment = deque.segments_.Get(i);
        IEnumerator<T>* segmentEnum = segment.GetEnumerator();
        int head = segment.GetHead();
        int tail = segment.GetTail();
        int size = segment.GetSize();
        int capacity = deque.segmentLength_;

        os << "[";
        for (int j = 0; j < capacity; ++j) {
            if (j > 0) os << ", ";
            if (size == 0 || j < head || j > tail) {
                os << "_";
            } else {
                segmentEnum->MoveNext();
                os << segmentEnum->GetCurrent();
            }
        }
        os << "]";
        delete segmentEnum;

        if (i < deque.segments_.GetLength() - 1)
            os << " ";
    }
    os << " }";
    return os;
}
