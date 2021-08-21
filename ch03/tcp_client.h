#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif // _WIN32_WINNT
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>
#pragma comment(lib, "ws2_32.lib")

#define IS_VALID_SOCKET(s) (s) != INVALID_SOCKET
#define CLOSE_SOCKET(s) closesocket(s)
#define GET_SOCKET_ERRNO(s) WSAGetLastError()

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define SOCKET int

#define IS_VALID_SOCKET(s) ((s) >= 0)
#define CLOSE_SOCKET(s) close(s)
#define GET_SOCKET_ERRNO() errno

#endif // _WIN32

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

static inline void win_init(void)
{
#ifdef __WIN32
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2) & d))
    {
        fprintf(stderr, "Failed to initialize.\n");
        return -1;
    }
#endif // __WIN32
}

#endif //TCP_CLIENT_H
