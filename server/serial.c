#include "serial.h"

/**
 * 设置串行端口的参数
 *
 * @param fd 文件描述符，代表打开的串行端口
 * @param nspeed 波特率
 * @param nbits 数据位数
 * @param nevent 校验位类型（'O'为奇校验, 'E'为偶校验, 'N'为无校验）
 * @param nstop 停止位数（1或2）
 * @return 成功返回0，失败返回-1
 */
int set_serial(int fd, int nspeed, int nbits, char nevent, int nstop)
{
	struct termios newttys1, oldttys1;
	// 保存原有的串口配置
	if (tcgetattr(fd, &oldttys1) != 0)
	{
		perror("setserial 1\n");
		return -1;
	}
	// 恢复串口为阻塞状态
	if (fcntl(fd, F_SETFL, 0) < 0)
	{
		printf("fcntl failed!\n");
		return -1;
	}
	else
	{
		printf("fcntl=%d\n", fcntl(fd, F_SETFL, 0));
	}

	bzero(&newttys1, sizeof(newttys1));
	// CREAD开启串行数据接收，CLOCAL打开本地连接模式
	newttys1.c_cflag |= (CLOCAL | CREAD);
	newttys1.c_cflag &= ~CSIZE; // 设置数据位
	// 数据位选择
	switch (nbits)
	{
	case 7:
		newttys1.c_cflag |= CS7;
		break;
	case 8:
		newttys1.c_cflag |= CS8;
		break;
	}
	// 设置奇偶校验位
	switch (nevent)
	{
	case 'O':						// 奇校验
		newttys1.c_cflag |= PARENB; // 开启奇偶校验
		// INPCK打开输入奇偶校验，ISTRIP去除字符的第八个比特
		newttys1.c_iflag |= (INPCK | ISTRIP);
		newttys1.c_cflag |= PARODD; // 启动奇校验（默认偶校验）
		break;
	case 'E': // 偶校验
		newttys1.c_cflag |= PARENB;
		newttys1.c_cflag |= (INPCK | ISTRIP);
		newttys1.c_cflag &= ~PARODD;
		break;
	case 'N':
		newttys1.c_cflag &= ~PARENB;
		break;
	}
	// 设置波特率
	switch (nspeed)
	{
	case 2400:
		cfsetispeed(&newttys1, B2400);
		cfsetospeed(&newttys1, B2400);
		break;
	case 4800:
		cfsetispeed(&newttys1, B4800);
		cfsetospeed(&newttys1, B4800);
		break;
	case 9600:
		cfsetispeed(&newttys1, B9600);
		cfsetospeed(&newttys1, B9600);
		break;
	case 115200:
		cfsetispeed(&newttys1, B115200);
		cfsetospeed(&newttys1, B115200);
		break;
	default:
		cfsetispeed(&newttys1, B9600);
		cfsetospeed(&newttys1, B9600);
		break;
	}
	// 设置停止位,若停止位为1，则清除CSTOPB，若停止位为2，则激活CSTOPB
	if (nstop == 1)
	{
		newttys1.c_cflag &= ~CSTOPB;
	}
	else if (nstop == 2)
	{
		newttys1.c_cflag |= CSTOPB;
	}
	// 设置最少字符和等待时间，对于接收字符和等待时间没有特别要求时
	newttys1.c_cc[VTIME] = 0; // 非规范模式读取时的超时时间
	newttys1.c_cc[VMIN] = 0;  // 非规范模式读取时的最小字符数
	// tcflush清空终端未完成的输入/输出请求及数据；
	// TCIFLUSH表示清空正收到的数据，且不读去出来
	tcflush(fd, TCIFLUSH);
	// 激活配置使其生效
	if ((tcsetattr(fd, TCSANOW, &newttys1)) != 0)
	{
		perror("com set error");
		return -1;
	}
	return 0;
}
