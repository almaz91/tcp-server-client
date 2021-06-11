#include "shared_memory_client.h"

Address::Address()
{

}

Address::Address( unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short new_port )
{
    address = ( a << 24 ) | ( b << 16 ) | ( c << 8 ) | d;
    port = new_port;
}

Address::Address( unsigned int new_address, unsigned short new_port )
{
    address = new_address;
    port = new_port;
}

unsigned int Address::GetAddress() const
{
    return address;
}

unsigned short Address::GetPort() const
{
    return port;
}

Client::Client()
{

}

Client::~Client()
{
    shutdown(client_socket, 0);    // Обрыв соединения сокета
    close(client_socket);          // Закрытие сокета
}


Client::status Client::getStatus() const
{
    return _status;
}

#ifdef _WIN32 // Windows NT
Client::status Client::open( unsigned short port )
{
    WSAStartup(MAKEWORD(2, 2), &w_data);        //Задаём версию WinSocket

    //Инициализируем наш сокет и проверяем корректно ли прошла инициализация
    //в противном случае возвращаем статус с ошибкой
    if(static_cast<int>(client_socket = socket(AF_INET, SOCK_STREAM, 0)) == SOCKET_ERROR)
    {
        return _status = status::err_socket_init;
    }

    SOCKADDR_IN address;                        //Структура хост/порт/протокол для инициализации сокета
    address.sin_addr.S_un.S_addr = INADDR_ANY;  //Любой IP адресс
    address.sin_port = htons(port);             //Задаём порт
    address.sin_family = AF_INET;               //AF_INET - Cемейство адресов для IPv4

    //Присваиваем к сокету адресс и порт и проверяем на коректность сокет
    //в противном случае возвращаем статус с ошибкой
    if(bind(client_socket, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
    {
        return _status = status::err_socket_bind;
    }

    int nonBlocking = 1;
    if ( fcntl(client_socket, F_SETFL, O_NONBLOCK, nonBlocking ) == SOCKET_ERROR)
    {
        return _status = status::err_set_non_blocking_socket;
    }
    return _status = status::up;;
}

bool Client::sendData(const Address &destination, string data, int size)
{
    SOCKADDR_IN address;                                                //Структура хост/порт/протокол для инициализации сокета
    address.sin_addr.s_addr = htonl( destination.GetAddress() );        //Задаём IP адрес
    address.sin_port = htons( destination.GetPort() );                  //Задаём порт
    address.sin_family = AF_INET;                                       //AF_INET - Cемейство адресов для IPv4

    // Соединение с сервером
    if (connect(client_socket, (const sockaddr*)&address, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        //return false;
    }

    // Передача строки на сервер
    int sent_bytes = send(client_socket, data.c_str(), size * sizeof(char), 0);

    if ( sent_bytes != size )
    {
        return false;
    }

    return true;
}

#else // *nix
Client::status Client::open( unsigned short port )
{
    // Создание сокета
    client_socket = socket(AF_INET, SOCK_STREAM, 0); // TCP

    if ( client_socket <= 0 )
    {
       return _status = status::err_socket_init;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((unsigned short)port);

    if ( bind(client_socket, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0)
    {
        return _status = status::err_socket_bind;
    }

    int nonBlocking = 1;
    if ( fcntl(client_socket, F_SETFL, O_NONBLOCK, nonBlocking ) == -1)
    {
        return _status = status::err_set_non_blocking_socket;
    }
    return _status = status::up;;
}

bool Client::sendData(const Address &destination, string data, int size)
{
    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl( destination.GetAddress() );
    dest.sin_port = htons( destination.GetPort() );

    // Соединение с сервером
    if (connect(client_socket, (const sockaddr*)&dest, sizeof(sockaddr_in)) < 0)
    {
        //return false;
    }

    // Передача строки на сервер
    int sent_bytes = send(client_socket, data.c_str(), size * sizeof(char), 0);

    if ( sent_bytes != size )
    {
        return false;
    }

    return true;
}
#endif


