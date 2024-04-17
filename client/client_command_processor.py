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
        response = self.__client.viewAll()
        for contact in str(response).split(';'):
            self.pretty_print(contact)