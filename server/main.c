#include "serial.h"

int main()
{
	int camera_fd;
	pthread_t thread;

        int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY) ;//1、打开USB0串口，路径名为"/dev/ttyUSB0"
	if (fd < 0)
	{
		printf("fd=%d:open USB error\n", fd);
		perror("open:");
		return -1;
	}
	printf("%d\n", fd);

	int ret;
	//2、调用set_serial()函数，并且初始化串口属性（文件描述符，波特率，数据位，校验位，停止位）
	int ret = set_serial(fd, 115200, 8, 'N', 1);
	if (ret < 0)
	{
		perror("set_serial:");
		return -1;
	}

	// 3、创建camera_fd套接字，自己设置端口号和ip
	camera_fd = server_socket(6666, "0");
	ret = pthread_create(&thread, NULL, camera_pthread, (void *)&camera_fd); // 创建摄像头线程不用改
	if (ret == -1)
	{
		perror("pthread_create:");
		return -1;
	}
	printf("pthread_create succeed!\n");

	// 控制模块初始化
    server_init(fd);

	//关闭摄像头套接字
    close(camera_fd);
	return 0;
}
