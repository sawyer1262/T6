#include "sys.h"
#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"  
#include "eport_drv.h"
#include "delay.h"
#include "string.h"

#define IPSRSTCR  		 *(volatile UINT32*) (CPM_BASE_ADDR + 0xbc)	

UINT8 string1[4][16]={"                ",
                      "                ",
                      "                ",
                      "                ",};
//OLED???
//??????.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 			   

void Set_LCD_Reset(UINT8 state)		//GINT12(Eport1[4])
{
	if(state)
		EPORT1->EPDR |= 0x10;
	else
		EPORT1->EPDR &= 0xEF;
}
											
/**********************************************
//IIC Start
**********************************************/
void IIC_Start()
{

	OLED_SCLK_Set() ;
	OLED_SCLK_Set() ;
	OLED_SDIN_Set();
	OLED_SDIN_Set();
	OLED_SDIN_Clr();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
	OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop()
{
	OLED_SCLK_Set() ;
	OLED_SCLK_Set() ;
	OLED_SDIN_Clr();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
	OLED_SDIN_Set();
}

void IIC_Wait_Ack()
{
	OLED_SCLK_Set() ;
	OLED_SCLK_Set() ;
	OLED_SCLK_Clr();
	OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/
void Write_IIC_Byte(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)
	{
			m=da;
		//	OLED_SCLK_Clr();
		m=m&0x80;
		if(m==0x80)
		{OLED_SDIN_Set();OLED_SDIN_Set();}
		else {OLED_SDIN_Clr();OLED_SDIN_Clr();}
//		 {OLED_SDIN_Clr();OLED_SDIN_Clr();}  //反白显示
//		else {OLED_SDIN_Set();OLED_SDIN_Set();} 
			da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
		OLED_SCLK_Clr();
	}
}

void Write_IIC_Byte2(unsigned char IIC_Byte)
{
	unsigned char i;
	unsigned char m,da;
	da=IIC_Byte;
	OLED_SCLK_Clr();
	OLED_SCLK_Clr();
	for(i=0;i<8;i++)
	{
			m=da;
		//	OLED_SCLK_Clr();
		m=m&0x80;
		if(m==0x80)
//		{OLED_SDIN_Set();OLED_SDIN_Set();}
//		else {OLED_SDIN_Clr();OLED_SDIN_Clr();}
		 {OLED_SDIN_Clr();OLED_SDIN_Clr();}  //反白显示
		else {OLED_SDIN_Set();OLED_SDIN_Set();} 
			da=da<<1;
		OLED_SCLK_Set();
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
		OLED_SCLK_Clr();
	}
}
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
#ifndef OLED_Hardware_i2c
   IIC_Start();
   Write_IIC_Byte(0x78);            //Slave address,SA0=0
	IIC_Wait_Ack();
   Write_IIC_Byte(0x00);			//write command
	IIC_Wait_Ack();
   Write_IIC_Byte(IIC_Command);
	IIC_Wait_Ack();
   IIC_Stop();
#else
   unsigned char buf[5];
   buf[0]=0x78;
   buf[1]=0x00;
   buf[2]=IIC_Command;
   I2C_MasterWriteData(I2C3,buf[0],buf+1,2);
#endif
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
#ifndef OLED_Hardware_i2c
   IIC_Start();
   Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();
   Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();
	   Write_IIC_Byte(IIC_Data);			//write data
//   Write_IIC_Byte2(IIC_Data);   //反白显示
	IIC_Wait_Ack();
   IIC_Stop();
#else
   unsigned char buf[5];
   buf[0]=0x78;
   buf[1]=0x40;
   buf[2]=IIC_Data;
   I2C_MasterWriteData(I2C3,buf[0],buf+1,2);
#endif
}

void Write_IIC_Data2(unsigned char IIC_Data)
{
#ifndef OLED_Hardware_i2c
   IIC_Start();
   Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
	IIC_Wait_Ack();
   Write_IIC_Byte(0x40);			//write data
	IIC_Wait_Ack();
   Write_IIC_Byte2(IIC_Data);  //反白显示
	IIC_Wait_Ack();
   IIC_Stop();
#else
   unsigned char buf[5];
   buf[0]=0x78;
   buf[1]=0x40;
   buf[2]=~IIC_Data;
   I2C_MasterWriteData(I2C3,buf[0],buf+1,2);
#endif
}

void OLED_WR_Byte(unsigned dat,unsigned cmd)
{
	if(cmd)
			{

   Write_IIC_Data(dat);
   
		}
	else {
   Write_IIC_Command(dat);
		
	}


}

/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		OLED_WR_Byte(0xb0+m,0);		//page0-page1
		OLED_WR_Byte(0x00,0);		//low column start address
		OLED_WR_Byte(0x10,0);		//high column start address
		for(n=0;n<128;n++)
			{
				OLED_WR_Byte(fill_Data,1);
			}
	}
}

void OLED_Show_electricity(unsigned char ele)  
{
	unsigned char t,i;
	t=ele*12/100;
	OLED_Set_Pos(112,0);	
	OLED_WR_Byte(0xFF,OLED_DATA);
	for(i=0;i<t;i++)
	{
		OLED_WR_Byte(0xff,OLED_DATA);
	}
	for(t=i;t<12;t++)
	{
		OLED_WR_Byte(0x81,OLED_DATA);
	}
	OLED_WR_Byte(0xff,OLED_DATA);
	OLED_WR_Byte(0x3c,OLED_DATA);
	OLED_WR_Byte(0x3c,OLED_DATA);
}

/***********************Delay****************************************/
void Delay_50ms(unsigned int Del_50ms)
{
//	unsigned int m;
//	for(;Del_50ms>0;Del_50ms--)
//	;
		DelayMS(1);
}

void Delay_1ms(unsigned int Del_1ms)
{
	DelayMS(1);
}

//????

void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f),OLED_CMD); 
}   	  
//??OLED??    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC??
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//??OLED??     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC??
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			  
void OLED_Clear(void)  
{  
	UINT8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);     
		OLED_WR_Byte (0x00,OLED_CMD);       
		OLED_WR_Byte (0x10,OLED_CMD);       
		for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
	}  
}
void OLED_On(void)  
{  
	UINT8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);     
		OLED_WR_Byte (0x00,OLED_CMD);       
		OLED_WR_Byte (0x10,OLED_CMD);       
		for(n=0;n<128;n++)OLED_WR_Byte(1,OLED_DATA); 
	} 
}
 
void OLED_ShowChar(UINT8 x,UINT8 y,UINT8 chr,UINT8 Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' '; 
		if(x>Max_Column-1){x=0;y=y+2;}
		if(Char_Size ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i],OLED_DATA);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_Byte(F8X16[c*16+i+8],OLED_DATA);
			}
			else {	
				OLED_Set_Pos(x,y);
				for(i=0;i<6;i++)
				OLED_WR_Byte(F6x8[c][i],OLED_DATA);
				
			}
}
//m^n??
UINT32 oled_pow(UINT8 m,UINT8 n)
{
	UINT32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
 		  
void OLED_ShowNum(UINT8 x,UINT8 y,UINT32 num,UINT8 len,UINT8 size2)
{         	
	UINT8 t,temp;
	UINT8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
 
void OLED_ShowString(UINT8 x,UINT8 y,UINT8 *chr,UINT8 Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{
		OLED_ShowChar(x,y,chr[j],Char_Size);
			x+=8;
		  if(x>120){x=0;y+=2;}
			j++;
	}
}

void OLED_ShowCHinese(UINT8 x,UINT8 y,UINT8 no)
{      			    
//	UINT8 t,adder=0;
///*
//中文字符为16*16大小（英文为8*16）
//可以显示4行
//每行显示8个汉字	
//*/	
//	OLED_Set_Pos(x*16,y);	
//	
//    for(t=0;t<16;t++)
//	{
//				OLED_WR_Byte(Hzk[2*no][t],OLED_DATA);
//				adder+=1;
//    }	
//	OLED_Set_Pos(x*16,y+1);	
//    for(t=0;t<16;t++)
//			{	
//				OLED_WR_Byte(Hzk[2*no+1][t],OLED_DATA);
//				adder+=1;
//      }					
}
void OLED_show_chinese(UINT8 x,UINT8 y,UINT8 *chr)
{
	UINT32 len,i,j,t;
	
	len=sizeof(HZlib)/sizeof(HZlib[0]);
 	for(j=1;j<len;j++)
 	if(((UINT8)HZlib[j].Index[0]==chr[0])&&((UINT8)HZlib[j].Index[1]==chr[1]))
		break;
	
	if(j==len)
	{
		j=0;
	}
/*
中文字符为16*16大小（英文为8*16）
可以显示4行
每行显示8个汉字	
*/	
	OLED_Set_Pos(x,y);	
	
  for(t=0;t<16;t++)
	{
			OLED_WR_Byte(HZlib[j].Msk[t],OLED_DATA);
  }	
	OLED_Set_Pos(x,y+1);	
  for(t=0;t<16;t++)
	{	
			OLED_WR_Byte(HZlib[j].Msk[t+16],OLED_DATA);
  }		
		
}
void OLED_ShowCHinese_string(UINT8 x,UINT8 y,UINT8 *chr,UINT8 size)
{      			    
	UINT8 t,adder=0,i;
/*
中文字符为16*16大小（英文为8*16）
可以显示4行
每行最多显示8个汉字	
*/	
//若增加新汉字，需要按照格式在HZlib中按照规定格式添加相应数据，以便查找
	UINT8 echar;
	memcpy(&string1[y/2],chr,16);
	while(1)
	{
		if (*chr == 0)
		{
			return;
		}
		if (*chr > 0x80)           //汉字
		{
#if 1
		   OLED_show_chinese(x,y,chr);
#endif
		   x += size;
		   chr += 2;
		}
		else                        //英文字符
		{
			echar = *chr;
			OLED_ShowChar(x,y,echar,size);
			x += size/2;
			chr += 1;
		}
	}
}

void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[])
{ 	
 unsigned int j=0;
 unsigned char x,y;
  
  if(y1%8==0) y=y1/8;      
  else y=y1/8+1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
	    {      
	    	OLED_WR_Byte(BMP[j++],OLED_DATA);	    	
	    }
	}
} 

void OLED_IO_Init(void)
{
	OLED_SCL_OUT;
	OLED_SDA_OUT;
	EPORT_ConfigGpio(EPORT_PIN12, GPIO_OUTPUT);		//RST_SEN(指纹Sensor复位)
}
//SSD1306					    
void OLED_Init(void)
{ 	
	I2C_TypeDef *i2c = (I2C_TypeDef *)I2C3_BASE_ADDR;
	
#ifndef OLED_Hardware_i2c
	OLED_SCL_OUT;
	OLED_SDA_OUT;
#else
	IPSRSTCR |= 0x00020000;				//RESET I2C3
	IPSRSTCR &= ~0x00020000;
	
	I2C->PCR &= ~0xc0;//???i2c??
	I2C_MasterInit(I2C3);
	//I2C
#endif

	EPORT_ConfigGpio(EPORT_PIN12, GPIO_OUTPUT);		
	
	Set_LCD_Reset(1);
DelayMS(1);
	Set_LCD_Reset(0);
DelayMS(1);
	Set_LCD_Reset(1);
DelayMS(1);
	printf("I2C_Init success!\n");	
	
	OLED_WR_Byte(0xAE,OLED_CMD);//--display off
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  
	OLED_WR_Byte(0xB0,OLED_CMD);//--set page address
	OLED_WR_Byte(0x81,OLED_CMD); // contract control
	OLED_WR_Byte(0xFF,OLED_CMD);//--128   
	OLED_WR_Byte(0xA1,OLED_CMD);//set segment remap 
	OLED_WR_Byte(0xA6,OLED_CMD);//--normal / reverse
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F,OLED_CMD);//--1/32 duty
	OLED_WR_Byte(0xC8,OLED_CMD);//Com scan direction
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset
	OLED_WR_Byte(0x00,OLED_CMD);//
	
	OLED_WR_Byte(0xD5,OLED_CMD);//set osc division
	OLED_WR_Byte(0x80,OLED_CMD);//
	
	OLED_WR_Byte(0xD8,OLED_CMD);//set area color mode off
	OLED_WR_Byte(0x05,OLED_CMD);//
	
	OLED_WR_Byte(0xD9,OLED_CMD);//Set Pre-Charge Period
	OLED_WR_Byte(0xF1,OLED_CMD);//
	
	OLED_WR_Byte(0xDA,OLED_CMD);//set com pin configuartion
	OLED_WR_Byte(0x12,OLED_CMD);//
	
	OLED_WR_Byte(0xDB,OLED_CMD);//set Vcomh
	OLED_WR_Byte(0x30,OLED_CMD);//
	
	OLED_WR_Byte(0x8D,OLED_CMD);//set charge pump enable
	OLED_WR_Byte(0x14,OLED_CMD);//
	
	OLED_WR_Byte(0xAF,OLED_CMD);//--turn on oled panel
}  
