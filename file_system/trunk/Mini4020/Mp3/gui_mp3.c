#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define BASE_X						18
#define BASE_Y						10

#define IDC_MP3_STATIC				1100
#define IDC_MP3_BUTTON				(IDC_MP3_STATIC + 1)
#define IDC_MP3_BUTTON1				(IDC_MP3_STATIC + 2)


static int fd;

static DLGTEMPLATE DlgMP3 =
{
	WS_BORDER | WS_CAPTION,WS_EX_NONE,	0,	0,	320,	240,	"MP3",	0,	0,	2,	 NULL,	0
};

static CTRLDATA CtrlMP3 [] =
{ 
	{	CTRL_BUTTON,WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON, BASE_X + 200,BASE_Y + 53, 60, 28,IDC_MP3_BUTTON, "ON",0	},

	{	CTRL_BUTTON,WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON,BASE_X + 200, BASE_Y + 115, 60, 28, IDC_MP3_BUTTON1, "OFF", 0	},
};
static pid_t pid;
static int DialogBoxMP3Proc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{	
	char LoopTimes[3] = {0};
	int i , j , k;
   switch (message) 
	{
   	case MSG_INITDIALOG:

		return 1;
        
		case MSG_COMMAND:			switch (wParam) 
			{
				case IDC_MP3_BUTTON:
					if((pid = fork()) < 0)
					{
						perror("Cannot create a new process\n");
						return 1;
					}
					else
						if(pid == 0)
						{
							execl("/tmp/madplay.arm","madplay.arm","prisonbreak.mp3",NULL);
							perror("execl faiMP3 to run madplay\n");
						}
					EnableWindow(GetDlgItem(hDlg,IDC_MP3_BUTTON),FALSE);
				break;
				case IDC_MP3_BUTTON1:
					kill(pid,2);
					EnableWindow(GetDlgItem(hDlg,IDC_MP3_BUTTON),TRUE);
				break;			
			}
		break;

		case MSG_PAINT:
		{
			HDC hdc;
			hdc=BeginPaint(hDlg);
			SetBkMode(hdc, BM_TRANSPARENT);

	   	TextOut(hdc,BASE_X + 12,BASE_Y + 59,"µã»÷ON ¿ªÊ¼²¥·ÅÒôÀÖ:");
	   	TextOut(hdc,BASE_X + 12,BASE_Y + 123,"µã»÷OFFÍ£Ö¹²¥·ÅÒôÀÖ:");

	    	EndPaint (hDlg, hdc);
		}
		break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{ 
   DlgMP3.controls = CtrlMP3;
   DialogBoxIndirectParam (&DlgMP3, HWND_DESKTOP, DialogBoxMP3Proc, 0L);

   return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

