#include "contact.h"

#include <map>
#include <cstring>

using namespace std::literals;

std::string Contact::operator [](const std::string& key) const {
    std::map<std::string, std::string> key_value;
    key_value["name"] = name;
    key_value["surname"] = surname;
    key_value["middlename"] = middlename;
    key_value["phoneNumber"] = phoneNumber;
    key_value["description"] = description;
    return key_value.at(key);
}

Contact::operator std::string() const {
    std::string result = "";
    result += "id:"s + std::to_string(id) + ",";
    result += "name:"s + name + ",";
    result += "surname:"s + surname + ",";
    result += "middlename:"s + middlename + ",";
    result += "phoneNumber:"s + phoneNumber + ",";
    result += "description:"s + description;
    return result;
}

void Contact::add(const std::string& key, const std::string& value) {
    if(key == "name") {
        strcpy(name, value.c_str());
        name[std::min(sizeof(name) - 1, value.size())] = '\0';
    } else if (key == "surname") {
        strcpy(surname, value.c_str());
        surname[std::min(sizeof(surname) - 1, value.size())] = '\0';
    } else if (key == "middlename") {
        strcpy(middlename, value.c_str());
        middlename[std::min(sizeof(middlename) - 1, value.size())] = '\0';
    } else if (key == "phoneNumber") {
        strcpy(phoneNumber, value.c_str());
        phoneNumber[std::min(sizeof(phoneNumber) - 1, value.size())] = '\0';
    } else if (key == "description") {
        strcpy(description, value.c_str());
        description[std::min(sizeof(description) - 1, value.size())] = '\0';
    }
}