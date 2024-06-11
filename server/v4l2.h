#ifndef _V4L2_H
#define _V4L2_H

#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     //Linux标准头文件，定义了各种符号常数和类型，并声明了各种函数
#include <string.h>
#include <errno.h>      //错误号头文件，包含系统中各种出错号
#include <fcntl.h>      //文件控制头文件，用于文件及其描述符的操作控制常数符号的定义
#include <sys/mman.h>   //内存管理声明
#include <sys/ioctl.h>  //设备驱动程序中对设备的I/O通道进行管理的函数

#include <linux/videodev2.h>

#define REQBUFS_COUNT   4   //缓存字节数,一般不超过5个

/*缓存区结构体类型*/
struct cam_buf{
    void *start;
    size_t length;  
};

struct  v4l2_requestbuffers reqbufs;        //向驱动申请帧缓冲请求，里面包含申请的个数
struct  cam_buf cam_bufs[REQBUFS_COUNT]; //声明摄像头缓存区大小


#define QDEBUG
#ifdef QDEBUG
    #define qDebug(fmt, ...)  fprintf(stdout,"%s--%d--%s\n",__func__, __LINE__,fmt);
#else
    #define qDebug(fmt, ...)
#endif

/*错误调试函数*/
void perr_exit(const char *s);

int camera_init(char *devpath, unsigned int *width, unsigned int *height, unsigned int *size, unsigned int *ismjpeg);

int camera_start(int camera_fd); //启动摄像头开始采集

int camera_debuf(int fd, void **buf, unsigned int *pic_size, unsigned int *index);

int camera_eqbuf(int fd, unsigned int index);

int camera_stop(int fd); //

int camera_exit(int fd);
#endif
