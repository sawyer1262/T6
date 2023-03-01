/*
 * icc_hardware.c
 *
 *  Created on: 2017��5��26��
 *      Author: YangWenfeng
 */
#include "delay.h"
#include "iccemv.h"
#include "cpm_drv.h"
#include "usi_drv.h"
#include "sys.h"

/*******************************************************************************
* Function Name  : InverseChar
* Description    : Inverse UINT8 data
* Input          : ch:UINT8 data
* Output         : None
* Return         : Inversed data
*******************************************************************************/
static UINT8 InverseChar(UINT8 ch)
{
	UINT8 tmp=0;
	UINT8 i;
	ch =~ch;
	for(i=0;i<8;i++)
	{
		tmp<<=1;
		tmp|=(ch&1);
		ch>>=1;
	}
	return(tmp);
}

/*******************************************************************************
* Function Name  : delayETU
* Description    : Delay a ETU
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void delay1ETU(void)
{
	vu32 counter=0;		//һ��Ҫ�����volatile���ͣ���ֹ�������Դ�������Ż�����ʱ��׼ȷ
	
//	if (changeTa1 == 0)		//93us
//	{
//		for (counter=0; counter<(g_sys_clk/2000000*5); counter++);
//	}
//	else if (changeTa1 == 2)
//	{
//		for (counter=0; counter<(g_sys_clk/4000000*3); counter++);
//	}
//	else if (changeTa1 == 4)
//	{
//		for (counter=0; counter<(g_sys_clk/5000000*3); counter++);
//	}
	
	if (g_sys_clk == EFLASH_SYS_CLK_120M)
	{
		if (changeTa1 == 0)		//93us
		{
			for (counter=0; counter<244; counter++);
		}
		else if (changeTa1 == 2)
		{
			for (counter=0; counter<122; counter++);
		}
		else if (changeTa1 == 4)
		{
			for (counter=0; counter<61; counter++);
		}
	}
}

/*******************************************************************************
* Function Name  : DelayNETU
* Description    : Delay N ETUs
* Input          : count: Delay Parame
* Output         : None
* Return         : None
*******************************************************************************/
void DelayNETU(UINT16 count)
{
	ushort i;
	for(i=0;i<count;i++)
	{
		delay1ETU();
	}
}
/*******************************************************************************
* Function Name  : SC_VoltageConfig
* Description    : Configures the card power voltage.
* Input          : - SC_Voltage: specifies the card power voltage.
*                    This parameter can be one of the following values:
*                        - MB525 SC_Voltage_5V: 5V cards.
*                        - MB525 SC_Voltage_3V: 3V cards.
*                        - NCN6001 0x01 1.8V
*                        - NCN6001 0x02 3V
*                        - NCN6001 0x03 5V
* Output         : None
* Return         : None
*******************************************************************************/
void SC_VoltageConfig(u32 SC_Voltage)
{
	guc_Class = SC_Voltage;
}

/*******************************************************************************
* Function Name  : Open_ICCard_Vcc_3V
* Description    : open ic card vcc = 5V
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Open_ICCard_Vcc_5V(void)
{
	OPEN_ICCARD_5V;
}

/*******************************************************************************
* Function Name  : Open_ICCard_Vcc_3V
* Description    : open ic card vcc = 3V
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Open_ICCard_Vcc_3V(void)
{
	OPEN_ICCARD_3V;
}

/*******************************************************************************
* Function Name  : Open_ICCard_Vcc_1V8
* Description    : open ic card vcc = 1V8
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Open_ICCard_Vcc_1V8(void)
{
	OPEN_ICCARD_1V8;
}


/*******************************************************************************
* Function Name  : Close_ICCard_Vcc
* Description    : close ic card vcc
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Close_ICCard_Vcc(void)
{
	CLOSE_ICCARD_5V;
	CLOSE_ICCARD_3V;
	CLOSE_ICCARD_1V8;
}

/*******************************************************************************
* Function Name  : Open_ICCard_VCC
* Description    : open ic card vcc
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Open_ICCard_VCC(void)
{
	if (guc_Class == 1)		// 1.8v
	{
		Open_ICCard_Vcc_1V8();
	}
	else if (guc_Class == 2)	// 3v
	{
		Open_ICCard_Vcc_3V();
	}
	else		// 5v
	{
		Open_ICCard_Vcc_5V();
	}
}

/*******************************************************************************
* Function Name  : SC_Init
* Description    : Initializes all peripheral used for Smartcard interface.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SC_Init(void)
{
	USIMasterInit();
}

/*******************************************************************************
* Function Name  : SC_HotInit
* Description    : Smart card �ȸ�λ��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SC_HotInit(void)
{
	USIMasterHotInit();
}

/*******************************************************************************
* Function Name  : SC_DeInit
* Description    : Deinitializes all ressources used by the Smartcard interface.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SC_DeInit(void)
{
	USI_DeInit();
	DelayMS(1);
	Close_ICCard_Vcc();		//�ر�7816��Դ
}

/*******************************************************************************
* Function Name  : SC_RSTSET
* Description    : Sets or clears the Smartcard reset pin.
* Input          : - ResetState: this parameter specifies the state of the Smartcard
*                    reset pin.
*                    BitVal must be one of the BitAction enum values:
*                       - Bit_RESET: to clear the port pin.
*                       - Bit_SET: to set the port pin.
* Output         : None
* Return         : None
*******************************************************************************/
void SC_RSTSET(UINT8 ResetState)
{
	if(!ResetState)
		g_USIReg->USIPDR &= ~0x01; //isorst L
	else
		g_USIReg->USIPDR |= 0x01; //isorst H
}


/*******************************************************************************
* Function Name  : USART_ByteReceive
* Description    : Receives a new data while the time out not elapsed.
* Input          : None
* Output         : None
* Return         : An ErrorStatus enumuration value:
*                         - SUCCESS: New data has been received
*                         - ERROR: time out was elapsed and no further data is
*                                  received
*******************************************************************************/
ErrorStatus USART_ByteReceive(UINT8 *Data, UINT32 TimeOut)
{
	UINT32 Counter = 0;
	UINT8  recetimes=0, temp;
	
	temp = temp;

accept1:
	k_IccErrPar=0;
	k_timeover=0;
	k_Ic_TimeCount=0x00;//ÿ���ַ�����ǰ��0

	//ʹ��WTC
	while( (g_USIReg->USISR & USISR_SB_MASK) != USISR_SB_MASK && (Counter != TimeOut))
	{
		Counter++;
		if ((g_USIReg->USISR&USISR_WTO_MASK) == USISR_WTO_MASK)
		{
			*Data = g_USIReg->USIRDR;	//����������п��ܴ��ڵ�����
			return ERROR;
		}
		if (k_total_timeover==1)		//��ʱ�䳬ʱ
		{
			k_total_timeover=0;
			return ERROR;
		}
	}

	if(Counter != TimeOut)
	{
		USI_OpenWTC(k_IcMax_EtuCount);		//��������һ���ֽڵĵȴ�ʱ�䡣WTC���յ���ʼλʱ�Զ����أ�����Ϊ���������յ�SB�źź�������WTC
		//�ȴ�������һ���ֽ�
		Counter = 0;
		while( (g_USIReg->USISR & USISR_RDRF_MASK) != USISR_RDRF_MASK && (Counter != TimeOut))
		{
			Counter++;
		}
	}

	if((g_USIReg->USISR & 0X08) == 0X08)
	{
		k_IccErrPar=1;//��ż����*/
		//��������־(reading USISR and then reading USIRDR)
		temp=g_USIReg->USISR;

		if((k_IccErrPar==1)&&((Parity_Check_Enable==1)))//ֻ����ż������ΪT=0Э�������
		{
			temp=g_USIReg->USIRDR;		//������������־
			if(recetimes<4)
			{
				recetimes++;
				goto accept1;
			}
			else
			{  return ERROR;}///110620
		}
	}

	if(Counter != TimeOut)
	{
		*Data = g_USIReg->USIRDR;
		if (SC_Slot_Active->SC_Convention==1) *Data=InverseChar(*Data);
			return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}


/*******************************************************************************
* Function Name  : USART_TsReceive
* Description    : Receives a new data while the time out not elapsed.
* Input          : None
* Output         : None
* Return         : An ErrorStatus enumuration value:
*                         - SUCCESS: New data has been received
*                         - ERROR: time out was elapsed and no further data is
*                                  received
*******************************************************************************/

ErrorStatus USART_TsReceive(UINT8 *Data, UINT32 TimeOut)
{
	UINT32 Counter = 0;
	k_IccErrPar=0;
	k_timeover=0;
	k_Ic_TimeCount=0x00;//ÿ���ַ�����ǰ��0

	//FPGA
	g_USIReg->USICR2 = 0x80;			//����ģʽ��żУ���ʱ�Զ����������źţ�ֻT0������
	g_USIReg->USIFIFOINTCON |= 0x20;	//�ر���żУ��

#ifdef ISO7816_AUTO_RETRANSMIT
	g_USIReg->USICR2 |= 0x0C; 			//�򿪴����Զ��ط����ܣ��ط�4�Σ���������5��
#endif

	//ʹ��WTC
	while( (g_USIReg->USISR & USISR_SB_MASK) != USISR_SB_MASK && (Counter != TimeOut))
	{
		Counter++;
		if ((g_USIReg->USISR&USISR_WTO_MASK) == USISR_WTO_MASK)
		{
			*Data = g_USIReg->USIRDR;	//����������п��ܴ��ڵ�����
			return ERROR;
		}
	}

	if(Counter != TimeOut)
	{
		USI_OpenWTC(k_IcMax_EtuCount);		//��������һ���ֽڵĵȴ�ʱ�䡣WTC���յ���ʼλʱ�Զ����أ�����Ϊ���������յ�SB�źź�������WTC
		//�ȴ�������һ���ֽ�
		Counter = 0;
		while( (g_USIReg->USISR & USISR_RDRF_MASK) != USISR_RDRF_MASK && (Counter != TimeOut))
		{
			Counter++;
		}
	}

	if(Counter != TimeOut)
	{
		*Data = g_USIReg->USIRDR;

		g_USIReg->USIFIFOINTCON &= ~0x20;		//����żУ��

		return SUCCESS;
	}
	else
	{
		return ERROR;
	}
}

/*******************************************************************************
* Function Name  : SC_GetByte
* Description    : Get byte from IO
* Input          : Data pointer
* Output         : None
* Return         : None
*******************************************************************************/

ErrorStatus SC_GetByte(UINT8 *Data)
{
	UINT32 TimeOut;

	TimeOut=0x8FFFFF;
	return USART_ByteReceive(Data, TimeOut);
}

/*******************************************************************************
* Function Name  : SC_PutByte
* Description    : Put byte
* Input          : Data to send
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 SC_PutByte(UINT8 Data)
{
#ifndef ISO7816_AUTO_RETRANSMIT		//ʹ���Զ��ط�
	UINT8  sendtimes = 0;
	UINT16 i;
#endif
	UINT8  test1;
	
	test1 = test1;
	k_IccErrPar = 0;

#ifdef ISO7816_AUTO_RETRANSMIT		//ʹ���Զ��ط�
	if (SC_Slot_Active->SC_Convention==1) Data=InverseChar(Data);
	TransmitByte(Data);

	if(Parity_Check_Enable==0)  		//T=1���ط�
	{
		//T=1���շ���⵽��żУ���ʱ������������ź�(�͵�ƽ)�������ڷ���ʱ���ͷ���֪�����շ��Ƿ������ȷ
		return 0x00;
	}
	if((g_USIReg->USISR & 0X08) == 0X08)
	{
		k_IccErrPar=1;//��ż����*/

		 //��������־(reading USISR and then writing 1 to ERROR bit)
		test1=g_USIReg->USISR;
		g_USIReg->USISR |= 0x08;
		return 0xff;
	}

	return 0x00;		//�ɹ�

#else		//ʹ���ֶ��ط�
resendb:
	if (SC_Slot_Active->SC_Convention==1) Data=InverseChar(Data);
	TransmitByte(Data);

	if((g_USIReg->USISR & 0X08) == 0X08)
	{
		k_IccErrPar=1;//��ż����*/

		 //��������־(reading USISR and then writing 1 to ERROR bit)
		test1=g_USIReg->USISR;
		g_USIReg->USISR |= 0x08;
	}

	if(Parity_Check_Enable==0)  return 0x00;//T=1���ط�
	if((Parity_Check_Enable==1)&&(k_IccErrPar==1))//T=0�ط�
	{
		if(sendtimes<4)
		{
			k_IccErrPar=0;//��ż����
			sendtimes++;
			goto resendb;
		}
		else
		{
			return 0xff;
		}
	}
	return 0x00;		//�ɹ�


#endif
}



