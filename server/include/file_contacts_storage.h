#pragma once

#include "contacts_storage.h"
#include "contact.h"

#include <fstream>
#include <map>
#include <mutex>
#include <iostream>
#include <optional>

class FileContactsStorage: public ContactsStorage {
public:
    FileContactsStorage(const std::string& file_path);
    virtual std::optional<Contact> saveContact(Contact& c);
    virtual std::optional<Contact> deleteContactById(unsigned contact_id);
    virtual std::vector<Contact> findContactBy(const std::string& property_name, const std::string& property_value);
    virtual std::optional<Contact> findContactById(unsigned contact_id);
    virtual std::vector<Contact> findAll();
    ~FileContactsStorage();

private:
    std::fstream _file;
    std::map<unsigned, bool> _is_taken_map;
    unsigned next_id = 0;
    std::mutex file_mutex;

    bool isMatch(const Contact& c, const std::string& property_name, const std::string& property_value);
};