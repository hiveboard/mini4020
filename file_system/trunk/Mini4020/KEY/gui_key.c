#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

static DLGTEMPLATE DlgBoxInputLen =
{
    WS_BORDER | WS_CAPTION,WS_EX_NONE, 0, 0, 320, 240,"键盘测试程序",		 0,	 0,	 3,	 NULL,	0
};

#define IDC_SIZE_MM     100
#define IDC_SIZE_INCH   110

static CTRLDATA CtrlInputLen [] =
{ 
  	{	CTRL_STATIC,WS_VISIBLE | SS_SIMPLE,	10,	15,	380,	18,	IDC_STATIC,	"请点击按键",	 0	},
   {	CTRL_EDIT,WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_READONLY,	10,	45,	260,	24,	IDC_SIZE_MM,	NULL,	0	},
   { CTRL_BUTTON,	WS_TABSTOP | WS_VISIBLE,	10,	100,	60,	25,	IDOK,	"确定",	 0	}
};

static int InputLenDialogBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	char buffer[30] = {0,};
	switch (message) 
	{
    	case MSG_INITDIALOG:	return 1;
		break;
  #if 1      
		case MSG_KEYDOWN:
			printf ("MGS_KEYDOWN: key %d\n", LOWORD(wParam));
		break;

		case MSG_KEYUP:
			printf ("MGS_KEYUP: key %d\n", LOWORD(wParam));
		break;
	#endif
    	case MSG_COMMAND:

     	break;

		case MSG_CHAR:
			bzero(buffer,30);
			sprintf(buffer,"The character is: %c",wParam);
			printf("wparam = %c\n",wParam);
			SetWindowText(GetDlgItem(hDlg, IDC_SIZE_MM),buffer);      
		break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static void InputLenDialogBox (HWND hWnd)
{
    DlgBoxInputLen.controls = CtrlInputLen;
    
    DialogBoxIndirectParam (&DlgBoxInputLen, hWnd, InputLenDialogBoxProc, 0);
}

int MiniGUIMain (int argc, const char* argv[])
{
#ifdef _LITE_VERSION
    SetDesktopRect(0, 0, 1024, 768);
#endif
    
    InputLenDialogBox (HWND_DESKTOP);

    return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

