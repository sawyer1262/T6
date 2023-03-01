#include "uart_drv.h"
#include "uart_fp.h"
#include "i2c_drv.h"
#include "uart.h"
#include "timer.h"
#include "debug.h"
#include "audio.h"
#include "key.h"
#include "keypad_menu.h"
#include "eport_drv.h"
#include "uart_back.h"
#include "lpm.h"
#include "delay.h"
#include "string.h"
#include "led.h"
#include "lock_config.h"
#include "rtc.h"
#include "bat.h"
#include "wdt_drv.h"
#include "lock_record.h"


LdFpStr_t LdFp;
UartTxStr_t FpUartTxQ[FP_TX_Q_SIZE];
uint8_t LastCmd=0;
uint8_t FpCurIdx=0;

uint8_t LdFpAck=0;                         //ָ��ģ��Ӧ���ʾ
uint16_t idyId=0;                          //ʶ��ɹ���ָ��ID
FpAdd_t FpAdd={FP_ADD_GET_IMG,0,0};        //���ָ�� 
FpScan_t FpScan={FP_IDLE,0,0};             //ɨ��ָ��
FpDel_t FpDel={FP_DEL_READ_USER,0,0};      //ɾ��ָ��

uint8_t FpIdyLock=0;
uint8_t FpPowerDown=0; 
uint8_t nofingertimes=0;

/******************************************************************************/
/*
//ָ�ƴ��ڽṹ��ʼ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UartFp_DatStrInit(void)
{
		UartStruct[UART_FP_INDEX].pTxStart=0;
		UartStruct[UART_FP_INDEX].pTxEnd=0;
		UartStruct[UART_FP_INDEX].rp=UartStruct[UART_FP_INDEX].wp=0;
		UartStruct[UART_FP_INDEX].RxStat=RX_IDLE;
		UartStruct[UART_FP_INDEX].TxStat=TX_IDLE;
		UartStruct[UART_FP_INDEX].RtxTimeout=0;
		UartStruct[UART_FP_INDEX].TxTime=0;
		UartStruct[UART_FP_INDEX].RxTime=0;
	
		FpCurIdx=0xFF;
		
		FpMcuPowerIoInit();
}

/******************************************************************************/
/*
//ָ�Ƶ�Դ�봥������IO��ʼ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void FpMcuPowerIoInit(void)
{
		EPORT_ConfigGpio(FP_TOUCHOUT_PIN,GPIO_INPUT);
		EPORT_PullConfig(FP_TOUCHOUT_PIN,EPORT_DISPULL);  
		FpMcuPowerOn();
}
/******************************************************************************/
/*
//��ָ�Ƶ�Դ
input:   none
output   none
return   none 
*/
/******************************************************************************/
void FpMcuPowerOn(void)
{
		I2C_ConfigGpio(I2C2,FP_MCU_VCC_PIN,GPIO_OUTPUT);
		DelayMS(5);
		I2C_WriteGpioData(I2C2,FP_MCU_VCC_PIN,Bit_RESET);
}
/******************************************************************************/
/*
//�ر�ָ�Ƶ�Դ
input:   none
output   none
return   none 
*/
/******************************************************************************/
void FpMcuPowerOff(void)
{
		I2C_ConfigGpio(I2C2,FP_MCU_VCC_PIN,GPIO_OUTPUT);
		DelayMS(5);
		I2C_WriteGpioData(I2C2,FP_MCU_VCC_PIN,Bit_SET); 
}
/******************************************************************************/
/*
//ָ�ƴ��ڳ�ʼ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UartFp_Init(void)
{
    UART_InitTypeDef UART_InitStruct;
    UART_InitStruct.UART_BaudRate = 57600;
    UART_InitStruct.UART_Mode = UART_INT_MODE;//����CPU�жϷ�ʽ
		UART_InitStruct.UART_Parity = UART_PARITY_NONE;
		UART_InitStruct.UART_FrameLength = UART_DATA_FRAME_LEN_10BIT;
    UART_InitStruct.UART_StopBits = 1;
		UartStruct[UART_FP_INDEX].RtxTimeout=10;
		UART_Init(UART_FP_SCI, &UART_InitStruct);
}
/******************************************************************************/
/*
//ָ�ƴ��ڳ���
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UartFp_Deinit(void)
{
		//��ʹ������������������ʹ������������оƬ������ʹ�ܺ�����Ǹߣ�����ϵͳ�������ߣ��ѷ�����ԭ��
		EPORT_ConfigGpio(FP_TOUCHOUT_PIN,GPIO_INPUT);
		EPORT_PullConfig(FP_TOUCHOUT_PIN,EPORT_DISPULL);    
		//RX�����
		UART_ConfigGpio(UART_FP_SCI,UART_RX,GPIO_OUTPUT);
		UART_WriteGpioData(UART_FP_SCI,UART_RX,Bit_RESET);
		//TX�����
		UART_ConfigGpio(UART_FP_SCI,UART_TX,GPIO_OUTPUT);
		UART_WriteGpioData(UART_FP_SCI,UART_TX,Bit_RESET);
	
		//�ص�Դ
		FpMcuPowerOff();
}
/******************************************************************************/
/*
//ָ�ƴ��ڷ�������
input:   buf -- �����͵�����
         len -- ���ݳ���
output   none
return   none
*/
/******************************************************************************/
void UartFp_SendNbytes(uint8_t* buf,uint16_t len)      
{
		if(UartStruct[UART_FP_INDEX].TxStat!=TX_IDLE)return;
		
		UartStruct[UART_FP_INDEX].TxStat=TX_RUNNING;
		UartStruct[UART_FP_INDEX].pTxStart=buf;
		UartStruct[UART_FP_INDEX].pTxEnd=buf+len;
		UartStruct[UART_FP_INDEX].TxTime=GetTimerCount();
		UART_FP_SCI->SCIDRL=*UartStruct[UART_FP_INDEX].pTxStart++;
		UART_FP_SCI->SCICR2|=SCICR2_TIE_MASK;  
	
//		printf("fpsend start\n");
}

/******************************************************************************/
/*
//ָ�ƴ�������У��
input:   cpRecBuf -- ����У������ݰ�
output   none
return   nwCHK  -- ���ɵ�У��
*/
/******************************************************************************/
uint16_t GenFpChkSum(uint8_t *cpRecBuf)
{
		uint16_t wLen;
		uint16_t i;
		uint16_t nwCHK = 0;

		wLen = ((uint16_t)cpRecBuf[7]<<8) + cpRecBuf[8];

		for(i = 6; i < 7+wLen;i++)            //wLen-2+3
		{
				nwCHK += cpRecBuf[i];
		}
			
		cpRecBuf[7+wLen] = nwCHK>>8;
		cpRecBuf[8+wLen] = nwCHK;

		return(nwCHK);
}
/******************************************************************************/
/*
//ָ�ƴ��ڷ���ָ��
input:   type -- ָ�����ͣ���Э���ĵ�
         buf -- ָ�������
         len -- ָ������ݳ���
				 ack -- ����ָ���Ƿ���ҪӦ��1-��Ҫ�� 0-����Ҫ��  2-��Ҫ�������͵�Ӧ������
         timeout -- �ȴ�Ӧ��ʱʱ�䣬��λΪ����
output   none
return   none 
*/
/******************************************************************************/
void UartFp_SendCmdData(FpPktType_t type,uint8_t *buf,uint16_t len,uint8_t ack,uint32_t timeout)
{
		uint8_t i=0,j=0;
		uint16_t chksum=0;
	
		for(i=0;i<FP_TX_Q_SIZE;i++)
		{
				if(FpUartTxQ[i].DatStat==0)
				{
						FpUartTxQ[i].DatBuf[0]=FP_HEAD_H;
						FpUartTxQ[i].DatBuf[1]=FP_HEAD_L;
						FpUartTxQ[i].DatBuf[2]=0xFF;
						FpUartTxQ[i].DatBuf[3]=0xFF;
						FpUartTxQ[i].DatBuf[4]=0xFF;
						FpUartTxQ[i].DatBuf[5]=0xFF;
						FpUartTxQ[i].DatBuf[6]=type;
						FpUartTxQ[i].DatBuf[7]=(len+2)>>8;    //lenֻ�������ݣ�������У��
						FpUartTxQ[i].DatBuf[8]=len+2;
						for(j=0;j<len;j++)
						{
								FpUartTxQ[i].DatBuf[9+j]=buf[j];
						}
						chksum=GenFpChkSum(FpUartTxQ[i].DatBuf);
						FpUartTxQ[i].DatBuf[9+len]=chksum>>8;
						FpUartTxQ[i].DatBuf[10+len]=chksum;
						
						FpUartTxQ[i].DatBufLen=len+11;
						FpUartTxQ[i].DatStat=1;
						FpUartTxQ[i].NeedAck=ack;
						FpUartTxQ[i].Timeout=timeout;
						FpUartTxQ[i].Retry=0;
						return;
				}
		}
}
/******************************************************************************/
/*
//ָ�ƴ������ݷ��ʹ���
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UartFp_DataSendHandle(void)
{
		uint8_t i=0;
		
		if(UartStruct[UART_FP_INDEX].TxStat!=TX_IDLE)return;
		
		for(i=0;i<FP_TX_Q_SIZE;i++)
		{
				if(FpUartTxQ[i].DatStat==1 && FpUartTxQ[i].NeedAck==2)        
				{
						UartFp_SendNbytes(FpUartTxQ[i].DatBuf,FpUartTxQ[i].DatBufLen);
						FpUartTxQ[i].DatStat=0;
						FpCurIdx=0xFF;
						return;
				}
		}
		
		if(FpCurIdx<FP_TX_Q_SIZE)
		{
				if(GetTimerElapse(FpUartTxQ[FpCurIdx].TxTime)<FpUartTxQ[FpCurIdx].Timeout)return;
				
				UartFp_SendNbytes(FpUartTxQ[FpCurIdx].DatBuf,FpUartTxQ[FpCurIdx].DatBufLen);
				LastCmd=FpUartTxQ[FpCurIdx].DatBuf[FP_LEN_IDX+2];
				FpUartTxQ[FpCurIdx].TxTime=GetTimerCount();
				if(++FpUartTxQ[FpCurIdx].Retry>=3)
				{
						FpUartTxQ[FpCurIdx].DatStat=0;
						FpCurIdx=0xFF;
				}
				return;
		}
		
		for(i=0;i<FP_TX_Q_SIZE;i++)
		{
				if(FpUartTxQ[i].DatStat==1)
				{
						UartFp_SendNbytes(FpUartTxQ[i].DatBuf,FpUartTxQ[i].DatBufLen);
						LastCmd=FpUartTxQ[i].DatBuf[FP_LEN_IDX+2];        //������һ�����ݵ�CMD
						if(FpUartTxQ[i].NeedAck==1)
						{
								FpUartTxQ[i].TxTime=GetTimerCount();
								FpUartTxQ[i].Retry++;
								FpCurIdx=i;
						}
						else
						{
								FpUartTxQ[i].DatStat=0;
								FpCurIdx=0xFF;
						}
						return;
				}
		}
}
/******************************************************************************/
/*
//���յ���ָ�ƴ������ݴ���
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UartFp_RxDataHandle(void)
{
		uint8_t pBufRec[UART_RECV_MAX_LEN]={0};
		
		uint8_t *pFrameHead=0;
		uint16_t i=0,datLen=0,pktchksum=0,pktLen=0;
		
		if(UartStruct[UART_FP_INDEX].RxFrameOk==0)return;
		
		UartStruct[UART_FP_INDEX].RxFrameOk=0;
	
		while(UartStruct[UART_FP_INDEX].wp != UartStruct[UART_FP_INDEX].rp)
		{
				pBufRec[i++]=UartStruct[UART_FP_INDEX].dat[UartStruct[UART_FP_INDEX].rp++];
				
				if(UartStruct[UART_FP_INDEX].rp>=UART_RECV_MAX_LEN)
				{
						UartStruct[UART_FP_INDEX].rp=0;
				}
		}
		datLen=i;
		for(i=0;i<datLen;i++)
		{
				if(FP_HEAD_H==pBufRec[i])
				{
						if(i+1<datLen && FP_HEAD_L==pBufRec[i+1])break;
				}
		}
	//	UartFp_SendNbytes(pBufRec,i);        //�շ�����
		datLen-=i;
		
		if(datLen<11)return;
		pFrameHead=&pBufRec[i];
		
		pktLen=((uint16_t)pFrameHead[FP_LEN_IDX]<<8)|pFrameHead[FP_LEN_IDX+1];
		pktchksum=((uint16_t)pFrameHead[pktLen+FP_LEN_IDX]<<8)|pFrameHead[pktLen+FP_LEN_IDX+1];
		 
		if(pktchksum!=GenFpChkSum(pFrameHead))return;   
		
		if(pFrameHead[FP_TYPE_IDX]==FP_ACK)     //����Ӧ���
		{
				if(FpCurIdx<FP_TX_Q_SIZE)           
				{
						FpUartTxQ[FpCurIdx].DatStat=0;
						FpCurIdx=0xFF;
				}
				switch(LastCmd)      //ָ����
				{
						case PS_ReadSysPara:            //��ϵͳ����
								LdFpAck=FP_ACK_FLAG+pFrameHead[FP_LEN_IDX+2];
								if(pFrameHead[FP_LEN_IDX+2]==PS_OK)           //ȷ����OK
								{
										LdFp.SysStat=(uint16_t)pFrameHead[FP_LEN_IDX+3]<<8|pFrameHead[FP_LEN_IDX+4];
										LdFp.SenType=(uint16_t)pFrameHead[FP_LEN_IDX+5]<<8|pFrameHead[FP_LEN_IDX+6];
										LdFp.MaxFpSum=(uint16_t)pFrameHead[FP_LEN_IDX+7]<<8|pFrameHead[FP_LEN_IDX+8];
										LdFp.SecLevel=(uint16_t)pFrameHead[FP_LEN_IDX+9]<<8|pFrameHead[FP_LEN_IDX+10];
										LdFp.DevAddr=(uint32_t)pFrameHead[FP_LEN_IDX+11]<<24|(uint32_t)pFrameHead[FP_LEN_IDX+12]<<16\
										|(uint32_t)pFrameHead[FP_LEN_IDX+13]<<8|pFrameHead[FP_LEN_IDX+14];
										LdFp.PktSize=(uint16_t)pFrameHead[FP_LEN_IDX+15]<<8|pFrameHead[FP_LEN_IDX+16];
										LdFp.BaudRate=(uint16_t)pFrameHead[FP_LEN_IDX+17]<<8|pFrameHead[FP_LEN_IDX+18];
								}
								break;
						case PS_ReadIndexTable:      //��Чģ���� 
								LdFpAck=FP_ACK_FLAG+pFrameHead[FP_LEN_IDX+2];
								if(pFrameHead[FP_LEN_IDX+2]==PS_OK)
								{
										for(i=0;i<13;i++)
										{
												LdFp.IndexTab[i]=pFrameHead[FP_LEN_IDX+3+i];
											//	printf("fpinfo:0x%02x\n",LdFp.IndexTab[i]);
										}
								}
								break;
						case PS_DeletChar:             //ɾ��ָ��
						case PS_Empty:                 //���ָ�ƿ� 
								LdFpAck=FP_ACK_FLAG+pFrameHead[FP_LEN_IDX+2];
								break;
						case PS_Search:                //����ָ�� 
								idyId=(uint16_t)pFrameHead[FP_LEN_IDX+3]<<8|pFrameHead[FP_LEN_IDX+4];
								printf("fp idyid: 0x%04x\n",idyId);
						case PS_GetImage:              //��ȡͼ��
						case PS_GenChar:               //����ͼ������
						case PS_RegModel:              //�ϲ�����������ģ��
						case PS_StoreChar:             //����ģ��
						case PS_Sleep:                 //����ָ��
								LdFpAck=FP_ACK_FLAG+pFrameHead[FP_LEN_IDX+2];
								break;
						default:break; 
				}
		}
}
/******************************************************************************/
/*
//���ݷ��ͽ��ճ�ʱ����
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UartFp_RtxTimeoutoutHandle(void)
{
		if(UartStruct[UART_FP_INDEX].RxTime>0 && GetTimerElapse(UartStruct[UART_FP_INDEX].RxTime)>UartStruct[UART_FP_INDEX].RtxTimeout)
		{
				UartStruct[UART_FP_INDEX].RxStat=RX_IDLE;
				UartStruct[UART_FP_INDEX].RxTime=0;
				UartStruct[UART_FP_INDEX].RxFrameOk=1;
		}
		if(UartStruct[UART_FP_INDEX].TxTime>0 && GetTimerElapse(UartStruct[UART_FP_INDEX].TxTime)>UartStruct[UART_FP_INDEX].RtxTimeout) 
		{
				UartStruct[UART_FP_INDEX].TxStat=TX_IDLE;
				UartStruct[UART_FP_INDEX].TxTime=0;
				UART_FP_SCI->SCICR2&=~SCICR2_TIE_MASK;  
		}
}
/******************************************************************************/
/*
//ָ��ɨ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void LdFpStatHandle(void)
{
		uint8_t temp[6]={0};
		
		static uint8_t idyok=0;
		static uint8_t fpfailtimes=0;
		static LdFpPdStat_t FpPd={PD_START}; 
		static uint8_t FpError=0;
		
		if(SysConfig.Bits.FpLock==1)return;                                //ָ������
		
		switch(FpScan.LdFpStat)
		{
				case FP_POWER_DOWN:                  //ָ������ 
						switch(FpPd)
						{
								case PD_START:
										if(FpScan.StTime==0)
										{
												temp[0]=PS_Sleep;
												UartFp_SendCmdData(FP_CMD,temp,1,1,500);
												FpScan.StTime=GetTimerCount();
										}
										else if(LdFpAck&FP_ACK_FLAG)
										{
												temp[0]=LdFpAck&0x7F;
												if(temp[0]==PS_OK)
												{
														FpPd=PD_WAIT_PD;
														FpScan.StTime=0;
												}
										}
										else if(GetTimerElapse(FpScan.StTime)>=2000)
										{
												FpScan.StTime=0;
												printf("FP POWER DOWN TIMEOUT RETRY\n");
												break;
										}
										break;
								case PD_WAIT_PD:
										if(EPORT_ReadGpioData(FP_TOUCHOUT_PIN)==Bit_RESET)
										{
												DelayMS(10);
												if(EPORT_ReadGpioData(FP_TOUCHOUT_PIN)==Bit_RESET)
												{
														printf("NO FIG POWER DOWN \n");
														FpPd=PD_WAIT_WK;
														UartFp_Deinit();                               //ָ��IO����
														FpPowerDown=1;
														LPM_SetStopMode(LPM_FP_ID,LPM_Enable);         //���־ 
												}
										}
										else if(FpScan.StTime==0)
										{
												FpScan.StTime=GetTimerCount();
										}
										else if(GetTimerElapse(FpScan.StTime)>=200)
										{
												FpPd=PD_START;
												FpScan.StTime=0;
										}
										break;
								case PD_WAIT_WK:
										if(EPORT_ReadGpioData(FP_TOUCHOUT_PIN)==Bit_SET)
										{
												DelayMS(10);
												if(EPORT_ReadGpioData(FP_TOUCHOUT_PIN)==Bit_SET)
												{
														nofingertimes=0;                               //����ָ���£���0����
														UartFp_DatStrInit();                           //����ʹ��ָ��ģ��
														UartFp_Init();
														FpScan.LdFpStat=FP_IDLE;                       //����״̬��
														FpPd=PD_START;
														FpPowerDown=0;                                 //�����߱�־
												}
										}
										break;
						}
						break;
				case FP_IDLE:
						if(FpError==1)break;
						nofingertimes=0; 
						LPM_SetStopMode(LPM_FP_ID,LPM_Disable);
						if(FpScan.StTime==0)             //�ϵ���ʱ
						{
								FpScan.StTime=GetTimerCount();
						}
						else if(GetTimerElapse(FpScan.StTime)>=FP_POWERON_DELAY)  //��ָ�ƴ�������Ϣ
						{
								FpScan.StTime=0;
								FpScan.LdFpStat=FP_CHECK_SENSOR;
								FpScan.ComRetry=0;
						}
						break;
				case FP_CHECK_SENSOR:
						if(FpScan.StTime==0)
						{
								temp[0]=PS_ReadSysPara;
								UartFp_SendCmdData(FP_CMD,temp,1,1,300); 
								LdFpAck=0;
								FpScan.StTime=GetTimerCount();
						}
						else if(LdFpAck==FP_ACK_FLAG && LdFp.DevAddr==0xFFFFFFFF)    //�ɹ�����ָ����Ϣ���������߼����ָ�Ƿ���
						{
								printf("fp addr:0x%08x \n",LdFp.DevAddr);
								FpScan.StTime=0;
								FpScan.LdFpStat=FP_READ_IDXTABLE;
								FpScan.ComRetry=0;
						}
						else if(GetTimerElapse(FpScan.StTime)>=1500)
						{
								FpScan.StTime=0;
								if(++FpScan.ComRetry>=FP_STEP_RTY_MAX)
								{
										printf("read syspara timeout \n");
										FpScan.ComRetry=0;
										FpError=1;
										LPM_SetStopMode(LPM_FP_ID,LPM_Enable);
										FpScan.LdFpStat=FP_IDLE;
								}
						}
						break;
				case FP_READ_IDXTABLE:
						if(FpScan.StTime==0)
						{
								printf("read index \n");
								temp[0]=PS_ReadIndexTable;
								UartFp_SendCmdData(FP_CMD,temp,1,1,300); 
								LdFpAck=0;
								FpScan.StTime=GetTimerCount();
						}
						else if(LdFpAck==FP_ACK_FLAG)     
						{
								LdFp.ActFpSum=Fp_GetValidFpSum();
								printf("ActFpSum: %02d \n",LdFp.ActFpSum);
								FpScan.StTime=0;
								FpScan.LdFpStat=FP_GET_IMG;
								FpScan.ComRetry=0;
						}
						else if(GetTimerElapse(FpScan.StTime)>=1500)
						{
								FpScan.StTime=0;
								if(++FpScan.ComRetry>=FP_STEP_RTY_MAX)
								{
										printf("read index timeout \n");
										FpScan.ComRetry=0;
										FpScan.LdFpStat=FP_IDLE;
								}
						}
						break;
				case FP_GET_IMG:
						if(FpScan.StTime==0)                           //���ͻ�ȡͼ��ָ��
						{
						//		printf("get img \n");
								temp[0]=PS_GetImage;
								UartFp_SendCmdData(FP_CMD,temp,1,1,500);
								LdFpAck=0;
								FpScan.StTime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)                  //�ɹ��յ�Ӧ��
						{
								temp[0]=LdFpAck&0x7F;
								LdFpAck=0;
								FpScan.StTime=0;
								if(temp[0]==PS_OK)
								{
										nofingertimes=0;                   //��⵽��ָ����0����ָ����
										if(SysConfig.Bits.Activate==0)
										{
												if(idyok==0)
												{
														idyok=1;
														AudioPlayVoice(GetVolIndex("ָ����"),BREAK);						//��������
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please activate the product":"�뼤���Ʒ"),UNBREAK);
														
														PirLed_SetStat(LOGO_OPEN_SUCCESS);
														UserControlLock(CTL_OPEN_LOCK,FP_OPEN,0XFF);            //��ʾģʽ����
												}
										}
										else
										{
												if(idyok==0)            //ʶ����ָ������δ��֤�ɹ�    
												{
														FpScan.LdFpStat=FP_GEN_CHAR;          //��ȡ�ɹ�����������
												}
										}
								}
								else if(temp[0]==PS_NO_FINGER )//ʶ��ɹ�����ָ�뿪
								{
										idyok=0;
										if(++nofingertimes>10)           //ɨ��10������ָ����������
										{
												FpScan.LdFpStat=FP_POWER_DOWN;
												FpPd=PD_START;
												FpScan.StTime=0;
										}
								}
						}
						else if(GetTimerElapse(FpScan.StTime)>1500)   //Ӧ��ʱ�����»�ȡͼ��
						{
								FpScan.StTime=0;
						}
						break;
				case FP_GEN_CHAR:
						if(FpScan.StTime==0)                          //������������ָ��
						{
								printf("gen char \n");
								temp[0]=PS_GenChar;
								temp[1]=1;         
								UartFp_SendCmdData(FP_CMD,temp,2,1,500);
								LdFpAck=0;
								FpScan.StTime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)
						{
								temp[0]=LdFpAck&0x7F;
								LdFpAck=0;
								FpScan.StTime=0;
								if(temp[0]==PS_OK)         //��ȡͼ��ɹ�������ָ�ƿ�
								{
										FpScan.LdFpStat=FP_SEARCH_LIB;
								}
						}
						else if(GetTimerElapse(FpScan.StTime)>1500)
						{
								FpScan.StTime=0;
								FpScan.LdFpStat=FP_GET_IMG;
						}
						break;
				case FP_SEARCH_LIB:
						if(FpScan.StTime==0)
						{
								printf("search lib \n");
								temp[0]=PS_Search;
								temp[1]=1;
								temp[2]=0;
								temp[3]=0;
								temp[4]=0;
								temp[5]=100;
								UartFp_SendCmdData(FP_CMD,temp,6,1,2000);
								LdFpAck=0;
								FpScan.StTime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)
						{
								temp[0]=LdFpAck&0x7F;
								FpScan.StTime=0;
								LdFpAck=0;
								if(temp[0]==PS_OK)        //��֤�ɹ�
								{
										LockUpdatePeripLockStat(PERIP_FP_INDEX,0);
								
										fpfailtimes=0;
										if(KeyPadMode==KEYPAD_CONFIG)      //00����ָ��
										{
												if(idyId==0x0)
												{
														idyok=1;
														MenuNow=Menu_KeyPadConfigSelect;
												}
												else 
												{
														idyok=2;              //��֤ʧ��
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Verification failed":"��֤ʧ��"),BREAK);
												}
										}
										else if(SysConfig.Bits.MutiIdy==1)
										{
												idyok=1;
												if(MutiIdyItem&(~IDY_FP))        //��ָ����֤�ɹ�+ָ����֤�ɹ�
												{
														MutiIdyItem=0;
														AudioPlayVoice(GetVolIndex("ָ����"),BREAK);						//��������
														PirLed_SetStat(LOGO_OPEN_SUCCESS);
														printf("idy ok: 0x%04x\n",idyId);
														UserControlLock(CTL_OPEN_LOCK,FP_OPEN,idyId); 
												}
												else
												{
														MutiIdyItem|=IDY_FP;
														PirLed_SetStat(LOGO_OPEN_SUCCESS_MUTIL);
												}
										}
										else
										{
												idyok=1;
												AudioPlayVoice(GetVolIndex("ָ����"),BREAK);						//��������
												PirLed_SetStat(LOGO_OPEN_SUCCESS);
												printf("idy ok: 0x%04x\n",idyId);
												UserControlLock(CTL_OPEN_LOCK,FP_OPEN,idyId);
										}
								}
								else                      //��֤ʧ��
								{
										idyok=2;              //��֤ʧ��
										PirLed_SetStat(LOGO_OPEN_FAIL);
										AudioPlayVoice(GetVolIndex("�쿨��"),BREAK);
										AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
										if(++fpfailtimes>=3)
										{
												fpfailtimes=0;
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Verification failed":"��֤ʧ��"),UNBREAK);
												LockUpdatePeripLockStat(PERIP_FP_INDEX,1);
										}
								}
								FpScan.LdFpStat=FP_GET_IMG;
						}
						else if(GetTimerElapse(FpScan.StTime)>6500)
						{
								FpScan.StTime=0;
								FpScan.LdFpStat=FP_GET_IMG;
						}
						break;
				default:break;
		}
}
/******************************************************************************/
/*
//ָ��ģ�����ָ��
input:   type -- �������ͣ�ADD_ADMIN -- ��������    ADD_NORMAL -- ��ͨ����
output   none
return   
				 WAIT_RESP=0,                    //�ȴ�Ӧ��
				 OP_READY,                       //ģ��׼����
				 OP_SUCCESS,                     //��ӳɹ�
				 OP_FULL,                        //������
				 OP_FAIL,                        //���ʧ��
				 OP_TIMEOUT                      //������ʱ  
*/
/******************************************************************************/
uint8_t FpAddHandle(uint8_t type) 
{
		uint8_t temp[10]={0};
		uint8_t freeslot=0;
		uint8_t retData=0xFF;
		
		
		if(FpPowerDown==1)
		{
				FpPowerDown=0;
				nofingertimes=0;
				UartFp_DatStrInit();
				UartFp_Init();
		}
		
		switch(FpAdd.LdFpAddStat)
		{
				case FP_ADD_READ_USER:
						if(FpAdd.StTime==0)
						{
						//		printf("FP read index \n");
								temp[0]=PS_ReadIndexTable;
								UartFp_SendCmdData(FP_CMD,temp,1,1,500); 
								LdFpAck=0;
								FpAdd.StTime=GetTimerCount();
						}
						else if(LdFpAck==FP_ACK_FLAG)     
						{
								LdFp.ActFpSum=Fp_GetValidFpSum();
							//	printf("ActFpSum: %02d \n",LdFp.ActFpSum);
								FpAdd.StTime=0;
								FpAdd.LdFpAddStat=FP_ADD_GET_IMG;
								FpAdd.VolStime=0;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add fingerprint":"���ָ��"),UNBREAK); 
								freeslot=Fp_GetFpFristFreeSlot();
								if(ADD_ADMIN==type)freeslot=0;
								if(freeslot<100)
								{
										AudioPlayVoice(freeslot/10+GetVolIndex(SysConfig.Bits.Language?"zero":"0"),UNBREAK); 
										AudioPlayVoice(freeslot%10+GetVolIndex(SysConfig.Bits.Language?"zero":"0"),UNBREAK);
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);     //����
								}
								else 
								{
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Fingerprint library is full":"ָ�ƿ�����"),UNBREAK);
										retData=FP_FAIL;
								}
						}
						else if(GetTimerElapse(FpAdd.StTime)>=1500)
						{
								FpAdd.StTime=0;
								if(++FpAdd.ComRetry>=10)
								{
										printf("fp timeout\n"); 
										FpAdd.ComRetry=0;
										retData=FP_FAIL;
								}
						}
						break;
				case FP_ADD_GET_IMG:
						if(FpAdd.StTime==0)
						{
								temp[0]=PS_GetImage;
								UartFp_SendCmdData(FP_CMD,temp,1,1,1000);
								LdFpAck=0;
								FpAdd.StTime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)   //�ȴ�Ӧ��
						{
								temp[0]=LdFpAck&0x7F;
								LdFpAck=0;
								FpAdd.StTime=0;
								if(FpAdd.AlreadGetIMG==0)    //δ�ɼ�ָ�ƣ��ȴ���ָ����
								{
										if(temp[0]==PS_NO_FINGER)
										{
												if(FpAdd.VolStime==0)
												{ 
														FpAdd.VolStime=GetTimerCount();
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press finger on the reader":"�밴��ָ"),UNBREAK);          //�밴��ָ 
												}
												else if(GetTimerElapse(FpAdd.VolStime)>5000)
												{
														FpAdd.VolStime=0;
												}
										}
										else if(temp[0]==PS_OK)    
										{
												FpAdd.LdFpAddStat=FP_ADD_GEN_CHAR;  //��ȡ�ɹ�����������
										}
								}
								else                       //�Ѳɼ�ָ�ƣ���ʾ�ɿ���ָ
								{
										if(temp[0]==PS_OK)    
										{
//												if(FpAdd.VolStime==0)
//												{
//														FpAdd.VolStime=GetTimerCount();
//												}
//												else if(GetTimerElapse(FpAdd.VolStime)>5000)
//												{
//														FpAdd.VolStime=0;
//														//AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press your finger on the reader again":"���ٰ���ָ"),BREAK);          //2S����ʾ�ɿ���ָ
//												}
										}
										else if(temp[0]==PS_NO_FINGER)
										{
												FpAdd.AlreadGetIMG=0;
												FpAdd.VolStime=0;
										}
								}
						}
						else if(GetTimerElapse(FpAdd.StTime)>3000)
						{
								FpAdd.StTime=0;
								retData=FP_FAIL;
						}
						break;
				case FP_ADD_GEN_CHAR:
						if(FpAdd.StTime==0)
						{
								printf("gen char \n");
								temp[0]=PS_GenChar;
								temp[1]=FpAdd.GenCharCnt+1;          //���12��1-12��
								UartFp_SendCmdData(FP_CMD,temp,2,1,500);
								LdFpAck=0;
								FpAdd.StTime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)
						{
								temp[0]=LdFpAck&0x7F;
								LdFpAck=0;
								FpAdd.StTime=0;
								if(temp[0]==PS_OK)         //��ȡͼ��ɹ�
								{
										FpAdd.AlreadGetIMG=1;
										AudioPlayVoice(GetVolIndex("ָ����"),BREAK);
										if(++FpAdd.GenCharCnt>=FP_REG_PRESS_TIMES)
										{
												FpAdd.LdFpAddStat=FP_ADD_REG_MODE;
										}
										else 
										{
												FpAdd.VolStime=0;
												FpAdd.LdFpAddStat=FP_ADD_GET_IMG;
										}
								}
								else
								{
										FpAdd.LdFpAddStat=FP_ADD_GET_IMG;   //ʧ�����»�ȡͼ��
								}
						}
						else if(GetTimerElapse(FpAdd.StTime)>2000)
						{
								FpAdd.StTime=0;
								retData=FP_FAIL;
						}
						break;
				case FP_ADD_REG_MODE:
						if(FpAdd.StTime==0)
						{
								printf("reg mod \n");
								temp[0]=PS_RegModel;
								UartFp_SendCmdData(FP_CMD,temp,1,1,500);
								FpAdd.StTime=GetTimerCount();
								LdFpAck=0;
						}
						else if(LdFpAck&FP_ACK_FLAG)
						{
								temp[0]=LdFpAck&0x7F;
								LdFpAck=0;
								FpAdd.StTime=0;
								if(temp[0]==PS_OK)        //����ģ��ɹ�
								{
										FpAdd.LdFpAddStat=FP_ADD_STORE_MODE;
								}
						}
						else if(GetTimerElapse(FpAdd.StTime)>2000)
						{
								FpAdd.StTime=0;
								retData=FP_FAIL;
						}
						break;
				case FP_ADD_STORE_MODE:
						if(FpAdd.StTime==0)
						{
								printf("sotre mod \n");
								temp[0]=PS_StoreChar;
								temp[1]=1;
								temp[2]=0;
								temp[3]=Fp_GetFpFristFreeSlot();
								if(ADD_ADMIN==type)temp[3]=0;
								UartFp_SendCmdData(FP_CMD,temp,4,1,500);
								LdFpAck=0;
								FpAdd.StTime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)
						{
								temp[0]=LdFpAck&0x7F;
								FpAdd.StTime=0;
								if(temp[0]==PS_OK)        //�洢ģ��ɹ�
								{
										retData=FP_SUCCESS;	
										
										LockAddUser(PERIP_FP_INDEX,(ADD_ADMIN==type)?0:Fp_GetFpFristFreeSlot());
										
										FpUpdateUser(1,ADD_ADMIN==type?0:Fp_GetFpFristFreeSlot(),type==ADD_COERCE?1:0);
										printf("sotre mod ok\n");
								}
								else                      //�洢ʧ��  
								{
										retData=FP_FAIL;	
										printf("sotre mod fail: 0x%02x \n",temp[0]);
								}
						}
						else if(GetTimerElapse(FpAdd.StTime)>2000)
						{
								FpAdd.StTime=0;
								retData=FP_FAIL;
						}
						break;
		}
		return retData;
}
/******************************************************************************/
/*
//ָ��ģ��ɾ��ָ��
input:   id -- ָ��ID
output   none
return   
				 WAIT_RESP=0,                    //�ȴ�Ӧ��
				 OP_READY,                       //ģ��׼����
				 OP_SUCCESS,                     //�����ɹ�
				 OP_FULL,                        //������
				 OP_FAIL,                        //����ʧ��
				 OP_TIMEOUT                      //������ʱ  
*/
/******************************************************************************/
uint8_t FpDelHandle(uint8_t id)
{
		uint8_t temp[10]={0};
		uint8_t retData=0xFF;
		
		
		if(FpPowerDown==1)
		{
				FpPowerDown=0;
				nofingertimes=0;
				UartFp_DatStrInit();
				UartFp_Init();
		}
		
		switch(FpDel.LdFpDelStat)
		{
				case FP_DEL_READ_USER:
						if(FpDel.StTime==0)
						{
								printf("read indextable \n");
								temp[0]=PS_ReadIndexTable;
								UartFp_SendCmdData(FP_CMD,temp,1,1,500); 
								LdFpAck=0;
								FpDel.StTime=GetTimerCount();
						}
						else if(LdFpAck==FP_ACK_FLAG)     
						{
								LdFp.ActFpSum=Fp_GetValidFpSum();
								printf("ActFpSum: %02d \n",LdFp.ActFpSum);
								FpDel.StTime=0;
								FpDel.LdFpDelStat=FP_DEL_RUN;
						}
						else if(GetTimerElapse(FpDel.StTime)>1500)
						{
								FpDel.StTime=0;
								if(++FpDel.ComRetry>=3)
								{
										printf("fp timeout\n"); 
										FpDel.ComRetry=0;
										retData=FP_FAIL;
								}
						}
						break;
				case FP_DEL_RUN:
						if(FpDel.StTime==0)
						{
								printf("del fp start \n");
								if(id==0xFF)
								{
										temp[0]=PS_Empty;
										UartFp_SendCmdData(FP_CMD,temp,1,1,1000); 
								}
								else
								{
										temp[0]=PS_DeletChar;
										temp[1]=0;
										temp[2]=id;
										temp[3]=0;
										temp[4]=1;
										UartFp_SendCmdData(FP_CMD,temp,5,1,1000); 
								}
								LockDelUser(PERIP_FP_INDEX,id);
								LdFpAck=0;
								FpDel.StTime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)
						{
								FpDel.StTime=0;
								temp[0]=LdFpAck&0x7F;
								printf("del fp ack: 0x%02x \n",temp[0]);
								if(temp[0]==PS_OK)
								{
										retData=FP_SUCCESS;
										FpUpdateUser(0,id,0);
								}
								else
								{
										retData=FP_FAIL;
								}
						}
						else if(GetTimerElapse(FpDel.StTime)>=1500)
						{
								FpDel.StTime=0;
								retData=FP_FAIL;
						}
						break;
		}
		return retData;
}
/******************************************************************************/
/*
//ָ��ģ������
input:   none
output   none
return   none
*/
/******************************************************************************/
uint8_t Fp_PowerDown(void)
{
		FpPd_t FpPd={0};
		uint8_t temp=0;
		uint8_t fails=0;
		
		if(FpPowerDown==1)return FpPowerDown;
		
		while(1)
		{
				WDT_FeedDog();                         //ι��
				Uart_RtxFrameHandle();
				if(FpPd.Stime==0)
				{
						temp=PS_Sleep;
						UartFp_SendCmdData(FP_CMD,&temp,1,1,500);
						FpPd.Stime=GetTimerCount();
				}
				else if(LdFpAck&FP_ACK_FLAG)
				{
						temp=LdFpAck&0x7F;
						if(temp==PS_OK)
						{
								if(EPORT_ReadGpioData(FP_TOUCHOUT_PIN)==Bit_RESET)
								{
										DelayMS(5);
										if(EPORT_ReadGpioData(FP_TOUCHOUT_PIN)==Bit_RESET)
										{
												FpPowerDown=1;
												printf("FP POWER DOWN OK \n");
												break;
										}
								}
								else if(GetTimerElapse(FpPd.Stime)>=1500)        
								{
										FpPd.Stime=0;
										if(++fails>=3)
										{
												printf("FP POWER DOWN TIMEOUT\n");
										}
								}
						}
				}
				else if(GetTimerElapse(FpPd.Stime)>=2000)
				{
						printf("FP POWER DOWN TIMEOUT\n");
						break;
				}
		}
		return FpPowerDown;
}

/******************************************************************************/
/*
//��ȡ��Ч��ָ������
input:   none
output   none
return   vSum -- ����ָ������ 
*/
/******************************************************************************/
uint8_t Fp_GetValidFpSum(void)
{
		uint8_t i=0,j=0;
		uint8_t vSum=0;
		for(i=0;i<13;i++)
		{
				for(j=0;j<8;j++)
				{
						if(LdFp.IndexTab[i]&(1<<j))
						{
								vSum++;
						}
				}
		}
		return vSum;
}
/******************************************************************************/
/*
//��ȡ���е�ָ��SLOT
input:   none
output   none
return   ��һ�����õ�SLOT
*/
/******************************************************************************/
uint8_t Fp_GetFpFristFreeSlot(void)
{
		uint8_t i=0,j=0;
	
		for(i=0;i<13;i++)
		{
				for(j=0;j<8;j++)
				{
						if((LdFp.IndexTab[i]&(1<<j))==0)
						{
								return (i*8+j);
						}
				}
		}
		return (i*8);     //13*8=104
}
/******************************************************************************/
/*
//����ָ���û�
input:   type -- �������ͣ�1-��� 0-ɾ��
         id -- ���µ�ID  ɾ��ʱFFΪɾ������
output   none
return   none
*/
/******************************************************************************/
void FpUpdateUser(uint8_t type,uint8_t id,uint8_t isCoerce)
{
		if(id>=FP_MAX_NUM && id!=0xFF)return;       //0-99
		if(id==0xFF && type==0) //ɾ������
		{
				LdFp.ActFpSum=0;
				memset(&LdFp.IndexTab[0],0,13);
				memset(&CoerceFp.CFP.CoerceFp[0],0,13);
				IntFlashWrite(CoerceFp.CoerceFpBuf,COERCE_FP_ADDR,sizeof(CoerceFp_t));
				return;
		}
		if(type)             //�������
		{
				LdFp.IndexTab[id/8]|=(1<<(id%8));
				LdFp.ActFpSum+=1;
				if(isCoerce==1)
				{
						CoerceFp.CFP.CoerceFp[id/8]|=(1<<(id%8));
						IntFlashWrite(CoerceFp.CoerceFpBuf,COERCE_FP_ADDR,sizeof(CoerceFp_t));
				}
		}
		else                 //ɾ������
		{
				if(LdFp.ActFpSum<1)return;
				LdFp.IndexTab[id/8]&=~(1<<(id%8));
				CoerceFp.CFP.CoerceFp[id/8]&=~(1<<(id%8));
				IntFlashWrite(CoerceFp.CoerceFpBuf,COERCE_FP_ADDR,sizeof(CoerceFp_t));
				LdFp.ActFpSum-=1;
		}
}


/**************************************�������ϻ���ش���****************************************/
FpTest_t FpTest={FP_TEST_ADD_READ_USER,FP_TEST_SCAN_GET_IMG,0};

uint8_t FpTest_Add(void)
{
		uint8_t temp[10]={0};
		uint8_t retData=0xFF;
		
		switch(FpTest.AddStat)
		{
				case FP_TEST_ADD_READ_USER:
						if(FpTest.Stime==0)
						{
								printf("FP read index \n");
								temp[0]=PS_ReadIndexTable;
								UartFp_SendCmdData(FP_CMD,temp,1,1,500); 
								LdFpAck=0;
								FpTest.Stime=GetTimerCount();
						}
						else if(LdFpAck==FP_ACK_FLAG)     
						{
								LdFp.ActFpSum=Fp_GetValidFpSum();
								printf("ActFpSum: %02d \n",LdFp.ActFpSum);
								FpTest.Stime=0;
								FpTest.AddStat=FP_TEST_ADD_GET_IMG;
						}
						else if(GetTimerElapse(FpTest.Stime)>=1500)
						{
								FpTest.Stime=0;
								if(++FpTest.ComRetry>=10)
								{
										printf("fp timeout\n"); 
										FpTest.ComRetry=0;
										retData=FP_FAIL;
								}
						}
						break;
				case FP_TEST_ADD_GET_IMG:
						if(FpTest.Stime==0)
						{
								temp[0]=PS_GetImage;
								UartFp_SendCmdData(FP_CMD,temp,1,1,1000);
								LdFpAck=0;
								FpTest.Stime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)   //�ȴ�Ӧ��
						{
								temp[0]=LdFpAck&0x7F;
								LdFpAck=0;
								FpTest.Stime=0;
								if(FpTest.AlreadGetIMG==0)    //δ�ɼ�ָ�ƣ��ȴ���ָ����
								{
										if(temp[0]==PS_NO_FINGER)
										{
												if(FpTest.VolStime==0)
												{ 
														FpTest.VolStime=GetTimerCount();
														AudioPlayVoice(GetVolIndex("�밴��ָ"),BREAK);          //�밴��ָ 
												}
												else if(GetTimerElapse(FpTest.VolStime)>5000)
												{
														FpTest.VolStime=0;
												}
										}
										else if(temp[0]==PS_OK)    
										{
												FpTest.AddStat=FP_TEST_ADD_GEN_CHAR;  //��ȡ�ɹ�����������
										}
								}
								else                       //�Ѳɼ�ָ�ƣ���ʾ�ɿ���ָ
								{
										if(temp[0]==PS_OK)    
										{
										}
										else if(temp[0]==PS_NO_FINGER)
										{
												FpTest.AlreadGetIMG=0;
												FpTest.VolStime=0;
										}
								}
						}
						break;
				case FP_TEST_ADD_GEN_CHAR:
						if(FpTest.Stime==0)
						{
								printf("gen char \n");
								temp[0]=PS_GenChar;
								temp[1]=FpTest.GenCharCnt+1;          //���12��1-12��
								UartFp_SendCmdData(FP_CMD,temp,2,1,500);
								LdFpAck=0;
								FpTest.Stime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)
						{
								temp[0]=LdFpAck&0x7F;
								LdFpAck=0;
								FpTest.Stime=0;
								if(temp[0]==PS_OK)         //��ȡͼ��ɹ�
								{
										FpTest.AlreadGetIMG=1;
										AudioPlayVoice(GetVolIndex("ָ����"),BREAK);
										if(++FpTest.GenCharCnt>=3)
										{
												FpTest.AddStat=FP_TEST_ADD_REG_MODE;
										}
										else 
										{
												FpTest.VolStime=0;
												FpTest.AddStat=FP_TEST_ADD_GET_IMG;
										}
								}
								else
								{
										FpTest.AddStat=FP_TEST_ADD_GET_IMG;   //ʧ�����»�ȡͼ��
								}
						}
						break;
				case FP_TEST_ADD_REG_MODE:
						if(FpTest.Stime==0)
						{
								printf("reg mod \n");
								temp[0]=PS_RegModel;
								UartFp_SendCmdData(FP_CMD,temp,1,1,500);
								FpTest.Stime=GetTimerCount();
								LdFpAck=0;
						}
						else if(LdFpAck&FP_ACK_FLAG)
						{
								temp[0]=LdFpAck&0x7F;
								LdFpAck=0;
								FpTest.Stime=0;
								if(temp[0]==PS_OK)        //����ģ��ɹ�
								{
										FpTest.AddStat=FP_TEST_ADD_STORE_MODE;
								}
						}
						else if(GetTimerElapse(FpTest.Stime)>2000)
						{
								FpTest.Stime=0;
								retData=FP_FAIL;
						}
						break;
				case FP_TEST_ADD_STORE_MODE:
						if(FpTest.Stime==0)
						{
								printf("sotre mod \n");
								temp[0]=PS_StoreChar;
								temp[1]=1;
								temp[2]=0;
								temp[3]=Fp_GetFpFristFreeSlot();
								UartFp_SendCmdData(FP_CMD,temp,4,1,500);
								LdFpAck=0;
								FpTest.Stime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)
						{
								temp[0]=LdFpAck&0x7F;
								FpTest.Stime=0;
								FpTest.AddStat=FP_TEST_ADD_DONE;
								if(temp[0]==PS_OK)        //�洢ģ��ɹ�
								{
										retData=FP_SUCCESS;	
										printf("sotre mod ok\n");
								}
								else                      //�洢ʧ��  
								{
										retData=FP_FAIL;	
										printf("sotre mod fail: 0x%02x \n",temp[0]);
								}
						}
						break;
				case FP_TEST_ADD_DONE:
						break;
				default:break;
		}
		return retData;
}

uint8_t FpTest_Scan(void)
{
		uint8_t temp[10]={0};
		uint8_t ret=0xFF;
		
		switch(FpTest.ScanStat)
		{
				case FP_TEST_SCAN_GET_IMG:
						if(FpTest.Stime==0)                           //���ͻ�ȡͼ��ָ��
						{
								temp[0]=PS_GetImage;
								UartFp_SendCmdData(FP_CMD,temp,1,1,500);
								LdFpAck=0;
								FpTest.Stime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)                  //�ɹ��յ�Ӧ��
						{
								temp[0]=LdFpAck&0x7F;
								LdFpAck=0;
								FpTest.Stime=0;
								if(temp[0]==PS_OK)
								{  
										FpTest.ScanStat=FP_TEST_SCAN_GEN_CHAR;          //��ȡ�ɹ�����������
								}
						}
						break;
				case FP_TEST_SCAN_GEN_CHAR:
						if(FpTest.Stime==0)                          //������������ָ��
						{
								temp[0]=PS_GenChar;
								temp[1]=1;         
								UartFp_SendCmdData(FP_CMD,temp,2,1,500);
								LdFpAck=0;
								FpTest.Stime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)
						{
								temp[0]=LdFpAck&0x7F;
								LdFpAck=0;
								FpTest.Stime=0;
								if(temp[0]==PS_OK)         //��ȡͼ��ɹ�������ָ�ƿ�
								{
										FpTest.ScanStat=FP_TEST_SCAN_SEARCH_LIB;
								}
						}
						break;
				case FP_TEST_SCAN_SEARCH_LIB:
						if(FpTest.Stime==0)
						{
								temp[0]=PS_Search;
								temp[1]=1;
								temp[2]=0;
								temp[3]=0;
								temp[4]=0;
								temp[5]=100;
								UartFp_SendCmdData(FP_CMD,temp,6,1,2000);
								LdFpAck=0;
								FpTest.Stime=GetTimerCount();
						}
						else if(LdFpAck&FP_ACK_FLAG)
						{
								temp[0]=LdFpAck&0x7F;
								FpTest.Stime=0;
								LdFpAck=0;
								FpTest.ScanStat=FP_TEST_SCAN_DONE;
								if(temp[0]==PS_OK)        //��֤�ɹ�
								{
										ret=FP_SUCCESS;
								}
								else                      //��֤ʧ��
								{
										ret=FP_FAIL;
								}
						}
						break;
				case FP_TEST_SCAN_DONE:
						break;
				default:break;
		}
		return ret;
}


