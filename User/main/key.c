#include "tsi_app.h"
#include "key.h"
#include "uart_face.h"
#include "uart_fp.h"
#include "timer.h"
#include "string.h"
#include "audio_demo.h"
#include "audio.h"
#include "led.h"
#include "keypad_menu.h"
#include "lpm.h"
#include "ioctrl_drv.h"
#include "card.h"
#include "led.h"
#include "pwm_drv.h"
#include "rtc.h"
#include "wdt_drv.h"
#include "uart.h"
#include "bat.h"
#include "lock_record.h"
#include "uart_back.h"


//����,SET��������PIR
typedef uint8_t (*GetPinStat)(uint8_t tPin);
typedef struct{
		uint8_t Pin;            //����
		uint8_t Dly;            //��ʱ
		uint8_t Stat;          //״̬ 
		uint32_t HiCnt;         //�ߵ�ƽ����
		uint32_t LoCnt;         //�͵�ƽ����
		uint32_t CntBk;         //�ߵ͵�ƽ��������
		GetPinStat Fun;         //��ȡ״̬���� 
}BtnStr_t;
BtnStr_t BtnStr[3];


//��������
KeyStat_t KeyStat={0};
uint32_t keydowncount=0;
uint32_t keyupcount=0;
uint32_t KeyDownStartTime=0;
uint32_t KeyUpStartTime=0;
//�����뱨��
uint8_t LockAlarm=0;
uint32_t TampStartTime=0;
//PIR
uint8_t isPirOn=0;

/******************************************************************************/
/*
//����SET������PIR��ʼ��
input:   none
output   none
return   none
*/
/******************************************************************************/
void TampSetButtonInit(void)
{
		if(SysConfig.Bits.PirOn==1)
		{
				PowerOnPir();
		}
		else
		{
				PowerOffPir();
		}
		EPORT_ConfigGpio(KEY_TAMPER_PIN,GPIO_INPUT);         //���𣺵͵�ƽ�ж�
		EPORT_PullConfig(KEY_TAMPER_PIN,EPORT_DISPULL);      //�ر����������ⲿ��2M������
		
		//���������
		IntFlashRead(TamperAlarm.TampBuf,TAMPER_ADDR,sizeof(TamperAlarm_t)); 
		if(TamperAlarm.Tamp.Flag!=0xAA || TamperAlarm.Tamp.Time==0 || TamperAlarm.Tamp.Time>60)
		{
				TamperAlarm.Tamp.Flag=0xAA;
				TamperAlarm.Tamp.Time=10;                                    //Ĭ��10S���� 
				TamperAlarm.Tamp.Stat=EPORT_ReadGpioData(KEY_TAMPER_PIN);    //�״��ϵ磬����״̬������
				TamperAlarm.Tamp.Day=0;
				TamperAlarm.Tamp.Hour=0;
				TamperAlarm.Tamp.Minute=0;
				TamperAlarm.Tamp.Second=0; 
				IntFlashWrite(TamperAlarm.TampBuf,TAMPER_ADDR,sizeof(TamperAlarm_t));
		}
		EPORT_ConfigGpio(KEY_BACK_PIN,GPIO_INPUT);          //��尴�����͵�ƽ�ж�
		EPORT_PullupConfig(KEY_BACK_PIN,ENABLE);            //ʹ������ 
	
		BtnStr[KEY_IDX_SET].Pin=KEY_BACK_PIN;                         //SET����
		BtnStr[KEY_IDX_SET].Fun=(GetPinStat)EPORT_ReadGpioData;     
	
		BtnStr[KEX_IDX_TAMP].Pin=KEY_TAMPER_PIN;                       //����
		BtnStr[KEX_IDX_TAMP].Fun=(GetPinStat)EPORT_ReadGpioData; 
		
		BtnStr[KEX_IDX_PIR].Pin=CPM_WAKEUP_PIN;                       //PIR����
		BtnStr[KEX_IDX_PIR].Fun=(GetPinStat)CPM_ReadGpioData; 
}
/******************************************************************************/
/*
//����SET������PIR����
input:   none
output   none
return   none
*/
/******************************************************************************/
void TampSetButtonDeInit(void)
{
		EPORT_Init(KEY_TAMPER_PIN,RISING_FALLING_EDGE_INT);	
		EPORT_Init(KEY_BACK_PIN,FALLING_EDGE_INT);
}

/******************************************************************************/
/*
//��PIR��Դ
input:   none
output   none
return   none
*/
/******************************************************************************/
void PowerOnPir(void)
{
		isPirOn=1;
		EPORT_ConfigGpio(PIR_POWER_PIN,GPIO_OUTPUT);
		EPORT_WriteGpioData(PIR_POWER_PIN,Bit_RESET);
}
/******************************************************************************/
/*
//�ر�PIR��Դ
input:   none
output   none
return   none
*/
/******************************************************************************/
void PowerOffPir(void)
{
		isPirOn=0;
		EPORT_ConfigGpio(PIR_POWER_PIN,GPIO_OUTPUT);
		EPORT_WriteGpioData(PIR_POWER_PIN,Bit_SET);
}

/******************************************************************************/
/*
//����������ʼ��
input:   none
output   none
return   none
*/
/******************************************************************************/
void Key_Init(void)
{
		app_tsi_init(&htsi); 
}
/******************************************************************************/
/*
//������������
input:   none
output   none
return   none
*/
/******************************************************************************/
void Key_DeInit(void)
{
		app_tsi_handle(&htsi);
		app_tsi_sleep(&htsi);
}

/******************************************************************************/
/*
//���ݴ���ֵ���ذ���ֵ
input:   TchNum -- ����ֵ
output   none
return   keyC -- ����ֵ
*/
/******************************************************************************/
uint8_t Key_GetNum(uint8_t TchNum)
{
		uint8_t keyC=0;
	
		switch(TchNum)
		{
				case 0:                    //5
						keyC=KEY_c;
						break;
				case 1:
						keyC=KEY_5;           //8
						break;
				case 2:
						keyC=KEY_2;        //0
						break;
				case 3:
						keyC=KEY_6;           //7
						break;
				case 4:
						keyC=KEY_3;           //*
						break;
				case 5:
						keyC=KEY_1;          //#
						break;
				case 6:
						keyC=KEY_4;          //9
						break;
				case 7:                  
						keyC=KEY_x;          //3
						break;
				case 8:
						keyC=KEY_7;           //6
						break;
				case 9:
						keyC=KEY_bell;           //bell
						break;
				case 10:
						keyC=KEY_8;           //1
						break;
				case 11:
						keyC=KEY_9;           //4
						break;
				case 12:
						keyC=KEY_0;           //2
						break;
		}
		return keyC;
}
/******************************************************************************/
/*
//����ɨ��
input:   none
output   none
return   none
*/
/******************************************************************************/
void Key_Scan(void)
{
		static uint8_t KeyDownBk=0xFF; 
		static KeyStat_t LastKeyStat={0};
	
		if(b20msFlag==1)
		{
				static uint8_t it=0;
				b20msFlag=0;
				TampSetButton_Scan();
				if(++it>=50)
				{
						it=0;
						WDT_FeedDog();
				}
		}
		if(b100msFlag==1)
		{
				b100msFlag=0;
				if(GetTimerElapse(0)>PowerOnCheckBatDelay)             //��ʱ1S�ٲ��ѹ
				{
						Bat_GetVoltage();
				}
				WriteUserOpera();
		}
		app_tsi_handle(&htsi);  
		
		if(KeyDownBk==0xFF)KeyDownBk=htsi.Touch.Current;
		
		if(SysConfig.Bits.KeyPADLock==1)
		{
				LedVal.ledval=0;
				KeyStat.Flag=0;
				if(htsi.Touch.Current < TSI_MULTI_VALID)
				{
						KeyUpStartTime=0;
						LPM_SetStopMode(LPM_KEYPAD_ID,LPM_Disable);
						if(activeAudioNumber!=GetVolIndex(SysConfig.Bits.Language?"Too many fails, locked for 3 minutes":"��֤�����������,ϵͳ����3����"))
						{
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Too many fails, locked for 3 minutes":"��֤�����������,ϵͳ����3����"),BREAK);
						}
				}
				else
				{
						KeyDownStartTime=0;
						if(KeyUpStartTime==0)KeyUpStartTime=GetTimerCount();
						else if(GetTimerElapse(KeyUpStartTime)>KEY_INPUT_TIMEOUT)
						{
								LPM_SetStopMode(LPM_KEYPAD_ID,LPM_Enable);
						}
				}
		}
		else
		{
				if(htsi.Touch.Current < TSI_MULTI_VALID)               //�а�������
				{
						LPM_SetStopMode(LPM_KEYPAD_ID,LPM_Disable);
						KeyUpStartTime=0;
						if(htsi.Touch.Current!=KeyDownBk)
						{
								KeyDownBk=htsi.Touch.Current;
								KeyDownStartTime=GetTimerCount();
						}
						else if(KeyDownStartTime>0)
						{
								if(GetTimerElapse(KeyDownStartTime)>10)
								{
										KeyDownStartTime=0;
										
										if(LastKeyStat.Numb!=Key_GetNum(htsi.Touch.Current))
										{
												KeyStat.Numb=Key_GetNum(htsi.Touch.Current);
												KeyStat.Flag=1;
												KeyStat.STime=GetTimerCount();
										}
										else if(GetTimerElapse(LastKeyStat.STime)>250)
										{
												KeyStat.Numb=Key_GetNum(htsi.Touch.Current);
												KeyStat.Flag=1;
												KeyStat.STime=GetTimerCount();
										}
								}
						}
				}
				else                                                
				{		
						KeyDownStartTime=0;
						if(KeyStat.STime==0)KeyStat.STime=GetTimerCount();
					
						if(KeyDownBk!=htsi.Touch.Current)
						{
								KeyUpStartTime=GetTimerCount();
								KeyDownBk=htsi.Touch.Current;
						}
						else if(KeyUpStartTime>0)
						{
								if(GetTimerElapse(KeyUpStartTime)>10)
								{
										KeyUpStartTime=0;
										LastKeyStat=KeyStat;          //�������𱣴��ϴε�ֵ
										KeyStat.Flag=0;
										KeyStat.STime=GetTimerCount();
								}
						}
				}	
		}		
}

//ATE�����ⲻ����
void Key_AteScan(void)
{
		static uint8_t KeyDownBk=0xFF; 
		static KeyStat_t LastKeyStat={0};
	
		if(b20msFlag==1)
		{
				static uint8_t it=0;
				b20msFlag=0;
				if(++it>=50)
				{
						it=0;
						WDT_FeedDog();
						Bat_PrintVoltage();
				}
		}
		//���ѹ
		if(b100msFlag==1)
		{
				b100msFlag=0;
				Bat_GetVoltage();
		}
		//�����������
		app_tsi_handle(&htsi);  
		if(KeyDownBk==0xFF)KeyDownBk=htsi.Touch.Current;
		if(htsi.Touch.Current < TSI_MULTI_VALID)               //�а�������
		{
				LPM_SetStopMode(LPM_KEYPAD_ID,LPM_Disable);
				KeyUpStartTime=0;
				if(htsi.Touch.Current!=KeyDownBk)
				{
						KeyDownBk=htsi.Touch.Current;
						KeyDownStartTime=GetTimerCount();
				}
				else if(KeyDownStartTime>0)
				{
						if(GetTimerElapse(KeyDownStartTime)>10)
						{
								KeyDownStartTime=0;
								
								if(LastKeyStat.Numb!=Key_GetNum(htsi.Touch.Current))
								{
										KeyStat.Numb=Key_GetNum(htsi.Touch.Current);
										KeyStat.Flag=1;
										KeyStat.STime=GetTimerCount();
								}
								else if(GetTimerElapse(LastKeyStat.STime)>250)
								{
										KeyStat.Numb=Key_GetNum(htsi.Touch.Current);
										KeyStat.Flag=1;
										KeyStat.STime=GetTimerCount();
								}
						}
				}
		}
		else                                                
		{		
				KeyDownStartTime=0;
				if(KeyStat.STime==0)KeyStat.STime=GetTimerCount();
			
				if(KeyDownBk!=htsi.Touch.Current)
				{
						KeyUpStartTime=GetTimerCount();
						KeyDownBk=htsi.Touch.Current;
				}
				else if(KeyUpStartTime>0)
				{
						if(GetTimerElapse(KeyUpStartTime)>10)
						{
								KeyUpStartTime=0;
								LastKeyStat=KeyStat;          //�������𱣴��ϴε�ֵ
								KeyStat.Flag=0;
								KeyStat.STime=GetTimerCount();
						}
				}
		}	
}

/******************************************************************************/
/*
//������SET����ɨ��
input:   none
output   none
return   none
*/
/******************************************************************************/
void TampSetButton_Scan(void)
{
		uint8_t i=0;
		//����״̬
		for(i=0;i<3;i++)
		{
				if(BtnStr[i].Dly>0)                //��ʱ����
				{
						BtnStr[i].Dly--;
						continue;
				}
				if(BtnStr[i].Fun(BtnStr[i].Pin))
				{
						BtnStr[i].Stat=1;
						BtnStr[i].CntBk=BtnStr[i].LoCnt;  
						BtnStr[i].LoCnt=0;     
				}
				else
				{
						BtnStr[i].Stat=0;
						BtnStr[i].CntBk=BtnStr[i].HiCnt;
						BtnStr[i].HiCnt=0;
				}
		}
		//���¼���
		for(i=0;i<3;i++)
		{
				if(BtnStr[i].Stat)BtnStr[i].HiCnt++;
				else BtnStr[i].LoCnt++;
		}
		//��尴������
		if(BtnStr[0].Stat==0)         //����
		{
				if(BtnStr[0].LoCnt>=SET_FACTORY_RESET_TIME)         //5s
				{
						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Factory reset":"�ָ���������"),BREAK);  
						LockFactoryResetOpera();
						//��λ����ģ�飺��Ϊ����ģ��ʶ��ʱ��Ƚϳ���������ʶ������ж��������������������
#ifndef NO_FACE_MODE
						FaceReady.Ready=0;
						UartFace_TxQueueReset();
						UartFace_PowerReset();
					
						FaceDel.DelStat=FACEDEL_READUSER;
						FaceDel.StTime=0;
						FaceDel.Retry=0;
#endif
						FpDel.LdFpDelStat=FP_DEL_READ_USER;
						FpDel.StTime=0;
						FpDel.ComRetry=0;
					
						uint8_t DeFpOk=0,DeFaceOk=0;
						uint8_t DelDone=0;
						uint8_t FpFailRetry=0,FaceFailRetry=0;
						while(1)
						{
								WDT_FeedDog();               //ι��
								if(DeFpOk==0)
								{	
										switch(FpDelHandle(0xFF))
										{
												case FP_SUCCESS:       //ɾ���ɹ�
														DeFpOk=1;
														FpFailRetry=0;
														printf("del fp ok\n");
														break;
												case FP_FAIL:          //ɾ��ʧ�ܣ�����
														if(++FpFailRetry>=3)
														{
																FpFailRetry=0;
																DeFpOk=2;
														}
														else
														{
																FpDel.LdFpDelStat=FP_DEL_READ_USER;
																FpDel.StTime=0;
																FpDel.ComRetry=0;
														}
														printf("del fp fail\n");
														break;
												default:break;
										}
								}
#ifndef NO_FACE_MODE
								if(DeFaceOk==0)
								{
										switch(FaceDelHandle(0xFF))
										{
												case OP_SUCCESS:
														FaceFailRetry=0;
														DeFaceOk=1;
														break;
												case OP_FAIL:
												case OP_TIMEOUT:
														if(++FaceFailRetry>=3)
														{
																FaceFailRetry=0;
																DeFaceOk=2;
														}
														else
														{
																FaceReady.Ready=0;
																UartFace_PowerReset();
																UartFace_TxQueueReset();
																FaceDel.DelStat=FACEDEL_READUSER;
																FaceDel.Retry=0;
																FaceDel.StTime=0;
														}
														break;
												default:break;
										}
								}
#endif
								if(
#ifndef NO_FACE_MODE									
									DeFaceOk==1 && 
#endif
									DeFpOk==1 && DelDone==0)
								{
										DelDone=1;
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Setting succeed":"���óɹ�"),UNBREAK);
										FactoryReset();            //ɾ����������
										LedVal.ledval=0;
								}
								else if(DeFpOk==2 || DeFaceOk==2)
								{
										if(DelDone==0)
										{
												DelDone=1;
												LedVal.ledval=0;
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Operation timed out":"������ʱ"),UNBREAK);
										}
								}
								Uart_RtxFrameHandle();
								Audio_RunningHandle();
								Led_UpdateVal();
								if(DelDone==1 && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0 && BtnStr[0].Fun(BtnStr[0].Pin))
								{
										Set_POR_Reset();
								}
						}
				}
				LPM_SetStopMode(LPM_BTN_BACK_ID,LPM_Disable);
		}
		else                      //����
		{
				if(BtnStr[0].CntBk>1)           //�������𣬱���֤����Ա����  
				{
						if(KeyPadMode!=KEYPAD_CONFIG )   
						{
								KeyPadMode=KEYPAD_CONFIG;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please verify administrator":"����֤����Ա"),UNBREAK);  
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Enter management mode":"�������ģʽ"),UNBREAK);
						}
				}
				LPM_SetStopMode(LPM_BTN_BACK_ID,LPM_Enable);
		}
		//������		
		if(TamperAlarm.Tamp.Stat!=BtnStr[1].Stat)
		{
				if(BtnStr[1].LoCnt>=2 || BtnStr[1].HiCnt>=2)
				{
						TamperAlarm.Tamp.Stat=BtnStr[1].Stat;
						//д��FLASH����״̬
						tm rtc_times={0};
						RTC_GetTime(&rtc_times);
						TamperAlarm.Tamp.Day=rtc_times.day;
						TamperAlarm.Tamp.Hour=rtc_times.hour;
						TamperAlarm.Tamp.Minute=rtc_times.minute;
						TamperAlarm.Tamp.Second=rtc_times.second;
						IntFlashWrite(TamperAlarm.TampBuf,TAMPER_ADDR,sizeof(TamperAlarm_t)); 
						//����Ǳ�����д���¼����������
						if(TamperAlarm.Tamp.Stat==0 && SysConfig.Bits.TampAct==1)
						{
								LockAlarm=1;
								TampStartTime=GetTimerCount();
								LPM_SetStopMode(LPM_TAMP_ID,LPM_Disable);
								LockTampTrigOpera();
						}
				}
		}
		if(LockAlarm==1 && TampStartTime>0)
		{
				if(GetTimerElapse(TampStartTime)<TamperAlarm.Tamp.Time*1000)
				{
						if(activeAudioNumber!=GetVolIndex("���𱨾���"))
						{
								AudioPlayVoice(GetVolIndex("���𱨾���"),BREAK);
						}
				}
				else 
				{
						TampStartTime=0;
						LPM_SetStopMode(LPM_TAMP_ID,LPM_Enable);
				}
		}
		//PIR����	
}















