/**
 * @file tcp_client.c
 * @author Jose S. Daniel (djose11164@gmail.com)
 * @brief A tiny TCP client. You can pass a hostname and a port; and then send 
 * and receive request and response.
 * @version 0.1
 * @date 2021-08-21
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "tcp_client.h"

int main(int argc, char const *argv[])
{
    // Init for Windows.
    win_init();

    if (argc < 3)
    {
        fprintf(stderr, "usage: tcp_client hostname port\n");
    }

    printf("Configuring remote address...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;
    if (getaddrinfo(argv[1], argv[2], &hints, &peer_address))
    {
        fprintf(stderr, "getaddrinfo() failed. (%d)", GET_SOCKET_ERRNO());
        return 1;
    }

    printf("Remote address is: ");
    char address_buffer[100];
    char service_buffer[100];
    getnameinfo(peer_address->ai_addr, peer_address->ai_addrlen, address_buffer,
                sizeof(address_buffer), service_buffer, sizeof(service_buffer),
                NI_NUMERICHOST);
    printf("%s %s.\n", address_buffer, service_buffer);

    printf("Creating socket...\n");
    SOCKET socket_peer = socket(peer_address->ai_family, peer_address->ai_socktype,
                                peer_address->ai_protocol);
    if (!IS_VALID_SOCKET(socket_peer))
    {
        fprintf(stderr, "socket() failed. (%d)\n", GET_SOCKET_ERRNO());
        perror("description");
        return 1;
    }

    printf("Connection...\n");
    if (connect(socket_peer, peer_address->ai_addr, peer_address->ai_addrlen) == -1)
    {
        fprintf(stderr, "connect() failed. (%d)\n", GET_SOCKET_ERRNO());
        perror("description");
        return 1;
    }
    freeaddrinfo(peer_address);

    printf("Connected.\n"
           "To send data, enter text followed by enter.\n"
           "-> ");

    while (true)
    {
        fd_set reads;
        FD_ZERO(&reads);
        FD_SET(socket_peer, &reads);
#ifndef __WIN32
        FD_SET(fileno(stdin), &reads);
#endif //__WIN32

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        if (select(socket_peer + 1, &reads, 0, 0, &timeout) < 0)
        {
            fprintf(stderr, "select() failed. (%d)\n", GET_SOCKET_ERRNO());
            return 1;
        }

        if (FD_ISSET(socket_peer, &reads))
        {
            char read[4096];
            int bytes_received = recv(socket_peer, read, 4096, 0);
            if (bytes_received < 1)
            {
                printf("Connection closed by peer.\n");
                break;
            }
            printf("Received (%d bytes): %.*s\n"
                   "-> ",
                   bytes_received, bytes_received, read);
        }

        fflush(stdout);
#ifdef __linux__
        if (FD_ISSET(fileno(stdin), &reads))
        {
#else
        if (_kbhit())
        {
#endif // __linux__
            char read[4096];
            if (!fgets(read, sizeof(read), stdin))
                break;
            printf("Sending: %s", read);
            int bytes_sent = send(socket_peer, read, strlen(read), 0);
            printf("Sent %d bytes.\n", bytes_sent);
        }
    }

    printf("Closing socket...\n");
    CLOSE_SOCKET(socket_peer);

#ifdef __WIN32
    WSACleanup();
#endif // __WIN32

    printf("Done.\n");
    return 0;
}
