/*===============================================================
*   Copyright (C) 2024 All rights reserved.
*   
*   文件名称：camera.c
*   创 建 者：
*   创建日期：2024年04月07日
*   描    述：
*
*   更新日志：
*
================================================================*/
#include "serial.h"
#include "video_capture.h"

void *camera_pthread(void* arg)
{

    char buff[BUFSIZ] = {0};         //接受数据的buf
    int confd = *(int *)arg;

    int num = 3;
    while(num--)
    {
        int n = read(confd, buff,sizeof(buff));
        write(STDOUT_FILENO, buff, n);
        
        if(strncmp(buff, "ON", 2) == 0)
        {
            camera_capture(confd);
        }
		else
		{
            printf("cam fail\n");
        }
	
	}
    return NULL;
}
