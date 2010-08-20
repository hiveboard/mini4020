#include "mini.h"
static DLGTEMPLATE DlgPwm =
{
    WS_BORDER | WS_CAPTION, WS_EX_NONE, 0, 0, 320, 240, "PWM TEST", 0, 0, 4, NULL,0
};
static CTRLDATA CtrlPwm [] =
{ 
	{	CTRL_STATIC,WS_VISIBLE,	55, 10, 80, 30,IDC_PWM_STATICCHOICE,"选择文件:",0},
	{	CTRL_COMBOBOX,WS_VISIBLE | CBS_SIMPLE | WS_TABSTOP, 28, 30, 150, 24,IDC_PWM_COMBOBOX,"请点击以下选项",130},	
	{	CTRL_BUTTON,WS_TABSTOP | WS_VISIBLE | WS_TABSTOP, 210, 100, 80, 30, IDC_PWMPLAY_BUTTON,  "播放", 0	},
	{	CTRL_BUTTON,WS_TABSTOP | WS_VISIBLE | WS_TABSTOP, 210, 160, 80, 30, IDC_PWMEND_BUTTON, "退出",	 0},  	
};
static void pwm_fill_boxes (HWND hDlg, char* path)
{
	struct dirent *e;
	char *dname = path;																//设置路径
	DIR *dir;
	int select;
   dir = opendir (dname);																//打开res目录

   if (!dir)return;

  	while ((e = readdir (dir)) != NULL)
	    {
    	if (!strstr (e->d_name, ".wav"))
		 {
 			    continue;
      		}
		SendDlgItemMessage(hDlg, IDC_PWM_COMBOBOX, CB_ADDSTRING, 0, (LPARAM)e->d_name);
    	}

	closedir (dir);

}

static int DialogBoxPwmProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	int i , length;
	static int fd;
	int on;
	FILE *fp_r;
	unsigned int nFileLen,loops;
	unsigned char read_byte[1024];

	static char buffer[20] = {0} , read_name[30] = {0};
	struct pwm_config *p;
	struct pwm_config pwm;
	unsigned long freq;

	switch (message) 
	{
   	case MSG_INITDIALOG:
		{
			pwm_fill_boxes (hDlg,"pwm");
#if 1
			fd = open("/dev/sep4020_pwm",O_RDWR);
			if(fd == -1)
			{
			  	printf("wrong\r\n");
			  	exit(-1);
			}
#endif
			return 1;
        	}
		break;

		case MSG_CHAR:
			if(wParam != '\033')break;
		case MSG_CLOSE:
		{
			//fclose(fp_r);
			close(fd);
			EndDialog (hDlg, wParam);
		}
      break;	

		case MSG_COMMAND:
			switch (wParam) 
			{
				case IDC_PWMPLAY_BUTTON:
				{
					strcpy(read_name,"pwm/");
					GetWindowText (GetDlgItem(hDlg,IDC_PWM_COMBOBOX),buffer,20);
					if((int)buffer[0] > 128)break;												//如果不是有效的路径则退出
					strcat(read_name,buffer);
					
#if 1
					fp_r = fopen(read_name, "r");

					fseek(fp_r,0,SEEK_END);     			//定位到文件末

					nFileLen = ftell(fp_r);       		//文件长度 

					on = 1;

					pwm.pwm_ctrl = 0;
					pwm.pwm_div = 4;
					pwm.pwm_period = 255;
					pwm.pwm_data = 1;
					
					p = &pwm;
					freq = (unsigned long)p;	
					ioctl(fd, on, freq);
					
					fp_r = fopen(read_name, "r");//open again
					loops = nFileLen/1024;
					
					for(i=0;i<loops;i++){
						fread(read_byte, 1024, 1, fp_r);
						write(fd,read_byte,1024);
						}
					ioctl(fd, 2, freq);
#endif
					bzero(read_name,30);
				}

				break;

				case IDC_PWMEND_BUTTON:
				{
					//fclose(fp_r);
					close(fd);
					EndDialog (hDlg, 0);
				}
				break;
			}
		break;
    	}
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int PWMPanel(HWND hWnd)
{	
	DlgPwm.controls = CtrlPwm;
    	
   DialogBoxIndirectParam (&DlgPwm, hWnd, DialogBoxPwmProc, 0L);

   return 0;
}
