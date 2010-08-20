#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "panels.h"

static char *xmBmpName[14];
static char *xmTitle[14];

static SDXMBUTTON SDxmButton [BTM_NUM];
static FDKBUTTON fdkButton ;

///////////////////////////////////////////////////////////////////////////////////////////

int myLoadBitmap (BITMAP* bm, const char* filename)
{
    char full_path [MAX_PATH + 1];
    strcpy (full_path, "./sdres/");
    strcat (full_path, filename);
    return LoadBitmap (HDC_SCREEN, bm, full_path);
}


int SDWhichBox(int x,int y)
{
  	int i;
  	for (i=0;i<BTM_NUM;i++)
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
  	for (i=0;i<BTM_NUM;i++)
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
	switch (message)
    	{     
		case MSG_CREATE:
			//CreateWindow(CTRL_STATIC,"dfhdfh",WS_VISIBLE | WS_CHILD, IDC_VIEW_STATIC, 18, 195, 80, 30, hWnd, 0);			
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
            OpenPanel(hWnd,up,Id_Type[up].type);
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
        	for(i=0;i<BTM_NUM;i++)
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

      case MSG_CLOSE:
     		{
        	int i;
        	for(i=0;i<BTM_NUM;i++)
        		UnloadBitmap(&(SDxmButton[i].bitmap));
        		UnloadBitmap(&fdkButton.myBitmap);
        		DestroyMainWindow (hWnd);
        		PostQuitMessage (hWnd);
        return 0;
      		}
    	}
	return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

static void InitControlPanelInfo (PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_CAPTION | WS_BORDER |WS_VISIBLE ;
    pCreateInfo->dwExStyle =  WS_EX_IMECOMPOSE;;
    pCreateInfo->spCaption = "SD¿¨";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ControlPanelWinProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 320;//530;
    pCreateInfo->by = 240;//410;
    pCreateInfo->iBkColor = COLOR_lightgray;
    pCreateInfo->dwAddData = (DWORD)0;
    pCreateInfo->hHosting = HWND_DESKTOP;
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
	for(i=0;i< (k + l);i++)
	{
		printf("Id_Type[%d].path = %s\n",i,Id_Type[i].path);
	
	}
}

static int ControlPanel(void* data)
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;
    initBTMs("/mnt/sd");
	 printf("intbtm over\n");
    InitControlPanelInfo (&CreateInfo);
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

int MiniGUIMain (int args, const char* arg[])
{
	//rename("/etc/profile","/etc/profile_SD");	
	//rename("/etc/profile_POS","/etc/profile");
	ControlPanel(NULL);
	return 0;
}

#ifndef _LITE_VERSION
#include <minigui/dti.c>
#endif

