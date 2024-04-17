import socket

class Contact:
    def __init__(self, name, surname, middlename, phoneNumber, description):
        self.__name = name
        self.__surname = surname
        self.__middlename = middlename
        self.__phoneNumber = phoneNumber
        self.__description = description

    def getPhoneNumber(self):
        return self.__phoneNumber
    
    def __str__(self):
        result = ""
        values = [
            "name:" + str(self.__name),
            "surname:" + str(self.__surname),
            "middlename:" + str(self.__middlename),
            "phoneNumber:" + str(self.__phoneNumber),
            "description:" + str(self.__description)
        ]
        result = ",".join(values)
        return result
    

class Client:
    def __init__(self, host, port):
        self.__sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.__sock.connect((host, port))

    def send(self, data):
        i_num = len(data)
        b_num = i_num.to_bytes(4, 'little', signed=False)
        self.__sock.send(b_num)
        self.__sock.send(data.encode())
    
    def recv(self):
        response_size = int.from_bytes(self.__sock.recv(4), 'little')
        response = self.__sock.recv(response_size)
        return response


    def addContact(self, contact):
        self.send("ADD " + str(contact))
        return self.recv()

    def deleteContact(self, contactId):
        self.send("DELETE " + str(contactId))
        return self.recv()

    def findContact(self, propertyName, value):
        self.send("FIND " + propertyName + " " + value)
        return self.recv()

    def viewContact(self, contactId):
        self.send("VIEW " + str(contactId))
        return self.recv()


class ClientCommandProcessor:
    def __init__(self, client):
        self.__client = client
    
    def pretty_print(self, response):
        for elem in str(response).split(','):
            print(elem)

    def processAddCommand(self):
        name = input("Enter name: ")
        surname = input("Enter surname: ")
        middlename = input("Enter middlename: ")
        phoneNumber = input("Enter phone number: ")
        description = input("Enter description: ")
        response = self.__client.addContact(Contact(name, surname, middlename, phoneNumber, description))
        self.pretty_print(response)

    def processDeleteCommand(self):
        contactId = int(input("Enter contact Id: "))
        response = self.__client.deleteContact(contactId)
        self.pretty_print(response)

    def processFindCommand(self):
        propertyName = input("Enter property name: ")
        value = input("Enter property value: ")
        response = self.__client.findContact(propertyName, value)
        self.pretty_print(response)
    
    def processViewCommand(self):
        contactId = int(input("Enter contact id: "))
        response = self.__client.viewContact(contactId)
        self.pretty_print(response)

    def processViewAll(self):
        for i in range(100):
            response = self.__client.viewContact(i)
            if not "not found" in str(response):
                self.pretty_print(response)

if __name__ == "__main__":
    command = "Start"

    client = Client("localhost", 8080)
    clientCommandProcessor = ClientCommandProcessor(client)

    commands_map = {
        "ADD": clientCommandProcessor.processAddCommand,
        "DELETE": clientCommandProcessor.processDeleteCommand,
        "FIND": clientCommandProcessor.processFindCommand,
        "VIEW": clientCommandProcessor.processViewCommand,
        "VIEWALL": clientCommandProcessor.processViewAll
    }

    while True:
        command = input("Enter command: ")
        if command == "END":
            break
        result = commands_map.get(command, lambda: "Invalid")
        if(result == "Invalid"):
            print("Unknown command")
            print("Avalible commands: ADD, DELETE, FIND, VIEW, VIEWALL")