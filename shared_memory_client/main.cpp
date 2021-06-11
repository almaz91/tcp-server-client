#include "shared_memory_client.h"
#include <iostream>
#include <fstream>

int main()
{
    // Создание клиента
    const int port = 55000;
    Client client;
    if (client.open(port) != Client::status::up)
    {
        std::cout << "Client start error! Error code: " << int(client.getStatus()) << std::endl;
        return -1;
    }

    // Прочитать имя передаваемого файла с конфигурационного файла
    ifstream configFile("../shared_memory_client/config.txt");
    string fileName;
    configFile >> fileName;

    ifstream file(fileName);
    if (!file.is_open())
    {
        std::cout << "No file with this name:" << fileName << std::endl;
        return -1;
    }

    // Чтение файла
    string buffer;
    string temp_string;
    while(getline(file, temp_string, '\n'))
    {
        buffer += temp_string;
        buffer += '\n';
    }
    file.close();

    for(int i = 0; i < 1; i++)
    {
        // Отправка файла
        client.sendData(Address(127, 0, 0, 1, 40000), buffer, buffer.size());
    }
    return 0;
}
