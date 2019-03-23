#ifndef COMMON_H
#define COMMON_H

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

int _request_done(char *buffer, int len);

int receive_msg(int skt, char *buffer, int size);

int send_msg(int skt, char *buffer, int size);

#endif