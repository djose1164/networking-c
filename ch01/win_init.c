#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main(int argc, char const *argv[])
{
    WSADATA d;
    if (WSAStartup(MAKEWORD(2, 2) &d))
    {
        printf("Failed to initialize.\n");
        return -1;
    }

    WSACleanup();
    printf("Ok. May not.\n")
    return 0;
}
