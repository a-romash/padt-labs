#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"

#include <cstring>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "include/collections/SegmentedDeque.h"
#include "include/models/Student.h"
#include "include/models/Teacher.h"

static bool operator==(const Student& a, const Student& b) { return a.GetId() == b.GetId(); }
static bool operator==(const Teacher& a, const Teacher& b) { return a.GetId() == b.GetId(); }

static std::ostream& operator<<(std::ostream& os, const Student& s) {
    return os << s.GetFullName() << " (id=" << s.GetId() << ")";
}
static std::ostream& operator<<(std::ostream& os, const Teacher& t) {
    return os << t.GetFullName() << " (id=" << t.GetId() << ")";
}

#include "include/collections/SegmentedDequeIO.h"

static std::string Format(int v) { return std::to_string(v); }
static std::string Format(double v) {
    std::ostringstream os;
    os << v;
    return os.str();
}
static std::string Format(char v) { return std::string(1, v); }
static std::string Format(const std::string& v) { return v; }
static std::string Format(const Student& s) {
    return s.GetFullName() + " (id=" + std::to_string(s.GetId()) + ")";
}
static std::string Format(const Teacher& t) {
    return t.GetFullName() + " (id=" + std::to_string(t.GetId()) + ")";
}

template <class T>
static std::string SeqToString(const Sequence<T>* seq) {
    if (seq == nullptr || seq->GetLength() == 0)
        return "[]";

    std::string out = "[";
    IEnumerator<T>* en = seq->GetEnumerator();
    bool first = true;
    try {
        while (en->MoveNext()) {
            if (!first) out += ", ";
            out += Format(en->GetCurrent());
            first = false;
        }
    } catch (...) {
        delete en;
        throw;
    }
    delete en;
    return out + "]";
}

// CLI-style segmented view: "{ [_, 2, 3] [4, 5, _] }" via SegmentedDeque's
// operator<<. GetSubsequence/Concat return Sequence<T>* that are really
// SegmentedDeque<T>, so we down-cast; anything else falls back to SeqToString.
template <class T>
static std::string DequeView(const Sequence<T>* seq) {
    if (const auto* d = dynamic_cast<const SegmentedDeque<T>*>(seq)) {
        std::ostringstream os;
        os << *d;
        return os.str();
    }
    return SeqToString<T>(seq);
}

template <class T> struct ElementEditor;

template <> struct ElementEditor<int> {
    int v = 0;
    void Draw(const char* id) {
        ImGui::PushID(id);
        ImGui::InputInt("value", &v);
        ImGui::PopID();
    }
    int Build() const { return v; }
};

template <> struct ElementEditor<double> {
    double v = 0.0;
    void Draw(const char* id) {
        ImGui::PushID(id);
        ImGui::InputDouble("value", &v);
        ImGui::PopID();
    }
    double Build() const { return v; }
};

template <> struct ElementEditor<char> {
    char buf[2] = {0, 0};
    void Draw(const char* id) {
        ImGui::PushID(id);
        ImGui::InputText("value", buf, sizeof(buf));
        ImGui::PopID();
    }
    char Build() const {
        if (buf[0] == 0) throw std::invalid_argument("character is empty");
        return buf[0];
    }
};

template <> struct ElementEditor<std::string> {
    char buf[256] = {0};
    void Draw(const char* id) {
        ImGui::PushID(id);
        ImGui::InputText("value", buf, sizeof(buf));
        ImGui::PopID();
    }
    std::string Build() const { return std::string(buf); }
};

template <class Person>
struct PersonEditor {
    char first[128] = {0};
    char last[128] = {0};
    char patronymic[128] = {0};
    char birthDate[64] = {0};
    int id = 0;
    void Draw(const char* idp) {
        ImGui::PushID(idp);
        ImGui::InputText("first name", first, sizeof(first));
        ImGui::InputText("last name", last, sizeof(last));
        ImGui::InputText("patronymic", patronymic, sizeof(patronymic));
        ImGui::InputInt("id", &id);
        ImGui::InputText("birth date", birthDate, sizeof(birthDate));
        ImGui::PopID();
    }
    Person Build() const {
        if (id < 0) throw std::invalid_argument("id cannot be negative");
        return Person(first, last, patronymic, static_cast<size_t>(id), birthDate);
    }
};

template <> struct ElementEditor<Student> : PersonEditor<Student> {};
template <> struct ElementEditor<Teacher> : PersonEditor<Teacher> {};

template <class T>
class DequePanel {
public:
    ~DequePanel() {
        delete a_;
        delete b_;
    }

    void Render() {
        ImGui::InputInt("segment length", &segLen_);
        if (segLen_ < 2) segLen_ = 2;
        ImGui::SameLine();
        if (ImGui::Button(a_ == nullptr ? "Create deque" : "Reset deque")) {
            Reset();
        }

        if (a_ == nullptr) {
            ImGui::TextDisabled("Create a deque to start.");
            return;
        }

        ImGui::TextWrapped("A = %s   (length %d)", DequeView<T>(a_).c_str(), a_->GetLength());
        ImGui::Separator();

        valueEditor_.Draw("valA");
        if (ImGui::Button("Append"))  Guard([&] { a_->Append(valueEditor_.Build()); });
        ImGui::SameLine();
        if (ImGui::Button("Prepend")) Guard([&] { a_->Prepend(valueEditor_.Build()); });
        ImGui::SameLine();
        if (ImGui::Button("PopFirst")) Guard([&] { Log("PopFirst -> " + Format(a_->PopFirst())); });
        ImGui::SameLine();
        if (ImGui::Button("PopLast"))  Guard([&] { Log("PopLast -> " + Format(a_->PopLast())); });

        if (ImGui::Button("GetFirst")) Guard([&] { Log("GetFirst -> " + Format(a_->GetFirst())); });
        ImGui::SameLine();
        if (ImGui::Button("GetLast"))  Guard([&] { Log("GetLast -> " + Format(a_->GetLast())); });

        ImGui::InputInt("index", &index_);
        if (ImGui::Button("Get at index")) Guard([&] { Log("Get[" + std::to_string(index_) + "] -> " + Format(a_->Get(index_))); });
        ImGui::SameLine();
        if (ImGui::Button("InsertAt"))     Guard([&] { a_->InsertAt(valueEditor_.Build(), index_); });

        ImGui::InputInt("sub start", &subStart_);
        ImGui::SameLine();
        ImGui::InputInt("sub end", &subEnd_);
        if (ImGui::Button("GetSubsequence")) {
            Guard([&] {
                Sequence<T>* sub = a_->GetSubsequence(subStart_, subEnd_);
                Log("Subsequence[" + std::to_string(subStart_) + ".." + std::to_string(subEnd_) + "] = " + DequeView<T>(sub));
                delete sub;
            });
        }

        ImGui::Separator();
        ImGui::TextWrapped("B = %s   (length %d)", DequeView<T>(b_).c_str(), b_->GetLength());
        bEditor_.Draw("valB");
        if (ImGui::Button("Append to B")) Guard([&] { b_->Append(bEditor_.Build()); });
        ImGui::SameLine();
        if (ImGui::Button("PopLast B"))   Guard([&] { b_->PopLast(); });
        ImGui::SameLine();
        if (ImGui::Button("Clear B"))     Guard([&] { delete b_; b_ = new SegmentedDeque<T>(segLen_); });

        if (ImGui::Button("Concat A + B")) {
            Guard([&] {
                Sequence<T>* r = a_->Concat(*b_);
                Log("A concat B = " + DequeView<T>(r));
                delete r;
            });
        }
        ImGui::SameLine();
        if (ImGui::Button("Find B in A")) {
            Guard([&] {
                int idx = a_->FindSubsequence(*b_);
                Log(idx >= 0 ? "Found B at index " + std::to_string(idx) : "B not found in A");
            });
        }

        ImGui::Separator();
        if (ImGui::Button("Clear log")) log_.clear();
        ImGui::BeginChild("log", ImVec2(0, 180), true);
        for (const std::string& line : log_)
            ImGui::TextUnformatted(line.c_str());
        ImGui::EndChild();
    }

private:
    void Reset() {
        delete a_;
        delete b_;
        a_ = nullptr;
        b_ = nullptr;
        try {
            a_ = new SegmentedDeque<T>(segLen_);
            b_ = new SegmentedDeque<T>(segLen_);
            Log("Created deque with segment length " + std::to_string(segLen_));
        } catch (const std::exception& e) {
            Log(std::string("Error: ") + e.what());
        }
    }

    template <class F>
    void Guard(F&& fn) {
        try {
            fn();
        } catch (const std::exception& e) {
            Log(std::string("Error: ") + e.what());
        }
    }

    void Log(const std::string& s) {
        log_.push_back(s);
        if (log_.size() > 200) log_.erase(log_.begin());
    }

    int segLen_ = 4;
    int index_ = 0;
    int subStart_ = 0;
    int subEnd_ = 0;
    SegmentedDeque<T>* a_ = nullptr;
    SegmentedDeque<T>* b_ = nullptr;
    ElementEditor<T> valueEditor_;
    ElementEditor<T> bEditor_;
    std::vector<std::string> log_;
};

class App {
public:
    void Render() {
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(1080, 740), ImGuiCond_FirstUseEver);
        ImGui::Begin("Lab 3 GUI");

        const char* types[] = {"int", "double", "string", "char", "Student", "Teacher"};
        ImGui::Combo("element type", &type_, types, IM_ARRAYSIZE(types));
        ImGui::Separator();

        switch (type_) {
            case 0: pInt_.Render(); break;
            case 1: pDouble_.Render(); break;
            case 2: pString_.Render(); break;
            case 3: pChar_.Render(); break;
            case 4: pStudent_.Render(); break;
            case 5: pTeacher_.Render(); break;
        }

        ImGui::End();
    }

private:
    int type_ = 0;
    DequePanel<int> pInt_;
    DequePanel<double> pDouble_;
    DequePanel<std::string> pString_;
    DequePanel<char> pChar_;
    DequePanel<Student> pStudent_;
    DequePanel<Teacher> pTeacher_;
};

int main() {
    sf::RenderWindow window(sf::VideoMode({1100u, 760u}), "Lab 3 GUI");
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
