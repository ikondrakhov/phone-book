#pragma once

#include <string>

struct Contact {
    unsigned id;
    char name[50];
    char surname[50];
    char middlename[50];
    char phoneNumber[13];
    char description[100];

    std::string operator [](const std::string& key) const;
    operator std::string() const;
    void add(const std::string& key, const std::string& value);
};