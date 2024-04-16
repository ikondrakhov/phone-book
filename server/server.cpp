#ifdef _WIN32
#include <windows.h>
#define MSG_CONFIRM 0
#endif
#ifdef linux
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <map>
#include <fstream>
#include <mutex>
#include <memory>
#include <filesystem>
#include <optional>
#include <cstring>

using namespace std;

class Socket {
public:
    Socket(int sock_desc): sock_desc_(sock_desc) {
    }

    Socket(Socket&& s): sock_desc_(s.sock_desc_) {
        s.sock_desc_ = -1;
    }

    void bind(struct sockaddr_in server_address) {
        server_address_ = server_address;
        if(::bind(sock_desc_, (struct sockaddr*)&server_address_, sizeof(server_address_)) == -1) {
            std::cerr << "Error binding" << std::endl;
            throw std::runtime_error("Error binding socket");
        }
    }

    void listen(int max_queue_size) {
        ::listen(sock_desc_, max_queue_size);
    }

    Socket accept() {
        return Socket(::accept(sock_desc_, nullptr, nullptr));
    }

    int recv(int* data, unsigned data_size) {
        int recived_count = ::recv(sock_desc_, reinterpret_cast<char*>(data), data_size, 0);
        return recived_count;
    }

    int recv(std::string& data, unsigned data_size) {
        int recived_total = 0;
        std::vector<char> result;
        char buffer[1024];
        while(recived_total < data_size) {
            int recived_count = ::recv(sock_desc_, buffer, data_size, 0);
            if(recived_count > 0) {
                result.insert(result.end(), buffer, buffer + recived_count);
                recived_total += recived_count;
            }
        }
        if(result.size() > 0) {
            data = result.data();
        } else {
            return -1;
        }
        return recived_total;
    }

    void send(const std::string message) {
        ::send(sock_desc_, message.data(), message.size(), MSG_CONFIRM);
    }

    void send(int message) {
        ::send(sock_desc_, reinterpret_cast<char*>(&message), sizeof(int), MSG_CONFIRM);
    }

    ~Socket() {
        if(sock_desc_ > 0) {
            #ifdef _WIN32
            closesocket(sock_desc_);
            #endif
            #ifdef linux
            close(sock_desc_);
            #endif
        }
    }

private:
    int sock_desc_;
    struct sockaddr_in server_address_;
};

enum class Command {
    ADD,
    DEL,
    FIND,
    VIEW,
    UNKNOWN
};

Command getCommand(const std::string& s_command) {
    std::cout << "Get command: " << s_command << std::endl;
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

struct Contact {
    unsigned id;
    char name[50];
    char surname[50];
    char middlename[50];
    char phoneNumber[13];
    char description[100];

    std::string operator [](const std::string& key) const {
        std::map<std::string, std::string> key_value;
        key_value["name"] = name;
        key_value["surname"] = surname;
        key_value["middlename"] = middlename;
        key_value["phoneNumber"] = phoneNumber;
        key_value["description"] = description;
        return key_value.at(key);
    }

    operator std::string() {
        std::string result = "";
        result += "id:"s + to_string(id) + ",";
        result += "name:"s + name + ",";
        result += "surname:"s + surname + ",";
        result += "middlename:"s + middlename + ",";
        result += "phoneNumber:"s + phoneNumber + ",";
        result += "description:"s + description;
        return result;
    }

    void add(const std::string& key, const std::string& value) {
        if(key == "name") {
            strcpy(name, value.c_str());
        } else if (key == "surname") {
            strcpy(surname, value.c_str());
        } else if (key == "middlename") {
            strcpy(middlename, value.c_str());
        } else if (key == "phoneNumber") {
            strcpy(phoneNumber, value.c_str());
        } else if (key == "description") {
            strcpy(description, value.c_str());
        }
    }
};

class ContactsStorage {
public:
    virtual std::optional<Contact> saveContact(Contact& c) = 0;
    virtual std::optional<Contact> deleteContactById(unsigned contact_id) = 0;
    virtual std::vector<Contact> findContactBy(const std::string& property_name, const std::string& property_value) = 0;
    virtual std::optional<Contact> findContactById(unsigned contact_id) = 0;
};

class FileContactsStorage: public ContactsStorage {
public:
    FileContactsStorage(const std::string& file_path) {
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

    virtual std::optional<Contact> saveContact(Contact& c) {
        next_id = 0;
        while(_is_taken_map[next_id]) {
            next_id++;
        }
        std::cout << "Added contact with id: " << next_id << std::endl;
        std::cout << "Contact desc: " << c.description << std::endl;
        c.id = next_id++;
        _file.seekp(std::ios::beg + c.id * sizeof(c));
        std::cout << "File position: " << _file.tellp() << std::endl;
        _file.write(reinterpret_cast<const char*>(&c), sizeof(Contact));
        if(_file.good()) {
            std::cout << "File is good" << std::endl;
        } else {
            std::cout << "File is not good" << std::endl;
        }
        _is_taken_map[c.id] = true;
        return c;
    }

    virtual std::optional<Contact> deleteContactById(unsigned contact_id) {
        std::optional<Contact> c = findContactById(contact_id);
        if(!c.has_value()) {
            return nullopt;
        }
        _file.seekp(std::ios::beg + contact_id * sizeof(Contact));
        Contact empty_contact = {};
        empty_contact.id = -1;
        _file.write(reinterpret_cast<const char*>(&empty_contact), sizeof(Contact));
        _is_taken_map[contact_id] = false;
        return c;
    }
    
    virtual std::vector<Contact> findContactBy(const std::string& property_name, const std::string& property_value) {
        std::vector<Contact> result;
        for(auto [id, is_taken]: _is_taken_map) {
            if(is_taken) {
                Contact c;
                _file.seekg(id * sizeof(Contact));
                _file.read(reinterpret_cast<char*>(&c), sizeof(c));
                if(isMatch(c, property_name, property_value)) {
                    result.push_back(c);
                }
            }
        }
        return result;
    }

    virtual std::optional<Contact> findContactById(unsigned contact_id) {
        Contact c = {};
        if(!_is_taken_map[contact_id]) {
            return nullopt;
        }
        _file.seekg(_file.beg + contact_id * sizeof(Contact));
        _file.read(reinterpret_cast<char*>(&c), sizeof(Contact));
        if(_file.good()) {
            std::cout << "Found contact by id: " << std::string(c) << std::endl;
        } else {
            std::cout << "File is not good" << std::endl;
        }
        return c;
    }

    ~FileContactsStorage() {
        _file.close();
    }

private:
    std::fstream _file;
    std::map<unsigned, bool> _is_taken_map;
    unsigned next_id = 0;
    std::mutex file_mutex;

    bool isMatch(const Contact& c, const std::string& property_name, const std::string& property_value) {
        if(property_name == "description") {
            return c[property_name].find(property_value) != std::string::npos;
        }
        return c[property_name] == property_value;
    }
};

class ConnectionProcessor {
public:
    std::string process_message(const std::string& message) {
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
        return "Ok. Success " + std::string(c.description);
    }

    std::string addContact(std::stringstream& input) {
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

    std::string deleteContact(std::stringstream& input) {
        unsigned contact_id;
        input >> contact_id;
        auto contact = storage->deleteContactById(contact_id);
        if(contact.has_value()) {
            return contact.value();
        }
        return "No contact with id " + to_string(contact_id) + " found";
    }

    std::string findContact(std::stringstream& input) {
        std::string field_name, value;
        input >> field_name >> value;
        std::cout << field_name << value;
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
    
    std::string viewContact(std::stringstream& input) {
        unsigned contact_id;
        input >> contact_id;
        std::cout << contact_id << std::endl;
        auto contact = storage->findContactById(contact_id);
        if(contact.has_value()) {
            return contact.value();
        } else {
            return "Contact with id " + to_string(contact_id) + " not found";
        }
    }

private:
    std::unique_ptr<ContactsStorage> storage = std::make_unique<FileContactsStorage>("contacts.db");
};

class Server {
public:
    Server(int port): socket_(socket(AF_INET, SOCK_STREAM, 0)) {
        struct sockaddr_in server_address_;
        server_address_.sin_family = AF_INET;
        server_address_.sin_port = htons(8080);
        server_address_.sin_addr.s_addr = INADDR_ANY;
        socket_.bind(server_address_);
    }

    void listen() {
        socket_.listen(5);
    }

    void process_connection(Socket&& s) {
        int message_size;
        while(s.recv(&message_size, 4) > 0) {
            std::string message = "";
            s.recv(message, message_size);
            std::string response = connection_processor.process_message(message);
            std::cout << response << std::endl;
            s.send(response.size());
            s.send(response);
        }
        std::cout << "Connection closed" << std::endl;
    }

    void accept() {
        Socket clientSocket = socket_.accept();
        std::cout << "New connection" << std::endl;
        thread_pool.push_back(std::thread(&Server::process_connection, this, std::move(clientSocket)));
    }

private:
    Socket socket_;
    std::vector<std::thread> thread_pool;
    ConnectionProcessor connection_processor;
};

int main() {
    #ifdef _WIN32
    WSADATA wsdata; 
    WSAStartup(0x0101,&wsdata);
    #endif 
    std::ifstream config;
    config.open("config.txt");
    std::string param;
    int port = 8080;
    config >> param;
    if(param == "PORT") {
        config >> port;
    }
    config.close();

    std::cout << "Creating server on port " << port << std::endl;
    Server server = Server(port);
    server.listen();
    while(1) {
        server.accept();
    }
    #ifdef _WIN32
    WSACleanup();
    #endif
}