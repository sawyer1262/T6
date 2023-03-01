/*
 * iccemv.c
 *
 *  Created on: 2017��5��26��
 *      Author: YangWenfeng
 */

#include "stddef.h"
#include "stdarg.h"
#include "stdlib.h"
#include "string.h"
#include "iccemv.h"
#include "pit32_drv.h"
#include "debug.h"

#define EMV_Co_Test
#define   EMV_SW1            asyncard_info[current_slot].sw1
#define   EMV_SW2            asyncard_info[current_slot].sw2

/********************************************************************
 ----------------------------ȫ�ֱ�������----------------------------
 ********************************************************************/
volatile UINT32          k_Ic_TotalTimeCount = 0;         //�ַ����ܳ�ʱ�����м����
volatile UINT32          k_Ic_TimeCount = 0;              //�ַ������ʱ�����м����
volatile UINT32          k_IcMax_TotalTimeCount = 0;      //�����ܳ�ʱ����ֵ
volatile UINT32          k_IcMax_TimeCount = 0;           //�ַ������ʱ����ֵ
volatile int             k_timeover,k_total_timeover = 0; //�ַ���ʱ���ܳ�ʱ��־
volatile UINT32          k_IcMax_EtuCount = 0;           //�ַ������ʱ����ֵ����λETU


static   UINT8           card_Outputdata[ICMAXBUF];
static   UINT8           rstbuf[40];
volatile static UINT8    WTX;
volatile static UINT32   WWT,CWT,BWT;
volatile  UINT8          current_slot = 0;
volatile  UINT8          Parity_Check_Enable; /* żУ����󵥸��ַ��ط������־  */
volatile  UINT8          k_IccErrPar = 0;       /* ��żУ��    */

vu16                     respLength = 0;
volatile UINT8           SC_ATR_Table[40];
volatile UINT8           SC_T0_T1_Status = 0;
volatile UINT8           SC_ATR_Len = 0;
volatile UINT8           guc_hotrstFlag = 0;
volatile UINT8           guc_Leflag = 0;

volatile UINT8           changeTa1     = 0;          //20110729
volatile UINT8           guc_AdjustETU = 0;		//�����ַ����ETU��Ҫ��1��1������
volatile UINT8           guc_GuardTime = 0;		//20131220������Ϊint����(��ԭֵ����ƥ��)������������GT�Ĵ���ΪĬ��12ETU
volatile UINT8           guc_Class = 0;
volatile UINT8           ICCardResetOK = 0;
volatile UINT8           ICCardInsert = 0;
volatile UINT8           ICCardInfoSent = 0;            //ic�������Ϣ���ͣ�00 ��ʼ״̬   01 ������   02 ���Ƴ�
volatile UINT8           g_usbCommOK = 0;
ASYNCARD_INFO            asyncard_info[IC_MAXSLOT];

SC_State                 *locState;  //test
SC_ATR                   SC_A2R;
SC_ADPU_Commands         SC_ADPU;
SC_ADPU_Responce         SC_Responce;
SC_READER_SET            SC_Slot_Default= {0,0,0x0A,0x0D,0x04,22,1,2,0x20,0,0,0,0,0,0x6fffff,0x6fffff,SC_POWER_ON};
SC_READER_SET            *SC_Slot_Active=&SC_Slot_Default;

#ifndef ISO7816_PBOC_CARD
const UINT16 FDSelection_Table[30] = {744,0x01,372,0x02,186,0x03,93,0x04,46,0x05,23,0x06,12,0x07,31,0x08,512,0x09,256,0x0A,128,0x0B,64,0x0C,32,0x0D,16,0x0E,8,0x0F};
const UINT16 FI_Table[24] = {0x00,372,0x01,372,0x02,558,0x03,744,0x04,1116,0x05,1488,0x06,1860,0x09,512,0x0A,768,0x0B,1024,0x0C,1536,0x0D,2048};
const UINT16 DI_Table[18] = {0x01,1,0x02,2,0x03,4,0x04,8,0x05,16,0x06,32,0x07,64,0x08,12,0x09,20};

//����0��ʾTA1����
BOOLEAN Get_FiDi(UINT8 TA1, UINT16 *Fi, UINT8 *Di)
{
	UINT8 i;
	UINT8 temp;

	temp = TA1>>4;	
	for (i=0;i<12;i++)
	{
		if (temp == FI_Table[2*i])
		{
			*Fi = FI_Table[2*i+1];
			break;
		}
	}
	if (i == 12)
		return FALSE;
	
	temp = TA1&0x0F;
	for (i=0;i<9;i++)
	{
		if (temp == DI_Table[2*i])
		{
			*Di = DI_Table[2*i+1];
			break;
		}
	}
	if (i == 9)
		return FALSE;

	return TRUE;
}

//����0��ʾTA1����
UINT8 Get_FD_Selection(UINT16 Fi, UINT8 Di)
{
	UINT8 i;
	UINT8 temp;
	UINT16 FdivD;

	FdivD = Fi/Di;		//ȡ�������ּ���
	for (i=0;i<15;i++)
	{
		if (FdivD == FDSelection_Table[2*i])
		{
			temp = FDSelection_Table[2*i+1];
			break;
		}
	}
	if (i == 15)
		return 0;

	return temp;
}

//����ֵ: 0��ʾ����;1��ʾ����TA1��ʹ����FD��2��ʾû�з���TA1��ʹ��Ĭ�ϵ�FD;
UINT8 PPS_Process(uchar slot)
{
	u8 ret;
	u8 PPS_Requst[4];
	u8 PPS_Response[4];
	u8 i;

	//GT��WT��ATR��ͬ
	
	PPS_Requst[0] = 0xFF;
	PPS_Requst[1] = 0x10+(asyncard_info[slot].TD1&0x0f);
	PPS_Requst[2] = asyncard_info[slot].TA1;
	PPS_Requst[3] = PPS_Requst[0]^PPS_Requst[1]^PPS_Requst[2];

	Parity_Check_Enable=0x00;		//�����ط�
	for (i=0;i<4;i++)
	{
		ret=SC_PutByte(PPS_Requst[i]);
		if(ret)
		{
			return 0;
		}
	}

	if(asyncard_info[slot].TD1==0)
		Parity_Check_Enable=0x01; 		//��������
	else 
		Parity_Check_Enable=0x00;
	
	//����PPSS
	if(SC_GetByte(&PPS_Response[0]) == ERROR)
	{
		return 0;			//��ʱ������ط�ʧ�ܺ󶼻ᵽ����
	}
	if (PPS_Response[0] != 0xFF)
		return 0;

	//����PPS0
	if(SC_GetByte(&PPS_Response[1]) == ERROR)
	{
		return 0;			//��ʱ������ط�ʧ�ܺ󶼻ᵽ����
	}
	if ((PPS_Response[1]&0x0F) != (PPS_Requst[1]&0x0F))
		return 0;

	if ((PPS_Response[1]&0x10) == 0x10)	//��PPS1
	{
		//����PPS1
		if(SC_GetByte(&PPS_Response[2]) == ERROR)
		{
			return 0;			//��ʱ������ط�ʧ�ܺ󶼻ᵽ����
		}
		if (PPS_Response[2] != PPS_Requst[2])
			return 0;

		//����XOR
		if(SC_GetByte(&PPS_Response[3]) == ERROR)
		{
			return 0;			//��ʱ������ط�ʧ�ܺ󶼻ᵽ����
		}
		if (PPS_Response[3] != PPS_Requst[3])
			return 0;

		return 1;
	}
	else		//û��PPS1
	{
		//����XOR
		if(SC_GetByte(&PPS_Response[2]) == ERROR)
		{
			return 0;			//��ʱ������ط�ʧ�ܺ󶼻ᵽ����
		}
		if ((PPS_Response[0]^PPS_Response[1]) != PPS_Response[2])
			return 0;

		return 2;
	}
}
#endif

/*******************************************************************************
* Function Name  : PIT2Open
* Description    : PIT2����
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void PIT2Open()
{
	PIT32_Init(PIT2, PIT32_CLK_DIV_1024, 58,TRUE);   //1ms  --ywf:̫С����CASE1702�޷�����ͨ����ÿ�ο���PTI�������¿�ʼ����
}

/*******************************************************************************
* Function Name  : PIT2Close
* Description    : PIT2�ر�
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void PIT2Close()
{
	PIT32_Stop(PIT2);
}

/*******************************************************************************
* Function Name  : Icc_Reset
* Description    :��ʼ��SLOTͨ����IC���������Կ��ϵ磬��λ���жϿ���Э�����ͣ�
*                 ��T=1��T=0��������ATR����λӦ�𣩶���*ATRָ���⡣
* Input          : slot ��Ҫ��ʼ����ͨ����
*                  ATR  ATR����BUF
*
* Output         : None
* Return         : None
******************************************************************************/
int Icc_Reset(UINT8 slot,UINT8 *ATR)
{
 	int ret=0, temp=0;
#ifdef debug_print
 	int i=0;
#endif
#ifndef ISO7816_PBOC_CARD
	u8 usibound;
#endif

 	ClearAtr(slot);//��ATR

	asyncard_info[slot].T=0;
	guc_AdjustETU = 0;
	guc_GuardTime = 0;
	changeTa1 = 0;

	ret=GetAtr(rstbuf);
	PIT2Close();

#ifdef debug_print
	printf ("atr:");
	for (i=0; i<rstbuf[0]; i++)
	{
		printf ("%02x",rstbuf[1+i]);
	}
	printf ("\n");
#endif

	// 20131231��1CE079��Ų��ǰ�������������ȸ�λǰ�޸��˼Ĵ���
	if(asyncard_info[slot].resetstatus==0)
	{
		asyncard_info[slot].resetstatus=0x01;
	}
	else
	{
		asyncard_info[slot].resetstatus=0x02;
	}
	if(ret)
	{
#ifdef debug_print
		printf ("atr error!\n");
		printf ("ret = %4d!\n",ret);
#endif

		if((ret!=ICC_ATR_TSERR)&&(ret!=ICC_ATR_TIMEOUT)
		 &&(ret!=ICC_PARERR)   &&(ret!=ICC_ATR_TCKERR)
		 &&(asyncard_info[slot].resetstatus==0x01) )
		{
			guc_hotrstFlag = 1;
		}
		// ����TS���󣬳�ʱ��У�顢TCK�������������д����ͷ�����
		return ret;
	}
#ifdef debug_print
		printf ("atr ok!\n");
#endif


#ifndef ISO7816_PBOC_CARD		//ִ��PPS����
	if(asyncard_info[slot].TA & 0X02)		//����ģʽ
	{
		 if(asyncard_info[slot].TA2 & 0x10)	//����Ĭ�ϲ����������޸�
		 {
		 }
		 else		//���սӿ��ֽڲ�������ATR�еĲ���
		 {
			if(asyncard_info[slot].TA & 0X01)		//TA1����
			{
				usibound = Get_FD_Selection(asyncard_info[slot].F, asyncard_info[slot].D);
				if (usibound != 0)
				{
					g_USIReg->USIBDR = usibound;
				}
			}
		 }
	}
	else		//Э��ģʽ����PPS
	{
		if(asyncard_info[slot].TA & 0X01)		//TA1����
		{
			usibound = Get_FD_Selection(asyncard_info[slot].F, asyncard_info[slot].D);
		}
		else		//������TA1������ʹ��TA1=18������
		{
			asyncard_info[slot].TA1 = 0x18;
			if (Get_FiDi(asyncard_info[slot].TA1, &asyncard_info[slot].F, &asyncard_info[slot].D) == FALSE)
			{
				return ICC_PARERR;
			}
			usibound = Get_FD_Selection(asyncard_info[slot].F, asyncard_info[slot].D);
		}
		if (usibound != 0)
		{
			DelayNETU(22+2);		//���򱣻�ʱ��
			//ִ��PPS����
			temp = PPS_Process(slot);
			if (temp == 0)
			{				
				return ICC_PARERR;		//ִ���µ�
			}
			else if (temp == 1)	//�޸�����
			{
				g_USIReg->USIBDR = usibound;
			}
		}
	}
#else		//PBOC��
	//ͨѶ����ת��
	if((asyncard_info[current_slot].D==1)
	 ||(asyncard_info[current_slot].D==2)
	 ||(asyncard_info[current_slot].D==4))//����ͨѶ����ת��
	{
		changeTa1=0;
		g_USIReg->USIBDR = 2;
		if(asyncard_info[current_slot].D==2)
		{
			changeTa1=2;
			g_USIReg->USIBDR = 3;
		}
		if(asyncard_info[current_slot].D==4)
		{
			changeTa1=4;
			g_USIReg->USIBDR = 4;
		}
	}
#endif

	//ͨѶЭ��ת��
	if ((asyncard_info[slot].TD1&0x0f)==0x01) //
	{
		g_USIReg->USICR1 = g_USIReg->USICR1 | 0x08;	// t=1;
		asyncard_info[slot].T=1;
		asyncard_info[slot].term_pcb=0x00;
		SC_Slot_Active->SC_Protocol=T1_PROTOCOL;
	}
	else if ((asyncard_info[slot].TD1&0x0f)==0x00)
	{
		g_USIReg->USICR1 = g_USIReg->USICR1 & 0xf7;	// t=0;
		asyncard_info[slot].T=0;
		SC_Slot_Active->SC_Protocol=T0_PROTOCOL;
	}
	else
	{
		return ICC_PROTOCALERR;
	}

	 // RSTBUF��ʽ:����+TS+TO+TA......
	for( temp=0 ; temp<rstbuf[0] ; temp++ )
	{
		*(ATR+temp) = rstbuf[temp+1];
	}
	SC_ATR_Len = rstbuf[0];

	//ͨѶETU����
	if(asyncard_info[current_slot].TC1 != 0xff)
	{
		temp = asyncard_info[current_slot].TC1 + 12;
		guc_GuardTime = temp;
	}
	else
	{
		if(asyncard_info[slot].T==1)
		{
			temp = 11;
			guc_GuardTime = temp;
		}
		if(asyncard_info[slot].T==0)
		{
			temp = 12;
			guc_GuardTime = temp;
		}
	}
	guc_GuardTime = temp;
	g_USIReg->USIGTRL = (unsigned char)(temp & 0xff);
	g_USIReg->USIGTRH = (unsigned char)((temp>>8) & 0xff);

	//asyncard_info[slot].open =1;

	//T1Э��IFS
	if(asyncard_info[slot].T==1)
	{
		//Ԥ�ô�IC�����յ�I����ȷ��PCB�����֣�˳�����1
		asyncard_info[slot].card_pcb=0x40;
		ret=T1_IFSD_command(0);
		return ret;
	}
	return ICC_SUCCESS;
}

/*******************************************************************************
* Function Name  : Icc_Command
* Description    :��������
*                 ֧��T0Э����T1Э��
*                 ��֧��CASE2���͵������
* Input          : slot     �������˿�
*                  ApduSend APDU����
*                  ApduResp Ӧ������
*
* Output         : None
* Return         : ���ز������
******************************************************************************/
int Icc_Command(UINT8 slot,SC_ADPU_Commands * ApduSend,SC_ADPU_Responce * ApduResp)
{
	UINT8   Cla,Ins,P1,P2;
	UINT16  Le,Lc,iT1Le,recelen;
	int     result;
	//UINT8   *ptr;

	EMV_SW1=0;
	EMV_SW2=0;
	ApduResp->SW1=0;
	ApduResp->SW2=0;
	if((ApduSend==NULL)||(ApduResp==NULL))
		return ICC_PARAMETERERR;

	if(asyncard_info[current_slot].T==0)		 //����T=0Э�鿨ƬӦ����żУ������ط�����
		Parity_Check_Enable=0x01;
	else
		Parity_Check_Enable=0x00;

	Cla=ApduSend->Header.CLA;
	Ins=ApduSend->Header.INS;
	P1= ApduSend->Header.P1;
	P2= ApduSend->Header.P2;
	Lc= (ushort )(ApduSend->Body.LC);//���͵��������ݳ���
	if(Lc>ICMAXBUF)
	{
		Parity_Check_Enable=0x00;
		return ICC_DATA_LENTHERR;
	}
	//���յ���Ӧ���ݳ���
	Le= (ushort)(ApduSend->Body.LE);
	if (Le > 256)
	{
		Le=256;
	}
	if( guc_Leflag == 1)//��Ҫ�·�LE��־
	{
		guc_Leflag = 0;
		Le = 256;
	}

	result=IccSendCommand( Cla,Ins, P1, P2,  Lc,ApduSend->Body.Data,Le,card_Outputdata);
	Parity_Check_Enable=0x00;
	//ptr=ApduResp->Data;
	if((asyncard_info[current_slot].T==1) && (result==0))
	{
		//���ΪT=1Э���ҽ�����ȷʱ���н������ݸ�ʽת��
		iT1Le = card_Outputdata[0]*256 +card_Outputdata[1];
		recelen=iT1Le-2;
		if(recelen>0)
		memcpy(ApduResp->Data,&card_Outputdata[2],recelen);

		ApduResp->SW1 = card_Outputdata[iT1Le];
		ApduResp->SW2 = card_Outputdata[iT1Le+1];
		respLength = recelen+2;
	}
	else if(result==0)
	{
		//T=0Э������ҽ�����ȷʱ���и�ʽת��
		recelen=card_Outputdata[0]*256+card_Outputdata[1];
		if(recelen>0)
		memcpy(ApduResp->Data,&card_Outputdata[2],recelen);

		respLength = recelen+2;

		ApduResp->SW1=EMV_SW1;
		ApduResp->SW2=EMV_SW2;
	}
	else
	{
		// respLength��ʼΪ0
		EMV_SW1=0;    EMV_SW2=0;

		ApduResp->SW1=0;
		ApduResp->SW2=0;

		if((result==ICC_T0_TIMEOUT)||(result==ICC_T1_BWTERR)||(result==ICC_T1_CWTERR))
			return ICC_ICCMESSOVERTIME;
	}
	return  result;
}

/*******************************************************************************
* Function Name  : exp2_n
* Description    :����2��N�η�����
* Input          :en : ����
*
* Output         : None
* Return         : ���ز������
******************************************************************************/
UINT16 exp2_n(UINT8 en)
{
	switch(en)
	{
		case 0:  return 1;
		case 1:  return 2;
		case 2:  return 4;
		case 3:  return 8;
		case 4:  return 16;
		case 5:  return 32;
		case 6:  return 64;
		case 7:  return 128;
		case 8:  return 256;
		case 9:  return 512;
		case 10:  return 1024;
		case 11:  return 2048;
		case 12:  return 4096;
		case 13:  return 8192;
		case 14:  return 16384;
		case 15:  return 32768;
	}
	return 0xffff;
}

/*******************************************************************************
* Function Name  : GetAtr
* Description    :��ȡIC���ĸ�λӦ���ź�ATR
* Input          :Buff : ATR�Ĵ��BUFF
*
* Output         : None
* Return         : ���ز��������0x00���ɹ�
******************************************************************************/
int GetAtr(UINT8 *Buff)
{
	UINT8   i/*,len*/,Flag,x;
	UINT8   *Lenth_Point;
	//UINT8   *ptr_atr;
	UINT8   T,edc,TCK,CWI,BWI;
	UINT16  N,j;
	UINT8   Lenth=2;
	UINT8  locData;

	edc=0;T=0;
	asyncard_info[current_slot].TA=0;
	asyncard_info[current_slot].TB=0;
	asyncard_info[current_slot].TC=0;
	asyncard_info[current_slot].TD=0;
	asyncard_info[current_slot].TS=0xff;
	//if(asyncard_info[current_slot].TS == 0xff)

	Lenth_Point=Buff;
	Parity_Check_Enable=0x00; //�ڸ�λӦ���н�ֹżУ������ط�
	*Buff=0x00;

	//��ʱ�������ʼ��
	k_Ic_TotalTimeCount=0;//ÿ����ʱ����0
	k_IcMax_TotalTimeCount=0;
	k_total_timeover=0x00;

	k_IcMax_TimeCount = 0;
	k_IcMax_EtuCount = 116;		// 10.6msԼΪ114��ETU
	USI_OpenWTC(k_IcMax_EtuCount);
	//���������¸��ֽڵĵȴ�ʱ�䣬���յ��¸��ֽڵ���ʼλʱ����WTC
	k_IcMax_TimeCount = 0;
	k_IcMax_EtuCount = 9600+480+50;

	if((USART_TsReceive(&locData, SC_Receive_Timeout)) == SUCCESS)
	{
	}
	else
	{
		return(ICC_ATR_TIMEOUT);	// 1ce 054 20131217
	}

	SC_Slot_Active->SC_Convention = 0x00;
	/*As to 7816-3 protocol,TS must be 0x3B or 0x03*/
	if (locData==0x3B||locData==0x03)
	{
		/*�����ֽ�Э����ж�*/
		if(locData==0x03)
		{
			SC_Slot_Active->SC_Convention=0x01;
			*(++Buff)=0x3F;
			g_USIReg->USICR1 |= 0x01; // ��У��
		}
		else
		{
			*(++Buff)=locData;
			g_USIReg->USICR1 &= (~0x01); // żУ��
		}
	}
	else
	{
		return ICC_ATR_TSERR;		// 20140102,TS��Ӧ�µ�
	}

	edc=0;
	//ptr_atr=Buff;

	k_IcMax_TotalTimeCount=1875;		//��ʱ���TS��ʼλ��ʼ����,�ȴ�ʱ��>19200+480 etu, ��19200+4800 ETU���µ磬1ETU=93us, ��19200+480+480=1875ms	,Ҫ����Сֵ��΢���
//	k_IcMax_TotalTimeCount=1950;		//��ʱ���TS��ʼλ��ʼ����,�ȴ�ʱ��>19200+480 etu, ��19200+4800 ETU���µ磬1ETU=93us, ��19200+480+480=1875ms	,Ҫ����Сֵ��΢���
//	k_IcMax_TotalTimeCount=1939;
	PIT2Open();		//ATR��ʱ����PIT��ʱ

	if((USART_ByteReceive(&locData, SC_Receive_Timeout)) == SUCCESS)//T0
	{
		if(k_IccErrPar)
		{
			return ICC_PARERR;
		}
		*(++Buff) = locData;
	}
	else
	{
		return(ICC_ATR_TIMEOUT);
	}

	edc=edc^*Buff;
	Flag=*Buff;
	x=Flag&0x0f;
	i = 1;
	for(;;)
	{
		// TA
		if((Flag&0x10)==0x10)
		{
			if((USART_ByteReceive(&locData, SC_Receive_Timeout)) == SUCCESS)
			{
				if(k_IccErrPar)
				{
					return ICC_PARERR;
				}
				*(++Buff) = locData;
			}
			else
			{
				return(ICC_ATR_TIMEOUT);
			}

			if(i==1){
				asyncard_info[current_slot].TA1 = *Buff;
				asyncard_info[current_slot].TA|=0X01;
			}
			else if(i==2){
				asyncard_info[current_slot].TA2 = *Buff;
				asyncard_info[current_slot].TA|=0X02;
			}
			else if(i==3){
				asyncard_info[current_slot].TA3 = *Buff;
				asyncard_info[current_slot].TA|=0X04;
			}
			else    asyncard_info[current_slot].TA4 = *Buff;
			edc=edc^*Buff;
			Lenth++;
		}
		//TB
		if((Flag&0x20)==0x20)
		{
			if((USART_ByteReceive(&locData, SC_Receive_Timeout)) == SUCCESS)
			{
				if(k_IccErrPar)
				{
					return ICC_PARERR;
				}
				*(++Buff) = locData;
			}
			else
			{
				return(ICC_ATR_TIMEOUT);
			}

			if(i==1){
				asyncard_info[current_slot].TB1 = *Buff;
				asyncard_info[current_slot].TB|=0X01;
			}
			else if(i==2){
				asyncard_info[current_slot].TB2 = *Buff;
				asyncard_info[current_slot].TB|=0X02;
			}
			else if(i==3) {
				asyncard_info[current_slot].TB3 = *Buff;
				asyncard_info[current_slot].TB|=0X04;
			}
			else asyncard_info[current_slot].TB4 = *Buff;
			edc=edc^*Buff;
			Lenth++;
		}
		// TC
		if((Flag&0x40)==0x40)
		{
			if((USART_ByteReceive(&locData, SC_Receive_Timeout)) == SUCCESS)
			{
				if(k_IccErrPar)
				{
					return ICC_PARERR;
				}
				*(++Buff) = locData;
			}
			else
			{
				return(ICC_ATR_TIMEOUT);
			}

			if(i==1){
				asyncard_info[current_slot].TC1 = *Buff;
				asyncard_info[current_slot].TC|=0X01;
			}
			else if(i==2){
				asyncard_info[current_slot].TC2 = *Buff;
				asyncard_info[current_slot].TC|=0X02;
			}
			else if(i==3){
				asyncard_info[current_slot].TC3 = *Buff;
				asyncard_info[current_slot].TC|=0X04;
			}
			else asyncard_info[current_slot].TC4 = *Buff;
			edc=edc^*Buff;
			Lenth++;
		}
		//  no TD
		if((Flag&0x80)==0x00)
		{
			for(i=0;i<x;i++)		//��ʷ�ֽ�
			{
				if((USART_ByteReceive(&locData, SC_Receive_Timeout)) == SUCCESS)
				{
					if(k_IccErrPar)
					{
						return ICC_PARERR;
					}
					*(++Buff) = locData;
				}
				else
				{
					return(ICC_ATR_TIMEOUT);
				}
				edc=edc^*Buff;
			}
			Lenth=Lenth+x;

			if(T)	//���ΪT<>1Э�鿨Ƭһ����TCK
			{
				if((USART_ByteReceive(&locData, SC_Receive_Timeout)) == SUCCESS)
				{
					if(k_IccErrPar)
					{
						return ICC_PARERR;
					}
					*(++Buff) = locData;
				}
				else
				{
					return ICC_ATR_TIMEOUT;		//20131205,SC_DeInit̫�̵���VCCû�б�����?
				}

				TCK=*Buff;
  			if(edc!=TCK)   return ICC_ATR_TCKERR;
				Lenth++;
			}
			*Lenth_Point=Lenth;

#ifndef ISO7816_PBOC_CARD		//��Է�PBOC���������ϸ���ж��Ա���ݸ���ATR
			//TA1
			if(asyncard_info[current_slot].TA & 0X01)
			{ 
				if (Get_FiDi(asyncard_info[current_slot].TA1, &asyncard_info[current_slot].F, &asyncard_info[current_slot].D) == FALSE)
				{
					return ICC_ATR_TA1ERR;
				}
			}
				
			//TC2  
			if((asyncard_info[current_slot].TC&0X02) == 0X00)
			{                    
				asyncard_info[current_slot].TC2=0x0a; 		//If TC2 is absent, then the default value is WI = 10.
			} 
			WWT=asyncard_info[current_slot].TC2*960;

			//TA3
			if(asyncard_info[current_slot].TA&0X04)
			{
				if(asyncard_info[current_slot].TA3==0xff) return ICC_ATR_TA3ERR;
			}
			else 
			{
				asyncard_info[current_slot].TA3=0x20; 
			}
			asyncard_info[current_slot].IFSC=asyncard_info[current_slot].TA3; 

			//TB3
			if(T==1)
			{  
				if((asyncard_info[current_slot].TB&0X04)==0X00) return ICC_ATR_TB3ERR; 
				CWI=asyncard_info[current_slot].TB3&0x0f; 
				CWT=11+exp2_n(CWI);
				WTX=0; 
				BWI=(asyncard_info[current_slot].TB3 >> 4)&0x0f; 
				BWT=11+960*exp2_n(BWI);
			} 
			

#else		//PBOC��

			if(asyncard_info[current_slot].TA & 0X02)
			{
				if(asyncard_info[current_slot].TA2 & 0x10) 		//��֧��Ĭ�ϵĴ������
					return ICC_ATR_TA2ERR;
			}
			// ֧��TA1=��11�� '12'  '13'ֵ��ATR
			if(asyncard_info[current_slot].TA & 0X01)
			{
#ifndef EMV_Co_Test
				///*////////PBOC 4.3a//////*/

				if((asyncard_info[current_slot].TA1 < 0x11) || (asyncard_info[current_slot].TA1 > 0x13))
				{
					return ICC_ATR_TA1ERR;
				}
#else
				///*////////EMV 4.3c for case 1702/1755//////*/

				if((asyncard_info[current_slot].TA1 != 0x11)
				&& (asyncard_info[current_slot].TA1 != 0x12)
				&& (asyncard_info[current_slot].TA1 != 0x13)
				&& (asyncard_info[current_slot].TA1 != 0x17)
				&& (asyncard_info[current_slot].TA1 != 0xD1)
				&& (asyncard_info[current_slot].TA1 != 0xD9))
				{
					return ICC_ATR_TA1ERR;
				}

#endif
			}
			if(!(asyncard_info[current_slot].TA & 0X02)) asyncard_info[current_slot].TA1 = 0x11;

			if(asyncard_info[current_slot].TA1==0x11)
				asyncard_info[current_slot].D=1;
			else if(asyncard_info[current_slot].TA1==0x12)
				asyncard_info[current_slot].D=2;
			else if(asyncard_info[current_slot].TA1==0x13)
				asyncard_info[current_slot].D=4;
			///*////////EMV 4.3c for case 1702//////*/
			else if(asyncard_info[current_slot].TA1==0x17)
				asyncard_info[current_slot].D=64;
			else if(asyncard_info[current_slot].TA1==0xD1)
				asyncard_info[current_slot].D=1;
			else if(asyncard_info[current_slot].TA1==0xD9)
				asyncard_info[current_slot].D=20;
			///*////////EMV 4.3c for case 1702 end//////*/
			else
			{
				asyncard_info[current_slot].TA1=0x11;
				asyncard_info[current_slot].D=1;
			}

			//  �临λ����TB1ֵ��TB1��=0ʱ������ȸ�λ
			if(asyncard_info[current_slot].resetstatus==0x00)
			{
				if(((asyncard_info[current_slot].TB&0X01)==0X00)||(asyncard_info[current_slot].TB1!=0x00))
				{
				return ICC_ATR_TB1ERR;		// 1ce,064_01/03�� 20131218
				}
			}
			// ��Ч��TDֵ
			if((asyncard_info[current_slot].TD&0X01)==0X01)
			{
				if((asyncard_info[current_slot].TD1 & 0x0f)>1)
				{
					return ICC_ATR_TD1ERR;
				}
				if(asyncard_info[current_slot].TD1 & 0x0f)
				{
					if((asyncard_info[current_slot].TD & 0X02)&&((asyncard_info[current_slot].TD2&0x0f)!=0x01))
					{
						return ICC_ATR_TD2ERR;
					}
				}
				else
				{
					if(((asyncard_info[current_slot].TD&0x02)==0x02)&&((asyncard_info[current_slot].TD2&0x0f)!=0x01)
					&&((asyncard_info[current_slot].TD2&0x0f)!=0x0e))
					{
						return ICC_ATR_TD2ERR;
					}
				}
			}
			// ��Ч��TB2�ֽ�
			if(asyncard_info[current_slot].TB&0X02)
			{
				return ICC_ATR_TB2ERR;
			}

			//��Ч��TC2�ֽ�
			if(asyncard_info[current_slot].TC&0X02)
			{
				if(asyncard_info[current_slot].TC2!=0x0a)
				{
					return ICC_ATR_TC2ERR;
				}
			}
			asyncard_info[current_slot].TC2=0x0a;
			WWT=960*asyncard_info[current_slot].TC2;

			// ��Ч��TA3�ֽ�
			if(asyncard_info[current_slot].TA&0X04)
			{
				if(asyncard_info[current_slot].TA3<0x10)
				{
					return ICC_ATR_TA3ERR;
				}
				if(asyncard_info[current_slot].TA3==0xff)
				{
					return ICC_ATR_TA3ERR;
				}
			}
			else
			{
				asyncard_info[current_slot].TA3=0x20;
			}
			asyncard_info[current_slot].IFSC=asyncard_info[current_slot].TA3;

			// ��Ч��TB3�ֽ�
			if(T==1)
			{
				if((asyncard_info[current_slot].TB&0X04)==0X00)
				{
					return ICC_ATR_TB3ERR;
				}
				CWI=asyncard_info[current_slot].TB3&0x0f;
				if(CWI>5)
				{
					return ICC_ATR_TB3ERR;
				}
				CWT=11+exp2_n(CWI);
				WTX=0;
				BWI=(asyncard_info[current_slot].TB3 >> 4)&0x0f;
				if(BWI>4)
				{
					return ICC_ATR_TB3ERR;
				}
				BWT=11+960*exp2_n(BWI);

				if(asyncard_info[current_slot].TC1==0xff)
				{
					N=0;
				}
				else
				{
					N=asyncard_info[current_slot].TC1+1;
				}
				j=(ushort)exp2_n(CWI);
				if(j<=N)
				{
					return ICC_ATR_TB3ERR;
				}
			}

			// ��Ч��TC3ֵ
			if(asyncard_info[current_slot].TC & 0X04)
			{
				if(asyncard_info[current_slot].TC3)
				{
					return ICC_ATR_TC3ERR;
				}
			}
#endif
			return ICC_SUCCESS;
		}
		if(Flag & 0x80){ //  TD1  TD2
			if((USART_ByteReceive(&locData, SC_Receive_Timeout)) == SUCCESS)
			{
				if(k_IccErrPar)
				{
					return ICC_PARERR;
				}
				*(++Buff) = locData;
			}
			else
			{
				return(ICC_ATR_TIMEOUT);
			}

			if(i==1){
				asyncard_info[current_slot].TD1=*Buff;
				asyncard_info[current_slot].TD|=0X01;
				T=*Buff&0x0f;
			}
			else if(i==2){
				asyncard_info[current_slot].TD2=*Buff;
				asyncard_info[current_slot].TD|=0X02;
				T|=*Buff&0x0f; // add 2002/04/23
			}
			else{
				asyncard_info[current_slot].TD3=*Buff;
				T|=*Buff&0x0f;
			}
			edc=edc^*Buff;
			Lenth++;
			Flag=*Buff;
		}
		i++;
	}
//	return ICC_SUCCESS;
}

/*******************************************************************************
* Function Name  : ClearAtr
* Description    :���ATR BUF
* Input          :Channel : ��Ҫ�����ATRbuf���߼���
*
* Output         : None
* Return         : None
******************************************************************************/
void ClearAtr(UINT8 Channel)
{
	asyncard_info[Channel].TA1 = 0x11;
	asyncard_info[Channel].D   = 0x01;
	asyncard_info[Channel].TA2 = 0x00;
	asyncard_info[Channel].TA3 = 0x00;
	asyncard_info[Channel].TA4 = 0x00;
	asyncard_info[Channel].TB1 = 0x00;
	asyncard_info[Channel].TB2 = 0x00;
	asyncard_info[Channel].TB3 = 0x00;
	asyncard_info[Channel].TB4 = 0x00;
	asyncard_info[Channel].TC1 = 0x00;
	asyncard_info[Channel].TC2 = 0x00;
	asyncard_info[Channel].TC3 = 0x00;
	asyncard_info[Channel].TC4 = 0x00;
	asyncard_info[Channel].TD1 = 0x00;
	asyncard_info[Channel].TD2 = 0x00;
	asyncard_info[Channel].TD3 = 0x00;
#ifndef ISO7816_PBOC_CARD
	asyncard_info[Channel].F = 372;
#endif
}

/*******************************************************************************
* Function Name  : IccSendCommand
* Description    :�ֱ����T=1��T=0Э�齫CLA+INS+P1+P2+LC��LE��+INDATA���������ݰ���ʽ
*                 ���� ת����������Ӧ�ĸ�ʽ��IC�����н��������������״̬������OUTDATA��
*                 T=1ʱ�� OUTDATA�� LEN��2��+DATA+SW��1��2����
*                 T=0ʱ�� OUTDATA�� LEN��2��+DATA�� SW���浽ȫ�ֱ�����
* Input          :cla : CLA
*                 ins : INS
*                 p1  : P1
*                 p2  : P2
*                 p3  : P3
*                 indata:data
*                 le   :Le
*                 outdata:���BUF
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��
******************************************************************************/
int IccSendCommand(UINT8 cla, UINT8 ins,UINT8 p1,UINT8 p2,UINT16 p3,UINT8 *indata,UINT16 le,UINT8 *outdata)
{
	UINT8 test[5];
	UINT8 cmd_case;
	UINT8 inbuf[ICMAXBUF];     //������������
	UINT8 outbuf[ICMAXBUF];    //������Ӧ����
	UINT8 *ptr,len;
	int   result;
	UINT16 i;
	EMV_SW1=0xff;
	EMV_SW2=0xff;
	//����T=1��T=0Э�������Ӧ�Ĵ���
	switch(asyncard_info[current_slot].T)
	{
		case 1:            // T=1����ʽ: cla ins p1 p2 lc data le
		{
			i=4;
			inbuf[2]=cla;
			inbuf[3]=ins;
			inbuf[4]=p1;
			inbuf[5]=p2;
			ptr=&inbuf[6];
			if((p3==0)&&(le==0))
			{
				i++;
				*ptr++=0;
			}
			if(p3>0)
			{
				len=(UINT8)p3;
				i+=len+1;
				*ptr++=len;
				if(len>0)
				{
					memcpy(ptr,indata,len);
					ptr+=len;
				}
			}
			if(le>0)
			{
				if(le>=256)  le=0x00;
				i++;
				*ptr++=(UINT8)le;
			}
			inbuf[0]=i/256;
			inbuf[1]=i%256;
			result=T1_send_command(0x00,inbuf,outbuf);
			if(result==0)
				memcpy(outdata,&outbuf[1],outbuf[1]*256+outbuf[2]+2);

			return result;
		}
		case 0:             //T=0����ʽ: cla ins p1 p2 lc data��cla ins p1 p2 le
		{
			cmd_case=0x01;
			if((p3==0)&&(le==0))  cmd_case=0x01;  // case 1 command
			if((p3==0)&&(le>0))   cmd_case=0x02;  // case 2 command
			if((p3>0)&&(le==0))   cmd_case=0x03;  // case 3 command
			if((p3>0)&&(le>0))    cmd_case=0x04;  // case 4 command
			test[0]=cmd_case;

			if(p3>0)		// ����IC������P3���ȵ����ݣ�����LE�ֽ�
			{
				len=(uchar)p3;
				memcpy(inbuf,indata,p3);
			}
			else		 //���IC������LE���������ݣ�ֻLE����
			{
				if(le>=256)  le=0x00;
				len=(uchar)le;
			}
			result=T0_send_command(cla,ins,p1,p2,len,inbuf,outbuf,test);

			if(result)   return result;
			memcpy(outdata,outbuf,outbuf[0]*256+outbuf[1]+2);
			EMV_SW1=test[0];
			EMV_SW2=test[1];
			return ICC_SUCCESS;
		}
		default:
			break;
	}
	return ICC_PROTOCALERR;
}

/*******************************************************************************
* Function Name  : T1_send_command
* Description    :T=1Э���µ��������ݷ����������Ӧ���ݡ�
*                   ע��: T1��֧����չָ����Ҫ֧������Ҫ�޸�
* Input          :NAD : ����ַ������Ϊ0X00����
*                 INBUF : ���͵���Ϣ�ֶ�LEN+INF��INBUF[0]Ϊ���ȣ�
*                 OUTBUF: ����Ӧ�����ݵ�Ԫ����ʽNAD+LEN��2�ֽڣ�+DATA+SW1+SW2
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��
******************************************************************************/
int T1_send_command(UINT8 nad,UINT8 *inbuf,UINT8 *outbuf)
{
	UINT8   sour[300],oldsour[300],respbuf[300],oldrblock[5] = {0};
	UINT8   OLDIFSC;
	UINT8   /*NAD,*/PCB/*,slotpcb*/,tmpch,tmpch1;
	UINT16  len,len1/*,totallen,currentlen,remainlen*/;
	UINT16  total_send_len,sent_len,remain_len;
	UINT8   re_send_i_times,re_send_r_times;
//	UINT16  i;
	int     result;

	OLDIFSC=asyncard_info[current_slot].IFSC;

start_t1_exchange:
	re_send_i_times=0;
	re_send_r_times=0;
	total_send_len=inbuf[0]*256+inbuf[1];
	remain_len=inbuf[0]*256+inbuf[1];
	sent_len=0;
	outbuf[0]=0x00;
	outbuf[1]=0x00;
	outbuf[2]=0x00;
	len=0;len1=0;
	asyncard_info[current_slot].sw1=0Xff;
	asyncard_info[current_slot].sw2=0xff;

send_remain_data:
	if(remain_len==0)
	{
		return ICC_ICCMESERR;
	}
	if(remain_len>asyncard_info[current_slot].IFSC)		// ������ݰ�������ifscʱ���򽫰������зֺ��ٷ���//
	{
		asyncard_info[current_slot].term_pcb|=0x20; //��I����������ӷ�//
		memcpy(&sour[3],&inbuf[2+sent_len],asyncard_info[current_slot].IFSC);
		sour[2] = asyncard_info[current_slot].IFSC;
	}
	else	//����ֻ��һ���Ϳ���ȫ���� //
	{
		asyncard_info[current_slot].term_pcb &= 0xdf;		//ȥ�����ӷ�
		memcpy(&sour[3],&inbuf[2+sent_len],remain_len);
		sour[2]=remain_len;
	}
	sent_len+=sour[2]; // �趨�ѷ������ݳ���  //
	remain_len=total_send_len-sent_len;
	sour[1]=asyncard_info[current_slot].term_pcb; // �趨���Ͱ�ͷ��PCBЭ������ֽ�//
	sour[0]=0x00;		//nad
	memcpy(oldsour,sour,sour[2]+3); // ���ݷ��͵�I���� //
	asyncard_info[current_slot].term_pcb=sour[1] ^ 0x40;	//�����ն��¸�Ҫ���͵�I�������
	re_send_i_times=0;

re_send_i_block:
	if(re_send_i_times>2)		//��������3���κο��û����Ӧ���µ�
	{
		return ICC_T1_MOREERR;
	}
	re_send_i_times++;
	re_send_r_times=0;
	sour[0]=0x00;
	T1_Send_Block(sour);

receive_one_block:
	result=T1_Rece_Block(respbuf);

	if((result==ICC_T1_CWTERR)||(result==ICC_T1_BWTERR)||(result==ICC_T1_ABORTERR))
	{
		//��ʱ��ִ���µ����������R��
		return result;
	}

	if((result==ICC_T1_PARITYERR)||(result==ICC_T1_EDCERR))
	{
		if(re_send_r_times!=0)		//��һ����R�������ط�R��,test case 1778/1784 20140521
		{
			//������һ��R���ҽ��տ���Чʱ,��R�鱻�ط�
			sour[0] = oldrblock[0];
			sour[1] = oldrblock[1];
			sour[2] = oldrblock[2];
			//re_send_r_times = 0;
		}
		else
		{
			// 1.������һ��I���ҽ��տ���Ч�ͷ���һ��R��,R�������N(R)������ʹԤ�ڵ�I��N(S)=N(R)
			// 2.����һ��S(...��Ӧ)���ҽ��յ��Ŀ���Чʱ,�ͷ���һ��R��
			tmpch = asyncard_info[current_slot].card_pcb^0x40;	//���Ϊ���ڽ��տ�Ƭ��I����˳��ţ���Ϊ�ϴν��յ��¸���ţ�//
			sour[1]=((tmpch >> 2) & 0x10) | 0x81; // ����ż/EDC����
			sour[2]=0;
		}
		goto re_send_r_block;
	}
	if(result==ICC_T1_INVALIDBLOCK)
	{
		if(re_send_r_times!=0)		//��һ����R�������ط�R��,test case 1778/1784 20140521
		{
			//������һ��R���ҽ��տ���Чʱ,��R�鱻�ط�
			sour[0] = oldrblock[0];
			sour[1] = oldrblock[1];
			sour[2] = oldrblock[2];
			//re_send_r_times = 0;
			goto re_send_r_block;
		}
		else
		{
			// 1.������һ��I���ҽ��տ���Ч�ͷ���һ��R��,R�������N(R)������ʹԤ�ڵ�I��N(S)=N(R)
			// 2.����һ��S(...��Ӧ)���ҽ��յ��Ŀ���Чʱ,�ͷ���һ��R��
			goto set_rblock_with_othererr;
		}
	}

	PCB=respbuf[1]; // ��ȡ��Ӧ���ݰ�ͷ
	//I��
	if((PCB&0X80)==0X00)
	{
		if(remain_len>0) 		//�������ʣ��������Ӧ���յ�I��
		{
			if(re_send_r_times!=0)		// 20140520
			{
				//������һ��R���ҽ��տ���Чʱ,��R�鱻�ط�
				sour[0] = oldrblock[0];
				sour[1] = oldrblock[1];
				sour[2] = oldrblock[2];
				//re_send_r_times = 0;
				goto re_send_r_block;
			}
			else
			{
				// 1.������һ��I���ҽ��տ���Ч�ͷ���һ��R��,R�������N(R)������ʹԤ�ڵ�I��N(S)=N(R)
				// 2.����һ��S(...��Ӧ)���ҽ��յ��Ŀ���Чʱ,�ͷ���һ��R��
				goto set_rblock_with_othererr;
			}
		}

		//  �յ���I�������кŲ�ͬ����ǰ�յ���I�������к�  ʱ��ȷ�Ͻ�����ȷ����������յ����ݵ�OUTBUF�С�
		if((PCB & 0x40) != (asyncard_info[current_slot].card_pcb & 0x40))
		{
			re_send_r_times=0;
			if(respbuf[2]>0)		//����������ݳ��Ȳ�Ϊ���򱣴��������
			{
				len1 = outbuf[1] * 256 + outbuf[2];
				len=len1 + respbuf[2];
				outbuf[1]= len / 256;
				outbuf[2]= len % 256;
				memcpy(&outbuf[len1+3],&respbuf[3],respbuf[2]);
			}
			asyncard_info[current_slot].card_pcb=PCB;		//�������óɸ��յ������к�

			if((PCB&0X20)==0X20)		// �յ�������I����
			{
				tmpch = asyncard_info[current_slot].card_pcb ^ 0x40;		// ���ó��¸�I_Block˳���
				sour[1]=((tmpch >>2 ) & 0x10) | 0x80;
				sour[2]=0;
				goto re_send_r_block;
			}
			// ����ȷ���յ������ӵ�I�������˳�
			result=ICC_SUCCESS;
			goto end_t1_exchange;
		}
		else 		//���кŴ���
		{
			if(re_send_r_times!=0)		// 20140520
			{
				//������һ��R���ҽ��տ���Чʱ,��R�鱻�ط�
				sour[0] = oldrblock[0];
				sour[1] = oldrblock[1];
				sour[2] = oldrblock[2];
				//re_send_r_times = 0;
				goto re_send_r_block;
			}
			else
			{
				// 1.������һ��I���ҽ��տ���Ч�ͷ���һ��R��,R�������N(R)������ʹԤ�ڵ�I��N(S)=N(R)
				// 2.����һ��S(...��Ӧ)���ҽ��յ��Ŀ���Чʱ,�ͷ���һ��R��
				goto set_rblock_with_othererr;
			}
		}
	}
	//R��
	else if((PCB&0XC0)==0X80)
	{
		tmpch=asyncard_info[current_slot].term_pcb;
		if((tmpch&0x40)==((PCB&0x10)<<2))		// �յ�����ŵ����ն��¸�Ҫ���͵�I������ű���IC��������һ�����ӿ�
		{
			if((PCB&0xef)==0x80)		// r-block�޴�����Ϣ
			{
				if((oldsour[1]&0x20)==0x20) // �ն˸շ��͵�I�������ӱ�ʶ
				{
					goto send_remain_data;
				}
				else 	//�ն�����  ������
				{

					if(re_send_r_times!=0)		// 20140520
					{
						//������һ��R���ҽ��տ���Чʱ,��R�鱻�ط�
						sour[0] = oldrblock[0];
						sour[1] = oldrblock[1];
						sour[2] = oldrblock[2];
						//re_send_r_times = 0;
						goto re_send_r_block;
					}
					else
					{
						//������Ч�Ŀ��ط������к�����һ�������յ���I������кţ������ն˳�ʼI��Ϊ0�������յ���I��ҲΪ0��
						tmpch = asyncard_info[current_slot].card_pcb ^ 0x40;
						tmpch1=((tmpch >> 2 ) & 0x10) | 0x82;  	//���������Ĵ���

						re_send_r_times=0;
						sour[1]=tmpch1;
						sour[2]=0;
						goto re_send_r_block;
					}
				}
			}
			else 		// ���д����ʶ��R����
			{
				if(re_send_r_times!=0)		 // �ط�R����
				{
					//sour[1]=sour[1]&0xf0;		//test case 1772/1809 20140521
					//������һ��R���ҽ��տ���Чʱ,��R�鱻�ط�
					sour[0] = oldrblock[0];
					sour[1] = oldrblock[1];
					sour[2] = oldrblock[2];
					re_send_r_times = 0;			//test case 1774 20140523
					goto re_send_r_block;
				}
				else		// ��һ����R����
				{
					/*tmpch = asyncard_info[current_slot].card_pcb ^ 0x40;
					sour[1]=((tmpch >>2 ) & 0x10) | 0x80;
					sour[2]=0;
					goto re_send_r_block; */
					goto set_rblock_with_othererr;		// test case 1771, 20140521
				}
			}

		}

		if((tmpch&0x40)!=((PCB&0X10)<<2))		// �����ŵ����ն��ѷ��͵�I����������ط���I����
		{
			if((asyncard_info[current_slot].card_pcb&0x20)==0x20) 		//b6�̶�Ϊ0
			{
				goto re_send_r_block;
			}

			if(OLDIFSC!=asyncard_info[current_slot].IFSC)
			{
				//���յ��µ�IFSCЭ�̺����յ�R���������ط�ʱ��
				//�Ȼָ��ն�I�������кţ�����ͷ����
				OLDIFSC=asyncard_info[current_slot].IFSC;
				asyncard_info[current_slot].term_pcb=oldsour[1];
				goto start_t1_exchange;
			}
			memcpy(sour,oldsour,oldsour[2]+3);
			goto re_send_i_block;
		}
		// �ط�R����
		//goto re_send_r_block;
	}
	//S��
	else if((PCB&0XC0)==0XC0)
	{
		if((PCB & 0x20)==0X00)		//���յ���IC�����͵�S�����ź�/
		{
			//����յ�����IC�����͵������ź�����ݴ�����������
			//�����޸���Ӧ���趨������S��Ӧ�����IC��
			sour[1]=0x20 | respbuf[1] ;
			sour[2]=respbuf[2];
			memcpy(&sour[3],&respbuf[3],sour[2]);
			if((respbuf[1] & 0x1f) ==0x03)		// ����BWT����  BWT=BWT * respbuf[3];
			{
				WTX= respbuf[3];
			}
			else if ((respbuf[1] & 0x1f) == 0x01)		 //��Ϣ�ֶ���������
			{
				asyncard_info[current_slot].IFSC = respbuf[3];
			}
			else if((respbuf[1]&0x1f)==0x00)		 // ͬ������
			{
			}
			sour[0]=0x00;  //����S��Ӧ�����IC��
			T1_Send_Block(sour);
			re_send_r_times=0;
			goto receive_one_block;
		}
		else 		//���յ���IC�����͵�S��Ӧ�ź�/
		{
		/*
			if((sour[1]&0xe0)==0xc0)
			{
				// ����ն˷�����������Ϣʱ�������Ӧ������Ӧ�Ĳ���
				re_send_r_times=0;
				if ((respbuf[1] & 0x1f) == 0x01)		//ifsd response
				{
					asyncard_info[current_slot].IFSD = respbuf[3];
				}
				else if((respbuf[1] & 0x1f) ==0x00)	//��ͬ����Ӧ
				{    //  Reset or deactivation
					asyncard_info[current_slot].term_pcb=0x00;
					asyncard_info[current_slot].card_pcb=0x40;
					goto start_t1_exchange;
				}
				goto end_t1_exchange;
			}
		*/
			if(re_send_r_times!=0)		// test case 1778, 20140521
			{
				//������һ��R���ҽ��տ���Чʱ,��R�鱻�ط�
				sour[0] = oldrblock[0];
				sour[1] = oldrblock[1];
				sour[2] = oldrblock[2];
				//re_send_r_times = 0;
				goto re_send_r_block;
			}
		}
	}

set_rblock_with_othererr:
	tmpch = asyncard_info[current_slot].card_pcb ^ 0x40;
	sour[1]=((tmpch >>2 ) & 0x10) | 0x82; // �����������R����
	sour[2]=0;

re_send_r_block:
	if(re_send_r_times!=0)
	{
		if((oldrblock[0]!=sour[0])||(oldrblock[1]!=sour[1])||(oldrblock[2]!=sour[2]))
			re_send_r_times=0;
	}
	if(re_send_r_times==0)
	{
		oldrblock[0]=sour[0];
		oldrblock[1]=sour[1];
		oldrblock[2]=sour[2];
	}
	if(re_send_r_times>1)
	{
		return ICC_T1_MOREERR;
	}
	T1_Send_Block(sour);
	re_send_r_times++;
	goto receive_one_block;

end_t1_exchange:
	if(result!=0)
	{
		//���ͨ���봦��ʧ�ܣ�����SW1 SW2Ϊ0XFF��
		asyncard_info[current_slot].sw1=0Xff;
		asyncard_info[current_slot].sw2=0xff;
		return result;
	}
	len=outbuf[1]*256+outbuf[2]+3;
	// �����յ��������λ���ݱ��浽SW1��SW2
	asyncard_info[current_slot].sw1=outbuf[len-2];
	asyncard_info[current_slot].sw2=outbuf[len-1];
	return ICC_SUCCESS;
}

/*******************************************************************************
* Function Name  : T0_send_command
* Description    :T=0Э���·�������ͷ��CLA+INS+P1+P2+P3�����ݰ���DATABUF��������ȡIC����Ӧ�����ݣ�OUTBUF����״̬�ֽڣ�RSLT����
* Input          :Class : CLA
*                 Ins : INS
*                 p1  : P1
*                 p2  : P2
*                 p3  : P3
*                 databuf�����ն˷��������ݰ������䳤��ΪP3����
*                 outbuf:��IC���������������ݣ�[0]Ϊ���ݳ��ȣ�
*                 relt:��CASE����ţ�1��2��3��4��
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��       ����ʱ����SW1��SW2״̬�ֽڡ�
******************************************************************************/
int T0_send_command(UINT8 cla,UINT8 ins,UINT8 p1,UINT8 p2,UINT8 p3, UINT8 *databuf,UINT8 *outbuf,UINT8 *rslt)
{
	UINT16  len,i,len1,len2/*,Tc2*/;
	UINT8   ret;
	UINT8   tempsw1,tempsw2;
	UINT8   *ptr1,*ptr2,status[2],cmdbuf[5];
	UINT8   cmd_case,cardtype,auto_resp,is_rece_send;
	UINT8   Change_Status_Word;
#ifdef EMV_Co_Test
	UINT8   GetIns = 0;
#endif
	UINT16  /*NS,*/NR/*, temp*/;
	UINT8   first_error_status = 0;

	NR=16+2;                 // ���򱣻�ʱ��,����������2��etu
	cmd_case=*rslt;        // ����CASE�� 1��2��3��4//
	cardtype = asyncard_info[current_slot].autoresp;			//�Ƿ��Զ�����GetResponse
	if(cardtype)           // �����Զ�Ӧ���־ //
		auto_resp=0x00;   // ���Զ� //
	else
		auto_resp=0x01;   // �Զ�//
	Change_Status_Word=0;  // ���·���״̬�ֽڱ�־��1-���� //

	//��ʱ�������ʼ��
	k_Ic_TotalTimeCount=0;//ÿ����ʱ����0
	k_IcMax_TotalTimeCount=0;
	k_total_timeover=0x00;

	//ʹ��WTC��ʱ�ַ��ȴ�ʱ��
	k_IcMax_TimeCount = 0;
	k_IcMax_EtuCount = (WWT+480+50)*(asyncard_info[current_slot].D);
	USI_OpenWTC(k_IcMax_EtuCount);

	is_rece_send=0;        //��ǰ�������ݰ����������״̬��0=���գ�1=����
	if(cmd_case==0x01) is_rece_send=0;
	else if(cmd_case==0x02) is_rece_send=0;
	else if(cmd_case==0x03) is_rece_send=1;
	else is_rece_send=1;

	//������ͷ�� //
	cmdbuf[0]=cla;
	cmdbuf[1]=ins;
	cmdbuf[2]=p1;
	cmdbuf[3]=p2;
	cmdbuf[4]=p3;
	status[0]=0xff;
	status[1]=0xff;
	len=0;
	ptr1=databuf;
	ptr2=outbuf+2;
	tempsw1=0xff;
	tempsw2=0xff;

	while(1)
	{
flg1:
		len1=(ushort)cmdbuf[4];  // �����������ݳ���
		len2=(ushort)cmdbuf[4];  // ������Ӧ���ݳ���
		if (len2 == 0)		// le=0ʱ�ָ���256��20140525
		{
			len2 = 256;
		}

		DelayNETU(NR);		//���򱣻�ʱ��
		for(i=0;i<5;i++)
		{
			ret=SC_PutByte(cmdbuf[i]);		//����CLA INS P1 P2 P3
#if 0
   			printf ("%x",cmdbuf[i]);
#endif
			if(ret)
			{
				return ICC_T0_MORESENDERR;
			}
		}

flg2:
		//��ȡһ�����ֽ�
		if(SC_GetByte(&status[0]) == ERROR)		//�෴���򣬼��ն˷�������IC������
		{
#if 0
   			printf ("%x",status[0]);
#endif

			return ICC_T0_TIMEOUT;			//��ʱ������ط�ʧ�ܺ󶼻ᵽ����
		}

		// �����ֽڵ���INS----���ͻ������������ݰ�  //
		if(status[0]==cmdbuf[1])
		{
#ifdef EMV_Co_Test
			GetIns = 1;
#endif
			if(is_rece_send) 		//Lc����
			{
				DelayNETU(NR);		//���򱣻�ʱ��
				for(i=0;i<len1;len1--)
				{
					ret=SC_PutByte(*ptr1);
					if(ret)
					{
						return ICC_T0_MORESENDERR;
					}
					ptr1++;
				}

				goto flg2;  // �ȴ���һ�������ֽ� //
			}
			else		// Le����
			{
				len+=len2;
				if(len>256)
				{
					return ICC_DATA_LENTHERR;
				}
				for(i=0;i<len2;len2--)
				{
					if(SC_GetByte(ptr2) == ERROR)
					{
						return ICC_T0_TIMEOUT;			//��ʱ������ط�ʧ�ܺ󶼻ᵽ����
					}
					ptr2++;
				}
				goto flg2;
			}
		}

		// ����INS�Ĳ���  ���ջ�����һ������  //
		else if(status[0]==(uchar)(~cmdbuf[1]))
		{
#ifdef EMV_Co_Test
			GetIns = 1;
#endif
			if(is_rece_send)		//Lc����
			{
				DelayNETU(NR);		//���򱣻�ʱ��
				ret=SC_PutByte(*ptr1);
				if(ret){
					return ICC_T0_MORESENDERR;
				}
				ptr1++;
				len1--;
				goto flg2;
			}
			else 		//Le����
			{
				if(SC_GetByte(ptr2) == ERROR)
				{
					return ICC_T0_TIMEOUT;			//��ʱ������ط�ʧ�ܺ󶼻ᵽ����
				}

				ptr2++;len++;
				len2--;
				if(len>256){
					return ICC_DATA_LENTHERR;
				}
				goto flg2;
			}
		}
		else if(status[0]==0x60)
		{ //  �ն��ṩ���ӵĹ����ȴ�ʱ��  //
			goto flg2;
		}
#ifdef EMV_Co_Test
		else
		{
			if(cmd_case != 4)
			{
				GetIns = 1;
			}
		}
#endif
		if(((status[0]&0xf0)!=0X60)&&((status[0]&0xf0)!=0x90))		//sw1
		{  //�յ�����Ĺ��̻�״̬�ֽں��ͷŴ���
			return ICC_T0_INVALIDSW;
		}
		// ���SW1Ϊ��6X����9X������60�����⣩ʱ����ȴ��ù��̵���һ��״̬�ֽ�SW2
		if(SC_GetByte(&status[1]) == ERROR)
		{
			return ICC_T0_TIMEOUT;			//��ʱ������ط�ʧ�ܺ󶼻ᵽ����
		}
#ifdef EMV_Co_Test
		/*�����û���յ�INSǰ�յ�SW����ôֱ�ӷ���״̬��*/
		if(auto_resp && GetIns) 		//�Զ���Ӧ  //
		{
			GetIns = 0;
#else
		if(auto_resp) 		//�Զ���Ӧ  //
		{
#endif
			if(status[0]==0x61)
			{    // ���SW1=0X61���ն����ͳ�һ��GET RESPONSE����ͷ��
				// ��IC�������е�P3=SW2 //
				cmdbuf[0]=0x00;cmdbuf[1]=0xc0;cmdbuf[2]=0x00;
				cmdbuf[3]=0x00;cmdbuf[4]=status[1];
				is_rece_send=0;  // ����Ĳ���ֻ�ܴ�IC���������ݰ�//
				if(cmd_case==0x04) cmd_case=0x02;
				goto flg1;       // �����¸�����ͷ��
			}
			if(status[0]==0x6c)
			{    // ���SW1=0X6C���ն˴�����ǰ������ͷ���IC����
				// ����P3=SW2�� //
				cmdbuf[4]=status[1];
				is_rece_send=0;
				if(cmd_case==0x04) cmd_case=0x02;
				goto flg1;
			}
			if(cmd_case==0x04)//&&(FirstByte_Is_Status==0))
			{
				if (first_error_status==0)
				{
					if (status[0]==0x62 && status[1]==0x81)
					{
						first_error_status = 1;		// test case 1744(x=3) 20140524
						goto flg1;		//�ط�������
					}
					if((status[0]==0x62)||(status[0]==0x63)||
						(((status[0]&0xf0)==0x90) && (!(status[0]==0x90 && status[1]==0x00))))   	// sw = 6283/6335/9FFF
					{
						cmdbuf[0]=0x00;cmdbuf[1]=0xc0;cmdbuf[2]=0x00;
						cmdbuf[3]=0x00;cmdbuf[4]=0x00;
						is_rece_send=0;
						Change_Status_Word=1;
						cmd_case=0x02; //�޸�CASE״̬��ֹ�ط� //
						tempsw1=status[0];
						tempsw2=status[1];
						goto flg1;
					}
				}
				else
				{
					if (status[0]==0x9F && status[1]==0xFF)
					{
						//�˳�
					}
					else
					{
						goto flg1;		//�ط�������
					}
				}
			}

		}
		// ���յ��������ݵĹ����ֽ�ʱ�򱣴���״̬�ֽڲ�����
		if(Change_Status_Word)
		{
			*rslt=tempsw1;
			*(rslt+1)=tempsw2;//status[1];//
		}
		else
		{
			*rslt=status[0];
			*(rslt+1)=status[1];
		}
		*outbuf=len/256;
		*(outbuf+1)=len%256; // ������յ����ݳ��� //
		return ICC_SUCCESS;
	}
//	return ICC_SUCCESS;
}

/*******************************************************************************
* Function Name  : T1_IFSD_command
* Description    :���յ���ȷ�ĸ�λӦ��󣬶���T=1��Ƭ�뷢��һ��IFSD������ָʾ�ն�����ȡ������鳤�ȡ�
* Input          :slot : �����߼���
*
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��
******************************************************************************/
int  T1_IFSD_command(UINT8 slot)
{
	int   ret;
	UINT8 ifsdbuf[10],respbuf[300];
	UINT8 resendtimes=0;
//	UINT8 tmpch;

	if(asyncard_info[slot].T==0)
	{
		return ICC_SUCCESS;
	}
	ifsdbuf[0]=0x00;
	ifsdbuf[1]=0xc1;
	ifsdbuf[2]=0x01;
	ifsdbuf[3]=0xfe;
	ifsdbuf[4]=0x00; //0-3��� У��ֵ

	while(1)
	{
		if(resendtimes<3)
		{
			ret=T1_Send_Block(ifsdbuf);
		}
		else
		{
			return ICC_T1_IFSDERR;
		}
		resendtimes++;
		ret=T1_Rece_Block(respbuf);
		if((ret==ICC_T1_CWTERR)||(ret==ICC_T1_BWTERR)||(ret==ICC_T1_ABORTERR))
		{
			return ret;
		}

		//����һ��S(...����)���ҽ��յ���Ӧ����S(...��Ӧ)��(��IFD)����ʱ,��S(...����)���ط�
		if((ret==ICC_T1_PARITYERR)||(ret==ICC_T1_EDCERR)||(ret==ICC_T1_INVALIDBLOCK))
			continue;
		if(((respbuf[1]&0xe0)!=0xe0)||((respbuf[1]&0x0f)!=0x01))		//����ֵ����Ϊ00E101FE1E
			continue;
		if((respbuf[2]!=0x01)||(respbuf[3]!=0xfe))
			continue;

		asyncard_info[slot].IFSD=254;
		return ICC_SUCCESS;
	}
//	return ICC_SUCCESS;
}

/*******************************************************************************
* Function Name  : T1_Send_Block
* Description    :T=1Э���·���һ����
* Input          :Inbuf : �����������buf
*
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��
******************************************************************************/
int  T1_Send_Block(UINT8 *Inbuf)
{
	UINT8   edc;
	UINT16  i,len;
	UINT16  NR;

	NR=22+200;		//����������200��etu, Test case 1767_2x 20140530

	len=*(Inbuf+2)+3;   //  �����ܳ�NAD+PCB+LEN+��LEN��DATA��
	edc=0;              //  ��������������Ԫ��У��EDC

	DelayNETU(NR);//�����ͱ���ʱ����
	for(i=0;i<len;i++)
	{
		SC_PutByte(Inbuf[i]);
		edc=edc ^ Inbuf[i];
	}
	SC_PutByte(edc);

	//T1�Ľ��ն������⵽��żУ�������������źţ����Է��Ͷ˲����⵽�����źţ�ֱ�ӷ��سɹ�
	return ICC_SUCCESS;
}

/*******************************************************************************
* Function Name  : T1_Rece_Block
* Description    :T=1Э���½���һ����
* Input          :Outbuf : �������ݻ���buf
*
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��
******************************************************************************/
int  T1_Rece_Block(UINT8 *Outbuf)
{
	UINT8   i,ch,edc;
	UINT8   Parity_Error_Flag;
//	UINT32  T1BWT,T1CWT;
	UINT8   r_nad,r_pcb,r_len,r_inf;
	UINT8   r_len_temp;

	k_IcMax_TotalTimeCount=0x00;
	k_Ic_TotalTimeCount=0x00;
	k_total_timeover=0x00;

	if (WTX!=0)	//IC���շ�����WTX����
	{
		//ʹ��WTC��ʱ�ַ��ȴ�ʱ��
		k_IcMax_TimeCount = 0;
		k_IcMax_EtuCount = ((BWT-11)*(asyncard_info[current_slot].D)+11)*WTX+(960+50)*(asyncard_info[current_slot].D)*WTX;		//BWT*n+D*960*n
		USI_OpenWTC(k_IcMax_EtuCount);

		WTX=0;
	}
	else
	{
		k_IcMax_TimeCount = 0;
		k_IcMax_EtuCount = ((BWT-11)+960+50)*(asyncard_info[current_slot].D)+11;
		USI_OpenWTC(k_IcMax_EtuCount);
	}

		//���������¸��ֽڵĵȴ�ʱ�䣬���յ��¸��ֽڵ���ʼλʱ����WTC
		k_IcMax_TimeCount = 0;
		k_IcMax_EtuCount = CWT+4+1;		//CWT+4<�ն˷�Χ<CWT+4+2

	edc=0;
	Parity_Error_Flag=0;
	if(SC_GetByte(&ch) == ERROR)		//NAD
	{
		return ICC_T1_BWTERR;
	}

	if(k_IccErrPar) Parity_Error_Flag=0x01;//
	edc=edc ^ ch;
	*Outbuf++=ch;
	r_nad=ch;

	if(SC_GetByte(&ch) == ERROR)		//PCB
	{
		return ICC_T1_CWTERR;
	}
	if(k_IccErrPar) Parity_Error_Flag=0x01;//
	edc=edc ^ ch;
	*Outbuf++=ch;
	r_pcb=ch;

	if(SC_GetByte(&ch) == ERROR)		//��ȡ����
	{
		return ICC_T1_CWTERR;
	}
	if(k_IccErrPar) 			// 20131218 1CF 086.00/02��Ӧ�����������ַ����µ�
	{
		//T1ʱ��żУ���Ҳ�ܻ�ȡ��ȷ����
		Parity_Error_Flag=0x01;//
	}
	edc=edc ^ ch;
	*Outbuf++=ch;
	r_len=ch;
	r_len_temp=ch;
	if((r_pcb&0xc0)==0x80) 		// R��
	{
	//if(ch) r_len_temp=0;		// 1CF 087.01 20131219
	}
	else if((r_pcb&0xc0)==0xc0)		//S��
	{
	//if(ch>1) r_len_temp=1;		//  1CF 087.01 20131219
	}

	for(i=0;i<r_len_temp;i++)
	{
		if(SC_GetByte(&ch) == ERROR)
		{
			return ICC_T1_CWTERR;
		}
		if(k_IccErrPar) Parity_Error_Flag=0x01;
		edc=edc ^ ch;
		*Outbuf++=ch;
	}
	r_inf=ch;		//S�����Ϣ��ֻ��1���ֽ�
	if(SC_GetByte(&ch) == ERROR)		//EDC
	{
		if(((r_pcb&0x80)==0)&&(r_len==0xff)){ 		//I��ĳ���ΪFF������
			//           return ICC_T1_INVALIDBLOCK;		//��R��,20131220
			return ICC_T1_ABORTERR;		//�µ�,20131220
		}
		return ICC_T1_CWTERR;
	}
	if(k_IccErrPar) Parity_Error_Flag=0x01;//
	*Outbuf=ch;
	if(ch!=edc) return ICC_T1_EDCERR;
	if(Parity_Error_Flag) return ICC_T1_PARITYERR;
	if(r_nad) return ICC_T1_INVALIDBLOCK;   //nad!=0

	// I_BLOCK
	if((r_pcb&0x80)==0)
	{
		if(r_len==0xff) 	//I�����Ϣ�򳤶�ΪFF������
		{
#if 0      //��R��    	,20131220
			for (i=0; i<120; i++)		// 12etu*10
			{
			delay1ETU();	// 20131219, 1CF 089.02, 1CF 096.01 ���ܻᷢ�ͳ���255�ֽڵ����ݣ���ʵ��δ���գ����ICC�����ٷ�R��
			if ((i%12) == 0)		// 20131219, 1CF 092.02/ 1CF 094.06/ 1CF 098.01 �建����,ÿ���ֽ���һ��
			{
			test = USIRDR;		//������������ᵼ��1CF087.02����?20131219
			}
			}
			return ICC_T1_INVALIDBLOCK;
#else	//�µ�,20131220
			/*
			for (i=0; i<120; i++)		// 12etu*10
			{
			delay1ETU();	// 20131219, 1CF 089.02, 1CF 096.01 ���ܻᷢ�ͳ���255�ֽڵ����ݣ���ʵ��δ���գ����ICC�������µ�
			}
			*/
			return ICC_T1_ABORTERR;
#endif
		}
	}
	// R_BLOCK
	else if((r_pcb&0xc0)==0x80)
	{
		if(r_len) return ICC_T1_INVALIDBLOCK;		//r�����Ϣ�򳤶Ȳ�Ϊ0������
		if((r_pcb&0x20)==0x20) return ICC_T1_INVALIDBLOCK;  	//b6�̶�Ϊ0
	}
	// S_BLOCK
	else
	{
		if(r_pcb==0xc1){ // ifs request
			if((r_inf<0x10)||(r_inf>0xfe)) return ICC_T1_INVALIDBLOCK;
		}
		else if(r_pcb==0xc2)  return ICC_T1_ABORTERR;	// abort request
		else if(r_pcb==0xe3) return ICC_T1_INVALIDBLOCK; //S(WTX response)
		else if(r_pcb==0xe2) return ICC_T1_INVALIDBLOCK; //S(ABORT response)
		else if(r_pcb==0xe0) return ICC_T1_INVALIDBLOCK; //S(RESYNCH response)
		if(r_len!=1) return ICC_T1_INVALIDBLOCK;   	//s�����Ϣ�򳤶Ȳ�Ϊ1������
		if((r_pcb&0x1f)>4) return ICC_T1_INVALIDBLOCK; 	//������s������
	}
	return ICC_SUCCESS;
}
