#include "serial.h"

unsigned char envbuf[36] = {0}; // 存放控制命令的数组

int server_socket(unsigned short port, char *ip)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		return -1;
	}
	struct sockaddr_in seraddr;
	struct sockaddr_in cliaddr;
	socklen_t len = sizeof(cliaddr);
	bzero(&seraddr, sizeof(seraddr));
	bzero(&cliaddr, sizeof(cliaddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(port);
	seraddr.sin_addr.s_addr = inet_addr(ip);

	if (bind(sockfd, (const struct sockaddr *)&seraddr, sizeof(seraddr)) < 0)
	{
		perror("bind");
		return -1;
	}

	if (listen(sockfd, 5) < 0)
	{
		perror("listen");
		return -1;
	}

	int fd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
	if (fd < 0)
	{
		perror("accept");
		return -1;
	}
	printf("%s:%d connected!!\n", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));

	return fd;
}

int server_init(int fd)
{
	int serial_fd; 
	// 1、调用server_socket()函数创建serial_fd套接字，自己设置端口号和ip，注意不能和摄像头端口号一样
	serial_fd = server_socket(7777, "0");

	char buf[32] = {0}; // 接收命令的数组
	int ret;
	char data[20] = {0}; // 存放环境数据的数组，不用改
	while (1)
	{
		if ((ret = read(serial_fd, buf, sizeof(buf))) > 0) // 接收客户端发来的指令
		{
			// 比对客户端的指令及执行相应的功能
			if (strncmp(buf, "env", 3) == 0) // 执行获取环境数据的命令
			{
				// send 环境
				if (read(fd, envbuf, sizeof(envbuf)) == sizeof(envbuf))
				{
					int i;
					for (i = 0; i < 36; i++)
					{
						printf("%x ", envbuf[i]);
					}
					printf("\n");
					//处理环境数据
					sprintf(data, "%d:%d:%d", (envbuf[5] + envbuf[4] / 10), (envbuf[7] + envbuf[6] / 10),
							(envbuf[20] + (envbuf[21] << 8) + (envbuf[22] << 16) + (envbuf[23] << 24)));
					printf("data=%s\n", data);
					if (write(serial_fd, data, sizeof(data)) < 0)
					{
						perror("write");
						exit(-1);
					}
				}
			}
			else if (strncmp(buf, "light_on", 8) == 0)
			{
				// envbuf[]  = {0xdd,id,24,00,命令}
				// 发送命令开灯
				envbuf[0] = 0xdd;
				envbuf[4] = 0x00;
				if (write(fd, envbuf, 5) < 0)
				{
					perror("write");
					return -1;
				}
			}
			else if (strncmp(buf, "light_off", 9) == 0)
			{
				// 2、发送命令关灯
				envbuf[0] = 0xdd;
				envbuf[4] = 0x01;
				if (write(fd, envbuf, 5) < 0)
				{
					perror("write light_off");
					return -1;
				}
				
			}
			else if (strncmp(buf, "buzzer_on", 9) == 0)
			{
				// 3、发送命令打开蜂鸣器
				envbuf[0] = 0xdd;
				envbuf[4] = 0x02;
				if (write(fd, envbuf, 5) < 0)
				{
					perror("write buzzer_on");
					return -1;
				}

			}
			else if (strncmp(buf, "buzzer_off", 10) == 0)
			{
				// 4、发送命令关闭蜂鸣器
				envbuf[0] = 0xdd;
				envbuf[4] = 0x03;
				if (write(fd, envbuf, 5) < 0)
				{
					perror("write buzzer_off");
					return -1;
				}
			}
			else if (strncmp(buf, "feng_on", 7) == 0)
			{
				// 5、发送命令打开风扇
				envbuf[0] = 0xdd;
				envbuf[4] = 0x04;
				if (write(fd, envbuf, 5) < 0)
				{
					perror("write feng_on");
					return -1;
				}
			}
			else if (strncmp(buf, "feng_off", 8) == 0)
			{
				// 6、发送命令关闭风扇
				envbuf[0] = 0xdd;
				envbuf[4] = 0x08;
				if (write(fd, envbuf, 5) < 0)
				{
					perror("write feng_off");
					return -1;
				}
			}
		}
		else if (ret < 0)
		{
			perror("read");
			exit(-1);
		}
		else
		{
			printf("client quit!\n");
			break;
		}

		bzero(buf, sizeof(buf));
	}

	// 7、关闭控制模块的套接字serial_fd
}
