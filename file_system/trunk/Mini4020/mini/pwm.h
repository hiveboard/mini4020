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
#include <minigui/dti.c>


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
