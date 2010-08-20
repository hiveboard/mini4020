#include "time.h"
#include <stdio.h>									//定义输入／输出函数 
#include <stdlib.h>								 	//定义杂项函数及内存分配函数
#include <errno.h>								 	//定义错误码 
#include <string.h>									//字符串处理 
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <popt.h>
#include <ctype.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include <minigui/mywindows.h>
#include <minigui/filedlg.h>
#include <minigui/mgext.h>


///////////////////////////////////////////////////////////////menu
#define 	BMP_NUM 			25
#define	ID_TIMER			1020
#define 	TEXT_CELL1    	" "
#define 	TEXT_CELL2    	"时间:"
#define 	BTM_UP      	1
#define 	BTM_FLAT    	0
#define 	BTM_DOWN    	-1
#define 	BTM_NUM     	8

#define 	IDM_0				0
#define 	IDM_1        	1
#define 	IDM_2		   	2
#define 	IDM_3				3
#define 	IDM_4				4
#define 	IDM_5				5
#define 	IDM_6				6
#define 	IDM_7				7
////////////////////////////////////////////////////////////
typedef struct
{
	  int x,y,w,h;
	  int mode;															//0:flat,1:up,-1:down
}XMBUTTON;

struct HeadInfo 
{
    int l, t, r, b;													//(l,t)左上角坐标,(r,b)右下角坐标
    int fc;																//前景色
    int bc;																//背景色
    char szText [200];
};

static char* Message [] = 											//主窗口各项功能提示符
{
    "LED",
    "KEY",
    "MP3",
    "PSAM",
    "SD",
    "USB HOST",
    "SAME",
	 "PWM"
};

static char* Xinqi [] = 											//星期
{
    "日",
    "一",
    "二",
    "三",
    "四",
    "五",
    "六",
};
/////////////////////////////////////////////////////////////////////////////定义存储图片结构体
typedef struct
{
	char path[12];
	int number;
	BITMAP bitmap;
}BMP;

//////////////////////////////////////////////////////////////////////////////////////////LED	2009-09-11 09:18:55 
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
/////////////////////////////////////////////////////////////////////////////////////////KEY	2009-09-11 09:38:55 
#define IDC_KEY_STATIC			1100
#define IDC_KEY_EDIT				(IDC_KEY_STATIC + 1)
#define IDC_KEY_BUTTON			(IDC_KEY_STATIC + 2)
/////////////////////////////////////////////////////////////////////////////////////////Mp3	2009-09-11 09:48:57 
#define IDC_MP3_STATIC			1200
#define IDC_MP3_BUTTON			(IDC_MP3_STATIC + 1)
#define IDC_MP3_BUTTON1			(IDC_MP3_STATIC + 2)
////////////////////////////////////////////////////////////////////////////////////////PSAM	2009-09-11 10:30:37 
#define	 COLDRESET    0X11
#define  RECEIVE_LEN  0X12

#define IDC_PSAM_BUTTON_RESET			1300
#define IDC_PSAM_BUTTON_SELECT		(IDC_PSAM_BUTTON_RESET + 1)
#define IDC_PSAM_BUTTON_ENCRYPT		(IDC_PSAM_BUTTON_RESET + 2)
#define IDC_PSAM_STATIC					(IDC_PSAM_BUTTON_RESET + 3)
#define IDC_PSAM_MLEDIT_CHARS			(IDC_PSAM_BUTTON_RESET + 4)

///////////////////////////////////////////////////////////////////////////////////////SD	2009-09-11 10:58:43 

#define IDC_NULL_STATIC		1400 + 2
#define IDC_TEXT_EDIT		1400 + 3
#define IDC_EXIT_BUTTON		1400 + 4
//////////////////////////////

#define IDC_SD_EXIT			1400 + 5
#define IDM_JPG    	3
#define IDM_BMP   	4
#define IDM_TXT     	2
#define IDM_DIR     	5
#define IDM_OTHERS	1


#define SDBTM_NUM 10

#define TITLE_MAX 10
#define INFO_MAX 200

extern int SDOpenPanel(HWND,int,int,char*);										//对SD卡操作函数
extern int Same_Game (HWND);								
struct
{
	int 	id;
	int	type;
	char 	path[20];
}Id_Type[15];																		//保存图片格式和路径

typedef struct
{
  	int x,y,w,h;
  	BITMAP bitmap;
  	int bmpx,bmpy;
  	char title[TITLE_MAX];
  	int titlex,titley;
  	char info[INFO_MAX];
  	int mode;				//0:flat,1:up,-1:down
}SDXMBUTTON;																		//判断显示的各个图标

typedef struct
{
  	int x,y,w,h;
  	BITMAP *bitmap;
  	int bmpx,bmpy;
  	BITMAP myBitmap;
  	char *info;
  	char myInfo[INFO_MAX];
  	int infox,infoy,infow,infoh;
} FDKBUTTON;																		//

////////////////////////////////////////////////////////////////////////////pwm	2009-09-28 15:36:57 
#define IDC_PWM       			1050
#define IDC_PWM_STATICCHOICE	1050 + 1
#define IDC_PWM_COMBOBOX		1050 + 2
#define IDC_PWMPLAY_BUTTON		1050 + 3
#define IDC_PWMEND_BUTTON		1050 + 4

struct pwm_config																	
{
	unsigned long  pwm_ctrl;
	unsigned long  pwm_div;
	unsigned long  pwm_period;
	unsigned long  pwm_data;
} ;
extern int PWMPanel(HWND);																//对PWM操作函数	
