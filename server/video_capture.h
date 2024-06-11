#ifndef _VEDIO_CAPTURE_H
#define _VEDIO_CAPTURE_H

#include <stdio.h>
#include <strings.h>
//char *devpath =  "/dev/video0";
extern int camera_fd;
int camera_error(int fd, char *s);
int camera_capture(int sockfd);


#endif
