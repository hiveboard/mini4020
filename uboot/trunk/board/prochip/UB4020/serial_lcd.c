/* board/prochip/UB4020/serial_lcd.c
 *
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	E-mail:fpmystar@gmail.com
*
*	sep4020 128x64 lcd driver for SZ xzx.
*
*	Changelog:
*		2-NOV-2009 	FP	Initial version 	
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/
#include <common.h>
#include <asm/arch/hardware.h>



#define SET_RD(value)  {if (!value) *(volatile unsigned int*)GPIO_PORTB_DATA&=~0x0001;else *(volatile unsigned int*)GPIO_PORTB_DATA|=0x0001;}  //PB0

#define SET_WR(value) {if (!value) *(volatile unsigned int*)GPIO_PORTB_DATA&=~0x0002;else *(volatile unsigned int*)GPIO_PORTB_DATA|=0x0002;}   //PB1

#define SET_A0(value)  {if (!value) *(volatile unsigned int*)GPIO_PORTB_DATA&=~0x0004;else *(volatile unsigned int*)GPIO_PORTB_DATA|=0x0004;}  //PB2

#define SET_RES(value)  {if (!value) *(volatile unsigned int*)GPIO_PORTB_DATA&=~0x0008;else *(volatile unsigned int*)GPIO_PORTB_DATA|=0x0008;} //PB3

#define SET_CS(value)  {if (!value) *(volatile unsigned int*)GPIO_PORTB_DATA&=~0x0010;else *(volatile unsigned int*)GPIO_PORTB_DATA|=0x0010;}  //PB4


#define  MAX_X  128			//定义屏的宽度
#define  MAX_Y  64			//定义屏的高度

unsigned long LCD_ram[MAX_X*(MAX_Y/8)];

/****************************************COMMON OPERATE(FROM HERE)****************************************/
void data_send(unsigned char dat)
{
	unsigned char s,temp;
	int i;
	SET_WR(1);
	udelay(5);
	s=dat;
	for(i=0;i<8;i++)
	{
		SET_WR(0);
		udelay(5);
		temp = s & 0x80;
		if(temp)
		{ 
			*(volatile unsigned long *)GPIO_PORTB_DATA |= 0x01;
		}
		else 
		{
			*(volatile unsigned long *)GPIO_PORTB_DATA &= ~0x01;
		}
		udelay(5);
		SET_WR(1);
		udelay(5); 
		s=s<<1;
	}
}

void datasend(unsigned char dat)
{
	unsigned char s,temp;
	int i;
	SET_WR(1);
	udelay(4);
	s=dat;
	for(i=0;i<8;i++)
	{
		SET_WR(0);
		udelay(4);
		temp=s & 0x80;
	if(temp)
	{ 
		*(volatile unsigned long *)GPIO_PORTB_DATA |= 0x01;
		udelay(4);
		SET_WR(1);
		udelay(4); 
		s=s;
	}
	else 
	{
		*(volatile unsigned long *)GPIO_PORTB_DATA &= ~0x01;
	}
	udelay(4);
	SET_WR(1);
	udelay(4);     
	}
}
//写命令，串行模式，A0＝0为命令
void w_com(unsigned char x)
{
  SET_A0(0);
  SET_CS(0);
  data_send(x);
}
//写数据，串行模式，A0＝1为数据
void w_data(unsigned char dat)
{
   SET_A0(1);
   SET_CS(0);
   data_send(dat);
}
void wdata(unsigned char dat)
{
   SET_A0(1);
   SET_CS(0);
   datasend(dat);
}

//初始化LCD控制的GPIO口
void init_gpio(void)
{
   *(volatile unsigned long *)GPIO_PORTB_SEL |= 0x1f;
   *(volatile unsigned long *)GPIO_PORTB_DIR &= ~0x1f;		//片选
  
}

void display_map(unsigned char*p)

{
   unsigned char seg;
   unsigned char page;
   for(page=0xb0;page<0xb8;page++) //写页地址共8 页 0xb0----0xb8
   {
     w_com(page);
     w_com(0x10); //列地址，高低字节两次写入，从第0 列开始
     w_com(0x00);
     for(seg=0;seg<128;seg++)//写128 列
     { w_data(*p++); }
     
   }
}

//--------------------------------------------------------------------------
//调节对比度
//--------------------------------------------------------------------------
void LCDContrast(u8 Level)
{
	u8 Num,Temp1,Temp2;
	Temp1 = (Level/16)<<4;
	switch(Level%16)
	{
	case 10: 
		Temp2 = 0x0a;
		break;
	case 11: 
		Temp2 = 0x0b;
		break;
	case 12: 
		Temp2 = 0x0c;
		break;
	case 13: 
		Temp2 = 0x0d;
		break;
	case 14: 
		Temp2 = 0x0e;
		break;
	case 15: 
		Temp2 = 0x0f; 
		break;
	default:
		Temp2 = Level%16;
		break;
	}
	Num = Temp1|Temp2;
	w_com(0x81);
	w_com(Num);
}

//设置坐标函数
void LCDSetXY(int x, int y)
{
	w_com(0xB0|y);
	w_com(0x10|(x>>4));
	w_com((0x00|x)&0x0f);
}
//初始化LCDC控制器
void init_lcdc(void)
{
	int x,nX,nY;
	init_gpio();
	SET_RES(1);
	SET_CS(1);
	SET_WR(1);
	SET_RD(1);	
	SET_RES(0);
	udelay(4);
	SET_RES(1);
	udelay(4);	
	SET_CS(0);
	SET_CS(1);
	w_com(0xA2);
	w_com(0xA0);
	w_com(0xC8);
   	w_com(0x5F);
	w_com(0x26);
	w_com(0x81);
	w_com(0x10);
	w_com(0x2F);
	udelay(10);
	w_com(0xAF);
/****************调节对比度************************/
	LCDContrast(20);
	w_com(0xF8);
	w_com(0x00);

	 
   for(x=0;x<128*8;x++)
		LCD_ram[x]=0; 
   for(nY=0;nY<8;nY++)
	{
		LCDSetXY(0,nY);    
		for(nX=0;nX <= 128;nX++)
		{
        w_data (0x0);
		}
	}        
}

/****************************************DRAW LINE(FROM HERE)****************************************/
void SetPixelLine(unsigned char x,unsigned char y,unsigned char dotnum,unsigned char mode,unsigned char frame)
{
	unsigned char B0,R,Rnum;
	unsigned char drawchar,attribute,lcdchar,achar,lcdcharbak;
	if ((x>=131) ||(y>63))
		return;
	B0=y>>3;
	R=y &0x07;
	LCDSetXY(x,B0); 
	Rnum=dotnum &0x07;
	switch(frame &0x03) 
	{
		case 0:
			drawchar=0x0;
			break; 
		case 0x01:
			drawchar=0x0;
			if(Rnum<5)
				drawchar=0x80;
			break;   	   
		case 0x02:
			drawchar=0x80;
			break;
		case 0x03:
			drawchar=0x80;
			break;
		default:
		udelay(2);
	}
	attribute=1;
	switch(mode) 
	{
		case 0:
			attribute=0;
			break;
		case 1: 	   
			lcdchar=drawchar;
			break;
		case 2: 	   
			lcdchar=drawchar ;
			break;
		case 3: 	   
			lcdchar=drawchar;
			break;
		default:
 	   udelay(2);
	}
	if (attribute==1)
	{
		achar=0x01<<R;      
		if (lcdchar !=0)         	     	  
			lcdcharbak=achar;     	   
		else
		{           
			achar=~achar;
			lcdcharbak=achar;	
		}
		w_data(achar);
	}	
return;
}


void Set_PixelLine(unsigned char x,unsigned char y,unsigned char dotnum,unsigned char mode,unsigned char frame)
{
	unsigned char B0,R,Rnum;
	unsigned char drawchar,attribute,lcdchar,achar,lcdcharbak;
	if ((x>=131) ||(y>63))
		return;
	B0=y>>3;
	R=y &0x07;
	LCDSetXY(x,B0);
	Rnum=dotnum &0x07;

	switch(frame &0x03) 
	{
		case 0:
			drawchar=0x0;
			break; 
		case 0x01:
			drawchar=0x0;
			if(Rnum<5)
				drawchar=0x80;
			break;   	   
		case 0x02:
			drawchar=0x80;
			break;
		case 0x03:
			drawchar=0x80;
			break;
		default:
		udelay(2);
	}
	attribute=1;
	switch(mode) 
	{
		case 0:
			attribute=0;
			break;
		case 1: 	   
			lcdchar=drawchar;
			break;
		case 2: 	   
			lcdchar=drawchar ;
			break;
		case 3: 	   
			lcdchar=drawchar;
			break;
		default:
		udelay(2);
	}
	if (attribute==1)
	{
		achar=0x01<<R;      
		if (lcdchar !=0)       	     	  
			lcdcharbak=achar;     	   
      else
		{           
			achar=~achar;
			lcdcharbak=achar;	
		}
      wdata(achar);
	}	
return;
}
//画水平线
void LcdGraphicVert(unsigned char xPos,unsigned char yPos,unsigned char LineHigh,unsigned char mode,unsigned char frame)
{
	unsigned char i;
	for (i=0;i<LineHigh;i++)
		Set_PixelLine(xPos,yPos+i,i,mode,frame);
 	return;
}
//画垂直线
void LcdGraphicHorz(unsigned char xPos,unsigned char yPos,unsigned char LineWidth,unsigned char mode,unsigned char frame)
{
 	unsigned char i;
 	for (i=0;i<LineWidth;i++)
		SetPixelLine(xPos+i,yPos,i,mode,frame);
	return;
}

/****************************************SHOW CHINESE＆ASCII(FROM HERE)****************************************/
void LcdGraphicFont(unsigned char xPos,unsigned char yPos,unsigned char Matrwidth,unsigned char Matrhigh,unsigned char *FontZip,unsigned char mode)
{
 unsigned char i,j,B0,B1,R,R1u,R2u,k,Matrixchar,achar,tempup,lcdchar;
 unsigned char *Matrptr;
 B0=yPos>>3;
 B1=(yPos+Matrhigh)>>3;
 R=yPos &0x07;
 R1u=(yPos+Matrhigh) &0x07;
 R2u=Matrhigh>>3;
 if ((Matrhigh &0x07) !=0)
         R2u++;
 k=0;         
 Matrptr = FontZip;
 if (B0==B1) {       
            LCDSetXY(xPos,B0);
            for (i=1;i<Matrwidth;i++)
            {
               LCDSetXY(B0,xPos+i);     
               Matrixchar=*Matrptr;
               Matrixchar=Matrixchar<<R;
               achar=0xff>>(8-R-Matrhigh);
               Matrixchar=Matrixchar &achar; //新的填充数据
               lcdchar=Matrixchar;
               w_data(lcdchar);

		       Matrptr++;			   
            } // end of for
 }// end of if
 else {
         //B0
         LCDSetXY(xPos,B0);
         for (i=0;i<Matrwidth;i++){  
         	   LCDSetXY(xPos+i,B0);              
               Matrixchar=*Matrptr<<R;                              
               lcdchar=Matrixchar;
               w_data(lcdchar);
               
               Matrptr++;
                }
		k++;
        for (j=B0+1;j<B1;j++){
		  LCDSetXY(xPos,j);
		  for (i=0;i<Matrwidth;i++){                                                  
              Matrixchar=*Matrptr;
              Matrixchar=Matrixchar<<R;
                       
              tempup=*(Matrptr-Matrwidth);
              tempup=tempup>>(8-R);
                       
              lcdchar=tempup |Matrixchar;
              LCDSetXY(xPos+i,j);
              w_data(lcdchar);
                       
              Matrptr++;                    
          }
          k++;
		}	               
	if (R1u!=0){		    
		LCDSetXY(B1,xPos);   
        for (i=0;i<Matrwidth;i++){      
            LCDSetXY(xPos+i,B1);        
            tempup=*(Matrptr-Matrwidth);
            tempup=tempup>>(8-R);
               
			if (k != R2u){ //此情况为：最后一个page里为上次未写完的字节加本次要写的字节加剩余未覆盖的字节
				Matrixchar =* Matrptr;
               	Matrixchar=Matrixchar<<(8-(Matrhigh &0x07));
               	Matrixchar=Matrixchar>>(8-R-(Matrhigh &0x07));
            }
            else {
			    Matrixchar=0;
			}			               
            lcdchar=Matrixchar |tempup;  
            w_data(lcdchar);   
			Matrptr++;			   
		} // end of for
	} // end of if
   } // end of else
 return;
}


/*
void LcdUnzipZK(unsigned long offset,unsigned char num)
{
 	unsigned char i,total,oneline, zkchar;
 	unsigned char *ptr;
	ptr = FontUnzip;
 	total = 3*num;
 	oneline = 2*num;
 	for (i=0; i<total; i++)
	{
 		if (num ==3)
 			zkchar = ASCIIku[offset];
 		else
 			zkchar = HanziKu[offset];
 			
 		offset++;
 		if (i < oneline)
		{
 			*ptr = zkchar;
 			ptr++;
 		}
 		else 
		{
 			*ptr = zkchar & 0x0f;
 			ptr++;
 			zkchar >>= 4;
 			*ptr = zkchar & 0x0f;
 			ptr++;
 		} 
	 } 
 		 
	return;
}

unsigned char LcdTextChinese(unsigned char xPos,unsigned char yPos,unsigned short ChineseCode)
{
	unsigned char msb,lsb,qh,wh,achar,Hanzk_state;
 	unsigned long offset,Hanzk_visual_addr;
	lsb = ChineseCode & 0x00ff;
	msb = ChineseCode>>8;
	if (msb>128) 
	{
		if (lsb>128) 
		{
			Hanzk_state = 1;
			qh = msb - 0xa1;
			wh=lsb-0xa1;
			offset=94*qh+wh;
			offset=offset*18;	
			qh++;
			if (qh==1 || qh==2 || qh==3)			      
				Hanzk_visual_addr=Hanzk_addr;	
			if (qh==4 || qh==5)
				Hanzk_state=0;
			if (qh==6)
				Hanzk_visual_addr=Hanzk_sec6_offset;
			if (qh==7 || qh==8)
				Hanzk_state=0;
			if (qh==9)
				Hanzk_visual_addr=Hanzk_sec9_offset;                       	
			if (qh>9 && qh<16)
				Hanzk_state=0;
			if (qh>=16)
				Hanzk_visual_addr=Hanzk_han_offset;           		
			if (Hanzk_state==0)
				offset=Hanzk_space;
			if (Hanzk_state==1)      	  		 	                                 
	        	offset=offset+Hanzk_visual_addr;                       	   
			LcdUnzipZK(offset,6);  //查找中文字符点阵
			LcdGraphicFont(xPos,yPos,12,12,FontUnzip,15);
		}
		xPos=xPos+13;
	}
	else 
	{ 
		achar=0; 
	}     
	return achar;
}

unsigned char LcdTextChar(unsigned char xPos,unsigned char yPos,unsigned char charcode)
{
	unsigned char temp;
 	unsigned long offset;
	temp = charcode-32;
	offset = 9*temp + ASII_addr;     	         	        	     
	LcdUnzipZK(offset,3);          	          	     
	LcdGraphicFont(xPos,yPos,6,12,FontUnzip,15);
	return 1; 
}


unsigned char TextOut(unsigned char xPos,unsigned char yPos,unsigned char *GBCodeptr,unsigned char isWidth)
{
	unsigned char achar=1,x,y;
	unsigned short ttt;
	unsigned char countw=0; //用于计算输出下一个字符后屏幕是否会越界（起临时判断作用）
	unsigned char i,j;
	if (isWidth == TRUE)
	{
		i = 13;  //中文字符宽度
		j = 7;   //非中文字符宽度
	}
	else 
	{
		i = 12; 
		j = 6;
	}
	x=xPos;
	y=yPos;
	//countw=xPos+1; // 屏幕起点坐标为（1，0）
	countw=xPos;     // 屏幕起点坐标为（0，0）
	while ((*GBCodeptr) != 0)
	{
		if (*GBCodeptr > 128)
		{                   // display Chinese
			if (*(GBCodeptr+1) > 128)
			{
				countw = countw + i; 
				//if (countw > 132) break;       // 屏幕起点坐标为（1，0）
				if (countw > 131) break;         // 屏幕最后一个字符后面不用计算空格（最后一次因加i - 1，故可取到屏宽＋1    	
				ttt = (*GBCodeptr<<8) + *(GBCodeptr+1);	
				if(x==0) x=0x01;
				LcdTextChinese(x,y,ttt);				 
            x = x + i;
			}
			else 
			{
				achar = 0;                  //GBCode have error   
            return achar ;
			}
			GBCodeptr += 2;
		}
		else
		{             //display letter
			countw = countw + j;
			//if (countw > 132) break; // 屏幕起点坐标为（1，0）
			if(countw > 131) break;
			LcdTextChar(x,y,*GBCodeptr);
			x = x + j;
			GBCodeptr++;
		}    	
	} //end of while
	return achar;  
}

unsigned char LcdTextString(unsigned char xPos,unsigned char yPos,unsigned char *GBCodeptr)
{
	return TextOut(xPos,yPos,GBCodeptr,FALSE);
}
*/
void LCDclear(void)
{
	int i,n;

	for(i=0;i<8;i++)
	{
		LCDSetXY(0,i);

		for(n=0;n <= 128;n++)
		{
			w_data(0x0);
		}
	}

}

/****************************************uboot CODE(FROM HERE)****************************************/

