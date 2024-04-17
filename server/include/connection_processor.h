#pragma once

#include <string>
#include <memory>
#include <sstream>

#include "contacts_storage.h"
#include "file_contacts_storage.h"

enum class Command {
    ADD,
    DEL,
    FIND,
    VIEW,
    UNKNOWN
};

Command getCommand(const std::string& s_command);

class ConnectionProcessor {
public:
    std::string process_message(const std::string& message);
    std::string addContact(std::stringstream& input);
    std::string deleteContact(std::stringstream& input);
    std::string findContact(std::stringstream& input);
    std::string viewContact(std::stringstream& input);

private:
    std::unique_ptr<ContactsStorage> storage = std::make_unique<FileContactsStorage>("contacts.db");
};