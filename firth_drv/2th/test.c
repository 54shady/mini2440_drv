#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/* 
 * 如何使用该测试文件:
 * mknod /dev/xyz c major 0   
 *	其中major是主设备号可通过看/proc/devices得到
 */

int main(int argc, char **argv)
{

	int fd;
	int val = 1;
	
	if (argc != 2)
	{
		printf("Usage: %s <on | off>\n", argv[0]);
		return -2;
	}

	fd = open("/dev/xyz", O_RDWR);
	if (fd < 0)
	{
		printf("Can't open\n");
		return -1;
	}


	if (!(strcmp(argv[1], "on")))
		val = 1;
	else
		val = 0;	

	write(fd, &val, 4);
	close(fd);

	return 0;
}
