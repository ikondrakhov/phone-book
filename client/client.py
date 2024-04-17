import socket

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

    def viewAll(self):
        self.send("VIEWALL")
        return self.recv()