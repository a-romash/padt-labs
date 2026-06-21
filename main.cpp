#include <iostream>
#include <stdexcept>
#include <limits>
#include <string>
#include <memory>
#include "include/collections/ArraySequence.h"
#include "include/collections/lazysequence/LazySequence.h"
#include "include/collections/lazysequence/Streams.h"
#include "include/collections/lazysequence/StreamSort.h"

int readInt(const std::string& prompt) {
    int val;

    while (true) {
        std::cout << prompt;

        if (std::cin >> val) {
            return val;
        }

        std::cout << " Invalid input. Please enter an integer.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}


int readIntRange(const std::string& prompt, int a, int b) {
    while (true) {
        int val = readInt(prompt);

        if (val >= a && val <= b) {
            return val;
        }

        std::cout << " Value must be between " << a << " and " << b << ".\n";
    }
}


int readNonNegative(const std::string& prompt) {
    while (true) {
        int val = readInt(prompt);

        if (val >= 0) {
            return val;
        }

        std::cout << " Value cannot be negative.\n";
    }
}


double readDouble(const std::string& prompt) {
    double val;

    while (true) {
        std::cout << prompt;

        if (std::cin >> val) {
            return val;
        }

        std::cout << " Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}


std::string readString(const std::string& prompt) {
    std::cout << prompt;

    std::string val;
    std::cin >> val;
    return val;
}


char readChar(const std::string& prompt) {
    std::cout << prompt;

    char val;
    std::cin >> val;
    return val;
}


template <class T>
void PrintContents(const Sequence<T>* seq) {
    if (seq == nullptr) {
        std::cout << "null" << std::endl;
        return;
    }

    IEnumerator<T>* en = nullptr;

    try {
        std::cout << "[";

        en = seq->GetEnumerator();
        bool first = true;

        while (en->MoveNext()) {
            if (!first) {
                std::cout << ", ";
            }

            std::cout << en->GetCurrent();
            first = false;
        }

        std::cout << "]" << std::endl;

        delete en;
    }
    catch (...) {
        delete en;
        throw;
    }
}


// Печатает первые n элементов ленивой последовательности.
// Для бесконечной (или более длинной, чем n) дописывает "...".
void PrintLazyFirst(const LazySequence<int>& seq, std::size_t n) {
    Ordinal length = seq.GetLength();
    std::size_t limit = n;
    if (length.IsFinite() && length.FinitePart() < n) {
        limit = length.FinitePart();
    }

    std::cout << "[";
    for (std::size_t i = 0; i < limit; ++i) {
        if (i != 0) std::cout << ", ";
        std::cout << seq.Get(Ordinal::Finite(i));
    }
    if (length.IsInfinite() || length.FinitePart() > limit) {
        std::cout << (limit > 0 ? ", ..." : "...");
    }
    std::cout << "]  (length = " << length.ToString()
              << ", materialized = " << seq.GetMaterializedCount() << ")" << std::endl;
}


MutableArraySequence<int>* ReadIntSequence() {
    int n = readNonNegative("  Number of elements: ");
    MutableArraySequence<int>* seq = new MutableArraySequence<int>();

    try {
        for (int i = 0; i < n; ++i) {
            seq->Append(readInt("   [" + std::to_string(i) + "]: "));
        }
        return seq;
    }
    catch (...) {
        delete seq;
        throw;
    }
}


std::unique_ptr<LazySequence<int>> ReadFiniteLazy() {
    MutableArraySequence<int>* seq = ReadIntSequence();
    std::unique_ptr<LazySequence<int>> result(new LazySequence<int>(*seq));
    delete seq;
    return result;
}


std::unique_ptr<LazySequence<int>> ChooseInfinite() {
    std::cout << "  1. Natural numbers (i)\n";
    std::cout << "  2. Squares (i*i)\n";
    std::cout << "  3. Powers of two\n";
    std::cout << "  4. Fibonacci (recurrence)\n";

    int c = readIntRange("  Choice (1-4): ", 1, 4);

    if (c == 1) {
        return LazySequence<int>::Infinite([](std::size_t i) { return static_cast<int>(i); });
    }
    if (c == 2) {
        return LazySequence<int>::Infinite([](std::size_t i) { return static_cast<int>(i * i); });
    }
    if (c == 3) {
        return LazySequence<int>::Infinite([](std::size_t i) {
            int r = 1;
            for (std::size_t k = 0; k < i; ++k) r *= 2;
            return r;
        });
    }

    MutableArraySequence<int> seed;
    seed.Append(0);
    seed.Append(1);
    return std::unique_ptr<LazySequence<int>>(new LazySequence<int>(
        std::function<int(Sequence<int>*)>([](Sequence<int>* s) {
            int n = s->GetLength();
            return s->Get(n - 1) + s->Get(n - 2);
        }),
        &seed, Ordinal::Omega()));
}


void DemoStreamSort() {
    int arr[] = {5, 2, 9, 1, 5, 6, 3};
    MutableArraySequence<int> src(arr, 7);
    std::cout << "Source:      ";
    PrintContents<int>(&src);

    {
        SequenceReadStream<int> in(src);
        MutableArraySequence<int> dst;
        SequenceWriteStream<int> out(dst);
        SortStream<int>(in, out);
        std::cout << "Sorted asc:  ";
        PrintContents<int>(&dst);
    }
    {
        SequenceReadStream<int> in(src);
        MutableArraySequence<int> dst;
        SequenceWriteStream<int> out(dst);
        SortStream<int>(in, out, [](const int& a, const int& b) { return a > b; });
        std::cout << "Sorted desc: ";
        PrintContents<int>(&dst);
    }
}


void RunLazyDemos() {
    std::cout << "\n----- Finite lazy sequence (from array {10,20,30,40}) -----\n";
    int arr[] = {10, 20, 30, 40};
    LazySequence<int> finite(arr, 4);
    std::cout << "Sequence: ";
    PrintLazyFirst(finite, 12);
    std::cout << "GetFirst = " << finite.GetFirst()
              << ", GetLast = " << finite.GetLast() << std::endl;

    std::cout << "\n----- Infinite: natural numbers -----\n";
    std::unique_ptr<LazySequence<int>> nats =
        LazySequence<int>::Infinite([](std::size_t i) { return static_cast<int>(i); });
    PrintLazyFirst(*nats, 10);

    std::cout << "\n----- Infinite: squares -> Map(x+1) -> Where(even) (lazy chain) -----\n";
    std::unique_ptr<LazySequence<int>> squares =
        LazySequence<int>::Infinite([](std::size_t i) { return static_cast<int>(i * i); });
    std::cout << "squares:     ";
    PrintLazyFirst(*squares, 10);
    std::unique_ptr<LazySequence<int>> mapped = squares->Map<int>([](int x) { return x + 1; });
    std::cout << "Map(+1):     ";
    PrintLazyFirst(*mapped, 10);
    std::unique_ptr<LazySequence<int>> evens = mapped->Where([](int x) { return x % 2 == 0; });
    std::cout << "Where(even): ";
    PrintLazyFirst(*evens, 10);

    std::cout << "\n----- Infinite: Fibonacci via recurrence -----\n";
    MutableArraySequence<int> seed;
    seed.Append(0);
    seed.Append(1);
    LazySequence<int> fib(
        std::function<int(Sequence<int>*)>([](Sequence<int>* s) {
            int n = s->GetLength();
            return s->Get(n - 1) + s->Get(n - 2);
        }),
        &seed, Ordinal::Omega());
    PrintLazyFirst(fib, 12);

    std::cout << "\n----- Operations: Append / Prepend / Concat / GetSubsequence -----\n";
    int a[] = {1, 2, 3};
    int b[] = {7, 8, 9};
    LazySequence<int> la(a, 3);
    LazySequence<int> lb(b, 3);
    std::cout << "la = "; PrintLazyFirst(la, 12);
    std::cout << "lb = "; PrintLazyFirst(lb, 12);
    std::unique_ptr<LazySequence<int>> appended = la.Append(100);
    std::cout << "la.Append(100):       "; PrintLazyFirst(*appended, 12);
    std::unique_ptr<LazySequence<int>> prepended = la.Prepend(0);
    std::cout << "la.Prepend(0):        "; PrintLazyFirst(*prepended, 12);
    std::unique_ptr<LazySequence<int>> concat = la.Concat(lb);
    std::cout << "la.Concat(lb):        "; PrintLazyFirst(*concat, 12);
    std::unique_ptr<LazySequence<int>> sub = concat->GetSubsequence(std::size_t(1), std::size_t(4));
    std::cout << "concat[1..4]:         "; PrintLazyFirst(*sub, 12);

    std::cout << "\n----- Task 10.2: sorting a data stream with a binary heap -----\n";
    DemoStreamSort();
}


void LazyMenu() {
    int defaults[] = {1, 2, 3, 4, 5};
    std::unique_ptr<LazySequence<int>> current(new LazySequence<int>(defaults, 5));

    int cmd = -1;
    while (cmd != 0) {
        std::cout << "\n=== LazySequence<int> Operations ===\n";
        std::cout << "Current: ";
        PrintLazyFirst(*current, 12);

        std::cout << "1.  Append element\n";
        std::cout << "2.  Prepend element\n";
        std::cout << "3.  InsertAt element\n";
        std::cout << "4.  Concat with another finite sequence\n";
        std::cout << "5.  Append another finite sequence\n";
        std::cout << "6.  Map (presets)\n";
        std::cout << "7.  Where / filter (presets)\n";
        std::cout << "8.  GetSubsequence [start, end]\n";
        std::cout << "9.  Get element by index\n";
        std::cout << "10. Reduce (sum / product of first N)\n";
        std::cout << "11. Take first N -> materialize to Sequence\n";
        std::cout << "12. Replace current (finite input)\n";
        std::cout << "13. Replace current (infinite preset)\n";
        std::cout << "0.  Back\n";

        cmd = readIntRange("Choice (0-13): ", 0, 13);

        try {
            if (cmd == 1) {
                current = current->Append(readInt("Value: "));
            }
            else if (cmd == 2) {
                current = current->Prepend(readInt("Value: "));
            }
            else if (cmd == 3) {
                int v = readInt("Value: ");
                int idx = readNonNegative("Index: ");
                current = current->InsertAt(v, static_cast<std::size_t>(idx));
            }
            else if (cmd == 4) {
                std::unique_ptr<LazySequence<int>> other = ReadFiniteLazy();
                current = current->Concat(*other);
            }
            else if (cmd == 5) {
                std::unique_ptr<LazySequence<int>> other = ReadFiniteLazy();
                current = current->Append(*other);
            }
            else if (cmd == 6) {
                std::cout << "  1. x * 2\n  2. x + 1\n  3. x * x\n  4. -x\n";
                int m = readIntRange("  Choice (1-4): ", 1, 4);
                std::function<int(int)> f;
                if (m == 1) f = [](int x) { return x * 2; };
                else if (m == 2) f = [](int x) { return x + 1; };
                else if (m == 3) f = [](int x) { return x * x; };
                else f = [](int x) { return -x; };
                current = current->Map<int>(f);
            }
            else if (cmd == 7) {
                std::cout << "  1. even\n  2. odd\n  3. > threshold\n";
                int w = readIntRange("  Choice (1-3): ", 1, 3);
                std::function<bool(int)> p;
                if (w == 1) p = [](int x) { return x % 2 == 0; };
                else if (w == 2) p = [](int x) { return x % 2 != 0; };
                else {
                    int t = readInt("  threshold: ");
                    p = [t](int x) { return x > t; };
                }
                current = current->Where(p);
            }
            else if (cmd == 8) {
                int s = readNonNegative("Start index: ");
                int e = readNonNegative("End index:   ");
                current = current->GetSubsequence(static_cast<std::size_t>(s),
                                                  static_cast<std::size_t>(e));
            }
            else if (cmd == 9) {
                int idx = readNonNegative("Index: ");
                std::cout << "Value: "
                          << current->Get(Ordinal::Finite(static_cast<std::size_t>(idx)))
                          << std::endl;
            }
            else if (cmd == 10) {
                int n = readNonNegative("How many first elements: ");
                long long sum = current->ReduceFirstN<long long>(
                    static_cast<std::size_t>(n), 0LL,
                    [](long long acc, int x) { return acc + x; });
                long long product = current->ReduceFirstN<long long>(
                    static_cast<std::size_t>(n), 1LL,
                    [](long long acc, int x) { return acc * x; });
                std::cout << "Sum     = " << sum << std::endl;
                std::cout << "Product = " << product << std::endl;
            }
            else if (cmd == 11) {
                int n = readNonNegative("N: ");
                std::unique_ptr<Sequence<int>> taken = current->Take(static_cast<std::size_t>(n));
                std::cout << "Taken: ";
                PrintContents<int>(taken.get());
            }
            else if (cmd == 12) {
                current = ReadFiniteLazy();
            }
            else if (cmd == 13) {
                current = ChooseInfinite();
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
}


void StreamMenu() {
    int cmd = -1;
    while (cmd != 0) {
        std::cout << "\n=== Stream Operations (task 10.2: heap sort) ===\n";
        std::cout << "1. Sort entered numbers ascending (binary heap)\n";
        std::cout << "2. Sort entered numbers descending (binary heap)\n";
        std::cout << "3. Sort numbers from a text line (string stream)\n";
        std::cout << "4. Read a LazySequence through a stream\n";
        std::cout << "0. Back\n";

        cmd = readIntRange("Choice (0-4): ", 0, 4);

        try {
            if (cmd == 1 || cmd == 2) {
                MutableArraySequence<int>* src = ReadIntSequence();
                try {
                    MutableArraySequence<int> dst;
                    SequenceReadStream<int> in(*src);
                    SequenceWriteStream<int> out(dst);
                    if (cmd == 1) {
                        SortStream<int>(in, out);
                    }
                    else {
                        SortStream<int>(in, out, [](const int& a, const int& b) { return a > b; });
                    }
                    std::cout << "Result: ";
                    PrintContents<int>(&dst);
                    delete src;
                }
                catch (...) {
                    delete src;
                    throw;
                }
            }
            else if (cmd == 3) {
                std::cout << "Enter integers separated by spaces, then press Enter:\n> ";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::string line;
                std::getline(std::cin, line);

                StringReadStream<int> in(line, [](const std::string& s) { return std::stoi(s); });
                MutableArraySequence<int> dst;
                SequenceWriteStream<int> out(dst);
                SortStream<int>(in, out);
                std::cout << "Sorted: ";
                PrintContents<int>(&dst);
            }
            else if (cmd == 4) {
                std::unique_ptr<LazySequence<int>> seq = ReadFiniteLazy();
                LazySequenceReadStream<int> in(*seq);
                in.Open();
                std::cout << "Stream contents: [";
                bool first = true;
                while (!in.IsEndOfStream()) {
                    if (!first) std::cout << ", ";
                    std::cout << in.Read();
                    first = false;
                }
                std::cout << "]" << std::endl;
                in.Close();
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
}


int main() {
    std::cout << "==== Lab 4 ====" << std::endl;

    int cmd = -1;
    while (cmd != 0) {
        std::cout << "\n=== Main Menu ===\n";
        std::cout << "1. Run demos (finite/infinite + task 10.2)\n";
        std::cout << "2. LazySequence<int> operations\n";
        std::cout << "3. Stream operations (heap sort)\n";
        std::cout << "0. Exit\n";

        cmd = readIntRange("Choice (0-3): ", 0, 3);

        try {
            switch (cmd) {
            case 1:
                RunLazyDemos();
                break;
            case 2:
                LazyMenu();
                break;
            case 3:
                StreamMenu();
                break;
            default:
                std::cout << "Wrong choice";
                break;
            }

        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
