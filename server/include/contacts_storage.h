#pragma once

#include <optional>
#include <vector>

#include "contact.h"

class ContactsStorage {
public:
    virtual std::optional<Contact> saveContact(Contact& c) = 0;
    virtual std::optional<Contact> deleteContactById(unsigned contact_id) = 0;
    virtual std::vector<Contact> findContactBy(const std::string& property_name, const std::string& property_value) = 0;
    virtual std::optional<Contact> findContactById(unsigned contact_id) = 0;
};