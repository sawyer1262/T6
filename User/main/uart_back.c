#include "uart_drv.h"
#include "uart_back.h"
#include "uart.h"
#include "timer.h"
#include "eport_drv.h"
#include "lpm.h"
#include "debug.h"
#include "rtc.h"
#include "audio.h"
#include "flash.h"
#include "startup_demo.h"
#include "bat.h"
#include "lock_record.h"
#include <string.h>
#include "key.h"
#include "password.h"
#include "keypad_menu.h"
#include "card.h"
#include "time_cal.h"
#include "uart_face.h"
#include "uart_fp.h"
#include "led.h"


UartTxStr_t GmtBackUartTxQ[UART_BACK_TX_Q_SIZE];       //���Ͷ���  
uint8_t BackCurIdx=0;
LockStat_t LockStat;

RemoteDel_t RemoteDel={0,0};
RemoteAdd_t RemoteAdd={0};
RemoteDelSelect_t RemoteDelSelect={0};
//OTA���
OtaInfo_t OtaInfo={0};
/******************************************************************************/
/*
//���ͨѶ���ڽṹ��ʼ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UartBack_DatStrInit(void)
{
		UartStruct[UART_BACK_INDEX].pTxStart=0;
		UartStruct[UART_BACK_INDEX].pTxEnd=0;
		UartStruct[UART_BACK_INDEX].rp=UartStruct[UART_BACK_INDEX].wp=0;
		UartStruct[UART_BACK_INDEX].RxStat=RX_IDLE;
		UartStruct[UART_BACK_INDEX].TxStat=TX_IDLE;
		UartStruct[UART_BACK_INDEX].RtxTimeout=0;
		UartStruct[UART_BACK_INDEX].TxTime=0;
	//	UartStruct[UART_BACK_INDEX].RxTime=0;              //�͹��Ļ���ʱ����Ҫ��0
	
		BackCurIdx=0xFF;
}

/******************************************************************************/
/*
//���ͨѶ���ڳ�ʼ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UartBack_Init(void)
{
    UART_InitTypeDef UART_InitStruct;
    UART_InitStruct.UART_BaudRate = 9600;
    UART_InitStruct.UART_Mode = UART_INT_MODE;//����CPU�жϷ�ʽ
		UART_InitStruct.UART_Parity = UART_PARITY_NONE;
		UART_InitStruct.UART_FrameLength = UART_DATA_FRAME_LEN_10BIT;
    UART_InitStruct.UART_StopBits = 1;
		UartStruct[UART_BACK_INDEX].RtxTimeout=50;
		UART_Init(UART_BACK_SCI, &UART_InitStruct);
}

/******************************************************************************/
/*
//���ͨѶUART����
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UartBack_Deinit(void)
{
		UART_ConfigGpio(UART_BACK_SCI,UART_RX,GPIO_INPUT);         //RX���룬Ĭ������
	
		UART_ConfigGpio(UART_BACK_SCI,UART_TX,GPIO_OUTPUT);        //TX����� 
		UART_WriteGpioData(UART_BACK_SCI,UART_TX,Bit_SET);
	
		EPORT_ConfigGpio(NT_BACK_PIN,GPIO_INPUT);                  //����NT_BACK���ڻ���
		EPORT_PullupConfig(NT_BACK_PIN,ENABLE);
		EPORT_Init(NT_BACK_PIN,FALLING_EDGE_INT);
}
/******************************************************************************/
/*
//����У��
input:   buf -- ��Ҫ����У������ݰ�
         len -- ���ݰ�����
output   none
return   16λУ��
*/
/******************************************************************************/
uint16_t GenGMTChksum(uint8_t* buf,uint16_t len)
{
		uint32_t dat=0x10000;
		uint16_t i=0;
		for(i=0;i<len;i++)
		{
				dat-=buf[i];
		}
		return (dat&0xffff);
}
/******************************************************************************/
/*
//��崮�ڷ�������
input:   buf -- ���ݻ���
         len -- ���ݳ���
output   none
return   none 
*/
/******************************************************************************/
void UartBack_SendNbytes(uint8_t* buf,uint16_t len)      
{
		if(UartStruct[UART_BACK_INDEX].TxStat!=TX_IDLE)return;
		
		UartStruct[UART_BACK_INDEX].TxStat=TX_RUNNING;
		UartStruct[UART_BACK_INDEX].pTxStart=buf;
		UartStruct[UART_BACK_INDEX].pTxEnd=buf+len;
		UartStruct[UART_BACK_INDEX].TxTime=GetTimerCount();
		UART_BACK_SCI->SCIDRL=*UartStruct[UART_BACK_INDEX].pTxStart++;
		UART_BACK_SCI->SCICR2|=SCICR2_TIE_MASK;  
}

/******************************************************************************/
/*
//��崮�ڷ���ָ��
input:   cmd -- ָ�����ͣ���Э���ĵ�
         buf -- ָ�������
         len -- ָ������ݳ���
ack -- ����ָ���Ƿ���ҪӦ��1-��Ҫ�� 0-����Ҫ�� 2-Ӧ�������Ҫ��������
         timeout -- �ȴ�Ӧ��ʱʱ�䣬��λΪ����
output   none
return   1 -- ���ָ��ɹ�   0 -- ���Ͷ������� 
*/
/******************************************************************************/
uint8_t UartBack_SendCmdData(uint8_t cmd,uint8_t *buf,uint8_t len,uint8_t ack,uint32_t timeout)
{
		uint8_t i=0,j=0;
		uint16_t chksum=0;
		
		for(j=0;j<UART_BACK_TX_Q_SIZE;j++)
		{
				if(GmtBackUartTxQ[j].DatStat==0)
				{
						GmtBackUartTxQ[j].DatStat=1;
						GmtBackUartTxQ[j].NeedAck=ack;
						GmtBackUartTxQ[j].Timeout=timeout;
						GmtBackUartTxQ[j].DatBufLen=UART_BACK_WAKE_SIZE+len+7;
						GmtBackUartTxQ[j].Retry=0;
						for(i=0;i<UART_BACK_WAKE_SIZE;i++)
						{
								GmtBackUartTxQ[j].DatBuf[i]=0;
						}
						GmtBackUartTxQ[j].DatBuf[UART_BACK_WAKE_SIZE]=GMT_LOCK_HEAD_8_L;
						GmtBackUartTxQ[j].DatBuf[UART_BACK_WAKE_SIZE+1]=GMT_LOCK_HEAD_8_H;  
						GmtBackUartTxQ[j].DatBuf[UART_BACK_WAKE_SIZE+2]=len+1;            
						GmtBackUartTxQ[j].DatBuf[UART_BACK_WAKE_SIZE+3]=0;
						GmtBackUartTxQ[j].DatBuf[UART_BACK_WAKE_SIZE+4]=cmd;
						for(i=0;i<len;i++)
						{
								GmtBackUartTxQ[j].DatBuf[UART_BACK_WAKE_SIZE+5+i]=buf[i];
						}
						chksum=GenGMTChksum(&GmtBackUartTxQ[j].DatBuf[UART_BACK_WAKE_SIZE],len+5);
						GmtBackUartTxQ[j].DatBuf[UART_BACK_WAKE_SIZE+5+i]=chksum;
						GmtBackUartTxQ[j].DatBuf[UART_BACK_WAKE_SIZE+6+i]=chksum>>8;
						
						LPM_SetStopMode(LPM_BACKLOCK_TX_ID,LPM_Disable);
						return 1;
				}
		}
		return 0;
}
/******************************************************************************/
/*
//��崮�����ݷ��ʹ���
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UartBack_DataSendHandle(void)
{
		uint8_t i=0;
		
		if(UartStruct[UART_BACK_INDEX].TxStat!=TX_IDLE)return;
		
		//����Ƿ�����Ҫ�������͵�Ӧ��
		for(i=0;i<UART_BACK_TX_Q_SIZE;i++)
		{
				if(GmtBackUartTxQ[i].DatStat==1 && GmtBackUartTxQ[i].NeedAck==2)        
				{
						UartBack_SendNbytes(GmtBackUartTxQ[i].DatBuf,GmtBackUartTxQ[i].DatBufLen);
						GmtBackUartTxQ[i].DatStat=0;
						BackCurIdx=0xFF;
						return;
				}
		}
		//����Ƿ�����ҪӦ������ݻ��ڵȴ�Ӧ��
		if(BackCurIdx<UART_BACK_TX_Q_SIZE)
		{
				if(GetTimerElapse(GmtBackUartTxQ[BackCurIdx].TxTime)<GmtBackUartTxQ[BackCurIdx].Timeout)return;
				
				UartBack_SendNbytes(GmtBackUartTxQ[BackCurIdx].DatBuf,GmtBackUartTxQ[BackCurIdx].DatBufLen);
				GmtBackUartTxQ[BackCurIdx].TxTime=GetTimerCount();
				if(++GmtBackUartTxQ[BackCurIdx].Retry>=3)
				{
						GmtBackUartTxQ[BackCurIdx].DatStat=0;
						BackCurIdx=0xFF;
				}
				return;
		}
		//�����Ƿ���������Ҫ����
		for(i=0;i<UART_BACK_TX_Q_SIZE;i++)
		{
				if(GmtBackUartTxQ[i].DatStat==1)
				{
						UartBack_SendNbytes(GmtBackUartTxQ[i].DatBuf,GmtBackUartTxQ[i].DatBufLen);
						if(GmtBackUartTxQ[i].NeedAck==1)
						{
								GmtBackUartTxQ[i].TxTime=GetTimerCount();
								GmtBackUartTxQ[i].Retry++;
								BackCurIdx=i;
						}
						else
						{
								GmtBackUartTxQ[i].DatStat=0;
								BackCurIdx=0xFF;
						}
						return;
				}
		}
		if(i==UART_BACK_TX_Q_SIZE)   //����û������Ҫ����
		{
				LPM_SetStopMode(LPM_BACKLOCK_TX_ID,LPM_Enable);
		}
}
/******************************************************************************/
/*
//���յ��ĺ�����ݴ���
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UartBack_RxDataHandle(void)
{
		uint8_t pBufRec[UART_RECV_MAX_LEN]={0};
		uint8_t ackDat[20]={0};
		uint8_t *pDatRec=0;
		uint16_t i=0,datlen=0,chksum=0,pktLen=0;
		
		if(UartStruct[UART_BACK_INDEX].RxFrameOk==0)return;
		
		UartStruct[UART_BACK_INDEX].RxFrameOk=0;
	
		LPM_SetStopMode(LPM_BACKLOCK_RX_ID,LPM_Enable);
			
		while(UartStruct[UART_BACK_INDEX].wp != UartStruct[UART_BACK_INDEX].rp)
		{
				pBufRec[i++]=UartStruct[UART_BACK_INDEX].dat[UartStruct[UART_BACK_INDEX].rp++];
				if(UartStruct[UART_BACK_INDEX].rp>=UART_RECV_MAX_LEN)
				{
						UartStruct[UART_BACK_INDEX].rp=0;
				}
		}
		datlen=i;
	//	UartBack_SendNbytes(pBufRec,i);

		for(i=0;i<datlen;i++)
		{
				if(pBufRec[i]==GMT_LOCK_HEAD_8_L)
				{
						if(i+1<datlen && pBufRec[i+1]==GMT_LOCK_HEAD_8_H)
						{
								break;
						}
				}
		}
		datlen-=i;
		if(datlen<GMT_LOCK_FRAME_MIN)return;//������ݣ�2�ֽ�ͷ+2�ֽڳ���+1�ֽ�����+2�ֽ�У��
		pDatRec=&pBufRec[i];
		pktLen=((uint16_t)pDatRec[3]<<8)+pDatRec[2];
		chksum=((uint16_t)pDatRec[pktLen+6]<<8)+pDatRec[pktLen+5];
		if(chksum!=GenGMTChksum(pDatRec,pktLen+5))return;   //У�鲻ͨ��
		
		//UartBack_SendNbytes(pDatRec,datlen);
		if(BackCurIdx<UART_BACK_TX_Q_SIZE && GmtBackUartTxQ[BackCurIdx].NeedAck==1
				&& GmtBackUartTxQ[BackCurIdx].DatBuf[UART_BACK_WAKE_SIZE+4]+1==pDatRec[4])
		{
				GmtBackUartTxQ[BackCurIdx].DatStat=0;
				BackCurIdx=0xFF;
		}
		switch(pDatRec[4])
		{
				case GET_VER:                           //����ǰ��汾��ID��ϢӦ�𣬹��ⲿ��ȡ
						UartBack_SendCmdData(pDatRec[4]+1,(uint8_t*)&ver[0],strlen(ver),2,1000);
						break;
				case REQ_LOCK_STAT_ACK:                           //����״̬Ӧ�𣺱�����״̬�����������Ƿ��ϵ����״̬
						//0-��������  1-�ѿ���   2-����ʧ��   3-��������  4-������  5-����ʧ��  6-������
						LockStat.Lock=pDatRec[5];
						switch(LockStat.Lock)
						{
								case 0:case 3:         //�ѹ���
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Door close":"���ѹ�"),UNBREAK);
										break;
								case 1:                //�ѿ��� 
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Door open":"���ѿ�"),UNBREAK);
										
										if(BatLow==1)
										{
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Low battery, please replace it":"��������,��������"),UNBREAK);
										}
										break;
								case 4:case 5:         //������  
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Door unclosed":"��δ�غ�"),UNBREAK);
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Door unclosed":"��δ�غ�"),UNBREAK);
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Door unclosed":"��δ�غ�"),UNBREAK);
										break;
								case 6:                //������:10S��һ��2.5S
										LockAlarm=1;
										TampStartTime=GetTimerCount();
										LPM_SetStopMode(LPM_TAMP_ID,LPM_Disable);  
										break;
								case 2:                //����ʧ��
										AudioPlayVoice(GetVolIndex("������"),UNBREAK);
										AudioPlayVoice(GetVolIndex("������"),UNBREAK);
										AudioPlayVoice(GetVolIndex("������"),UNBREAK);
										break;
								default:break;
						}
						break;
				case CTRL_LOCK_ACK:                              //����������Ӧ�����账��
						break;
				case LOCK_STAT_REPORT:                           //��״̬�ϱ� 
						LockStat.Lock=pDatRec[5];
						switch(LockStat.Lock)
						{
								case 0:case 3:         //�ѹ���
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Door close":"���ѹ�"),UNBREAK);
										break;
								case 1:                //�ѿ��� 
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Door open":"���ѿ�"),UNBREAK);
										
										if(BatLow==1)
										{
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Low battery, please replace it":"��������,��������"),UNBREAK);
										}
										break;
								case 4:case 5:                //������  
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Door unclosed":"��δ�غ�"),UNBREAK);
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Door unclosed":"��δ�غ�"),UNBREAK);
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Door unclosed":"��δ�غ�"),UNBREAK);
										break;
								case 6:                //������:10S��һ��2.5S
										LockAlarm=1;
										TampStartTime=GetTimerCount();
										LPM_SetStopMode(LPM_TAMP_ID,LPM_Disable);
										break;
								case 2:                //����ʧ��
										AudioPlayVoice(GetVolIndex("������"),UNBREAK);
										AudioPlayVoice(GetVolIndex("������"),UNBREAK);
										AudioPlayVoice(GetVolIndex("������"),UNBREAK);
										break;
								default:break;
						}
						UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],0,2,1000);    //Ӧ��
						break;
				case REQ_TIME_ACK:                           //����ʱ��Ӧ��
						if(pDatRec[5]!=0 || pDatRec[6]!=0 || pDatRec[7]!=0 || pDatRec[8]!=0)         //ʱ����Ч
						{
								uint32_t timeUTC=0;
								t_ctimes st = {0};
								tm rtc_times={0};
								
								timeUTC=(uint32_t)pDatRec[8]<<24;
								timeUTC|=(uint32_t)pDatRec[7]<<16;
								timeUTC|=(uint32_t)pDatRec[6]<<8;
								timeUTC|=pDatRec[5];
								
								timeUTC+=SysConfig.Bits.TimeZone*3600;   //��ʱ�������ȥ
								
								utc_to_mytime(timeUTC,&st,0);
								mytime_to_rtc(st,&rtc_times);
								RTC_SetTime(rtc_times);
								
								NetTime.NT.Stat=1;
								NetTime.NT.Day=rtc_times.day;
								NetTime.NT.Hour=rtc_times.hour;
								NetTime.NT.Minute=rtc_times.minute;
								NetTime.NT.Second=rtc_times.second;
								NetTime.NT.Flag=0xAA;
								IntFlashWrite(NetTime.NTBuf,NET_TIME_ADDR,sizeof(NetTime_t)); 
						}
						break;
				case USER_CHG_REPORT_ACK:                         //���ɾ���û��ϱ�Ӧ��
						break;
				case CONFIG_NET_ACK:                              //��������Ӧ��
						break;
				case CTRL_FAST_SLEEP_ACK:                         //��������Ӧ��
						break;
				case CTRL_POWERSAVE_ACK:                          //��ʡ��ģʽ����Ӧ��
						break;
				case NET_STAT_REPORT:                             //�ϱ�����״̬   :1-���ӳɹ�   0-����ʧ��
						AudioPlayVoice(GetVolIndex(pDatRec[5]?"���ӳɹ�":"WIFI����ʧ��"),UNBREAK);
						UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],0,2,1000);    //Ӧ��
						break;
				case CHILDLOCK_STAT_REPORT:                       //ͯ���������������еã�ǰ��ֻ���ڿ���ͯ���Ǹ�����ʾ�������������
						SysConfig.Bits.ChildLock=pDatRec[5]?1:0;
						if(SysConfig.Bits.ChildLock)
						{
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Child lock is on":"ͯ���ѿ���"),UNBREAK);
						}
						else
						{
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Child lock is off":"ͯ���ѽ��"),UNBREAK);
						}
						UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],0,2,1000);
						break;
				case PRIV_STAT_REPORT:                                                  //����忪����ر���˽ģʽ
						SysConfig.Bits.PrivMode=pDatRec[5]?1:0;
						if(SysConfig.Bits.PrivMode)
						{
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn on privacy mode":"������˽ģʽ"),UNBREAK);
						}
						else
						{
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn off privacy mode":"�ر���˽ģʽ"),UNBREAK);
						}
						UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],0,2,1000);
						break;
				case SET_SN_NUMBER:            //��SN��
						if(pktLen>SN_MAX_LEN)break;        //�32λ
						for(i=0;i<pktLen;i++)
						{
								SN[i]=pDatRec[5+i];            //����SN����
						}
						for(;i<SN_MAX_LEN;i++)
						{
								SN[i]=0;                       //�����ֽ���0 
						}
						SysConfig.Bits.Activate=0;         //дSN���弤���־
						IntFlashWrite((uint8_t*)SN,SN_ADDR,SN_MAX_LEN); 
						UpdateLockSn();
						UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],pktLen,2,1000);
						break;
				case SET_BOOT_MODE:            //����ROM����ģʽ
						if(pDatRec[5]==0x11 && pDatRec[6]==0x22)
						{
								Demo_StartupConfig(Startup_From_ROM, ENABLE);
						}
						UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],0,2,1000);
						break;
				case GET_SN_NUMBER:            //��SN
						UartBack_SendCmdData(pDatRec[4]+1,(uint8_t*)SN,strlen(SN),2,1000);
						break;
				case GET_OP_RECORD:            //���û�������¼
						LockReadUserOpera(ackDat,pDatRec[5]);
						UartBack_SendCmdData(pDatRec[4]+1,ackDat,20,2,1000);
						break;
				case CLEAR_ACT_FLAG:            //�弤���־
						if(pDatRec[5]==0xa5 && pDatRec[6]==0x5a)
						{
								SysConfig.Bits.Activate=0;
								UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],2,2,1000);
						}
						break;
				case READ_CFG:                 //��������  
				{
						tm RT={0};
						t_ctimes gt={0};
						RTC_GetTime(&RT);	                     //��ȡоƬRTCʱ��
						rtctime_to_mytime(&gt,RT);	           //��ȡʱ���
						ackDat[0]=SysConfig.B8[0];
						ackDat[1]=SysConfig.B8[1];
						ackDat[2]=SysConfig.B8[2];
						ackDat[3]=SysConfig.B8[3];
						ackDat[4]=BatPerCent;
						ackDat[5]=(uint8_t)gt.years;
						ackDat[6]=(uint8_t)(gt.years>>8);
						ackDat[7]=gt.months;
						ackDat[8]=gt.days;
						ackDat[9]=gt.hours;
						ackDat[10]=gt.minutes;
						ackDat[11]=gt.seconds;
						ackDat[12]=gt.weeks;
						ackDat[13]=Pwd.PwdInfo.Item.PwdSum;
						ackDat[14]=Pwd.PwdInfo.Item.PwdTimSum;
						ackDat[15]=CardInfo.Card.CardSum;
						ackDat[16]=FaceUser.Sum;
						ackDat[17]=LdFp.ActFpSum;
						ackDat[18]=LockStat.Lock;
						ackDat[19]=TamperAlarm.Tamp.Time;
						UartBack_SendCmdData(pDatRec[4]+1,ackDat,20,2,1000);
						break;
				}
				case SET_CFG:
						switch(pDatRec[5])
						{
								case 0:            //PIR����
										SysConfig.Bits.PirOn=pDatRec[6]?1:0;
										if(SysConfig.Bits.PirOn==1)
										{
												PowerOnPir();
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn On":"��"),BREAK);       
										}
										else 
										{
												PowerOffPir();
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn Off":"�ر�"),BREAK);       
										}
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Human body induction":"�����Ӧ"),UNBREAK);     
										break;
								case 1:            //��������
										if(pDatRec[6]<3)
										{
												SysConfig.Bits.Volume=pDatRec[6];
												if(pDatRec[6]==1)
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Low volume":"������"),BREAK); 
												}
												else if(pDatRec[6]==2)
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"High volume":"������"),BREAK); 
												}
												else
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Mute":"����"),BREAK);
												}
												Audio_SetVolume(SysConfig.Bits.Volume);	
										}
										break;
								case 2:            //APP��������ģʽ
										SysConfig.Bits.KeepOpen=pDatRec[6]?1:0;
										if(SysConfig.Bits.KeepOpen)
										{
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn On":"��"),UNBREAK);
										}
										else
										{
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn Off":"�ر�"),UNBREAK);
										}
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"keep open mode":"����ģʽ"),UNBREAK);
										break;
								case 3:     //��ʡ������
										SysConfig.Bits.SupPowSave=pDatRec[6]?1:0;
										if(SysConfig.Bits.SupPowSave)
										{
//												SysConfig.Bits.Volume=0;          //�˴���������������Ȼ������������Ų�����
//												Audio_SetVolume(SysConfig.Bits.Volume);
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn On":"��"),UNBREAK);
												if(SysConfig.Bits.PirOn==1)
												{
														SysConfig.Bits.PirOn=0;
														PowerOffPir();
												}
										}
										else
										{
												SysConfig.Bits.Volume=2;
												Audio_SetVolume(SysConfig.Bits.Volume);
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn Off":"�ر�"),UNBREAK);
												if(SysConfig.Bits.PirOn==0)
												{
														SysConfig.Bits.PirOn=1;
														PowerOnPir();
												}
										}
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Ultra power saving mode":"��ʡ��ģʽ"),UNBREAK);
										break;
								case 4:          //��������
										SysConfig.Bits.TampAct=pDatRec[6]?1:0;
										break;
								case 5:         //���𱨾�ʱ��   
										if(pDatRec[6]>=5 && pDatRec[6]<=35)
										{
												TamperAlarm.Tamp.Time=pDatRec[6];
												IntFlashWrite(TamperAlarm.TampBuf,TAMPER_ADDR,sizeof(TamperAlarm_t));
										}
										break;
								case 6:         //����ʱ��  
										if((int8_t)pDatRec[6]>=-11 && (int8_t)pDatRec[6]<=12)
										{
												SysConfig.Bits.TimeZone=(int8_t)pDatRec[6];
										}
										break;
								default:break;
						}
						UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],2,2,1000);
						break;
				case APP_MANAGE_ADMIN_CODE:   //�޸Ĺ���Ա����
						if(0!=IdyPassWord(&pDatRec[5]))     //У�������
						{
								ackDat[0]=2;
								UartBack_SendCmdData(pDatRec[4]+1,ackDat,2,2,1000);      //������֤��ͨ�������ʧ��
								break;
						}
						for(i=0;i<pktLen;i++)               //������������ʼ��ַ
						{
								if(pDatRec[5+i]==0 && pDatRec[6+i]!=0)break;
						}  
						AddPassWord(ADMIN,&pDatRec[6+i],NULL,&ackDat[1]);
						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),UNBREAK);
						ackDat[0]=1;                                                     //��ӳɹ�
						UartBack_SendCmdData(pDatRec[4]+1,ackDat,2,2,1000); 
						break;
				case APP_MANAGE_NORMAL_USER:
						if(pDatRec[5]==0 || pDatRec[5]>2)break;          //��������
						if(pDatRec[5]==1)           //����û�
						{
								switch(pDatRec[6])
								{
										case 0x01:          //�������
												RemoteAdd.type.face=1;
												MenuNow=Menu_KeyPadAddUsers_Face;
												break;
										case 0x02:          //���ָ�� 
												RemoteAdd.type.fp=1;
												MenuNow=Menu_KeyPadAddUsers_Fp;
												break;
										case 0x04:          //��ӿ�Ƭ 
												RemoteAdd.type.card=1;
												MenuNow=Menu_KeyPadAddUsers_Card;
												break;
										case 0x03:          //������� 
												switch(AddPassWord(NORMAL,&pDatRec[7],NULL,&ackDat[2]))
												{
														case PWD_ADD_SUCESS:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),UNBREAK);
																break;
														case PWD_ADD_CODE_EXIST:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password already exists":"�����Ѵ���"),UNBREAK);
																break;
														case PWD_ADD_MEM_FULL:
														case PWD_ADD_TIME_MEM_FULL:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password library is full":"���������"),UNBREAK);
																break;
														case PWD_ADD_TYP_WRONG:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add failed":"���ʧ��"),UNBREAK);
																break;
												}
												break;
										case 0x05:          //��Ӷ������
										{
												PwdExtStr_t ext={0};
												memcpy(&ackDat[3],&pDatRec[7],6);            //�̶�6�ֽ�����   
												ackDat[9]=0;
												//����
												ext.PwdTimeFreq.Type=PWD_TIME_FEQ;
												//ʱ��
												ext.PwdTimeFreq.TimeOutUtc=(uint32_t)pDatRec[16]<<24;
												ext.PwdTimeFreq.TimeOutUtc|=(uint32_t)pDatRec[15]<<16;
												ext.PwdTimeFreq.TimeOutUtc|=(uint32_t)pDatRec[14]<<8;
												ext.PwdTimeFreq.TimeOutUtc|=pDatRec[13];
												
												ext.PwdTimeFreq.TimeOutUtc+=(int32_t)SysConfig.Bits.TimeZone*3600;        //+ʱ��
												//����
												ext.PwdTimeFreq.Times=pDatRec[17];
												switch(AddPassWord(VISITOR,&ackDat[3],&ext,&ackDat[2]))
												{
														case PWD_ADD_SUCESS:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),UNBREAK);
																break;
														case PWD_ADD_CODE_EXIST:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password already exists":"�����Ѵ���"),UNBREAK);
																break;
														case PWD_ADD_MEM_FULL:
														case PWD_ADD_TIME_MEM_FULL:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password library is full":"���������"),UNBREAK);
																break;
														case PWD_ADD_TYP_WRONG:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add failed":"���ʧ��"),UNBREAK);
																break;
												}
												break;
										}
										case 0x06:        //��ӷÿ����룬������
										{
												PwdExtStr_t ext={0};
												memcpy(&ackDat[3],&pDatRec[7],6);            //�̶�6�ֽ�����   
												ackDat[9]=0;
												//����
												ext.PwdTimCyc.Type=PWD_TIME_CYC;
												//ʱ��
												ext.PwdTimCyc.TimeOutUtc=(uint32_t)pDatRec[16]<<24;
												ext.PwdTimCyc.TimeOutUtc|=(uint32_t)pDatRec[15]<<16;
												ext.PwdTimCyc.TimeOutUtc|=(uint32_t)pDatRec[14]<<8;
												ext.PwdTimCyc.TimeOutUtc|=pDatRec[13];
												//����
												ext.PwdTimCyc.Week=pDatRec[17];
												ext.PwdTimCyc.StartHour=pDatRec[18];
												ext.PwdTimCyc.StartMin=pDatRec[19];
												ext.PwdTimCyc.StopHour=pDatRec[20];
												ext.PwdTimCyc.StopMin=pDatRec[21];
												switch(AddPassWord(VISITOR,&ackDat[3],&ext,&ackDat[2]))
												{
														case PWD_ADD_SUCESS:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),UNBREAK);
																break;
														case PWD_ADD_CODE_EXIST:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password already exists":"�����Ѵ���"),UNBREAK);
																break;
														case PWD_ADD_MEM_FULL:
														case PWD_ADD_TIME_MEM_FULL:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password library is full":"���������"),UNBREAK);
																break;
														case PWD_ADD_TYP_WRONG:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add failed":"���ʧ��"),UNBREAK);
																break;
												}
												break;
										}
										case 7:              //���в������
												switch(AddPassWord(COERCE,&pDatRec[7],NULL,&ackDat[2]))
												{
														case PWD_ADD_SUCESS:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),UNBREAK);
																break;
														case PWD_ADD_CODE_EXIST:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password already exists":"�����Ѵ���"),UNBREAK);
																ackDat[2]=0xFF;
																break;
														case PWD_ADD_MEM_FULL:
														case PWD_ADD_TIME_MEM_FULL:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password library is full":"���������"),UNBREAK);
																break;
														case PWD_ADD_TYP_WRONG:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add failed":"���ʧ��"),UNBREAK);
																break;
												}
												break;
										case 8:              //���в��ָ��  
												RemoteAdd.type.fp=2;
												MenuNow=Menu_KeyPadAddUsers_Fp;
												break;
										default:break;
								}
						}
						else                        //ɾ���û�
						{
								switch(pDatRec[6])
								{
										case 0x01:          //ɾ������
												if(pDatRec[7]<100)
												{
														RemoteDel.type.face=1;
														RemoteDel.id=pDatRec[7];
														MenuNow=Menu_KeyPadDelUsers_Face;
												}
												break;
										case 0x02:          //ɾ��ָ�� 
												if(pDatRec[7]<100)
												{
														RemoteDel.type.fp=1;
														RemoteDel.id=pDatRec[7];
														MenuNow=Menu_KeyPadDelUsers_Fp;
												}
												break;
										case 0x03:          //ɾ������ 
										case 0x05:          //ɾ���������
										case 0x06:          //ɾ���ÿ����� 
												if(pDatRec[7]<100)
												{
														switch(DelPassWord(pDatRec[7]))
														{
																case PWD_DEL_SUCCESS:
																case PWD_DEL_CODE_NOT_EXIST:
																		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete succeed":"ɾ���ɹ�"),UNBREAK);
																		break;
																default:break;
														}
												}
												break;
										case 0x04:          //ɾ����Ƭ 
												if(pDatRec[7]<100)
												{
														switch(DelCard(pDatRec[7]))
														{
																case DEL_CARD_SUCCESS:
																case DEL_CARD_NOEXIST:
																		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete succeed":"ɾ���ɹ�"),UNBREAK);
																		break;
																default:break;
														}
												}
												break;
										case 0xAA:          //ɾ��ָ������û�
										{
												RemoteDelSelect.sum=pDatRec[7];          //ɾ���û�������
												if(RemoteDelSelect.sum==0 || RemoteDelSelect.sum>25)
												{
														RemoteDelSelect.sum=0;
														break;    //��Ŀ����
												}
												for(i=0;i<RemoteDelSelect.sum;i++)
												{
														RemoteDelSelect.id[i].u8[0]=pDatRec[8+i*2];     //�û�����
														RemoteDelSelect.id[i].u8[1]=pDatRec[8+i*2+1];   //�û����
												}
												RemoteDelSelect.type.select=1;
												MenuNow=Menu_KeyPadDelUsers_select;
												break;
										}
										case 0xFF:          //ɾ������ 
												RemoteDel.type.all=1;
												RemoteDel.id=0;
												MenuNow=Menu_KeyPadDelUsers_All;
												break;
										default:break;
								}
								ackDat[2]=pDatRec[7];         //ɾ��ʱ����ID
						}
						ackDat[0]=pDatRec[5];
						ackDat[1]=pDatRec[6];
						UartBack_SendCmdData(pDatRec[4]+1,ackDat,3,2,1000);
						break;
				case OTA_START:                //OTA��������
				{
						uT32_t ut32tmp={0};
						//��������״̬
						IapInfo.Det.stat=1;
						//�����������ܳ���
						ut32tmp.u8[0]=pDatRec[5];
						ut32tmp.u8[1]=pDatRec[6];
						ut32tmp.u8[2]=pDatRec[7];
						ut32tmp.u8[3]=pDatRec[8];
						IapInfo.Det.binSum=ut32tmp.u32;
						//����������У��
						ut32tmp.u8[0]=pDatRec[9];
						ut32tmp.u8[1]=pDatRec[10];
						ut32tmp.u8[2]=pDatRec[11];
						ut32tmp.u8[3]=pDatRec[12];
						IapInfo.Det.chkSum=ut32tmp.u32;
						//���浱ǰ�������ݳ���
						IapInfo.Det.pktSum=IapInfo.Det.binSum/64;
						if((IapInfo.Det.binSum%64)!=0)IapInfo.Det.pktSum+=1;
						//д״̬��ʶ
						IapInfo.Det.flag=OTA_FLAG;
						//����״̬
						
						IntFlashWrite(IapInfo.DetBuf,IAP_INFO_ADDR,sizeof(IapInfo_t));
						//�屸����
						IntFlashEraseBackup();
					 //Ӧ��
						UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],8,2,1000);
						//����״̬
						LPM_SetStopMode(LPM_OTA_ID,LPM_Disable);
						OtaInfo.Running=1;
						OtaInfo.TimeStart=GetTimerCount();
						OtaInfo.TimeoutCount=0;
						OtaInfo.NextPktNum=0;
						OtaInfo.GotFrame=0;   
						break;
				}
				case OTA_GET_FAME_ACK:
				{
						uT32_t ut32tmp={0};
						if(OtaInfo.NextPktNum!=((uint16_t)pDatRec[6]<<8|pDatRec[5]))
						{
								UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],8,2,1000);
								break;       //֡��Ų���
						}
						if(OtaInfo.NextPktNum<IapInfo.Det.pktSum)
						{
								IntFlashWrite(&pDatRec[7],BOOT_SIZE+APP_SIZE+OtaInfo.NextPktNum*64,(OtaInfo.NextPktNum==IapInfo.Det.pktSum-1)?(pktLen-2):64);
								OtaInfo.GotFrame=1;
								OtaInfo.NextPktNum++;
								OtaInfo.TimeoutCount=0;
						}
						else
						{
								IapInfo.Det.stat=3;
								//����������У��
								ut32tmp.u8[0]=pDatRec[7];
								ut32tmp.u8[1]=pDatRec[8];
								ut32tmp.u8[2]=pDatRec[9];
								ut32tmp.u8[3]=pDatRec[10];
								IapInfo.Det.chkSum=ut32tmp.u32;
							
								IntFlashWrite(IapInfo.DetBuf,IAP_INFO_ADDR,sizeof(IapInfo_t));
							
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Operation timed out":"��ӳɹ�"),BREAK);
								LPM_SetStopMode(LPM_OTA_ID,LPM_Enable);
								Set_POR_Reset();
						}
						break;
				}
				case ATE_START:
						if(MenuNow==Menu_KeyPadAteTest)break;
						if(pDatRec[5]==0x55 && pDatRec[6]==0xAA && pDatRec[7]==0xAA && pDatRec[8]==0x55)
						{
								MenuNow=Menu_KeyPadAteTest;  
						}
						UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],4,2,1000);
						break;
				case ATE_DOWNLINK:
						if(MenuNow!=Menu_KeyPadAteTest)break;
						AteStat=pDatRec[5];
						if(AteStat==1)LedVal.ledval=LED_MASK_ATE_ALL_ON;
						else if(AteStat==2)LedVal.ledval=LED_MASK_ATE_TOUCH;
						else if(AteStat==3)LedVal.ledval=0;
						else if(AteStat==4)LedVal.ledval=0;
						else if(AteStat==6)LedVal.ledval=0;
						else if(AteStat==7)LedVal.ledval=0;
						else if(AteStat==8)LedVal.ledval=0;
						else if(AteStat==11)LedVal.ledval=0;
						else if(AteStat==12)LedVal.ledval=0;
						else if(AteStat==13)LedVal.ledval=0;
						else if(AteStat==14)LedVal.ledval=0;
						else if(AteStat==9)LedVal.ledval=LED_MASK_ATE_ADDCARD;
						else if(AteStat==10)LedVal.ledval=LED_MASK_ATE_ADDCARD;
						else if(AteStat==5)LedVal.ledval=LED_MASK_ATE_DOORBELL;
						AteSubStat=0;
						UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],1,2,1000);
						break;
				case ATE_UPLINK_ACK:
						if(MenuNow!=Menu_KeyPadAteTest)break;
						break;
				case ATE_STOP:
						if(MenuNow!=Menu_KeyPadAteTest)break;
						if(pDatRec[5]==0xAA && pDatRec[6]==0x55 && pDatRec[7]==0x55 && pDatRec[8]==0xAA)
						{
								MenuNow=Menu_KeyPadNormal;  
						}
						UartBack_SendCmdData(pDatRec[4]+1,&pDatRec[5],4,2,1000);
						break;
			  default:break;
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
void UartBack_RtxTimeoutoutHandle(void)
{
		//��ֹ����ģʽ�£�������ݹ�����û������ͽ����˵͹���
		if(UartStruct[UART_BACK_INDEX].RxStat==RX_RUNNING && (LPM_GetStopModeValue()&LPM_BACKLOCK_RX_ID)==0)
		{
				LPM_SetStopMode(LPM_BACKLOCK_RX_ID,LPM_Disable);
		}
		//���մ���
		if(UartStruct[UART_BACK_INDEX].RxTime>0 && GetTimerElapse(UartStruct[UART_BACK_INDEX].RxTime)>UartStruct[UART_BACK_INDEX].RtxTimeout)
		{
				UartStruct[UART_BACK_INDEX].RxTime=0;
				UartStruct[UART_BACK_INDEX].RxStat=RX_IDLE;
				UartStruct[UART_BACK_INDEX].RxFrameOk=1;
		}
		//���ʹ���
		if(UartStruct[UART_BACK_INDEX].TxTime>0 && GetTimerElapse(UartStruct[UART_BACK_INDEX].TxTime)>UartStruct[UART_BACK_INDEX].RtxTimeout)   
		{
				UartStruct[UART_BACK_INDEX].TxTime=0;
				UartStruct[UART_BACK_INDEX].TxStat=TX_IDLE;
				UART_BACK_SCI->SCICR2&=~SCICR2_TIE_MASK;  
		}
		//OTA
		CheckFirmwareUpdate();
}


void CheckFirmwareUpdate(void)
{
		static uint32_t stime=0;
		uint8_t tmp[10]={0};
		
		if(OtaInfo.Running==0)return;
		
		if(OtaInfo.GotFrame==1)
		{
				tmp[1]=OtaInfo.NextPktNum/256;
				tmp[0]=OtaInfo.NextPktNum%256;
				UartBack_SendCmdData(OTA_GET_FAME,tmp,2,0,1000);
				stime=GetTimerCount();
				OtaInfo.GotFrame=0;
		}
		else if(GetTimerElapse(stime)>=4000)
		{
				OtaInfo.GotFrame=1;
				OtaInfo.TimeoutCount++;
				if(OtaInfo.TimeoutCount>=10)
				{
						OtaInfo.Running=0;
						OtaInfo.TimeoutCount=0;
						LPM_SetStopMode(LPM_OTA_ID,LPM_Enable);         //��״̬
						IapInfo.Det.stat=0;                             //����ʧ�ܣ����־
						IntFlashWrite(IapInfo.DetBuf,IAP_INFO_ADDR,sizeof(IapInfo_t));
				}
		}
}




