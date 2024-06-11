#include "v4l2.h"
#include "video_capture.h"

int camera_error(int fd, char *s)
{
    perror(s);
    camera_stop(fd);
    camera_exit(fd);
    return -1;
}

char *devpath = "/dev/video0";

int camera_capture(int sockfd)
{
    unsigned int width = 640;
    unsigned int height = 480;
    unsigned int pic_size ;   //装图片大p 
    unsigned int ismjpeg = 0;
    int camera_fd ;
    int ret ;
    //camera_init
    camera_fd = camera_init(devpath, &width, &height, &pic_size, &ismjpeg);
    if(-1 == camera_fd)
    {
        perror("camera_init fail");
        return -1;
    }
    printf("camera_init success!\n");
    printf("width = %-6dheight = %d\n",width, height);
    //camera_start
    ret = camera_start(camera_fd);
    if(-1 == ret)
    {
        camera_error(camera_fd,"camera_start: error");
    }
    //camera_debuf
    void *buf;
    char buf_sendPicSize[10] = {0};
    unsigned int pix_size;
    unsigned int index;
	

	while(1)
	{
		bzero(&index,sizeof(index));
		ret = camera_debuf(camera_fd, &buf, &pix_size, &index);
		if(-1 == ret)
		{
			camera_error(camera_fd, "camera_debuf: fail");
		}

		int n;  //接收write 返回值
		sprintf(buf_sendPicSize, "%d", pix_size ); //将一帧图片的大小转换成字符 放到buf_sendPicSize 里面
		n = write(sockfd, buf_sendPicSize, sizeof(buf_sendPicSize)); //发送图片大小到客户端
		if(0 >= n)
		{
			camera_error(camera_fd, "write pic_size fail");
		}

		n = write(sockfd, buf, pix_size);  //发送图片 内容到 客户端
		if(0 >= n)
		{
			camera_error(camera_fd, "camera_debuf:write picData fail");
		}

		printf("sendSize:%d--data:%d\n",strlen(buf_sendPicSize),pix_size);
	    
		//camera_eqbuf
	    ret = camera_eqbuf(camera_fd, index);
	    if(-1 == ret)
	    {
	        camera_error(camera_fd, "camera_eqbuf:fail");
	    }
		
	}
	
}

