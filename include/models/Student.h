#pragma once

#include <string>
#include <utility>

class Student {
private:
    std::string firstName_;
    std::string lastName_;
    std::string patronymic_;
    size_t id_;
    std::string birthDate_;

public:
    Student()
        : id_(0) {
    }

    Student(std::string firstName, std::string lastName, std::string patronymic, size_t id, std::string birthDate)
        : firstName_(std::move(firstName)),
          lastName_(std::move(lastName)),
          patronymic_(std::move(patronymic)),
          id_(id),
          birthDate_(std::move(birthDate)) {
    }

    const std::string &GetFirstName() const noexcept { return firstName_; }
    const std::string &GetLastName() const noexcept { return lastName_; }
    const std::string &GetPatronymic() const noexcept { return patronymic_; }
    size_t GetId() const noexcept { return id_; }
    const std::string &GetBirthDate() const noexcept { return birthDate_; }

    std::string GetFullName() const {
        return lastName_ + " " + firstName_ + " " + patronymic_;
    }
};
