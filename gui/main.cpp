#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "include/collections/ArraySequence.h"
#include "include/collections/lazysequence/LazySequence.h"
#include "include/collections/lazysequence/Streams.h"
#include "include/collections/lazysequence/StreamSort.h"

static std::vector<int> ParseInts(const std::string& text) {
    std::vector<int> out;
    std::string token;
    std::istringstream in(text);
    while (in >> token) {
        for (char& c : token)
            if (c == ',' || c == ';') c = ' ';
        std::istringstream piece(token);
        int v;
        while (piece >> v) out.push_back(v);
    }
    return out;
}

static MutableArraySequence<int> ToSequence(const std::vector<int>& items) {
    MutableArraySequence<int> seq;
    for (int v : items) seq.Append(v);
    return seq;
}

static std::unique_ptr<LazySequence<int>> LazyFromText(const std::string& text) {
    MutableArraySequence<int> seq = ToSequence(ParseInts(text));
    return std::unique_ptr<LazySequence<int>>(new LazySequence<int>(seq));
}

static std::string LazyView(const LazySequence<int>& seq, std::size_t n) {
    Ordinal length = seq.GetLength();
    std::size_t limit = n;
    if (length.IsFinite() && length.FinitePart() < n)
        limit = length.FinitePart();

    std::string s = "[";
    for (std::size_t i = 0; i < limit; ++i) {
        if (i != 0) s += ", ";
        s += std::to_string(seq.Get(Ordinal::Finite(i)));
    }
    if (length.IsInfinite() || length.FinitePart() > limit)
        s += (limit > 0 ? ", ..." : "...");
    s += "]";
    return s;
}

template <class T>
static std::string SeqToString(const Sequence<T>* seq) {
    if (seq == nullptr || seq->GetLength() == 0) return "[]";
    std::string out = "[";
    IEnumerator<T>* en = seq->GetEnumerator();
    bool first = true;
    try {
        while (en->MoveNext()) {
            if (!first) out += ", ";
            out += std::to_string(en->GetCurrent());
            first = false;
        }
    } catch (...) {
        delete en;
        throw;
    }
    delete en;
    return out + "]";
}

class Log {
public:
    void Add(const std::string& s) {
        lines_.push_back(s);
        if (lines_.size() > 300) lines_.erase(lines_.begin());
    }
    void Clear() { lines_.clear(); }

    void Render(const char* id, float height = 200.0f) {
        if (ImGui::Button("Clear log")) Clear();
        ImGui::BeginChild(id, ImVec2(0, height), true);
        for (const std::string& line : lines_)
            ImGui::TextUnformatted(line.c_str());
        if (scrollToBottom_) {
            ImGui::SetScrollHereY(1.0f);
            scrollToBottom_ = false;
        }
        ImGui::EndChild();
    }

    template <class F>
    void Guard(F&& fn) {
        try {
            fn();
            scrollToBottom_ = true;
        } catch (const std::exception& e) {
            Add(std::string("Error: ") + e.what());
            scrollToBottom_ = true;
        }
    }

private:
    std::vector<std::string> lines_;
    bool scrollToBottom_ = false;
};

class LazyPanel {
public:
    LazyPanel() {
        int defaults[] = {1, 2, 3, 4, 5};
        current_.reset(new LazySequence<int>(defaults, 5));
    }

    void Render() {
        ImGui::TextWrapped("Current: %s", LazyView(*current_, (std::size_t)previewN_).c_str());
        ImGui::Text("length = %s   materialized = %zu",
                    current_->GetLength().ToString().c_str(),
                    current_->GetMaterializedCount());
        ImGui::SetNextItemWidth(120);
        ImGui::InputInt("preview count", &previewN_);
        if (previewN_ < 0) previewN_ = 0;
        ImGui::Separator();


        ImGui::SetNextItemWidth(160);
        ImGui::InputInt("value", &value_);
        if (ImGui::Button("Append")) {
            log_.Guard([&] { current_ = current_->Append(value_); });
        }
        ImGui::SameLine();
        if (ImGui::Button("Prepend")) {
            log_.Guard([&] { current_ = current_->Prepend(value_); });
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::InputInt("index", &index_);
        ImGui::SameLine();
        if (ImGui::Button("InsertAt")) {
            log_.Guard([&] {
                if (index_ < 0) throw std::invalid_argument("index cannot be negative");
                current_ = current_->InsertAt(value_, (std::size_t)index_);
            });
        }
        ImGui::SameLine();
        if (ImGui::Button("Get at index")) {
            log_.Guard([&] {
                if (index_ < 0) throw std::invalid_argument("index cannot be negative");
                int v = current_->Get(Ordinal::Finite((std::size_t)index_));
                log_.Add("Get[" + std::to_string(index_) + "] -> " + std::to_string(v));
            });
        }

        ImGui::Separator();

        ImGui::SetNextItemWidth(360);
        ImGui::InputText("finite operand (space-separated)", finiteBuf_, sizeof(finiteBuf_));
        if (ImGui::Button("Concat")) {
            log_.Guard([&] {
                std::unique_ptr<LazySequence<int>> other = LazyFromText(finiteBuf_);
                current_ = current_->Concat(*other);
            });
        }
        ImGui::SameLine();
        if (ImGui::Button("Append seq")) {
            log_.Guard([&] {
                std::unique_ptr<LazySequence<int>> other = LazyFromText(finiteBuf_);
                current_ = current_->Append(*other);
            });
        }
        ImGui::SameLine();
        if (ImGui::Button("Replace with finite")) {
            log_.Guard([&] { current_ = LazyFromText(finiteBuf_); });
        }

        ImGui::Separator();

        const char* mapNames[] = {"x * 2", "x + 1", "x * x", "-x"};
        ImGui::SetNextItemWidth(160);
        ImGui::Combo("map preset", &mapChoice_, mapNames, IM_ARRAYSIZE(mapNames));
        ImGui::SameLine();
        if (ImGui::Button("Map")) {
            log_.Guard([&] {
                std::function<int(int)> f;
                switch (mapChoice_) {
                    case 0: f = [](int x) { return x * 2; }; break;
                    case 1: f = [](int x) { return x + 1; }; break;
                    case 2: f = [](int x) { return x * x; }; break;
                    default: f = [](int x) { return -x; }; break;
                }
                current_ = current_->Map<int>(f);
            });
        }

        const char* whereNames[] = {"even", "odd", "> threshold"};
        ImGui::SetNextItemWidth(160);
        ImGui::Combo("filter preset", &whereChoice_, whereNames, IM_ARRAYSIZE(whereNames));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::InputInt("threshold", &threshold_);
        ImGui::SameLine();
        if (ImGui::Button("Where")) {
            log_.Guard([&] {
                std::function<bool(int)> p;
                if (whereChoice_ == 0) p = [](int x) { return x % 2 == 0; };
                else if (whereChoice_ == 1) p = [](int x) { return x % 2 != 0; };
                else { int t = threshold_; p = [t](int x) { return x > t; }; }
                current_ = current_->Where(p);
            });
        }

        ImGui::Separator();

        ImGui::SetNextItemWidth(120);
        ImGui::InputInt("sub start", &subStart_);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(120);
        ImGui::InputInt("sub end", &subEnd_);
        ImGui::SameLine();
        if (ImGui::Button("GetSubsequence")) {
            log_.Guard([&] {
                if (subStart_ < 0 || subEnd_ < 0)
                    throw std::invalid_argument("indices cannot be negative");
                current_ = current_->GetSubsequence((std::size_t)subStart_, (std::size_t)subEnd_);
            });
        }

        ImGui::SetNextItemWidth(120);
        ImGui::InputInt("N", &takeN_);
        if (takeN_ < 0) takeN_ = 0;
        ImGui::SameLine();
        if (ImGui::Button("Take first N")) {
            log_.Guard([&] {
                std::unique_ptr<Sequence<int>> taken = current_->Take((std::size_t)takeN_);
                log_.Add("Take(" + std::to_string(takeN_) + ") = " + SeqToString<int>(taken.get()));
            });
        }
        ImGui::SameLine();
        if (ImGui::Button("Reduce first N (sum/product)")) {
            log_.Guard([&] {
                long long sum = current_->ReduceFirstN<long long>(
                    (std::size_t)takeN_, 0LL, [](long long acc, int x) { return acc + x; });
                long long product = current_->ReduceFirstN<long long>(
                    (std::size_t)takeN_, 1LL, [](long long acc, int x) { return acc * x; });
                log_.Add("Reduce first " + std::to_string(takeN_) +
                         ": sum = " + std::to_string(sum) +
                         ", product = " + std::to_string(product));
            });
        }

        ImGui::Separator();

        const char* infNames[] = {"Natural (i)", "Squares (i*i)", "Powers of two", "Fibonacci"};
        ImGui::SetNextItemWidth(200);
        ImGui::Combo("infinite preset", &infChoice_, infNames, IM_ARRAYSIZE(infNames));
        ImGui::SameLine();
        if (ImGui::Button("Replace with infinite")) {
            log_.Guard([&] { current_ = MakeInfinite(infChoice_); });
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset to {1,2,3,4,5}")) {
            log_.Guard([&] {
                int defaults[] = {1, 2, 3, 4, 5};
                current_.reset(new LazySequence<int>(defaults, 5));
            });
        }

        ImGui::Separator();
        log_.Render("lazylog");
    }

private:
    static std::unique_ptr<LazySequence<int>> MakeInfinite(int choice) {
        if (choice == 0)
            return LazySequence<int>::Infinite([](std::size_t i) { return (int)i; });
        if (choice == 1)
            return LazySequence<int>::Infinite([](std::size_t i) { return (int)(i * i); });
        if (choice == 2)
            return LazySequence<int>::Infinite([](std::size_t i) {
                int r = 1;
                for (std::size_t k = 0; k < i; ++k) r *= 2;
                return r;
            });
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

    std::unique_ptr<LazySequence<int>> current_;
    int previewN_ = 12;
    int value_ = 0;
    int index_ = 0;
    int subStart_ = 0;
    int subEnd_ = 0;
    int takeN_ = 5;
    int threshold_ = 0;
    int mapChoice_ = 0;
    int whereChoice_ = 0;
    int infChoice_ = 0;
    char finiteBuf_[256] = "7 8 9";
    Log log_;
};

class StreamPanel {
public:
    void Render() {
        ImGui::TextWrapped(
            "Sort a data stream through a binary heap, or read a lazy sequence "
            "through a stream.");
        ImGui::SetNextItemWidth(420);
        ImGui::InputText("numbers (space-separated)", numbersBuf_, sizeof(numbersBuf_));

        if (ImGui::Button("Sort ascending (heap)")) {
            log_.Guard([&] { SortNumbers(false); });
        }
        ImGui::SameLine();
        if (ImGui::Button("Sort descending (heap)")) {
            log_.Guard([&] { SortNumbers(true); });
        }
        ImGui::SameLine();
        if (ImGui::Button("Sort via string stream")) {
            log_.Guard([&] { SortViaStringStream(); });
        }

        if (ImGui::Button("Read sequence through a stream")) {
            log_.Guard([&] { ReadThroughStream(); });
        }

        ImGui::Separator();
        log_.Render("streamlog");
    }

private:
    void SortNumbers(bool descending) {
        MutableArraySequence<int> src = ToSequence(ParseInts(numbersBuf_));
        MutableArraySequence<int> dst;
        SequenceReadStream<int> in(src);
        SequenceWriteStream<int> out(dst);
        if (descending)
            SortStream<int>(in, out, [](const int& a, const int& b) { return a > b; });
        else
            SortStream<int>(in, out);
        log_.Add(std::string("Sorted ") + (descending ? "desc" : "asc") + ": " +
                 SeqToString<int>(&dst));
    }

    void SortViaStringStream() {
        StringReadStream<int> in(std::string(numbersBuf_),
                                 [](const std::string& s) { return std::stoi(s); });
        MutableArraySequence<int> dst;
        SequenceWriteStream<int> out(dst);
        SortStream<int>(in, out);
        log_.Add("Sorted (string stream): " + SeqToString<int>(&dst));
    }

    void ReadThroughStream() {
        std::unique_ptr<LazySequence<int>> seq = LazyFromText(numbersBuf_);
        LazySequenceReadStream<int> in(*seq);
        in.Open();
        std::string out = "[";
        bool first = true;
        while (!in.IsEndOfStream()) {
            if (!first) out += ", ";
            out += std::to_string(in.Read());
            first = false;
        }
        in.Close();
        log_.Add("Stream contents: " + out + "]");
    }

    char numbersBuf_[420] = "5 2 9 1 5 6 3";
    Log log_;
};

class DemoPanel {
public:
    void Render() {
        ImGui::TextWrapped(
            "Runs the finite/infinite lazy-sequence demos and the task 10.2 "
            "heap-sort demo, mirroring \"Run demos\" in the CLI.");
        if (ImGui::Button("Run demos")) {
            log_.Clear();
            log_.Guard([&] { RunDemos(); });
        }
        ImGui::Separator();
        log_.Render("demolog", 360.0f);
    }

private:
    void RunDemos() {
        log_.Add("----- Finite lazy sequence (from array {10,20,30,40}) -----");
        int arr[] = {10, 20, 30, 40};
        LazySequence<int> finite(arr, 4);
        log_.Add("Sequence: " + LazyView(finite, 12));
        log_.Add("GetFirst = " + std::to_string(finite.GetFirst()) +
                 ", GetLast = " + std::to_string(finite.GetLast()));

        log_.Add("");
        log_.Add("----- Infinite: natural numbers -----");
        std::unique_ptr<LazySequence<int>> nats =
            LazySequence<int>::Infinite([](std::size_t i) { return (int)i; });
        log_.Add(LazyView(*nats, 10));

        log_.Add("");
        log_.Add("----- squares -> Map(x+1) -> Where(even) (lazy chain) -----");
        std::unique_ptr<LazySequence<int>> squares =
            LazySequence<int>::Infinite([](std::size_t i) { return (int)(i * i); });
        log_.Add("squares:     " + LazyView(*squares, 10));
        std::unique_ptr<LazySequence<int>> mapped =
            squares->Map<int>([](int x) { return x + 1; });
        log_.Add("Map(+1):     " + LazyView(*mapped, 10));
        std::unique_ptr<LazySequence<int>> evens =
            mapped->Where([](int x) { return x % 2 == 0; });
        log_.Add("Where(even): " + LazyView(*evens, 10));

        log_.Add("");
        log_.Add("----- Infinite: Fibonacci via recurrence -----");
        MutableArraySequence<int> seed;
        seed.Append(0);
        seed.Append(1);
        LazySequence<int> fib(
            std::function<int(Sequence<int>*)>([](Sequence<int>* s) {
                int n = s->GetLength();
                return s->Get(n - 1) + s->Get(n - 2);
            }),
            &seed, Ordinal::Omega());
        log_.Add(LazyView(fib, 12));

        log_.Add("");
        log_.Add("----- Append / Prepend / Concat / GetSubsequence -----");
        int a[] = {1, 2, 3};
        int b[] = {7, 8, 9};
        LazySequence<int> la(a, 3);
        LazySequence<int> lb(b, 3);
        log_.Add("la = " + LazyView(la, 12));
        log_.Add("lb = " + LazyView(lb, 12));
        log_.Add("la.Append(100):  " + LazyView(*la.Append(100), 12));
        log_.Add("la.Prepend(0):   " + LazyView(*la.Prepend(0), 12));
        std::unique_ptr<LazySequence<int>> concat = la.Concat(lb);
        log_.Add("la.Concat(lb):   " + LazyView(*concat, 12));
        std::unique_ptr<LazySequence<int>> sub =
            concat->GetSubsequence(std::size_t(1), std::size_t(4));
        log_.Add("concat[1..4]:    " + LazyView(*sub, 12));

        log_.Add("");
        log_.Add("----- Task 10.2: sorting a data stream with a binary heap -----");
        int src[] = {5, 2, 9, 1, 5, 6, 3};
        MutableArraySequence<int> source(src, 7);
        log_.Add("Source:      " + SeqToString<int>(&source));
        {
            SequenceReadStream<int> in(source);
            MutableArraySequence<int> dst;
            SequenceWriteStream<int> out(dst);
            SortStream<int>(in, out);
            log_.Add("Sorted asc:  " + SeqToString<int>(&dst));
        }
        {
            SequenceReadStream<int> in(source);
            MutableArraySequence<int> dst;
            SequenceWriteStream<int> out(dst);
            SortStream<int>(in, out, [](const int& a, const int& b) { return a > b; });
            log_.Add("Sorted desc: " + SeqToString<int>(&dst));
        }
    }

    Log log_;
};

class App {
public:
    void Render() {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(900, 720), ImGuiCond_FirstUseEver);
        ImGui::Begin("Lab 4 GUI");

        if (ImGui::BeginTabBar("tabs")) {
            if (ImGui::BeginTabItem("LazySequence")) {
                lazy_.Render();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Streams (heap sort)")) {
                stream_.Render();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Demos")) {
                demo_.Render();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

private:
    LazyPanel lazy_;
    StreamPanel stream_;
    DemoPanel demo_;
};

int main() {
    sf::RenderWindow window(sf::VideoMode({940u, 760u}), "Lab 4 GUI");
    window.setFramerateLimit(60);

    if (!ImGui::SFML::Init(window))
        return 1;

    App app;
    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        ImGui::SFML::Update(window, clock.restart());
        app.Render();

        window.clear(sf::Color(30, 30, 35));
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
