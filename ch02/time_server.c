#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#endif

int main(int argc, char const *argv[])
{
#if defined(__WIN32)
    WSADATA d : if (WSAStartup(MAKEWORD(2, 2), &d))
    {
        fprintf(stderr, "Failed on allocate memory. File: %s Function: %s",
                __FILE__, __FUNCTION__);
        return 1;
    }
#endif //__WIN32
    printf("Configuring local address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);

    printf("Creating sockect...\n");
    SOCKET socket_listen;
    socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype,
                           bind_address->ai_protocol);

    if (!IS_VALID_SOCKET(socket_listen))
    {
        fprintf(stderr, "socket failed. (%d)\n", GET_SOCKET_ERRNO());
        return 1;
    }

    int option = 0;
    if(setsockopt(socket_listen, IPPROTO_IPV6, IPV6_V6ONLY, &option, sizeof(option)))
    {
        fprintf(stderr, "setsockopt() failed. (%d)", GET_SOCKET_ERRNO());
        return 1;
    }

    printf("Binding socket to local address...\n");
    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
    {
        fprintf(stderr, "bind failed. (%d)\n", GET_SOCKET_ERRNO());
        perror("bind failed");
        return 1;
    }
    freeaddrinfo(bind_address);

    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0)
    {
        fprintf(stderr, "listen failed. (%d)\n", GET_SOCKET_ERRNO());
        perror("listen failed");
        return 1;
    }

    printf("Waiting for connection...\n");
    struct sockaddr_storage client_address;
    socklen_t client_len = sizeof(client_address);
    SOCKET socket_client = accept(socket_listen,
                                  (struct sockaddr *)&client_address,
                                  &client_len);
    if (!IS_VALID_SOCKET(socket_listen))
    {
        fprintf(stderr, "accept failed. (%d)\n", GET_SOCKET_ERRNO());
        return -1;
    }

    printf("Client is connected... ");
    char address_buffer[100];
    getnameinfo((struct sockaddr *)&client_address, client_len, address_buffer,
                sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
    printf("%s\n", address_buffer);

    printf("Reading request...\n");
    char request[1024];
    int bytes_received = recv(socket_client, request, 1024, 0);
    printf("Received %d bytes.\n", bytes_received);
    //printf("##%.*s", bytes_received, request);

    printf("Sending response...\n");
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/plain\r\n\r\n"
        "Local time is: ";
    int bytes_sent = send(socket_client, response, strlen(response), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(response));

    time_t timer;
    time(&timer);
    char *time_msg = ctime(&timer);
    bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
    printf("Sent %d of %d bytes.\n", bytes_sent, (int)strlen(time_msg));

    printf("Closing connection...\n");
    CLOSE_SOCKET(socket_client);

    printf("Closing listening socket...\n");
    CLOSE_SOCKET(socket_listen);

#ifdef __WIN32
    WSACleanup();
#endif // __WIN32

    printf("Finished.\n");

    return 0;
}
