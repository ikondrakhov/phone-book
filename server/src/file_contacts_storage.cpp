#include "file_contacts_storage.h"

FileContactsStorage::FileContactsStorage(const std::string& file_path) {
    _file.open(file_path, std::ios::binary | std::ios::out | std::ios::in | std::ios::app);
    _file.close();
    _file.open(file_path, std::ios::binary | std::ios::out | std::ios::in | std::ios::ate);
    if(!_file.good()) {
        std::cout << "File is not good after open" << std::endl;
    }
    _file.seekg(_file.beg);
    _file.peek();
    if(!_file.eof()) {
        _file.seekg(_file.beg);
        while(!_file.eof()) {
            Contact c = {};
            _file.read(reinterpret_cast<char*>(&c), sizeof(c));
            if(_file.eof()) {
                break;
            }
            if(c.id != -1) {
                _is_taken_map[c.id] = true;
                std::cout << "Read contact with id: " << c.id << std::endl;
            }
        }
    }
    _file.clear();
    _file.seekg(_file.beg);
}

std::optional<Contact> FileContactsStorage::saveContact(Contact& c) {
    std::lock_guard<std::mutex> guard(this->file_mutex);
    next_id = 0;
    while(_is_taken_map[next_id]) {
        next_id++;
    }
    c.id = next_id++;
    _file.seekp(std::ios::beg + c.id * sizeof(c));
    _file.write(reinterpret_cast<const char*>(&c), sizeof(Contact));
    _is_taken_map[c.id] = true;
    return c;
}

std::vector<Contact> FileContactsStorage::findAll() {
    std::vector<Contact> result;
    for(auto [id, is_taken]: _is_taken_map) {
        if(is_taken) {
            result.push_back(this->findContactById(id).value());
        }
    }
    return result;
}

std::optional<Contact> FileContactsStorage::deleteContactById(unsigned contact_id) {
    std::optional<Contact> c = findContactById(contact_id);
    std::lock_guard<std::mutex> guard(this->file_mutex);
    if(!c.has_value()) {
        return std::nullopt;
    }
    _file.seekp(std::ios::beg + contact_id * sizeof(Contact));
    Contact empty_contact = {};
    empty_contact.id = -1;
    _file.write(reinterpret_cast<const char*>(&empty_contact), sizeof(Contact));
    _is_taken_map[contact_id] = false;
    return c;
}
    
std::vector<Contact> FileContactsStorage::findContactBy(const std::string& property_name, const std::string& property_value) {
    std::vector<Contact> result;
    std::lock_guard<std::mutex> guard(this->file_mutex);
    for(auto [id, is_taken]: _is_taken_map) {
        if(is_taken) {
            Contact c;
            _file.seekg(id * sizeof(Contact));
            _file.read(reinterpret_cast<char*>(&c), sizeof(c));
            if(isMatch(c, property_name, property_value)) {
                result.push_back(c);
                return result;
            }
        }
    }
    return result;
}

std::optional<Contact> FileContactsStorage::findContactById(unsigned contact_id) {
    Contact c = {};
    std::lock_guard<std::mutex> guard(this->file_mutex);
    if(!_is_taken_map[contact_id]) {
        return std::nullopt;
    }
    _file.seekg(_file.beg + contact_id * sizeof(Contact));
    _file.read(reinterpret_cast<char*>(&c), sizeof(Contact));
    return c;
}

FileContactsStorage::~FileContactsStorage() {
    _file.close();
}

bool FileContactsStorage::isMatch(const Contact& c, const std::string& property_name, const std::string& property_value) const {
    if(property_name == "description") {
        return c[property_name].find(property_value) != std::string::npos;
    }
    return c[property_name] == property_value;
}