from client import Client
from client_command_processor import ClientCommandProcessor

def read_config_data():
    config = open("config.txt", "r")
    host = "localhost"
    port = 8080
    line = config.readline()
    for line in config:
        param_name = line.split()[0]
        param_value = line.split()[1]
        if param_name == "PORT":
            port = int(param_value)
        elif param_name == "HOST":
            host = param_value
        line = config.readline()
    return host, port

if __name__ == "__main__":
    host, port = read_config_data()
    print("Host {} port {}".format(host, port))
    client = Client(host, port)
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
        result = commands_map.get(command, lambda: "Invalid")()
        if(result == "Invalid"):
            print("Unknown command")
            print("Avalible commands: ADD, DELETE, FIND, VIEW, VIEWALL")