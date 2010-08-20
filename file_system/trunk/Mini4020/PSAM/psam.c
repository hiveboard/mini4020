#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define	 COLDRESET    0X11
#define  RECEIVE_LEN  0X12

int main(int argc, char **argv)
{
	int fd, j;
	int receive_len[1] ={0};
	char receive_buf[50]={0}; 
	char len_buf[2] = {0};

char send3[10]={0x00,0xA4,0x04,0x0,0x05,0x11,0x22,0x33,0x44,0x55}; 
	char send5[5]={0X80,0X1A,0X08,0X01,0X00}; 
	char send6[29]=
{0X80,0XFA,0X07,0X00,0X18,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X02,0X03,0X04,0X05,0X06,0X07,0X08,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00};  
	char send7[29]=
{0X80,0XFA,0X03,0X00,0X18,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X88,0X88,0X88,0X88,0X88,0X88,0X88,0X88,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00}; 
	char send8[29]=
{0X80,0XFA,0X01,0X00,0X18,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X11,0X12,0X13,0X14,0X15,0X16,0X17,0X18,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00}; 

	fd = open("/dev/psam",O_RDWR);
	if(fd == -1)
	{
	  printf("wrong\r\n");
	  exit(-1);
	}

	
	ioctl(fd, COLDRESET);
	
	ioctl(fd, RECEIVE_LEN, receive_len);  

	read(fd, receive_buf, receive_len[0]);

	for(j = 0;j < receive_len[0]; j++)
	{
		printf("0x%x ",receive_buf[j]); 	
	}
	printf("now 选择文件:\n"); 	
	sleep(2);
	
	len_buf[0] = 1;
	len_buf[1] = 2;
	write(fd, send3,10);
	write(fd, len_buf,2);

	ioctl(fd, RECEIVE_LEN, receive_len); 

	read(fd, receive_buf, receive_len[0]);

	for(j = 0;j < receive_len[0]; j++)
	{
		printf("0x%x ",receive_buf[j]); 	
	}
	printf("now 加密: \n"); 	
	sleep(2);
	len_buf[0] = 2;
	len_buf[1] = 0;
	write(fd, send5,5);
	write(fd, len_buf,2);

	len_buf[0] = 1;
	len_buf[1] = 2;
	write(fd, send6,29);
	write(fd, len_buf,2);

	len_buf[0] = 1;
	len_buf[1] = 2;
	write(fd, send7,29);
	write(fd, len_buf,2);

	len_buf[0] = 1;
	len_buf[1] = 2;
	write(fd, send8,29);
	write(fd, len_buf,2);
	
	ioctl(fd, RECEIVE_LEN, receive_len);  

	read(fd, receive_buf, receive_len[0]);

	for(j = 0;j < receive_len[0]; j++)
	{
		printf("0x%x ",receive_buf[j]); 	
	}
	printf("\n"); 


	close(fd);
	return 0;
}

