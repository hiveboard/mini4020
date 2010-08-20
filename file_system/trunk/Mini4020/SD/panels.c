#include "panels.h"
static DLGTEMPLATE DlgJPG =
{
    WS_BORDER | WS_CAPTION,WS_EX_NONE,0, 0, 320, 240,"caption",0, 0, 1, NULL, 0
};
static CTRLDATA CtrlJPG[] =
{ 
	{CTRL_STATIC,WS_VISIBLE,0, 0, 0, 0,IDC_NULL_STATIC,"", 0}	
};
static int JPGProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static BITMAP bitmap;
	switch (message)
	{
		case MSG_INITDIALOG:
		{
			int i = lParam;
			char buf[30] = "/mnt/sd/";
			strcat(buf,Id_Type[lParam].path);
			printf("Param =%d\n",lParam);
			printf("buf = %s\n",buf);
			if (LoadBitmap (HDC_SCREEN, &bitmap, buf))
				return -1;
			bzero(buf,30);
   		}        
		break;

		case MSG_LBUTTONDOWN:
     	 	{
			UnloadBitmap(&bitmap); EndDialog (hDlg, wParam);break;
      		}
      break; 

		case MSG_CLOSE:EndDialog(hDlg, wParam);break;
		
		case MSG_PAINT:
		{
			HDC hdc;
        	hdc = BeginPaint (hDlg);
			FillBoxWithBitmap (hdc, 0, 0, 0, 0, &bitmap);
        	EndPaint (hDlg,hdc);
		}
		break;   
	}    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static DLGTEMPLATE DlgTEXT =
{
    WS_BORDER | WS_CAPTION,WS_EX_NONE,0, 0, 320, 240,"Text",0, 0, 2, NULL, 0
};
static CTRLDATA CtrlTEXT[] =
{ 
	{	CTRL_MEDIT,WS_VISIBLE | WS_BORDER |ES_READONLY | ES_AUTOWRAP | WS_VSCROLL,20, 10,212,200,IDC_TEXT_EDIT,"",0	},
	{  CTRL_BUTTON,WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP | WS_GROUP,255, 70, 35, 100,IDC_EXIT_BUTTON, "ÍË³ö",    0	},
};

static int TEXTProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static BITMAP bitmap;
	switch (message)
	{
		case MSG_INITDIALOG:
		{
			int fd,reallength;
			char dir[30] = "/mnt/sd/";
			char buffer[10000]={0};
			strcat(dir,Id_Type[lParam].path);
			printf("dir = %s\n",dir);
			fd = open(dir,O_RDONLY);
			if (fd <= 0)
        		{
   			MessageBox (hDlg, "Preview file failed","Warning", MB_OK | MB_ICONSTOP);
   			return FALSE;
        		 }
			if ((reallength=read(fd,buffer,10000)) >= 10000) 
   			MessageBox (hDlg, "Preview file failed","Error", MB_OK | MB_ICONEXCLAMATION);
  			close (fd);			
			SetWindowText (GetDlgItem (hDlg, IDC_TEXT_EDIT), buffer);
			bzero(buffer,sizeof(buffer));	
   		}        
		break;

		case MSG_COMMAND:
      	switch (wParam)
			{			        		
        		case IDC_EXIT_BUTTON:
           		EndDialog (hDlg, wParam);
        		break;
				return 1;
       			}		
    	break; 

    	case MSG_CLOSE:EndDialog(hDlg, wParam);break;		
		
		break;   
	}    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
static int JPG_Dialog(HWND hWnd,int id)
{
	DlgJPG.controls = CtrlJPG;    
	DialogBoxIndirectParam (&DlgJPG, hWnd, JPGProc, (LPARAM)id);
	printf("JPG\n");
	return 0;
}

static int BMP_Dialog(HWND hWnd,int id)
{
	DlgJPG.controls = CtrlJPG;    
	DialogBoxIndirectParam (&DlgJPG, hWnd, JPGProc, (LPARAM)id);
	printf("BMP\n");
	return 0;
}
static int TXT_Dialog(HWND hWnd,int id)
{
	DlgTEXT.controls = CtrlTEXT;    
	DialogBoxIndirectParam (&DlgTEXT, hWnd, TEXTProc, (LPARAM)id);
	printf("TXT\n");
	return 0;
}
static int DIR_Dialog(HWND hWnd,int id)
{
	char buf[30] = "/mnt/sd";
	strcat(buf,Id_Type[id].path);
	printf("Param =%d\n",buf);
	printf("buf = %s\n",buf);
	printf("DIR\n");
	return 0;
}

int OpenPanel(HWND hWnd,int id,int type)
{
	printf("type = %d\n",type);
  	switch(type)
  	{
        case  IDM_JPG:
            JPG_Dialog(hWnd,id);
        break ;

        case  IDM_BMP:
            BMP_Dialog(hWnd,id);
        break ;

        case  IDM_TXT:
            TXT_Dialog(hWnd,id);
        break ;

        case  IDM_DIR:
            DIR_Dialog(hWnd,id);
        break ;
       
        default:break;
  	}
}
