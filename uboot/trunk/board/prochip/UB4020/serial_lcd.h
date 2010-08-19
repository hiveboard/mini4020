#ifndef DM9161AE_H
#define DM9161AE_H




void display_map(unsigned char*p);
void LCDContrast(u8 Level);
//设置坐标函数
void LCDSetXY(int x, int y);
//初始化LCDC控制器
void init_lcdc(void);






#endif
