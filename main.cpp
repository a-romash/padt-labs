#include <iostream>
#include <stdexcept>
#include <limits>
#include "include/collections/ArraySequence.h"
#include "include/collections/ListSequence.h"
#include "include/collections/BitSequence.h"
#include "include/collections/AdaptiveSequence.h"
#include "include/utils/MapReduce.h"


// Безопасный ввод целого числа с повтором
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


// Ввод целого числа в диапазоне [a, b] включительно
int readIntRange(const std::string& prompt, int a, int b) {
    while (true) {
        int val = readInt(prompt);

        if (val >= a && val <= b) {
            return val;
        }

        std::cout << " Value must be between " << a << " and " << b << ".\n";
    }
}


void PrintSequence(const Sequence<int>* seq) {
    if (seq == nullptr) {
        std::cout << "null" << std::endl;
        return;
    }

    IEnumerator<int>* en = nullptr;

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


void PrintPairSequence(const Sequence<Pair<int, int>>* seq) {
    if (seq == nullptr) {
        std::cout << "null" << std::endl;
        return;
    }

    IEnumerator<Pair<int, int>>* en = nullptr;

    try {
        en = seq->GetEnumerator();

        while (en->MoveNext()) {
            const Pair<int, int>& p = en->GetCurrent();
            std::cout << "  (" << p.first << ", " << p.second << ")\n";
        }

        delete en;
    }
    catch (...) {
        delete en;
        throw;
    }
}

Sequence<int>* CreateSequence() {
    std::cout << "\n=== Select Sequence Type ===" << std::endl;
    std::cout << "1. MutableArraySequence<int>" << std::endl;
    std::cout << "2. MutableListSequence<int>" << std::endl;
    std::cout << "3. ImmutableArraySequence<int>" << std::endl;
    std::cout << "4. ImmutableListSequence<int>" << std::endl;
    std::cout << "5. AdaptiveSequence<int>" << std::endl;

    int type = readIntRange("Choice (1-5): ", 1, 5);

    int n = -1;

    while (n < 0) {
        n = readInt("Enter number of elements: ");

        if (n < 0) {
            std::cout << " Number of elements cannot be negative.\n";
        }
    }

    int* data = new int[n];
    Sequence<int>* seq = nullptr;

    try {
        for (int i = 0; i < n; ++i) {
            data[i] = readInt("  [" + std::to_string(i) + "]: ");
        }

        switch (type) {
        case 1:
            seq = new MutableArraySequence<int>(data, n);
            break;

        case 2:
            seq = new MutableListSequence<int>(data, n);
            break;

        case 3:
            seq = new ImmutableArraySequence<int>(data, n);
            break;

        case 4:
            seq = new ImmutableListSequence<int>(data, n);
            break;

        case 5:
            seq = new AdaptiveSequence<int>();

            for (int i = 0; i < n; ++i) {
                Sequence<int>* result = seq->Append(data[i]);

                if (result != seq) {
                    delete seq;
                    seq = result;
                }
            }

            break;
        }

        delete[] data;
        return seq;
    }
    catch (...) {
        delete[] data;
        delete seq;
        throw;
    }
}


void SequenceMenu(Sequence<int>*& seq) {
    int cmd = -1;

    while (cmd != 0) {
        std::cout << "\n=== Sequence Operations ===" << std::endl;
        std::cout << "Current: ";
        PrintSequence(seq);

        std::cout << "1.  Get element by index\n";
        std::cout << "2.  Append\n";
        std::cout << "3.  Prepend\n";
        std::cout << "4.  InsertAt\n";
        std::cout << "5.  GetSubsequence\n";
        std::cout << "6.  Concat with new sequence\n";
        std::cout << "7.  Map (x * 2)\n";
        std::cout << "8.  Where (even only)\n";
        std::cout << "9.  Reduce (sum)\n";
        std::cout << "10. Zip with new sequence\n";
        std::cout << "11. GetFirst / GetLast\n";
        std::cout << "12. GetLength\n";
        std::cout << "0.  Back\n";

        cmd = readIntRange("Choice (0-12): ", 0, 12);

        try {
            if (cmd == 1) {
                int i = readInt("Index: ");
                std::cout << "Value: " << seq->Get(i) << std::endl;
            }
            else if (cmd == 2) {
                int v = readInt("Value: ");

                Sequence<int>* result = seq->Append(v);

                if (result != seq) {
                    delete seq;
                    seq = result;
                }

                std::cout << "Done." << std::endl;
            }
            else if (cmd == 3) {
                int v = readInt("Value: ");

                Sequence<int>* result = seq->Prepend(v);

                if (result != seq) {
                    delete seq;
                    seq = result;
                }

                std::cout << "Done." << std::endl;
            }
            else if (cmd == 4) {
                int v = readInt("Value: ");
                int i = readInt("Index: ");

                Sequence<int>* result = seq->InsertAt(v, i);

                if (result != seq) {
                    delete seq;
                    seq = result;
                }

                std::cout << "Done." << std::endl;
            }
            else if (cmd == 5) {
                int s = readInt("Start index: ");
                int e = readInt("End index:   ");

                Sequence<int>* sub = nullptr;

                try {
                    sub = seq->GetSubsequence(s, e);

                    std::cout << "Subsequence: ";
                    PrintSequence(sub);

                    delete sub;
                }
                catch (...) {
                    delete sub;
                    throw;
                }
            }
            else if (cmd == 6) {
                Sequence<int>* other = nullptr;
                Sequence<int>* result = nullptr;

                try {
                    other = CreateSequence();
                    result = seq->Concat(*other);

                    std::cout << "Result: ";
                    PrintSequence(result);

                    delete other;
                    delete result;
                }
                catch (...) {
                    delete other;
                    delete result;
                    throw;
                }
            }
            else if (cmd == 7) {
                Sequence<int>* result = nullptr;

                try {
                    result = Map<int, int>(
                        seq,
                        [](const int& x) {
                            return x * 2;
                        },
                        []() -> Sequence<int>*{
                            return new MutableArraySequence<int>();
                        }
                    );

                    std::cout << "Map (x*2): ";
                    PrintSequence(result);

                    delete result;
                }
                catch (...) {
                    delete result;
                    throw;
                }
            }
            else if (cmd == 8) {
                Sequence<int>* result = nullptr;

                try {
                    result = Where<int>(
                        seq,
                        [](const int& x) {
                            return x % 2 == 0;
                        },
                        []() -> Sequence<int>*{
                            return new MutableArraySequence<int>();
                        }
                    );

                    std::cout << "Where (even): ";
                    PrintSequence(result);

                    delete result;
                }
                catch (...) {
                    delete result;
                    throw;
                }
            }
            else if (cmd == 9) {
                int sum = Reduce<int, int>(
                    seq,
                    [](const int& acc, const int& x) {
                        return acc + x;
                    },
                    0
                );

                std::cout << "Sum: " << sum << std::endl;
            }
            else if (cmd == 10) {
                Sequence<int>* other = nullptr;
                Sequence<Pair<int, int>>* zipped = nullptr;

                try {
                    other = CreateSequence();

                    zipped = Zip<int, int>(
                        seq,
                        other,
                        []() -> Sequence<Pair<int, int>>*{
                            return new MutableArraySequence<Pair<int, int>>();
                        }
                    );

                    std::cout << "Zip result:" << std::endl;
                    PrintPairSequence(zipped);

                    delete other;
                    delete zipped;
                }
                catch (...) {
                    delete other;
                    delete zipped;
                    throw;
                }
            }
            else if (cmd == 11) {
                std::cout << "First: " << seq->GetFirst() << std::endl;
                std::cout << "Last:  " << seq->GetLast() << std::endl;
            }
            else if (cmd == 12) {
                std::cout << "Length: " << seq->GetLength() << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
}


void BitSequenceMenu() {
    int n = -1;

    while (n <= 0) {
        n = readInt("Enter number of bits: ");

        if (n <= 0) {
            std::cout << " Must be at least 1.\n";
        }
    }

    BitSequence bs(n);

    int cmd = -1;

    while (cmd != 0) {
        std::cout << "\n=== BitSequence ===" << std::endl;
        std::cout << "Current: ";
        bs.Print();

        std::cout << "1. Set bit\n";
        std::cout << "2. Get bit\n";
        std::cout << "3. Flip bit\n";
        std::cout << "4. AND with new\n";
        std::cout << "5. OR with new\n";
        std::cout << "6. XOR with new\n";
        std::cout << "7. NOT\n";
        std::cout << "0. Back\n";

        cmd = readIntRange("Choice (0-7): ", 0, 7);

        try {
            if (cmd == 1) {
                int i = readInt("Index: ");
                int b = readIntRange("Bit (0/1): ", 0, 1);

                bs.Set(i, b);
            }
            else if (cmd == 2) {
                int i = readInt("Index: ");
                std::cout << "Bit[" << i << "] = " << bs.Get(i) << std::endl;
            }
            else if (cmd == 3) {
                int i = readInt("Index: ");
                bs.Flip(i);
            }
            else if (cmd == 4) {
                BitSequence other(n);

                std::cout << "Enter " << n << " bits:\n";

                for (int i = 0; i < n; ++i) {
                    int b = readIntRange("  [" + std::to_string(i) + "] (0/1): ", 0, 1);
                    other.Set(i, b);
                }

                BitSequence result(n);
                bs.AND(other, result);

                std::cout << "AND: ";
                result.Print();
            }
            else if (cmd == 5) {
                BitSequence other(n);

                std::cout << "Enter " << n << " bits:\n";

                for (int i = 0; i < n; ++i) {
                    int b = readIntRange("  [" + std::to_string(i) + "] (0/1): ", 0, 1);
                    other.Set(i, b);
                }

                BitSequence result(n);
                bs.OR(other, result);

                std::cout << "OR: ";
                result.Print();
            }
            else if (cmd == 6) {
                BitSequence other(n);

                std::cout << "Enter " << n << " bits:\n";

                for (int i = 0; i < n; ++i) {
                    int b = readIntRange("  [" + std::to_string(i) + "] (0/1): ", 0, 1);
                    other.Set(i, b);
                }

                BitSequence result(n);
                bs.XOR(other, result);

                std::cout << "XOR: ";
                result.Print();
            }
            else if (cmd == 7) {
                BitSequence result(n);
                bs.NOT(result);

                std::cout << "NOT: ";
                result.Print();
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
}


int main() {
    std::cout << "==== Lab 2 - Sequences ====" << std::endl;

    int cmd = -1;

    while (cmd != 0) {
        std::cout << "\n=== Main Menu ===" << std::endl;
        std::cout << "1. Work with Sequence<int>\n";
        std::cout << "2. Work with BitSequence\n";
        std::cout << "0. Exit\n";

        cmd = readIntRange("Choice (0-2): ", 0, 2);

        if (cmd == 1) {
            Sequence<int>* seq = CreateSequence();

            SequenceMenu(seq);

            delete seq;
        }
        else if (cmd == 2) {
            BitSequenceMenu();
        }
    }

    std::cout << "Bye!" << std::endl;

    return 0;
}