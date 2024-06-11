#include "v4l2.h"

/*错误调试函数*/
void perr_exit(const char *s)
{
    perror(s);
    exit(-1);
}

/*
int main()
{
    if( -1 == open("1.txt", O_RDWR))
    {
        fprintf(stdout,"camera->start:start capture\n");
        qDebug("open");
    }
}
*/

/************************************************
 *功能：摄像头初始化
 *参数：
 *  devpath    摄像头的文件的路径  "/dev/vedio0"
 *  width      采集图像的宽度
 *  height     采集图像的高度
 *  size       采集图像的大小
 *  ismjpeg    采集图像的格式
 *返回值：
 *   成功：返回摄像头文件描述符
 *   失败：返回-1；
*************************************************/
int camera_init(char *devpath, unsigned int *width, unsigned int *height, unsigned int *size, unsigned int *ismjpeg)
{
    int i , ret;    // 循环变量 i ,返回值判断变量ret
    int camera_fd;  //摄像头描述符号

    struct v4l2_buffer 		v4l2_buffer;        // 代表驱动中的一帧
    struct v4l2_format 		v4l2_format;        // 帧格式：宽高等
    struct v4l2_capability 	v4l2_capability;    //该设备的功能：是否是支持视频输入的设备

    camera_fd = open(devpath, O_RDWR); 	        //打开摄像头设备
    if(-1 == camera_fd)
    {
        perr_exit("camera_fd open error");      //
    }

    ret =  ioctl(camera_fd, VIDIOC_QUERYCAP, &v4l2_capability);//查询当前视频支持的标准
    if(-1 == ret)
    {
        perr_exit("camera_init ioctl");
    }

    if(!(v4l2_capability.capabilities & V4L2_CAP_VIDEO_CAPTURE))     /* 是否支持图像获取*/ 
    {
        //fprintf(stderr, "camera->init: device can not support V4L2_CAP_VIDEO_CAPTURE\n"); 
        //close(fd);
        //return -1  ; 
        perr_exit("camera_init:device can`t support V4L2_CAP_VIDEO_CAPTURE");
    }

    if(!(v4l2_capability.capabilities  & V4L2_CAP_STREAMING)) {
        //fprintf(stderr, "camera->init: device can not support V4L2_CAP_STREAMING\n");
        //close(fd);
        //return -1;
        perr_exit( "camera->init: device can not support V4L2_CAP_STREAMING\n");    
    }

    memset(&v4l2_format, 0, sizeof(v4l2_format));
    v4l2_format.type                 = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_format.fmt.pix.pixelformat  = V4L2_PIX_FMT_MJPEG;
    v4l2_format.fmt.pix.width        = *width;          //640
    v4l2_format.fmt.pix.height       = *height;         //480 
    v4l2_format.fmt.pix.field        = V4L2_FIELD_ANY;

    ret = ioctl(camera_fd, VIDIOC_S_FMT, &v4l2_format); // 得出 图片格式 为yuyv 还是 jpeg
    if(ret == -1)
    {
        perror("camera init fail!\n");
    }
    else {
        fprintf(stdout, "camera->init: picture format is mjpeg\n");
        *ismjpeg = 1;
        goto get_fmt;
    }

    //已经得到摄像头采集图像的格式信息
    
    memset(&v4l2_format, 0, sizeof(v4l2_format));
    v4l2_format.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    v4l2_format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    v4l2_format.fmt.pix.width       = *width;
    v4l2_format.fmt.pix.height      = *height;
    v4l2_format.fmt.pix.field       = V4L2_FIELD_ANY;
    ret = ioctl(camera_fd, VIDIOC_S_FMT, &v4l2_format);
    if(ret == -1)
    {
        perr_exit("camera_init" );
    } 
    else{
        *ismjpeg = 0;
        fprintf(stdout, "camera->init: picture v4l2_format is yuyv\n");//转换为标准输出文件
    }

get_fmt:
    ret = ioctl(camera_fd, VIDIOC_G_FMT, &v4l2_format);
    if (-1 == ret) {
        perr_exit("camera_init");
    }
    //向摄像头申请数据帧队列
    memset(&reqbufs, 0, sizeof(struct v4l2_requestbuffers));
    reqbufs.count	= REQBUFS_COUNT;
    reqbufs.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;          //数据流类型
    reqbufs.memory	= V4L2_MEMORY_MMAP;                     //内存映射
    ret = ioctl(camera_fd, VIDIOC_REQBUFS, &reqbufs);	
    if (ret == -1)
    {	
	    close(camera_fd);
	    perr_exit(" camera init fail!\n");       
    }

    for (i = 0; i < reqbufs.count; i++)		                  //4
    {
        memset(&v4l2_buffer, 0, sizeof(struct v4l2_buffer));
        v4l2_buffer.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;//数据流类型，必须永远是V4L2_BUF_TYPE_VIDEO_CAPTURE 

        v4l2_buffer.memory      = V4L2_MEMORY_MMAP;
        v4l2_buffer.index       = i;
        /***通过调用VIDIOC_QUERYBUF命令来获取这些缓存的地址，
         ***然后使用mmap函数转换成应用程序中的绝对地址，最后把这段缓存放入缓存队列*/
        ret = ioctl(camera_fd, VIDIOC_QUERYBUF, &v4l2_buffer);
        if (ret == -1) {
            close(camera_fd);
            perr_exit("camera_init fail");
        }
        cam_bufs[i].length      = v4l2_buffer.length;
        cam_bufs[i].start       = mmap(NULL, v4l2_buffer.length, PROT_READ | PROT_WRITE,\
                                    MAP_SHARED, camera_fd, v4l2_buffer.m.offset);       //读取buf，调用mmap函数
        if (cam_bufs[i].start   == MAP_FAILED)
        {
            close(camera_fd);
            perr_exit("camera_init fail");
        }

        v4l2_buffer.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_buffer.memory      = V4L2_MEMORY_MMAP;
        ret = ioctl(camera_fd, VIDIOC_QBUF, &v4l2_buffer);
        if (ret == -1) {
            close(camera_fd);
            perr_exit("camera_init fail");
        }
    }
////////////////////
    *width  = v4l2_format.fmt.pix.width;
    *height = v4l2_format.fmt.pix.height;
    *size   = cam_bufs[0].length;
    return camera_fd;
}

int camera_start(int camera_fd) //启动摄像头开始采集
{
    int ret;
    enum v4l2_buf_type vbuf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(camera_fd, VIDIOC_STREAMON, &vbuf_type);  // 开始采集视频
    if( -1 == ret )
    {
        perr_exit("camera_start: fail");
    }
    qDebug("camera_start:开始捕获\n");
    return 0; 
}


int camera_debuf(int fd, void **buf, unsigned int *pic_size, unsigned int *index)
{
    int ret;
    fd_set fds;
    struct timeval timeout;         //设置时间
    struct v4l2_buffer v4l2_buffer; //摄像头的缓存
    //select 方式处理
    while(1)
    {
        FD_ZERO(&fds);              //
        FD_SET(fd,&fds);            //
        timeout.tv_sec = 10;         //
        timeout.tv_usec = 0;        //

        ret = select(fd + 1, &fds, NULL, NULL, &timeout);       //select 的处理
        if(-1 == ret)
        {
            perror("camera_debuf fail");
            if(errno == EINTR)
                continue;
            else
                return -1;
        }else if(0 == ret){
            fprintf(stderr,"camera_debuf: dequeue buffer timeout\n"); //  超时提醒 ？
            continue;
        }else{
            v4l2_buffer.type  = V4L2_BUF_TYPE_VIDEO_CAPTURE;    // 设置数据流
            v4l2_buffer.memory = V4L2_MEMORY_MMAP ;             //申请的buffer 个数
            ret = ioctl(fd, VIDIOC_DQBUF, &v4l2_buffer);        //把数据放回返存队列
            if(-1 == ret)
            {
                perr_exit("camera_debuf: faile");
            }
            *buf = cam_bufs[v4l2_buffer.index].start;           //调用v4l2_buffer 中的内容
            *pic_size = v4l2_buffer.bytesused;   
            *index = v4l2_buffer.index;
            return 0;
        }
    }
}

int  camera_eqbuf(int fd, unsigned int index)
{
    int  ret ;
    struct v4l2_buffer v4l2_buffer;                 //定v4l2_buffer 结构体
    v4l2_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // 设置数据流
    v4l2_buffer.memory = V4L2_MEMORY_MMAP;          //得到一帧数据
    v4l2_buffer.index = index;
    ret = ioctl(fd, VIDIOC_QBUF, &v4l2_buffer);     //入队，放入缓存区
    if( -1 == ret)
    {
        perr_exit("camera_eqbuf: fail");
    }
    return 0;
}
/*摄像头停止采集*/
int camera_stop(int fd) //
{
    int  ret ;
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
    if(-1 == ret)
    {
        perr_exit("camera_stop");
    }
    qDebug("camera_stop  capture success");         //可控提示停止采集
    return 0;
}

int  camera_exit(int fd)
{
    int i, ret;
    struct v4l2_buffer v4l2_buffer;
    v4l2_buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // 设置数据流
    v4l2_buffer.memory = V4L2_MEMORY_MMAP;          //得到一帧数据

    for(i = 0; i < reqbufs.count; i++)
    {
        ret = ioctl(fd, VIDIOC_DQBUF, &v4l2_buffer);
        if(-1 ==ret)
        {
            break;
        }
    }
    for(i = 0; i < reqbufs.count; i++)
    {
        munmap(cam_bufs[i].start, cam_bufs[i].length);
    }
    qDebug("camera_exit success");                  //打印提示信息到屏幕
    return close(fd);

}

