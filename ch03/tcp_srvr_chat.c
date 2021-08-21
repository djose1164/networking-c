#include "tcp_client.h"

int main(int argc, char const *argv[])
{
    printf("Configuring socket...\n");
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *bind_address;
    getaddrinfo(0, "8080", &hints, &bind_address);

    printf("Creating socket...\n");
    SOCKET socket_listen = socket(bind_address->ai_family,
                                  bind_address->ai_socktype,
                                  bind_address->ai_protocol);
    if (!IS_VALID_SOCKET(socket_listen))
    {
        fprintf(stderr, "socket() failed. (%d)", GET_SOCKET_ERRNO());
        return 1;
    }

    if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
    {
        fprintf(stderr, "bind() failed. (%d)\n");
        return 1;
    }
    freeaddrinfo(bind_address);

    printf("Listening...\n");
    if (listen(socket_listen, 10) < 0)
    {
        fprintf(stderr, "listen() failed. (%d)", GET_SOCKET_ERRNO());
        return 1;
    }

    fd_set master;
    FD_ZERO(&master);
    FD_SET(socket_listen, &master);
    SOCKET max_socket = socket_listen;

    printf("Waiting for connection...\n");
    while (true)
    {
        fd_set reads;
        reads = master;
        if (select(max_socket + 1, &reads, 0, 0, 0) < 0)
        {
            fprintf(stderr, "select() failed. (%d)\n", GET_SOCKET_ERRNO());
            return 1;
        }

        for (SOCKET i = 1; i <= max_socket; i++)
        {
            if (FD_ISSET(i, &reads))
            {
                if (i == socket_listen)
                {
                    struct sockaddr_storage client_address;
                    socklen_t client_len = sizeof(client_address);
                    SOCKET socket_client = accept(socket_listen,
                                                  (struct sockaddr *)&client_address,
                                                  &client_len);
                    if (!IS_VALID_SOCKET(socket_client))
                    {
                        fprintf(stderr, "accept() failed. (%d)\n",
                                GET_SOCKET_ERRNO());
                        return 1;
                    }

                    FD_SET(socket_client, &master);
                    if (socket_client > max_socket)
                        max_socket = socket_client;

                    char address_buffer[1024];
                    getnameinfo((struct sockaddr *)&client_address, client_len,
                                address_buffer, sizeof(address_buffer), 0, 0,
                                NI_NUMERICHOST);
                    printf("New connection from: %s\n", address_buffer);
                }
                else
                {
                    char read[1024];
                    int bytes_received = recv(i, read, sizeof(read), 0);
                    if (bytes_received < 1)
                    {
                        FD_CLR(i, &master);
                        CLOSE_SOCKET(i);
                        continue;
                    }

                    for (SOCKET j = 1; j <= max_socket; ++j)
                    {
                        if (FD_ISSET(j, &master))
                        {
                            if (j == socket_listen || j == i)
                                continue;
                            else
                            {
                                read[strcspn(read, "\n")] = 0;
                                send(j, read, bytes_received, 0);
                            }
                        }
                    }
                }
            }
        }
    }

    printf("Closing listening socket...\n");
    CLOSE_SOCKET(socket_listen);

#ifdef __WIN32
    WSACleanup();
#endif // __WIN32
    printf("Done.\n");
    return 0;
}
