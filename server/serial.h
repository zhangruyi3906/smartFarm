#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

int server_init(int fd);
int set_serial(int fd,int nspeed,int nbits,char nevent,int nstop);
void *camera_pthread(void* arg);
int server_socket(unsigned short port,char *ip);

#define SERV_PORT 6666


#endif
