#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define OPEN 1
#define CLOSE 0

#define BASE_X						18
#define BASE_Y						10

#define IDC_LED_STATIC			1000
#define IDC_LED_EDIT				(IDC_LED_STATIC + 1)
#define IDC_LED_BUTTON			(IDC_LED_STATIC + 2)


#define IDC_BUZZER_STATIC		1010
#define IDC_BUZZER_BUTTON		(IDC_BUZZER_STATIC + 1)
#define IDC_BUZZER_BUTTON1		(IDC_BUZZER_STATIC + 2)

static int fd;

static DLGTEMPLATE DlgLED =
{
	WS_BORDER | WS_CAPTION,WS_EX_NONE,	0,	0,	320,	240,	"流水灯&蜂鸣器",	0,	0,	6,	 NULL,	0
};

static CTRLDATA CtrlLED [] =
{ 
	{	CTRL_STATIC,WS_VISIBLE | SS_GROUPBOX,	BASE_X, BASE_Y - 10, 280, 100,IDC_LED_STATIC,"LED",0	},
	{	CTRL_EDIT,WS_TABSTOP | WS_VISIBLE,BASE_X + 200, BASE_Y + 10, 60, 25,IDC_LED_EDIT,"5",0	},

	{	CTRL_BUTTON,WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON, BASE_X + 200,BASE_Y + 53, 60, 28,IDC_LED_BUTTON, "OK",0	},

	{	CTRL_STATIC,WS_VISIBLE | SS_GROUPBOX,BASE_X,	BASE_Y + 95, 280, 100,IDC_BUZZER_STATIC,"BUZZER", 0	},

   {	CTRL_BUTTON,WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON,BASE_X + 200, BASE_Y + 115, 60, 28, IDC_BUZZER_BUTTON, "ON", 0	},
   {	CTRL_BUTTON,WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON,BASE_X + 200, BASE_Y + 160, 60, 28, IDC_BUZZER_BUTTON1, "OFF", 0	},

};

static int DialogBoxLEDProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{	
	char LoopTimes[3] = {0};
	int i , j , k;
   switch (message) 
	{
   	case MSG_INITDIALOG:
			SendMessage(GetDlgItem(hDlg,IDC_LED_EDIT),EM_LIMITTEXT,2,0);
		return 1;
        
		case MSG_COMMAND:			switch (wParam) 
			{
				case IDC_LED_BUTTON:
					GetWindowText(GetDlgItem(hDlg,IDC_LED_EDIT),LoopTimes,3);
					for(j = 0; j < atoi(LoopTimes); j++)
					{
						for(i = 1; i <= 4; i++)
						{
							for(k=0;k<200000;k++);
							ioctl(fd, OPEN, i);

						}
						for(i = 1; i <= 4; i++)
						{
							for(k=0;k<200000;k++);
							ioctl(fd, CLOSE, i);

						}
					}
				break;
				case IDC_BUZZER_BUTTON:
					ioctl(fd, OPEN, 0);
				break;				

				case IDC_BUZZER_BUTTON1:
					ioctl(fd, CLOSE, 0);
				break;	
		}
		break;

		case MSG_PAINT:
		{
			HDC hdc;
			hdc=BeginPaint(hDlg);
			SetBkMode(hdc, BM_TRANSPARENT);
	   	TextOut(hdc,BASE_X + 12,BASE_Y + 15,"请输入循环次数:");
	   	TextOut(hdc,BASE_X + 12,BASE_Y + 59,"点击OK 流水灯演示:");
	   	TextOut(hdc,BASE_X + 12,BASE_Y + 123,"点击ON 打开蜂鸣器:");
	   	TextOut(hdc,BASE_X + 12,BASE_Y + 165,"点击OFF关闭蜂鸣器:");
	    	EndPaint (hDlg, hdc);
		}
		break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{ 
#if 1
	fd = open("/dev/flowled",0);
	if(fd == -1)
	{
	  printf("wrong\r\n");
	  exit(-1);
	}
#endif

   DlgLED.controls = CtrlLED;
   DialogBoxIndirectParam (&DlgLED, HWND_DESKTOP, DialogBoxLEDProc, 0L);

	close(fd);

   return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

