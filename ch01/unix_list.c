/**
 * @file unix_list.c
 * @author Jose S. Daniel (djose1164@gmail.com)
 * @brief Print details about the user's network. For Unix-like systems.
 * @version 0.1
 * @date 2021-08-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    // Struct with the corcen info.
    struct ifaddrs *addresses;

    // Error handler.
    if (getifaddrs(&addresses) == -1)
    {
        fprintf(stderr, "Failed on allocate memory. File: %s Function: %s",
                __FILE__, __FUNCTION__);
        return -1;
    }

    struct ifaddrs *address = addresses;

    /**
     * @brief For each network interface in the linked list, print the information.
     * 
     */
    while (address)
    {
        if (address->ifa_addr == NULL)
        {
            address = address->ifa_next;
            continue;
        }

        int family = address->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6)
        {

            printf("%s\t", address->ifa_name);
            printf("%s\t", family == AF_INET ? "IPv4" : "IPv6");

            char ap[100];
            const int family_size = (family == AF_INET) 
                                    ? sizeof(struct sockaddr_in) 
                                    : sizeof(struct sockaddr_in6);
            getnameinfo(address->ifa_addr,
                        family_size, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
            printf("\t%s\n", ap);
        }
        address = address->ifa_next;
    }

    freeifaddrs(addresses);
}