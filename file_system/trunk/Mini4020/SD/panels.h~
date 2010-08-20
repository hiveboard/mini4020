#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <popt.h>
#include <unistd.h>
#include <ctype.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mywindows.h>
#include <minigui/filedlg.h>
#include <minigui/mgext.h>

/////////////////////////////
//#define IDC_VIEW_STATIC		100 + 1
#define IDC_NULL_STATIC		1300 + 2
#define IDC_TEXT_EDIT		1300 + 3
#define IDC_EXIT_BUTTON		1300 + 4
//////////////////////////////

#define IDM_JPG    	3
#define IDM_BMP   	4
#define IDM_TXT     	2
#define IDM_DIR     	5
#define IDM_OTHERS	1


#define BTM_NUM 13
#define BTM_UP 1
#define BTM_FLAT 0
#define BTM_DOWN -1

#define TITLE_MAX 10
#define INFO_MAX 200

extern int OpenPanel(HWND,int,int);
extern void initBTMs(char *);
struct
{
	int 	id;
	int	type;
	char 	path[20];
}Id_Type[15];

struct
{
	int 	des;
	int	sou;
}Ref[15];

typedef struct
{
  	int x,y,w,h;
  	BITMAP bitmap;
  	int bmpx,bmpy;
  	char title[TITLE_MAX];
  	int titlex,titley;
  	char info[INFO_MAX];
  	int mode;//0:flat,1:up,-1:down
}XMBUTTON;

typedef struct
{
  	int x,y,w,h;
  	BITMAP *bitmap;
  	int bmpx,bmpy;
  	BITMAP myBitmap;
  	char *info;
  	char myInfo[INFO_MAX];
  	int infox,infoy,infow,infoh;
} FDKBUTTON;
