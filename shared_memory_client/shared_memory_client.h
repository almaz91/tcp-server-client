#ifndef SHARED_MEMORY_CLIENT_H
#define SHARED_MEMORY_CLIENT_H

#ifdef _WIN32 // Windows NT

#include <WinSock2.h>

#else // *nix

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#endif

#include <fcntl.h>
#include <iostream>
#include <unistd.h>

using namespace std;

class Address
{
public:
    Address();
    Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short new_port );
    Address(unsigned int new_address, unsigned short port );
    unsigned int    GetAddress() const;
    unsigned short  GetPort() const;

private:
    unsigned int address;
    unsigned short port;
};

class Client
{
public:
    // Статус клиента
   enum class status : uint8_t
   {
       up = 0,
       err_socket_init = 1,
       err_socket_bind = 2,
       err_set_non_blocking_socket = 3,
       close = 4
   };

    Client();
    Client(const Client& other);
    ~Client();
    status      open(unsigned short port);
    bool        sendData(const Address &destination, string data, int size);
    status      getStatus() const;


#ifdef _WIN32 // Windows NT
public:
    Client(SOCKET socket, SOCKADDR_IN address);
private:
    SOCKET              client_socket = INVALID_SOCKET;
    SOCKADDR_IN         address;
    WSAData             w_data;
#else // *nix
public:
    Client(int socket, struct sockaddr_in address);
private:
    int                 client_socket;
    struct sockaddr_in  address;
#endif
    status              _status = status::close;
};

#endif // SHARED_MEMORY_CLIENT_H
