#include <stdio.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
int main(int argc, char **argv) 
{ 
	int fd; 
	int j; 
	//char buf[2] = {0}; 
	fd = open("/dev/key",O_RDONLY);//|O_NONBLOCK); 
	if(fd == -1) 
	{ 
		printf("wrong\r\n"); exit(-1); 
	} 
	
	printf("key value\n"); 
	while(1)
	{
		char buf[2] = {0};
		read(fd,buf,2); 
		printf("key====%c\n\r", buf[0]); 
	}

	printf("\n"); 
	close(fd); 
	return 0; 
}
