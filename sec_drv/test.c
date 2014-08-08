#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int fd;
	char ret[6];
	int rBytes;
	int i;

	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0)
	{
		printf("Can't open the file\n");
		return -1;
	}

	while (1)
	{
		rBytes = read(fd, ret, sizeof(ret));
		for (i = 0; i < 6; i++)
			if (ret[i] == 0)
				printf("%dth button is press\n", i + 1);
	}
	
	return 0;
}
