#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "icc_demo.h"
#include "iccemv.h"
#include "pit32_drv.h"
#include "debug.h"
#include "delay.h"
#include "type.h"

 vu16 sw;//0719
 u16 lens;

 /*************************************************
 Function: LoopBackTest2
 Description: Э���loop backѭ�����Գ���
 Calls:
 Called By:
 Input: ��
 Output: ��
 Return: ��
 Others: ��
 *************************************************/
void LoopBackTest2(u16 PowerOffTime)
{
 	UINT8 CommBuffer[512];//0719
	//	u8  *ptr;
//	u8 profil;
	// u8 Delytimeout=0;
//	u8 rapdulrc,Datalen;
 	UINT16 i;
	u8 temp;
	UINT16 Capdulen;//������ܳ���
	UINT8 locresp[300];
	
	i = i;
	temp = temp;
	
	do
	{
		SC_A2R.SW1 = 0;
		SC_A2R.SW2 = 0;
		SC_DeInit();
	    DelayMS(1000*PowerOffTime);
		SC_VoltageConfig(3);		//5.0v��ֻ�����ã�û���ϵ�
		//SC_VoltageConfig(2);		// 3.0v��ֻ�����ã�û���ϵ�
//		SC_VoltageConfig(1);		// 1.8v��ֻ�����ã�û���ϵ�
	    SC_Reset(COLD_RESET);
		if(SC_A2R.SW1 == 0x90)
		{
			if(!SC_ATR_Len)//xjh add
				continue;//xjh add
			memcpy(&CommBuffer[8],(UINT8*)SC_ATR_Table,SC_ATR_Len);
			CommBuffer[8+SC_ATR_Len] = SC_A2R.SW1;
			CommBuffer[8+SC_ATR_Len+1] = SC_A2R.SW2;
			sw = 0x9000;
			lens = SC_ATR_Len;

			//����ĳЩ����ATR�ж�������			
			DelayMS(10);
			while((g_USIReg->USISR & USISR_RDRF_MASK) == USISR_RDRF_MASK)
			{
				temp = g_USIReg->USIRDR;	//��fifo
			}
		}
		else
		{
			lens= 0x00;
			sw = 0x6200;
		}
	  	if(sw == 0x9000)
	   	{
	  		memcpy(&CommBuffer[0],"\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31\x00",20);//ѡ��PSE
			Capdulen = 20;

			while(1)//(Delytimeout)
			{
#ifdef debug_print
			printf ("apdu:");
			for (i=0; i<Capdulen; i++)
			{
				printf ("%02x",CommBuffer[i]);
			}
			printf ("\n");
#endif

				sw = SC_Command(Capdulen,&CommBuffer[0], &lens, locresp);//��������
#ifdef debug_print
			printf ("response:");
			for (i=0; i<lens; i++)
			{
				printf ("%02x",locresp[i]);
			}
			printf ("\n");
			printf ("response:");
			printf ("SW:%04x",sw);
			printf ("\n");
#endif

				SC_Responce.SW1 = sw>>8;
				SC_Responce.SW2 = sw;

				if((SC_Responce.SW1 == 0x6A) && (SC_Responce.SW2 == 0x82))
						break;
				if(((SC_Responce.SW1&0xf0) != 0x60) && ((SC_Responce.SW1&0xf0) != 0x90))//110706 �ж���������������6x��9x������
						break;
				if((SC_Responce.SW1 == 0x6F) && (SC_Responce.SW2 == 0xF0))		//����������糬ʱ�ᵽ����
						break;
				if(lens < 6)//���ܴ���C-APDU��������Ƿ����SW1SW2��
				{
					memcpy(&CommBuffer[0],"\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31\x00",20);//ѡ��PSE
					Capdulen = 20;
				}
				else //����ѡ��CASE
				{
					if (locresp[1] == 0x70)		//INS=70Ӧ�µ�
					{
						break;
					}
					memcpy(&CommBuffer[0],&locresp,lens-2);
					Capdulen = lens-2;
				}
			}
	  	}
	 	else
	   	{

	   	}
	}
  	while(1);
}

void BCTC_Test_Demo()
{

	//CardVCC_Trim(0x02);	//Trim ClassB 3.06V
	
	while(1)
	{
	//	if (EPORT_Read_Gpio_Data(EPORT_PIN7) == Bit_RESET)
		{
			//LoopBackTest2(30);
			LoopBackTest2(1);
		}
	}
}

