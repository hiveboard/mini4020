#include <stdio.h>
#define OPEN 1
#define CLOSE 0
int main(int argc, char **argv)
{
	int fd;
	int i,j,k;
	fd = open("/dev/flowled",0);
	if(fd == -1)
	{
	  printf("wrong\r\n");
	  exit(-1);
	}
	for(j=0; j<20; j++)
	{
		for(i = 1; i <= 4; i++)
		{
			for(k=0;k<1000000;k++);
			ioctl(fd, OPEN, i);

		}
		for(i=4; i>=1; i--)
		{
			for(k=0;k<1000000;k++);
			ioctl(fd, CLOSE, i);

		}
	}
	close(fd);
	return 0;
}

