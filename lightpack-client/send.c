//
// Created by yawkat on 7/29/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include "send.h"

static struct addrinfo *target;
static int sock;

void lightpack_send_start(void) {
    if (getaddrinfo("192.168.1.147", "5000", NULL, &target) != 0) {
        exit(1);
    }
    /*
    if (getaddrinfo("192.168.1.113", "5000", NULL, &target) != 0) {
        exit(1);
    }
     */

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        exit(1);
    }
}

void lightpack_send(const char *string, size_t size) {
    sendto(sock, string, size, 0, target->ai_addr, target->ai_addrlen);
}


