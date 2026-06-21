#include <utility>

#include "../StreamSort.h"

template <class T>
void SortStream(ReadOnlyStream<T>& input, WriteOnlyStream<T>& output,
                std::function<bool(const T&, const T&)> comparator) {
    BinaryHeap<T> heap(std::move(comparator));

    input.Open();
    try {
        while (true)
            heap.Push(input.Read());
    } catch (const EndOfStream&) {
        // достигнут конец потока — вся последовательность прочитана
    }
    input.Close();

    output.Open();
    while (!heap.IsEmpty())
        output.Write(heap.Pop());
    output.Close();
}

template <class T>
void SortStream(ReadOnlyStream<T>& input, WriteOnlyStream<T>& output) {
    SortStream<T>(input, output, [](const T& a, const T& b) { return a < b; });
}
