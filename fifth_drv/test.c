#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>

int fd;

void my_sighandler(int signum)
{
	unsigned char key_val;
	unsigned char hi;
	unsigned char lo;
	read(fd, &key_val, 1);
	hi = key_val & 0xF0; 
	lo = key_val & 0x0F;
	printf("key %d is %s\n", lo, (hi == 0x0) ? "press" : "up");
}

int main(int argc, char **argv)
{
	signal(SIGIO, my_sighandler);

	int flags;
	
	fd = open("/dev/buttons", O_RDWR);
	if (fd < 0)
	{
		printf("can open /dev/buttons\n");
		return -1;
	}

	fcntl(fd, F_SETOWN, getpid());

	flags = fcntl(fd, F_GETFL);
	
	fcntl(fd, F_SETFL, flags | FASYNC);
	
	while (1)
	{
		sleep(50000);
	}

	close(fd);
	
	return 0;
}
