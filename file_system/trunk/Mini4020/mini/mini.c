#include "mini.h"

static pid_t pid;

static XMBUTTON xmButton [BTM_NUM];													//按钮信息
static BMP BitMap[BMP_NUM];															//声明一个结构体数组

static char *xmBmpName[SDBTM_NUM];
static char *xmTitle[SDBTM_NUM];

static SDXMBUTTON SDxmButton [SDBTM_NUM];
static FDKBUTTON fdkButton ;

void InitBMPLocation(void)																//初始化图标的坐标
{
	int i,j;
	for(i = 0;i< BTM_NUM ;i++)
	{
		xmButton[i].x = 3 + (i % 4) * 78;
		xmButton[i].y = 54 + (i / 4) * 62;
		xmButton[i].h = 54;
		xmButton[i].w = 73;
	}  
}

static int InitBMPs()																	//对所有图片进行初始化	2009-09-11 09:16:20 
{
    int i;
    struct dirent *e;
    char *dname = "res/";																//设置路径
    DIR *dir;
    char buff [30];
    char format[3];

   dir = opendir (dname);																//打开res目录

   if (!dir)return -1;

  	while ((e = readdir (dir)) != NULL)
	    {
    if (!strstr (e->d_name, ".bmp"))
		    {
 			    continue;
      		}
		for(i=1;i<BMP_NUM;i++)
		{
			sprintf(format,"%02d",i);
			if (strstr (e->d_name, format))
			{
 				break;
      			}
		}
 		strcpy (BitMap[i].path, dname);
		strcat (BitMap[i].path, e->d_name);
		BitMap[i].number = i;
    	}

	closedir (dir);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////SD	2009-09-11 10:42:57 
int myLoadBitmap (BITMAP* bm, const char* filename)
{
    char full_path [MAX_PATH + 1];
    strcpy (full_path, "./sdres/");
    strcat (full_path, filename);
    return LoadBitmap (HDC_SCREEN, bm, full_path);
}


int SDWhichBox(int x,int y)													//判断SD卡所选中的图标
{
  	int i;
  	for (i = 0;i < SDBTM_NUM;i++)
  	{
    	if ((x>SDxmButton[i].x) && (x<SDxmButton[i].x+SDxmButton[i].w) && (y>SDxmButton[i].y) && (y<SDxmButton[i].y+SDxmButton[i].h))
      return i;
 	}
  return -1;
}

int SDchangeMode(HWND hWnd,int x,int y,int mode)
{
  	int btm=SDWhichBox(x,y);
  	int i;
  	RECT rect;
  	static int lastbtm=-1;
  	static int lastmode=BTM_FLAT;
 	if ((btm==lastbtm)&&(mode==lastmode))
    	return btm;
  	for (i = 0;i < SDBTM_NUM;i++)
  	{
    	if(SDxmButton[i].mode==BTM_FLAT) continue;
    	SDxmButton[i].mode=BTM_FLAT;
    	rect.left=SDxmButton[i].x;
	   rect.right=SDxmButton[i].x+SDxmButton[i].w;
	   rect.top=SDxmButton[i].y;
	 	rect.bottom=SDxmButton[i].y+SDxmButton[i].h;
   	InvalidateRect(hWnd,&rect,FALSE);
  	}
  	if (btm!=-1)
  	{
    	SDxmButton[btm].mode=mode;
    	rect.left=SDxmButton[btm].x;
    	rect.right=SDxmButton[btm].x+SDxmButton[btm].w;
    	rect.top=SDxmButton[btm].y;
    	rect.bottom=SDxmButton[btm].y+SDxmButton[btm].h;
    	InvalidateRect(hWnd,&rect,FALSE);
    	fdkButton.info=SDxmButton[btm].info;
    	fdkButton.bitmap = & SDxmButton[btm].bitmap;
  	}
  	else
  	{
    	fdkButton.info = fdkButton.myInfo;
    	fdkButton.bitmap = &fdkButton.myBitmap;
  	}
  	rect.left = fdkButton.x;
  	rect.right= fdkButton.x+fdkButton.w;
  	rect.top= fdkButton.y;
  	rect.bottom= fdkButton.y+fdkButton.h;
  	if(btm!=lastbtm)
    	InvalidateRect(hWnd,&rect,FALSE);
  	lastbtm = btm;
  	lastmode = mode;
  	return btm;
}

int ControlPanelWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	static int lastdown=-1;
	static HWND hButton1;

	switch (message)
    	{     
		case MSG_CREATE:
			hButton1 = CreateWindow (CTRL_BUTTON,"exit1",WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,IDC_SD_EXIT,5, 200, 305, 16, hWnd, 0);
		break;		

		case MSG_COMMAND:
     		switch (wParam) 
			{
        		case IDC_SD_EXIT:
				{
           		int i;
#if 1
		        	for(i = 0; i < SDBTM_NUM; i++)
		        		UnloadBitmap(&(SDxmButton[i].bitmap));

	        		UnloadBitmap(&fdkButton.myBitmap);
#endif
	        		DestroyMainWindow (hWnd);
	        		//PostQuitMessage (hWnd);
				}
            break;
        		}
		break;
	
      case MSG_LBUTTONDOWN:
     	 	{
        	lastdown = SDchangeMode(hWnd,LOWORD(lParam),HIWORD(lParam),BTM_DOWN );
      		}
      break;   
                                                              
      case MSG_LBUTTONUP:
     		{
			
        	int up = SDchangeMode(hWnd,LOWORD(lParam),HIWORD(lParam),BTM_FLAT );
        	if(up==-1) break;
        	if(up==lastdown)
       			 {
				printf("Id_Type[%d].path = %s\n",up,Id_Type[up].path);
            SDOpenPanel(hWnd,up,Id_Type[up].type,(char*)GetWindowAdditionalData (hWnd));
       			 }
      		}
      break;    
                                              
      case MSG_MOUSEMOVE:
      		{
        	SDchangeMode(hWnd,LOWORD(lParam),HIWORD(lParam),BTM_UP );
      		}
      break;

		case MSG_PAINT:
      		{
       	HDC hdc;
        	RECT rc,rect;
        	int i;
       	hdc = BeginPaint (hWnd);

		  	GetWindowRect(hWnd,&rc);
        	Draw3DDownFrame (hdc, 3,  5,rc.right - rc.left - 9,rc.bottom - rc.top - 45,PIXEL_lightwhite);
        	for(i = 0;i < SDBTM_NUM;i++)
        		{
          	if(SDxmButton[i].mode == BTM_UP)
           	 	Draw3DUpFrame (hdc, SDxmButton[i].x,  SDxmButton[i].y ,SDxmButton[i].x+SDxmButton[i].w,SDxmButton[i].y+SDxmButton[i].h,PIXEL_lightwhite);
          	else  
					if(SDxmButton[i].mode == BTM_DOWN)
            		Draw3DDownFrame (hdc, SDxmButton[i].x,  SDxmButton[i].y ,SDxmButton[i].x+SDxmButton[i].w,SDxmButton[i].y+SDxmButton[i].h,PIXEL_lightwhite);
         			FillBoxWithBitmap (hdc, SDxmButton[i].x+SDxmButton[i].bmpx,SDxmButton[i].y+SDxmButton[i].bmpy,0, 0, &SDxmButton[i].bitmap);
         			TextOut(hdc,SDxmButton[i].x+SDxmButton[i].titlex,SDxmButton[i].y+SDxmButton[i].titley,SDxmButton[i].title);
        		}

        EndPaint (hWnd,hdc);
      		}
      break;

		case MSG_CHAR:
			if(wParam != '\033')break;
      case MSG_CLOSE:
     		{
        //	int i;
        //	for(i = 0;i < SDBTM_NUM;i++)
        //		UnloadBitmap(&(SDxmButton[i].bitmap));

        //		UnloadBitmap(&fdkButton.myBitmap);
        		DestroyMainWindow (hWnd);
        //		PostQuitMessage (hWnd);
        return 0;
      		}
    	}
	return DefaultMainWinProc (hWnd, message, wParam, lParam);
}


void initBTMs(char *dirpath)
{
	int i,j;
  	struct  dirent    *pDir;
	DIR               *dir;
	struct  stat      ftype;
	char              fullpath2 [PATH_MAX + NAME_MAX + 1];
	dir            = opendir (dirpath);
	int k=0,l=0;    

	memset(SDxmButton,0,sizeof(SDxmButton));
	while ( (pDir = readdir ( dir )) != NULL) 
	{	
		strncpy (fullpath2, dirpath, PATH_MAX);
		if ( fullpath2[ strlen(fullpath2) - 1 ] != '/' )
	        strcat (fullpath2, "/");

      strcat (fullpath2, pDir->d_name);	

		if (lstat (fullpath2, &ftype) < 0 )
		{
      	Ping();
       	continue;
        	}


		if ( S_ISDIR(ftype.st_mode) ) 
		{
			if(!(strcmp(pDir->d_name,".") && strcmp(pDir->d_name,"..")))continue;
			char *q = NULL,*p = NULL;
			q = (char*)malloc(20);
			memset(q,0,20);
			memcpy(q,"wenjianjia.bmp",strlen("wenjianjia.bmp"));
			xmBmpName[k+l] = q;
			
			p = (char*)malloc(20);
			memset(p,0,20);
			memcpy(p,pDir->d_name,strlen(pDir->d_name));
			xmTitle[k+l] = p;
			Id_Type[k+l].id = k +l;
			Id_Type[k+l].type = 5;
			printf("pDir = %s\n",pDir->d_name);
			strcpy(Id_Type[k+l].path,pDir->d_name);
			printf("file xmTitle[%d] =%s\n",k+l,xmTitle[k+l]);
			k++; 
        	}

		if ( S_ISREG(ftype.st_mode) ) 
		{
			printf("pDir->d_name = %s\n",pDir->d_name);
			char *p = NULL,*q = NULL;
			p = (char*)malloc(20);
			memset(p,0,20);
			memcpy(p,pDir->d_name,strlen(pDir->d_name));
			xmTitle[k+l] = p;

			strcpy(Id_Type[k+l].path,pDir->d_name);
			printf("Id_Type[k+l].path=%s\n",Id_Type[k+l].path);
			q = (char*)malloc(20);
			memset(q,0,20);
			if(strstr(pDir->d_name,".bmp")!=NULL)
			{
				Id_Type[k+l].id = k +l;
				Id_Type[k+l].type = 4;
				memcpy(q,"bmptu.bmp",strlen("bmptu.bmp"));
				xmBmpName[k+l] = q;
			}
			else 
				if(strstr(pDir->d_name,".jpg")!=NULL)
				{
					Id_Type[k+l].id = k +l;
					Id_Type[k+l].type = 3;
					printf("\nId_Type[%d]=%d\n",k+l,Id_Type[k+l].type);
					memcpy(q,"jpegtu.bmp",strlen("jpegtu.bmp"));
					xmBmpName[k+l] = q;					
				}
				else 
					if(strstr(pDir->d_name,".txt")!=NULL)
					{
						Id_Type[k+l].id = k +l;
						Id_Type[k+l].type = 2;
						memcpy(q,"note.bmp",strlen("note.bmp"));
						xmBmpName[k+l] = q;
					}
					else 
					{
						Id_Type[k+l].id = k +l;
						Id_Type[k+l].type = 1;
						memcpy(q,"ime.bmp",strlen("ime.bmp"));
						xmBmpName[k+l] = q;
					}
			l++;
        	}
		if((k + l) >= SDBTM_NUM)break;													//如果文件的个数大于10个，则退出	2009-09-23 14:15:20 
	}
   closedir(dir);

	SDXMBUTTON xmButtonTemp,xmButtonTemp1;
	char temp[20]={0},buf[20]={0};
	int intbuf;
	printf("k+l =%d\n",k+l);
#if 1
  	for(i=0;i< (k + l);i++)
	{
		for(j = i +1 ;j < (k + l); j++)
		{
			if(Id_Type[i].type < Id_Type[j].type)
			{		
				bzero(temp,20);
				strcpy(temp,xmBmpName [i]);


				strcpy(xmBmpName [i],xmBmpName [j]);
				strcpy(xmBmpName [j],temp);
				

				bzero(buf,20);
				strcpy(buf,xmTitle [i]);

				strcpy(xmTitle [i],xmTitle [j]);
				strcpy(xmTitle [j],buf);

				intbuf = Id_Type[i].type;
				Id_Type[i].type = Id_Type[j].type;
				Id_Type[j].type = intbuf;

				bzero(temp,20);
				strcpy(temp,Id_Type[i].path);
				strcpy(Id_Type[i].path,Id_Type[j].path);
				strcpy(Id_Type[j].path,temp);
			}
		}

	}
#endif	
  	for(i=0;i< (k + l);i++)
    	{  		
		printf("i =%d\n",i);
      SDxmButton[i].x= 10+(i%5)*55;//170 +
      SDxmButton[i].y=20+((int)(i/5))*80;//20 + 
      SDxmButton[i].h=60;
      SDxmButton[i].w=50;
      SDxmButton[i].bmpx=5;
      SDxmButton[i].bmpy=2;

      strcpy(SDxmButton[i].title,xmTitle[i]);
		printf("title = %s\n",xmTitle[i]);
      j=strlen(SDxmButton[i].title);
      SDxmButton[i].titlex=(8-j)*3;
      SDxmButton[i].titley=42;
      myLoadBitmap (&(SDxmButton[i].bitmap),xmBmpName [i]);
    	}
			printf("number is haha %d\n",k + l);
#if 0
	for(i=0;i< (k + l);i++)
	{
		printf("Id_Type[%d].path = %s\n",i,Id_Type[i].path);
	
	}
#endif
}

static void InitControlPanelInfo (PMAINWINCREATE pCreateInfo,HWND hWnd)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER |WS_VISIBLE ;
    pCreateInfo->dwExStyle =  WS_EX_IMECOMPOSE;;
   // pCreateInfo->spCaption = "SD卡";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlPanelWinProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 320;//530;
    pCreateInfo->by = 240;//410;
    pCreateInfo->iBkColor = COLOR_lightgray;
  //  pCreateInfo->dwAddData = (DWORD)0;
    pCreateInfo->hHosting = hWnd;
}
static int ControlPanel(HWND hWnd,char *name,char *path)
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;
    initBTMs(path);
	 printf("intbtm over\n");
    InitControlPanelInfo (&CreateInfo,hWnd);

	 CreateInfo.spCaption = name;
	 CreateInfo.dwAddData = (DWORD)path;

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return 0;

    while( GetMessage(&Msg, hMainWnd) )
         {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
         }

    MainWindowThreadCleanup(hMainWnd);
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////PSAM	2009-09-11 10:19:06 

static DLGTEMPLATE DlgPsam =
{
    WS_BORDER | WS_CAPTION,	WS_EX_NONE,0, 0, 320, 240,"PSAM CARD",	0,	0,	5,	NULL,	0
};

static CTRLDATA CtrlPsam [] =
{ 	{	CTRL_BUTTON,	WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,	40,	40,	80,	30,	IDC_PSAM_BUTTON_RESET,	"复位",	 0	},

	{	CTRL_BUTTON,	WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON,	40,	100,	80,	30,	IDC_PSAM_BUTTON_SELECT,	"选择文件", 	0	},

	{	CTRL_BUTTON,	WS_TABSTOP | WS_VISIBLE | BS_DEFPUSHBUTTON, 	40, 	160, 	80, 	30, 	IDC_PSAM_BUTTON_ENCRYPT, "3DES加密",	 0	},	

	{	CTRL_STATIC,	WS_VISIBLE | SS_CENTER,	180, 	40, 	100, 	30,	IDC_PSAM_STATIC,	"结果显示 :",	 0	},
	{	CTRL_MLEDIT,	WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_BASELINE | ES_AUTOWRAP,	180, 	70, 	100, 	120,	IDC_PSAM_MLEDIT_CHARS,	"",	0	},
};

static int DialogBoxPsamProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static char send[50] = {0};
	unsigned char i,msb,lsb,value,len,remainder;
	char high_level,low_level,j;
	char receive_buf[50] = {0};
	char receive_len[0];
	char receive[50] = {0};
	char len_buf[2] = {0};
	static int fd;

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
			fd = open("/dev/psam",O_RDWR);
			if(fd == -1)
			{
			 	printf("open psam error wrong\r\n");
			}
		return 1;

    	case MSG_CLOSE:		
			close(fd);	      
			EndDialog (hDlg, wParam);
      break;

		case MSG_COMMAND:			switch (wParam) 
			{
				case IDC_PSAM_BUTTON_RESET:
				{
					ioctl(fd, COLDRESET);
					ioctl(fd, RECEIVE_LEN, receive_len); 

					read(fd, receive_buf, receive_len[0]);

					for(j = 0;j < receive_len[0]; j++)
					{
						printf("0x%x ",receive_buf[j]); 	
					}
					printf("\n"); 	
					
					bzero(receive,50);
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
					SetWindowText(GetDlgItem (hDlg, IDC_PSAM_MLEDIT_CHARS),"\0");
					if(strlen(receive) < 2)strcpy(receive,"请确认已插入PSAM卡，如已经插入，请重新点击”复位“按钮");
					SetWindowText(GetDlgItem (hDlg, IDC_PSAM_MLEDIT_CHARS),receive);
				}
				break;
				case IDC_PSAM_BUTTON_SELECT:
				{
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
					SetWindowText(GetDlgItem (hDlg, IDC_PSAM_MLEDIT_CHARS),"\0");
					SetWindowText(GetDlgItem (hDlg, IDC_PSAM_MLEDIT_CHARS),receive);
				}					
				break;

				case IDC_PSAM_BUTTON_ENCRYPT:
				{
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
					SetWindowText(GetDlgItem (hDlg, IDC_PSAM_MLEDIT_CHARS),"\0");
					SetWindowText(GetDlgItem (hDlg, IDC_PSAM_MLEDIT_CHARS),receive);
				}
				break;				
			}
			break;
    	}	
  	return DefaultDialogProc (hDlg, message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////Mp3	2009-09-11 09:50:03 
static DLGTEMPLATE DlgMP3 =
{
	WS_BORDER | WS_CAPTION,WS_EX_NONE,	0,	0,	320,	240,	"MP3",	0,	0,	2,	 NULL,	0
};

static CTRLDATA CtrlMP3 [] =
{ 
	{	CTRL_BUTTON,WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON, BASE_X + 200,BASE_Y + 53, 60, 28,IDC_MP3_BUTTON, "ON",0	},

	{	CTRL_BUTTON,WS_TABSTOP | WS_VISIBLE | BS_PUSHBUTTON,BASE_X + 200, BASE_Y + 115, 60, 28, IDC_MP3_BUTTON1, "OFF", 0	},
};

static int DialogBoxMP3Proc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{	
	char LoopTimes[3] = {0};
	int i , j , k;
   switch (message) 
	{
   	case MSG_INITDIALOG:
			EnableWindow(GetDlgItem(hDlg,IDC_MP3_BUTTON1),FALSE);
		return 1;
        
		case MSG_COMMAND:			switch (wParam) 
			{
				case IDC_MP3_BUTTON:
					EnableWindow(GetDlgItem(hDlg,IDC_MP3_BUTTON1),TRUE);
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

		case MSG_CHAR:
        	switch (wParam)
			{
            case '\033':  				
            	EndDialog (hDlg, wParam);
            break;
       			}
		break;

		case MSG_PAINT:
		{
			HDC hdc;
			hdc=BeginPaint(hDlg);
			SetBkMode(hdc, BM_TRANSPARENT);

	   	TextOut(hdc,BASE_X + 12,BASE_Y + 59,"点击ON 开始播放音乐:");
	   	TextOut(hdc,BASE_X + 12,BASE_Y + 123,"点击OFF停止播放音乐:");

	    	EndPaint (hDlg, hdc);
		}
		break;

		case MSG_CLOSE:			      
			EndDialog (hDlg, wParam);
      break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////KEY	2009-09-11 00:49:47 
static DLGTEMPLATE DlgKey =
{
    WS_BORDER | WS_CAPTION,WS_EX_NONE, 0, 0, 320, 240,"键盘测试程序",		 0,	 0,	 3,	 NULL,	0
};
static CTRLDATA CtrlKey [] =
{ 
  	{	CTRL_STATIC,WS_VISIBLE | SS_SIMPLE,	BASE_X,	BASE_Y + 6,	200,	18,	IDC_KEY_STATIC,	"请按键",	 0	},
   {	CTRL_EDIT,WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_READONLY,	BASE_X,	BASE_Y + 36,	260,	24,	IDC_KEY_EDIT,	NULL,	0	},
   { CTRL_BUTTON,	WS_TABSTOP | WS_VISIBLE,	BASE_X,	100,	BASE_Y + 56,	25,	IDC_KEY_BUTTON,	"确定",	 0	}
};
static int DialogBoxKeyProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
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
			switch(LOWORD(wParam))
			{
				case IDC_KEY_BUTTON:EndDialog (hDlg, wParam);break;
				default:break;
			}								
     	break;

		case MSG_CHAR:
 			if(wParam == '\033')
          	EndDialog (hDlg, wParam);
			bzero(buffer,30);
			sprintf(buffer,"The character is: %c",wParam);
			printf("wparam = %c\n",wParam);
			SetWindowText(GetDlgItem(hDlg, IDC_KEY_EDIT),buffer);      
		break;

		case MSG_CLOSE:			      
			EndDialog (hDlg, wParam);
      break;
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////LED
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
	static int fd;
	int i , j , k;
   switch (message) 
	{
   	case MSG_INITDIALOG:
		#if 1
			fd = open("/dev/flowled",0);
			if(fd == -1)
			{
			  printf("wrong\r\n");
			  exit(-1);
			}
		#endif
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

		case MSG_CLOSE:	
			close(fd);		      
			EndDialog (hDlg, wParam);
      break;	
    }
    
    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

#if 1
///////////////////////////////////////////////////////////////////////////////////	2009-09-11 09:22:49 
static int PWM_Server(HWND hWnd)																					//SD卡实验
{	
	PWMPanel(hWnd);
   return 0;
}
#endif

#if 1
///////////////////////////////////////////////////////////////////////////////////	2009-09-11 09:22:49 
static int USB_Server(HWND hWnd)																					//SD卡实验
{	
	ControlPanel(hWnd,"USB host","/mnt/usb");
   return 0;
}
#endif

#if 1
///////////////////////////////////////////////////////////////////////////////////	2009-09-11 09:22:49 
static int SD_Server(HWND hWnd)																					//SD卡实验
{	
	ControlPanel(hWnd,"SD卡","/mnt/sd");
   return 0;
}
#endif
///////////////////////////////////////////////////////////////////////////////////	2009-09-11 09:22:49 
static int Psam_Server(HWND hWnd)																				//PSAM卡实验
{	
	DlgPsam.controls = CtrlPsam;
    	
   DialogBoxIndirectParam (&DlgPsam, hWnd, DialogBoxPsamProc, 0L);

   return 0;
}

///////////////////////////////////////////////////////////////////////////////////	2009-09-11 09:22:49 
static int Mp3_Server(HWND hWnd)																					//音频实验
{	
	DlgMP3.controls = CtrlMP3;
    	
   DialogBoxIndirectParam (&DlgMP3, hWnd, DialogBoxMP3Proc, 0L);

   return 0;
}

////////////////////////////////////////////////////////////////////////////////////	2009-09-11 09:22:54 
static int Key_Server(HWND hWnd)																					//键盘实验
{	
	DlgKey.controls = CtrlKey;
    	
   DialogBoxIndirectParam (&DlgKey, hWnd, DialogBoxKeyProc, 0L);

   return 0;
}

///////////////////////////////////////////////////////////////////////////////////	2009-09-11 09:22:58 
static int LED_Server(HWND hWnd)																				//流水灯实验
{	
   DlgLED.controls = CtrlLED;
   DialogBoxIndirectParam (&DlgLED, HWND_DESKTOP, DialogBoxLEDProc, 0L);

   return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
static int semphore = 0;
void OpenPanel(HWND hWnd,int id)
{
	RECT rect;
	if(semphore == 1)return;
	semphore = 1;
	switch(id)
  	{
		case  IDM_0:
      	LED_Server (hWnd);
			printf("It's time to exit\n");
      break ;

		case  IDM_1:
      	Key_Server (hWnd);
      break ;

		case  IDM_2:
      	Mp3_Server (hWnd);
      break ;

		case  IDM_3:
      	Psam_Server (hWnd);
      break ;

		case  IDM_4:
			semphore = 0;
      	SD_Server (hWnd);
      break ;

		case  IDM_5:
			semphore = 0;
      	USB_Server (hWnd);
      break ;
		
		case  IDM_6:
			printf("oneone\n");
      	Same_Game (hWnd);
      break ;
	
		case  IDM_7:
      	PWM_Server (hWnd);
      break ;
		
		default:break;
  	}
	semphore = 0;
	InvalidateRect(hWnd,&rect,FALSE);
}

static void DrawMessage(HDC hdc,HDC hdc_client,int i)
{
	RECT rect;
	SetPenColor (hdc, RGB2Pixel (HDC_SCREEN, 0x00, 0x00, 0x00));
	SetBkColor (hdc,RGB2Pixel (HDC_SCREEN, 0xFF, 0xFF, 0xBF));
	SetTextColor (hdc, RGB2Pixel (HDC_SCREEN, 0x00, 0x00, 0x00));
	SetBrushColor (hdc, RGB2Pixel (HDC_SCREEN, 0xFF, 0xFF, 0xBF));
	int j;
	for(j = 0;j < 8 ;j++)
	{
		if(j < 4)
			BitBlt (hdc_client, xmButton[j].x+23, xmButton[j].y - 20,strlen(Message[j]) * 8 +7, 20, hdc, xmButton[j].x+23, xmButton[j].y - 20,0);
		else
			BitBlt (hdc_client, xmButton[j].x+23, xmButton[j].y + 55,strlen(Message[j]) * 8 +7, 22, hdc, xmButton[j].x + 23, xmButton[j].y + 55,0);
	}
	if(i < 4)
	{
		FillBox (hdc, xmButton[i].x + 23, xmButton[i].y - 20, strlen(Message[i]) * 8 +5, 18); 
   	Rectangle (hdc, xmButton[i].x + 23, xmButton[i].y - 20, xmButton[i].x + strlen(Message[i]) * 8 + 28, xmButton[i].y - 2);
		TextOut (hdc, xmButton[i].x + 25, xmButton[i].y - 18, Message[i]);
	}
	else if(i != -1)
	{
		FillBox (hdc, xmButton[i].x + 23, xmButton[i].y + 55, strlen(Message[i]) * 8 +5, 18); 
		Rectangle (hdc, xmButton[i].x + 23, xmButton[i].y + 55, xmButton[i].x + strlen(Message[i]) * 8 + 28, xmButton[i].y + 74);
		TextOut (hdc, xmButton[i].x + 25, xmButton[i].y + 58, Message[i]);
	}
}

static void DrawTextFrame (HDC hdc, struct HeadInfo* info)
{
    SetPenColor (hdc, COLOR_lightgray);
    Rectangle (hdc, info->l, info->t, info->r, info->b);
    SetBrushColor (hdc, info->bc);
    FillBox (hdc, info->l + 1, info->t + 1, info->r - info->l - 1, info->b - info->t - 1); 
    SetBkColor (hdc, info->bc);
    SetTextColor (hdc, info->fc);
    TextOut (hdc, info->l + 2, info->t + 2, info->szText);
}


int WhichBox(int x,int y)																				//指定鼠标所在的区域
{
  	int i;
  	for (i=0;i<BTM_NUM;i++)
  	{
    	if ((x>xmButton[i].x)&&(x<xmButton[i].x+xmButton[i].w)&&(y>xmButton[i].y)&&(y<xmButton[i].y+xmButton[i].h))
      	return i;
  	}
  	return -1;
}
//每次执行这个函数都要对全部的按键进行重新处理，且首先将所有的按键置为平的状态，然后再对指定的按键进行再次处理
int changeMode(HWND hWnd,int tabstop,int mode)
{
	int btm=tabstop;																				//返回所受触发的按键编号
  	int i;
  	RECT rect;
  	static int lastbtm=-1;
  	static int lastmode=BTM_FLAT;
  	if ((btm==lastbtm)&&(mode==lastmode))													//如果为同一个按钮或者没有触发则返回
    	return btm;

  	for (i=0;i< BTM_NUM;i++)																	//寻找受触发的按键值
  	{
    	if(xmButton[i].mode==BTM_FLAT) continue;
    	xmButton[i].mode=BTM_FLAT;
    	rect.left=xmButton[i].x - 150;
    	rect.right=xmButton[i].x+xmButton[i].w+130;
    	rect.top=xmButton[i].y - 80;
    	rect.bottom=xmButton[i].y+xmButton[i].h+100;
    	InvalidateRect(hWnd,&rect,FALSE);
  	}
	if (btm!=-1)																					//对按键进行重新处理
  	{
    	xmButton[btm].mode=mode;
    	rect.left=xmButton[btm].x;
    	rect.right=xmButton[btm].x+xmButton[btm].w+130;
    	rect.top=xmButton[btm].y - 50;
    	rect.bottom=xmButton[btm].y+xmButton[btm].h+50;
    	InvalidateRect(hWnd,&rect,FALSE);
    	}
  	lastbtm = btm;
  	lastmode = mode;
  	return btm;
}

static DLGTEMPLATE DlgBoxInputLen =
{
    WS_BORDER | WS_VISIBLE | WS_CAPTION, WS_EX_NONE, 0, 0, 320, 240, "MENU",0,0, 1, NULL,  0
};

static CTRLDATA CtrlInputLen [] =
{
    { CTRL_STATIC, WS_VISIBLE |SS_RIGHT, 0,1,7,1, IDC_PSAM_STATIC, "",  0  }
};

static int InputLenDialogBoxProc (HWND hDlg, int message, WPARAM wParam, LPARAM lParam)
{
	static int lastdown=-1,up;
	static int tabstop = -1 , counter = 0 ,timeout = -1 ,backtabstop = -1;
	int i = 0;
	RECT rect;

	static HDC hdc,hdc_client;

	static struct HeadInfo cells[2] = {{20,  0, 40, 20},{50, 195, 320, 220}};
	switch (message)
	{
		case MSG_INITDIALOG:
			for (i = 0; i < 2; i++) 
			{
            strcpy (cells [i].szText, "  无");
            cells [i].fc =  COLOR_cyan;
            cells [i].bc = RGB2Pixel (HDC_SCREEN, 0x61, 0x91, 0x4C);
        		}
        	SetTimer (hDlg, ID_TIMER, 100);

			if (LoadBitmap (hdc, &(BitMap[20].bitmap), BitMap[20].path))return 1;

			hdc_client = CreateCompatibleDC(hdc);
	    	FillBoxWithBitmap (hdc_client,0, 0, 0, 0, &(BitMap[20].bitmap));

			for( i = 1;i <= BTM_NUM ;i++)
				LoadBitmap (hdc, &(BitMap[i].bitmap), BitMap[i].path);

		break;

		case MSG_MOUSEMOVE:
    		 {
			//printf("\ncounter = %d\n",counter);
			tabstop = WhichBox(LOWORD(lParam),HIWORD(lParam));
			//printf("tabstop11 = %d\n",tabstop);
			if(tabstop == -1)	break;
			else
				if(counter == 0)
				{	
					counter++;			
					backtabstop = tabstop;
					changeMode(hDlg,tabstop,BTM_UP );
				}
				else
					if((backtabstop == tabstop) && (timeout == 1))
					{
						OpenPanel(hDlg,tabstop);
						counter = 0;
						timeout = 0;
						tabstop = -1;
					}
					else
					{
						changeMode(hDlg,tabstop,BTM_UP );
						counter = 1;
						backtabstop = tabstop;
					}
/*
			changeMode(hDlg,tabstop,BTM_UP );
			if(counter >= 1 && tabstop != -1 && timeout == 1)
			{
				OpenPanel(hDlg,tabstop);
				timeout = 0;
				counter = 0;
			}
*/
		  }
			break;
#if 1
		case MSG_KEYDOWN:
			switch(LOWORD(wParam))
			{
				case SCANCODE_TAB :
					tabstop = (tabstop + 1) % BTM_NUM;
					printf("tabstop =%d\n",tabstop);
					changeMode(hDlg,tabstop,BTM_UP );
				break;

				case SCANCODE_ENTER : 
					lastdown = changeMode(hDlg,tabstop,BTM_DOWN );
				break;
			}
		break;
	  	
		case MSG_KEYUP:
			switch(LOWORD(wParam))
			{
				case SCANCODE_ENTER : 
					up = changeMode(hDlg,tabstop,BTM_FLAT );
					if(up==-1) break;
      			if(up==lastdown)
       					{
						printf("up=%d   lastdown=%d\n",up,lastdown);
						//tabstop = -1;
            		OpenPanel(hDlg,up);
						changeMode(hDlg,tabstop,BTM_UP );
        				}
				break;
			}
		break;
#endif
#if 0
    	case MSG_CLOSE:
			for( i = 1;i <= BTM_NUM ;i++)
				UnloadBitmap(&(BitMap[i].bitmap));  
			UnloadBitmap(&(BitMap[20].bitmap));           
			EndDialog (hDlg, wParam);
      break;		
#endif
		case MSG_ERASEBKGND:
		{
			HDC hdc ;   
			hdc = GetClientDC (hDlg);	 
	    	FillBoxWithBitmap (hdc,0, 0, 0, 0, &(BitMap[20].bitmap));
			ReleaseDC (hdc);
	    	return 0;	
		}
		break;

		case MSG_PAINT:
    		{
       	//HDC hdc;
        	RECT rcClient,rect;
			struct HeadInfo title;
        	hdc = BeginPaint (hDlg);

			title.fc = title.bc =  COLOR_cyan; title.bc = RGB2Pixel (HDC_SCREEN, 0x61, 0x91, 0x4C);

        	title.l = 0; title.t = 195; title.r = 320; title.b = 220;

        	if (RectVisible (hdc, (RECT*)(&title))) 
			{
            strcpy (title.szText, TEXT_CELL2);
            DrawTextFrame (hdc, &title); 
        		}

			for (i = 1; i < 2; i++) 
			{
            if (RectVisible (hdc, (RECT*)(cells + i)))
                DrawTextFrame (hdc, cells + i);
        		}

			for(i = 0;i < BTM_NUM;i++)
			{
				FillBoxWithBitmap (hdc, xmButton[i].x, xmButton[i].y,xmButton[i].w, xmButton[i].h, &(BitMap[i +1].bitmap));
				SetPenColor (hdc, RGB2Pixel (HDC_SCREEN, 0x84, 0x82, 0x84));
				Rectangle (hdc,  xmButton[i].x,  xmButton[i].y,xmButton[i].x+xmButton[i].w,xmButton[i].y+xmButton[i].h);		
			}
			for(i = 0;i < BTM_NUM;i++)
			{

				if(xmButton[i].mode == BTM_UP)															//如果有建移动到此区域，则显示矩形框
				{
					int row,col;
					for(row = 0;row < xmButton[i].w;row++ )
						for(col = 0;col < xmButton[i].h;col ++)
						{
							SetPixelInBitmap(&(BitMap[i +1].bitmap),row,col,GetPixelInBitmap (&(BitMap[i +1].bitmap), row, col) ^ 0xFFFFFF );
						}
					DrawMessage(hdc,hdc_client,i);
					FillBoxWithBitmap (hdc, xmButton[i].x , xmButton[i].y ,xmButton[i].w, xmButton[i].h, &(BitMap[i +1].bitmap));

					for(row = 0;row < xmButton[i].w;row++ )
						for(col = 0;col < xmButton[i].h;col ++)
						{
							SetPixelInBitmap(&(BitMap[i +1].bitmap),row,col,GetPixelInBitmap (&(BitMap[i +1].bitmap), row, col) ^ 0xFFFFFF );
						}	

				}
          	else
					if((xmButton[i].mode == BTM_DOWN) && i < BTM_NUM)											//如果有键按下，则显示3D效果
					{
						//DrawMessage(hdc,i);
 						Draw3DDownFrame (hdc, xmButton[i].x,  xmButton[i].y , xmButton[i].x+xmButton[i].w,xmButton[i].y+xmButton[i].h,PIXEL_blue);
	
						FillBoxWithBitmap (hdc, xmButton[i].x + 1, xmButton[i].y + 1,xmButton[i].w, xmButton[i].h, &(BitMap[i +1].bitmap));
					}
				}

        	EndPaint (hDlg,hdc);
    		}
		break;

		case MSG_TIMER:
			if (wParam == ID_TIMER) 
			{
				timeout = 1;
				time_t t;
            struct tm* tm;            
            time (&t);
            tm = localtime (&t);
            sprintf (cells[1].szText, 
                " %d年%02d月%02d日 %02d:%02d:%02d   星期%s", 
                tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, 
                tm->tm_hour, tm->tm_min, tm->tm_sec, 
                Xinqi[tm->tm_wday]);
				//printf("shijian =%s\n",cells[1].szText);
            InvalidateRect (hDlg, (RECT*)(cells + 1), FALSE);
         }
         break;
      return 0;
    }

    return DefaultDialogProc (hDlg, message, wParam, lParam);
}

static void InputLenDialogBox (HWND hWnd)
{
    DlgBoxInputLen.hIcon = LoadIconFromFile (HDC_SCREEN,"res/mill.ico", 0);
    printf("conset =%ld\n",DlgBoxInputLen.hIcon);
    DlgBoxInputLen.controls = CtrlInputLen;
    DialogBoxIndirectParam (&DlgBoxInputLen, hWnd, InputLenDialogBoxProc, 0L);
}
///////////////////////////////////////////////////////////////////////////////////end ChinaUnionPay
int MiniGUIMain (int argc, const char* argv[])
{
#ifdef _LITE_VERSION
        SetDesktopRect(0, 0, 1024, 768);
#endif
 
	InitBMPs();
   InitBMPLocation();
	if (!InitMiniGUIExt()) 
	{
        return 2;
    	}
	InputLenDialogBox (HWND_DESKTOP);

	MiniGUIExtCleanUp ();
   return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif
