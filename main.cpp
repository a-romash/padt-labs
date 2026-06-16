#include <iostream>
#include <stdexcept>
#include <limits>
#include <string>
#include "include/collections/SegmentedDeque.h"
#include "include/collections/SegmentedDequeIO.h"
#include "include/models/Student.h"
#include "include/models/Teacher.h"


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


std::ostream& operator<<(std::ostream& os, const Student& student) {
    return os << student.GetFullName() << " (id=" << student.GetId() << ")";
}


std::ostream& operator<<(std::ostream& os, const Teacher& teacher) {
    return os << teacher.GetFullName() << " (id=" << teacher.GetId() << ")";
}


bool operator==(const Student& lhs, const Student& rhs) {
    return lhs.GetId() == rhs.GetId();
}


bool operator==(const Teacher& lhs, const Teacher& rhs) {
    return lhs.GetId() == rhs.GetId();
}


template <class T>
T ReadElement(const std::string& prompt);

template <>
int ReadElement<int>(const std::string& prompt) {
    return readInt(prompt);
}

template <>
double ReadElement<double>(const std::string& prompt) {
    return readDouble(prompt);
}

template <>
std::string ReadElement<std::string>(const std::string& prompt) {
    return readString(prompt);
}

template <>
char ReadElement<char>(const std::string& prompt) {
    return readChar(prompt);
}

template <>
Student ReadElement<Student>(const std::string& prompt) {
    std::cout << prompt << "\n";

    std::string firstName = readString("  First name:  ");
    std::string lastName = readString("  Last name:   ");
    std::string patronymic = readString("  Patronymic:  ");
    int id = readNonNegative("  Id:          ");
    std::string birthDate = readString("  Birth date:  ");

    return Student(firstName, lastName, patronymic, static_cast<size_t>(id), birthDate);
}

template <>
Teacher ReadElement<Teacher>(const std::string& prompt) {
    std::cout << prompt << "\n";

    std::string firstName = readString("  First name:  ");
    std::string lastName = readString("  Last name:   ");
    std::string patronymic = readString("  Patronymic:  ");
    int id = readNonNegative("  Id:          ");
    std::string birthDate = readString("  Birth date:  ");

    return Teacher(firstName, lastName, patronymic, static_cast<size_t>(id), birthDate);
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


template <class T>
SegmentedDeque<T>* CreateDeque() {
    int segmentLength = 0;

    while (segmentLength < 2) {
        segmentLength = readInt("Enter segment length (>= 2): ");

        if (segmentLength < 2) {
            std::cout << " Segment length must be at least 2.\n";
        }
    }

    int n = -1;

    while (n < 0) {
        n = readInt("Enter number of elements: ");

        if (n < 0) {
            std::cout << " Number of elements cannot be negative.\n";
        }
    }

    SegmentedDeque<T>* deque = new SegmentedDeque<T>(segmentLength);

    try {
        for (int i = 0; i < n; ++i) {
            deque->Append(ReadElement<T>("  [" + std::to_string(i) + "]: "));
        }

        return deque;
    }
    catch (...) {
        delete deque;
        throw;
    }
}


template <class T>
void DequeMenu(SegmentedDeque<T>& deque) {
    int cmd = -1;

    while (cmd != 0) {
        std::cout << "\n=== SegmentedDeque Operations ===" << std::endl;
        std::cout << "Current: " << deque << std::endl;

        std::cout << "1.  Get element by index\n";
        std::cout << "2.  Append (push back)\n";
        std::cout << "3.  Prepend (push front)\n";
        std::cout << "4.  PopFirst (pop front)\n";
        std::cout << "5.  PopLast (pop back)\n";
        std::cout << "6.  InsertAt\n";
        std::cout << "7.  GetSubsequence\n";
        std::cout << "8.  Concat with new deque\n";
        std::cout << "9.  GetFirst / GetLast\n";
        std::cout << "10. GetLength\n";
        std::cout << "11. FindSubsequence\n";
        std::cout << "0.  Back\n";

        cmd = readIntRange("Choice (0-11): ", 0, 11);

        try {
            if (cmd == 1) {
                int i = readInt("Index: ");
                std::cout << "Value: " << deque.Get(i) << std::endl;
            }
            else if (cmd == 2) {
                deque.Append(ReadElement<T>("Value: "));
                std::cout << "Done." << std::endl;
            }
            else if (cmd == 3) {
                deque.Prepend(ReadElement<T>("Value: "));
                std::cout << "Done." << std::endl;
            }
            else if (cmd == 4) {
                std::cout << "Popped: " << deque.PopFirst() << std::endl;
            }
            else if (cmd == 5) {
                std::cout << "Popped: " << deque.PopLast() << std::endl;
            }
            else if (cmd == 6) {
                T value = ReadElement<T>("Value: ");
                int i = readInt("Index: ");

                deque.InsertAt(value, i);
                std::cout << "Done." << std::endl;
            }
            else if (cmd == 7) {
                int s = readInt("Start index: ");
                int e = readInt("End index:   ");

                Sequence<T>* sub = nullptr;

                try {
                    sub = deque.GetSubsequence(s, e);

                    std::cout << "Subsequence: ";
                    PrintContents<T>(sub);

                    delete sub;
                }
                catch (...) {
                    delete sub;
                    throw;
                }
            }
            else if (cmd == 8) {
                SegmentedDeque<T>* other = nullptr;
                Sequence<T>* result = nullptr;

                try {
                    other = CreateDeque<T>();
                    result = deque.Concat(*other);

                    std::cout << "Result: ";
                    PrintContents<T>(result);

                    delete other;
                    delete result;
                }
                catch (...) {
                    delete other;
                    delete result;
                    throw;
                }
            }
            else if (cmd == 9) {
                std::cout << "First: " << deque.GetFirst() << std::endl;
                std::cout << "Last:  " << deque.GetLast() << std::endl;
            }
            else if (cmd == 10) {
                std::cout << "Length: " << deque.GetLength() << std::endl;
            }
            else if (cmd == 11) {
                SegmentedDeque<T>* pattern = nullptr;

                try {
                    std::cout << "Enter subsequence to search for:" << std::endl;
                    pattern = CreateDeque<T>();

                    int index = deque.FindSubsequence(*pattern);

                    if (index >= 0) {
                        std::cout << "Found at index: " << index << std::endl;
                    }
                    else {
                        std::cout << "Not found." << std::endl;
                    }

                    delete pattern;
                }
                catch (...) {
                    delete pattern;
                    throw;
                }
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
}


template <class T>
void RunDeque() {
    SegmentedDeque<T>* deque = nullptr;

    try {
        deque = CreateDeque<T>();
        DequeMenu(*deque);
        delete deque;
    }
    catch (...) {
        delete deque;
        throw;
    }
}


int main() {
    SegmentedDeque<int> d(5);
    for (int i = 0; i < 4; i++) d.Append(i);

    std::cout << d.PopFirst() << std::endl;
    std::cout << d.PopFirst() << std::endl;

    std::cout << d << std::endl;


    std::cout << "==== Lab 3 ====" << std::endl;

    int cmd = -1;

    while (cmd != 0) {
        std::cout << "\n=== Main Menu (element type) ===" << std::endl;
        std::cout << "1. SegmentedDeque<int>\n";
        std::cout << "2. SegmentedDeque<double>\n";
        std::cout << "3. SegmentedDeque<string>\n";
        std::cout << "4. SegmentedDeque<char>\n";
        std::cout << "5. SegmentedDeque<Student>\n";
        std::cout << "6. SegmentedDeque<Teacher>\n";
        std::cout << "0. Exit\n";

        cmd = readIntRange("Choice (0-6): ", 0, 6);

        try {
            switch (cmd) {
            case 1:
                RunDeque<int>();
                break;
            case 2:
                RunDeque<double>();
                break;
            case 3:
                RunDeque<std::string>();
                break;
            case 4:
                RunDeque<char>();
                break;
            case 5:
                RunDeque<Student>();
                break;
            case 6:
                RunDeque<Teacher>();
                break;
            }
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }

    return 0;
}
