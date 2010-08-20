/* 
** $Id: edit.c,v 1.4 2003/07/04 08:29:13 weiym Exp $
**
** Listing 9.1
**
** edit.c: Sample program for MiniGUI Programming Guide
** 	    Usage of EDIT control.
**
** Copyright (C) 2003 Feynman Software.
**
** License: GPL
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define	 COLDRESET    0X11
#define  RECEIVE_LEN  0X12

static DLGTEMPLATE DlgBoxInputChar =
{
    WS_BORDER | WS_CAPTION, 
    WS_EX_NONE,
    0, 0, 320, 240, 
    "PSAM CARD",
    0, 0,
    5, NULL,
    0
};


#define ID_CHARS        110#define ID_RESET        120
#define ID_SELECT       130
#define ID_ENCRYPT      140



static CTRLDATA CtrlInputChar [] =
{ 	{
		CTRL_BUTTON,		WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 		40, 40, 80, 30, 
		ID_RESET, 
		 "复位",
		 0
	},

	{
		CTRL_BUTTON,		WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 		40, 100, 80, 30, 
		ID_SELECT, 
		 "选择文件",
		 0
	},

	{
		CTRL_BUTTON,		WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 		40, 160, 80, 30, 
		ID_ENCRYPT, 
		 "3DES加密",
		 0
	},

     
	

	{
		CTRL_STATIC,
		WS_VISIBLE | SS_CENTER,		190, 40, 100, 30, 
		IDC_STATIC, 		 "结果显示 :",		 0	},
	{
		CTRL_MLEDIT,
		WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_BASELINE | ES_AUTOWRAP,		190, 80, 100, 80, 
		ID_CHARS, 		 "",
		 0	},


};


static char send[50];

static int fd;




static int InputCharDialogBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static PLOGFONT my_font;
	unsigned char i,msb,lsb,value,len,remainder;
	char high_level,low_level,j;
	char receive_buf[50] = {0};
	char receive_len[0];
	char receive[50] = {0};
	char len_buf[2] = {0};

	char send3[10]={0x00,0xA4,0x04,0x0,0x05,0x11,0x22,0x33,0x44,0x55}; 
	char send5[5]={0X80,0X1A,0X08,0X01,0X00}; 
	char send6[29]=
{0X80,0XFA,0X07,0X00,0X18,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X01,0X02,0X03,0X04,0X05,0X06,0X07,0X08,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00};  
	char send7[29]=
{0X80,0XFA,0X03,0X00,0X18,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X88,0X88,0X88,0X88,0X88,0X88,0X88,0X88,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00}; 
	char send8[29]=
{0X80,0XFA,0X01,0X00,0X18,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X11,0X12,0X13,0X14,0X15,0X16,0X17,0X18,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00}; 


   HWND hwnd;

   switch (message) 
	{
   	case MSG_INITDIALOG:

			my_font = CreateLogFont (NULL, "Arial", "ISO8859-1", 
                        FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_SETWIDTH_NORMAL,
                        FONT_SPACING_CHARCELL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
                        					30, 0);

			return 1;
        
		case MSG_COMMAND:			switch (wParam) 
			{
				case ID_RESET:
					ioctl(fd, COLDRESET);
					ioctl(fd, RECEIVE_LEN, receive_len); 

					read(fd, receive_buf, receive_len[0]);

					for(j = 0;j < receive_len[0]; j++)
					{
						printf("0x%x ",receive_buf[j]); 	
					}
					printf("\n"); 	

					for(i=0; i<receive_len[0]; i++)
					{	
						high_level = 2*i;
						low_level = 2*i + 1;

						msb = receive_buf[i] & 0xf0;
						msb >>= 4;	
						if (msb > 9)
							msb += 87;
						else
							msb += 48;	

						lsb = receive_buf[i] & 0x0f;
						if (lsb > 9)
							lsb += 87;
						else 
							lsb += 48;
				
						receive[high_level] = msb;
						receive[low_level] = lsb;
					
					}
					SetWindowText(GetDlgItem (hDlg, ID_CHARS),"\0");
					SetWindowText(GetDlgItem (hDlg, ID_CHARS),receive);
					
					break;
				case ID_SELECT:
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
					printf("\n"); 	

					for(i=0; i<receive_len[0]; i++)
					{	
						high_level = 2*i;
						low_level = 2*i + 1;

						msb = receive_buf[i] & 0xf0;
						msb >>= 4;	
						if (msb > 9)
							msb += 87;
						else
							msb += 48;	

						lsb = receive_buf[i] & 0x0f;
						if (lsb > 9)
							lsb += 87;
						else 
							lsb += 48;
				
						receive[high_level] = msb;
						receive[low_level] = lsb;
					
					}
					SetWindowText(GetDlgItem (hDlg, ID_CHARS),"\0");
					SetWindowText(GetDlgItem (hDlg, ID_CHARS),receive);
					

					
					break;
				case ID_ENCRYPT:
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

					for(i=0; i<receive_len[0]; i++)
					{	
						high_level = 2*i;
						low_level = 2*i + 1;

						msb = receive_buf[i] & 0xf0;
						msb >>= 4;	
						if (msb > 9)
							msb += 87;
						else
							msb += 48;	

						lsb = receive_buf[i] & 0x0f;
						if (lsb > 9)
							lsb += 87;
						else 
							lsb += 48;
				
						receive[high_level] = msb;
						receive[low_level] = lsb;
					
					}
					SetWindowText(GetDlgItem (hDlg, ID_CHARS),"\0");
					SetWindowText(GetDlgItem (hDlg, ID_CHARS),receive);

					break;			
			
			}
			break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

int MiniGUIMain (int argc, const char* argv[])
{ 
	fd = open("/dev/psam",O_RDWR);
	if(fd == -1)
	{
	  printf("wrong\r\n");
	  exit(-1);
	}


   DlgBoxInputChar.controls = CtrlInputChar;
   DialogBoxIndirectParam (&DlgBoxInputChar, HWND_DESKTOP, InputCharDialogBoxProc, 0L);

	close(fd);

   return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

