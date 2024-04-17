#include "connection_processor.h"
#include "contact.h"

#include <string>
#include <cstring>

Command getCommand(const std::string& s_command) {
    if(s_command == "ADD") {
        return Command::ADD;
    } else if (s_command == "DELETE") {
        return Command::DEL;
    } else if (s_command == "FIND") {
        return Command::FIND;
    } else if (s_command == "VIEW") {
        return Command::VIEW;
    }
    return Command::UNKNOWN;
}

std::string ConnectionProcessor::process_message(const std::string& message) {
    std::stringstream input(message);
    std::string command;
    Contact c = {};
    input >> command;
    switch(getCommand(command)) {
        case Command::ADD:
            return addContact(input);
            break;
        case Command::DEL:
            return deleteContact(input);
            break;
        case Command::FIND:
            return findContact(input);
            break;
        case Command::VIEW:
            return viewContact(input);
            break;
        default:
            return "Error. Unknow command";
    }
    return "Ok. Success " + std::string(c);
}

std::string ConnectionProcessor::addContact(std::stringstream& input) {
    Contact contact = {};
    std::string message;
    std::getline(input, message);

    std::string current_str = "";
    std::string key;
    for(char c: message) {
        if(c == ':') {
            key = current_str;
            current_str = "";
        } else if (c == ',') {
            contact.add(key, current_str);
            current_str = "";
        } else {
            current_str += c;
        }
    }
    if(!current_str.empty()) {
        contact.add(key, current_str);
    }

    storage->saveContact(contact);
    return contact;
}

std::string ConnectionProcessor::deleteContact(std::stringstream& input) {
    unsigned contact_id;
    input >> contact_id;
    auto contact = storage->deleteContactById(contact_id);
    if(contact.has_value()) {
        return contact.value();
    }
    return "No contact with id " + std::to_string(contact_id) + " found";
}

std::string ConnectionProcessor::findContact(std::stringstream& input) {
    std::string field_name, value;
    input >> field_name >> value;
    std::vector<Contact> result;
    try {
        result = storage->findContactBy(field_name, value);
    } catch (std::out_of_range&) {
        return "No field with name: " + field_name;
    }
    if(result.empty()) {
        return "Nothing found";
    }
    return result[0];
}
    
std::string ConnectionProcessor::viewContact(std::stringstream& input) {
    unsigned contact_id;
    input >> contact_id;
    auto contact = storage->findContactById(contact_id);
    if(contact.has_value()) {
        return contact.value();
    } else {
        return "Contact with id " + std::to_string(contact_id) + " not found";
    }
}