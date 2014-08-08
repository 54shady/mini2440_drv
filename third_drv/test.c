#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/* 
 *thirddrvtest 
 */
int main(int argc, char **argv)
{
	int fd;
	unsigned char key_val;
	unsigned char hi;
	unsigned char lo;
	
	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0)
	{
		printf("can't open!\n");
		return -1;
	}

	while (1)
	{
		read(fd, &key_val, 1);
		/* hi 表示按或不按，lo 表示具体按键 */
		hi = key_val & 0xF0; 
		lo = key_val & 0x0F;
		printf("key %d is %s\n", lo, (hi == 0x0) ? "press" : "up");
	}

	close(fd);
	
	return 0;
}

