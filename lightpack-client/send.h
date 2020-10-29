//
// Created by yawkat on 7/29/18.
//

#include <glob.h>

#ifndef LIGHTPACK_CLIENT_SEND_H
#define LIGHTPACK_CLIENT_SEND_H

void lightpack_send_start(void);

void lightpack_send(const char *buf, size_t n);

#define SEND(x) lightpack_send(x, sizeof(x))

#endif //LIGHTPACK_CLIENT_SEND_H
