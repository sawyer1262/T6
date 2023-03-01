#include "keypad_menu.h"
#include "key.h"
#include "led.h"
#include <string.h>
#include "audio.h"
#include "lpm.h"
#include "password.h"
#include "uart_back.h"
#include "card.h"
#include "uart_fp.h"
#include "uart_face.h"
#include <math.h>
#include "uart.h"
#include "delay.h"
#include "rtc.h"
#include "lock_record.h"
#include "startup_demo.h"
#include "eport_drv.h"
#include "bat.h"


uint8_t InputBuf[INPUT_MAX];           //�������ݻ���
uint8_t InputIdx=0;                    //��������Index     :ע�ⲻҪԽ�硣
KeyMode_t KeyPadMode=KEYPAD_NORMAL;    //��������ģʽ
MenuFun MenuNow;                       //��ǰ���в˵�
uint32_t MenuNormalStart=0;            //�����濪ʼʱ��
/******************************************************************************/
/*
//���˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadNormal(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0,temp[5]={0};
		uint32_t KeyPressCount=0;
		//���ý�������
		if(SysConfig.Bits.Volume==0 || SysConfig.Bits.SupPowSave==1)      //�������߳�ʡ��ģʽ
		{
				Audio_SetVolume(0); 
		}
		else
		{
				Audio_SetVolume(SysConfig.Bits.Volume); 
		}
		//��λ��������״̬
		InputIdx=0;
		//��λ����״̬
#ifndef NO_FACE_MODE
		UartFace_PowerReset();
		UartFace_TxQueueReset();
		printf("face power on\n");    
		FaceReady.Ready=0;
		FaceIdy.FsStat=FACE_IDLE;
		FaceIdy.StTime=0;
		FaceIdy.IdyRetry=0;
#endif
		//��λˢ��״̬
		CdStat.StTime=0;
		//��λָ��״̬
		FpScan.LdFpStat=FP_IDLE;
		if(FpPowerDown==1)
		{
				FpPowerDown=0;
				UartFp_DatStrInit();
				UartFp_Init();
		}
		//��0������ǿ�����߼���
		ForcePowerDownCount=0;
		MenuNormalStart=GetTimerCount();
		while(1)
		{
				if((KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb) && SysConfig.Bits.KeyPADLock!=1)
				{
						LedVal.ledval=LED_MASK_NORMAL;
						KeyBit=GetKeyLedBit(KeyStat.Numb);		
#ifndef NO_FACE_MODE
						if(KeyStat.Flag!=0)
						{
								KeyPressCount++;
						}
#endif
						switch(KeyStat.Numb)
						{
								case KEY_bell:         //����
										if(KeyStat.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
											  UserControlLock(CTL_DOOR_BELL,CODE_OPEN,0XFF);
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:            //*�˸��
										if(KeyStat.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												InputBuf[InputIdx++]=KeyStat.Numb;
												if(InputIdx>=INPUT_MAX)InputIdx=0;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
												
												if(strstr((const char*)InputBuf,"***")!=0)
												{
#ifdef MY_DEBUG
														Demo_StartupConfig(Startup_From_ROM, ENABLE);               //����ģʽ***��������
#else
														LPM_ResettopModeValue();
														UartBack_SendCmdData(CTRL_FAST_SLEEP,temp,0,1,1000);        //��������ָ��
#endif
												}
										}
										break;
								case KEY_c:        //#ȷ�ϼ�
										if(KeyStat.Flag!=0)
										{
												uint8_t CodeBuf[INPUT_MAX]={0};
												uint8_t PwdIdx=0;
												
												LedVal.ledval&=~(1<<KeyBit);     //�������£���Ӧ��ָʾ��Ϩ��
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												InputBuf[InputIdx++]=KeyStat.Numb; //�����ֵ
												if(InputIdx>=INPUT_MAX)InputIdx=0;
												InputBuf[InputIdx]=0;       //������
											
												printf("inputbuf:%s\n",InputBuf);
												
												if(strcmp((const char*)InputBuf,"400#")==0)               //����400����绰
												{
														AudioPlayNumString("4008538996");
														InputIdx=0;
												}
												else if(strstr((const char*)InputBuf,"*#")!=0)            //*#,������
												{
														if(SysConfig.Bits.Activate==0)
														{
																char *ret1=NULL,inputLen=0;
																ret1=strstr((const char*)InputBuf,"*#");
																inputLen=strlen(ret1);
																if(inputLen>=8)
																{
																		ret1+=2;
																		printf("retbuf:%s\n",ret1);
																		GetCodeWithoutX(ret1,(char*)CodeBuf);
																		printf("codebuf:%s\n",CodeBuf);
																		if(CheckSnActiveCode((char*)CodeBuf))
																		{
																				SysConfig.Bits.Activate=1;
																				SysConfig.Bits.PirOn=1;
																				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Activation succeeded":"����ɹ�"),UNBREAK);
																		}
																		else
																		{
																				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please activate the product":"�뼤���Ʒ"),UNBREAK);
																		}
																		InputIdx=0;
																}
																else if(strstr((const char*)InputBuf,"*#06#")!=0) 
																{
																		AudioPlayNumString(SnNum);
																		InputIdx=0;
																		if(KeyPadMode==KEYPAD_CONFIG)KeyPadMode=KEYPAD_NORMAL;
																}
																else
																{
																		if(inputLen>2)   //��ȥ*#������������
																		{
																				printf("input len short\n");
																				InputIdx=0;
																		}
																		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please activate the product":"�뼤���Ʒ"),UNBREAK);
																}
														}
														else if(KeyPadMode!=KEYPAD_CONFIG)
														{	
																if(SysConfig.Bits.Volume==0 || SysConfig.Bits.SupPowSave==1)   //����ģʽ���Ȼָ�����
																{
																		Audio_SetVolume(1); 
																}
																KeyPadMode=KEYPAD_CONFIG;
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please verify administrator":"����֤����Ա"),UNBREAK);  
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Enter management mode":"�������ģʽ"),UNBREAK);
														}
														else if(strstr((const char*)InputBuf,"*#06#")!=0) 
														{
																AudioPlayNumString(SnNum);
																InputIdx=0;
																if(KeyPadMode==KEYPAD_CONFIG)KeyPadMode=KEYPAD_NORMAL;
														}
														else
														{
																char *ret=NULL,inputLen=0;
																ret=strstr((const char*)InputBuf,"*#");
																inputLen=strlen(ret);
																if(inputLen>=5)
																{
																		ret+=2;
																		printf("retbuf:%s\n",ret);
																		GetCodeWithoutX(ret,(char*)CodeBuf);
																		printf("codebuf:%s\n",CodeBuf);
																		PwdIdx=IdyPassWord(CodeBuf);
																		if(PwdIdx<PWD_MAX && Pwd.PwdGenStr[PwdIdx].Pwd.Type==ADMIN)
																		{
																				if(CheckAdminCode())
																				{
																						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Change administer password":"�޸Ĺ�������"),UNBREAK);
																						MenuNow=Menu_KeyPadModifyAdmUsers_Password;
																				}
																				else
																				{
																						MenuNow=Menu_KeyPadConfigSelect;               //�л��˵�
																				}
																				LockUpdatePeripLockStat(PERIP_CODE_INDEX,0);
																		}
																		else
																		{
																				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Verification failed":"��֤ʧ��"),UNBREAK);
																				PirLed_SetStat(LOGO_OPEN_FAIL);
																				LockUpdatePeripLockStat(PERIP_CODE_INDEX,1);
																		}
																		InputIdx=0;
																}
														}
												}
												else                                     //��֤���� 
												{
														printf("inputbuf:%s\n",InputBuf);
														GetCodeWithoutX((char*)InputBuf,(char*)CodeBuf);
														printf("codebuf:%s\n",CodeBuf);
													
														if(strlen((char*)CodeBuf)<6)break;       //����̫��
														InputIdx=0;
														if(SysConfig.Bits.Activate==0)
														{
																if(CheckSnActiveCode((char*)CodeBuf))
																{
																		SysConfig.Bits.Activate=1;
																		SysConfig.Bits.PirOn=1;
																		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Activation succeeded":"����ɹ�"),UNBREAK);
																}
																else
																{
																		AudioPlayVoice(GetVolIndex("ָ����"),UNBREAK);						//��������
																		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please activate the product":"�뼤���Ʒ"),UNBREAK);
																		PirLed_SetStat(LOGO_OPEN_SUCCESS);			
																		UserControlLock(CTL_OPEN_LOCK,CODE_OPEN,0XFF);
																}
																break;  
														}
														PwdIdx=IdyPassWord(CodeBuf);
														printf("PwdIdx:0x%02x\n",PwdIdx);
														if(PwdIdx<PWD_MAX)          //��������
														{
																LockUpdatePeripLockStat(PERIP_CODE_INDEX,0);
													
																if(KeyPadMode==KEYPAD_CONFIG && Pwd.PwdGenStr[PwdIdx].Pwd.Type==ADMIN)
																{
																		if(CheckAdminCode())
																		{
																				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Change administer password":"�޸Ĺ�������"),UNBREAK);
																				MenuNow=Menu_KeyPadModifyAdmUsers_Password;
																		}
																		else
																		{
																				MenuNow=Menu_KeyPadConfigSelect;               //�л��˵�
																		}
																}
																else if(KeyPadMode==KEYPAD_CONFIG)
																{
																		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Verification failed":"��֤ʧ��"),UNBREAK);
																		PirLed_SetStat(LOGO_OPEN_FAIL);
																		LockUpdatePeripLockStat(PERIP_CODE_INDEX,1);
																}
																else if(SysConfig.Bits.MutiIdy==1)
																{
																		if(MutiIdyItem&(~IDY_CODE))        //��������֤�ɹ�+������֤�ɹ�
																		{
																				MutiIdyItem=0;
																				AudioPlayVoice(GetVolIndex("ָ����"),UNBREAK);						//��������
																				PirLed_SetStat(LOGO_OPEN_SUCCESS);
																				UserControlLock(CTL_OPEN_LOCK,CODE_OPEN,PwdIdx);
																		}
																		else
																		{
																				MutiIdyItem|=IDY_CODE;  
																				PirLed_SetStat(LOGO_OPEN_SUCCESS_MUTIL);
																		}
																}
																else
																{
																		AudioPlayVoice(GetVolIndex("ָ����"),UNBREAK);						//��������
																		PirLed_SetStat(LOGO_OPEN_SUCCESS);
																		UserControlLock(CTL_OPEN_LOCK,CODE_OPEN,PwdIdx);
																}
														}
														else if(PwdIdx==PWD_IDY_ONETIME_CODE)       //һ��������
														{
																LockUpdatePeripLockStat(PERIP_CODE_INDEX,0);
																AudioPlayVoice(GetVolIndex("ָ����"),UNBREAK);						//��������
																PirLed_SetStat(LOGO_OPEN_SUCCESS);
																UserControlLock(CTL_OPEN_LOCK,CODE_OPEN,PwdIdx);
														}
														else
														{
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Verification failed":"��֤ʧ��"),UNBREAK);               //��֤ʧ��
																PirLed_SetStat(LOGO_OPEN_FAIL);
																LockUpdatePeripLockStat(PERIP_CODE_INDEX,1);
														}
												}
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:           //���ּ�
										if(KeyStat.Numb>='0' && KeyStat.Numb<='9')
										{
												if(KeyStat.Flag!=0)
												{
														LedVal.ledval&=~(1<<KeyBit);
														AudioPlayVoice(GetVolIndex("��"),BREAK);
														InputBuf[InputIdx++]=KeyStat.Numb;
														if(InputIdx>=INPUT_MAX)InputIdx=0;
												}
												else
												{
														LedVal.ledval|=(1<<KeyBit);
												}
										}
										else
										{
												LedVal.ledval=0;
										}
										break;
						}
						if(KeyBk.Numb==0 && KeyStat.Flag==0)      //�ϵ��⵽�������
						{
								LedVal.ledval=LED_MASK_WAKE;
						}
						KeyBk=KeyStat;
				}
				//����ɨ��
				Key_Scan();
				//ˢ��ɨ��
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(CdStat.StTime==0)
						{
								CardSearchHandle();
								CdStat.StTime=GetTimerCount();
						}
						else if(GetTimerElapse(CdStat.StTime)>CARD_SEARCH_INTVAL)
						{
								CdStat.StTime=0;
						}
				}
				//������ʱ
				if(SysConfig.Bits.KeyPADLock!=1 && KeyStat.Flag==0 && KeyStat.STime>0)        
				{
						if(GetTimerElapse(KeyStat.STime)>KEY_INPUT_TIMEOUT)     //10sδ�����˳�����
						{
								KeyStat.STime=0;
								InputIdx=0;
								LPM_SetStopMode(LPM_KEYPAD_ID,LPM_Enable);
						} 
//3Sδ����Ϩ�����������Ϊ�����е�,ʡһ���
//						else if(GetTimerElapse(KeyStat.STime)>3000)             
//						{
//								if(LedVal.ledval!=LED_MASK_WAKE)
//								{
//										LedVal.ledval=LED_MASK_WAKE;
//								}
//						}
				}
				//����
#ifndef NO_FACE_MODE
				if(KeyPressCount<3)           //�а��������Ͳ�ɨ������
				{
						OrbFaceScanHandle();
				}
				else
				{
						UartFace_PowerOff();
				}
#endif
				//ָ��
				LdFpStatHandle();
				//����
				Audio_RunningHandle();
				//����
				Uart_RtxFrameHandle();
				//PIR LED
				PirLed_Handle();
				//���� LED
				Led_UpdateVal();	
				//д������Ϣ
				LockCheckPeripLockStat();
				LockWriteSysConfig();
				LockWritePerpLcokInfo();
				//��ʱ
				SnycNetTime();
				//�͹��Ĵ���
				if(GetTimerElapse(MenuNormalStart)>5000)   //����������ͣ��5s
				{
						LPM_EnterLowPower();
				}
				//�����л�
				if(MenuNow!=Menu_KeyPadNormal && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)break;
		}
}

/******************************************************************************/
/*
//һ���Ӳ˵�ѡ��˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadConfigSelect(void)  
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
		
		InputIdx=0;
		
		UartFace_PowerOff();
		UartFace_TxQueueReset();
	
	
		if(SysConfig.Bits.Volume==0 || SysConfig.Bits.SupPowSave==1)
		{
				Audio_SetVolume(1); 
		}
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add users":"����û�"),BREAK);       
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"one":"1"),UNBREAK);   
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete users":"ɾ���û�"),UNBREAK);      
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);         
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);            
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Modify administrator account":"�޸Ĺ���Ա�˻�"),UNBREAK);    
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"three":"3"),UNBREAK);    //3
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Network settings":"��������"),UNBREAK);    //�������� 
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"four":"4"),UNBREAK);    //4
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"System settings":"ϵͳ����"),UNBREAK);     //ϵͳ���� 
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"five":"5"),UNBREAK);     //5
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Language settings":"��������"),UNBREAK);     //ϵͳ���� 
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"six":"6"),UNBREAK);     //5
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"��������":"Language settings"),UNBREAK);     //ϵͳ���� 
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"�밴":"Please press"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"6":"six"),UNBREAK);     //5
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);     //�˳�
		
		LedVal.ledval=LED_MASK_CONFIG_SEL;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_CONFIG_SEL;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);	
						switch(KeyBk.Numb)
						{
								case KEY_1:               //����û�
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit); 
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadAddUsers; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_2:               //ɾ���û�
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit); 
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadDelUsers; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_3:               //�޸Ĺ���Ա�˻�
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit); 
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadModifyAdmUsers; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_4:               //�������
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit); 
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadConfNet; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_5:               //ϵͳ���� 
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit); 
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadSysConf; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_6:               //�������� 
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit); 
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadSysConf_Language; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //*�˳�
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit); 
												if(MenuNow!=Menu_KeyPadNormal)
												{
														AudioPlayVoice(GetVolIndex("��"),BREAK);
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),UNBREAK);
														KeyPadMode=KEYPAD_NORMAL;
														MenuNow=Menu_KeyPadNormal;
												}
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				/*******************************************************************/
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				/*******************************************************************/
				//�����������,��������������������ֱ�Ӽ������
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				/*******************************************************************/
				if(MenuNow!=Menu_KeyPadConfigSelect && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}

/******************************************************************************/
/*
//����û��Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadAddUsers(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
	
#ifndef NO_FACE_MODE
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add face":"�������"),BREAK);      //�������
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"one":"1"),UNBREAK);     //1
#endif
#ifndef NO_FACE_MODE	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add fingerprint":"���ָ��"),UNBREAK);    //���ָ�� 
#else
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add fingerprint":"���ָ��"),BREAK);    //���ָ�� 
#endif
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);    //2
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add password":"�������"),UNBREAK);    //������� 
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"three":"3"),UNBREAK);    //3
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add card":"��ӿ�Ƭ"),UNBREAK);     //��ӿ�Ƭ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"four":"4"),UNBREAK);     //4
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add one-time password":"���һ��������"),UNBREAK);     //���
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"five":"5"),UNBREAK);     //5
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
				
		LedVal.ledval=LED_MASK_ADDUSER;
	
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_ADDUSER;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
#ifndef NO_FACE_MODE	
								case KEY_1:               //���������û�
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
												MenuNow=Menu_KeyPadAddUsers_Face;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
#endif
								case KEY_2:               //����ָ���û�
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
												MenuNow=Menu_KeyPadAddUsers_Fp;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_3:               //���������û�
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
												MenuNow=Menu_KeyPadAddUsers_Code;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_4:               //���ӿ��û� 
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
												MenuNow=Menu_KeyPadAddUsers_Card;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_5:               //����һ��������
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
												MenuNow=Menu_KeyPadAddUsers_OnetimeCode;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //���� 
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
												MenuNow=Menu_KeyPadConfigSelect;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //�����������
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadAddUsers && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//��������Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadAddUsers_Code(void)
{
	  uint8_t codeBk[PWD_LEN_MAX]={0};                  //�ݴ��һ�����������
		
		uint8_t FirstCodeOk=0;
		uint32_t VolFinishTime=0;
		
		uint8_t freeslot=0;
		
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint8_t newStart=1;
		
		UartFace_PowerOff();
		
		InputIdx=0;
		memset(InputBuf,0,INPUT_MAX);
		
		LedVal.ledval=LED_MASK_ADDUSER_CODE;
	
		while(1)
		{
				if(newStart==1 && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)      //�����ɣ�������һ��
				{
						freeslot=GetPassWordFreeSlot();
						if(freeslot==PWD_MAX)
						{
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password library is ful":"���������"),UNBREAK);
								MenuNow=Menu_KeyPadAddUsers;
						}
						else
						{
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add password":"�������"),BREAK);                     //�������
								AudioPlayVoice(freeslot/10+GetVolIndex(SysConfig.Bits.Language?"zero":"0"),UNBREAK); 
								AudioPlayVoice(freeslot%10+GetVolIndex(SysConfig.Bits.Language?"zero":"0"),UNBREAK);
								
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Hash key to confirm":"ȷ���밴#�ż�"),UNBREAK);     //ȷ��
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);     //����
						}
						newStart=0;
				}
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_ADDUSER_CODE;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_x:               //���� 
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit); 
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												if(InputIdx>0)
												{
														InputIdx--;
												}
												else 
												{
														MenuNow=Menu_KeyPadAddUsers; 
												}
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_c:
										if(KeyBk.Flag!=0)
										{
												InputBuf[InputIdx]=0;
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												if(InputIdx<PWD_LEN_MIN || InputIdx>PWD_LEN_MAX-1)              //�������
												{
														AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
														AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
												}
												else if(FirstCodeOk==0)    //��һ����������
												{
														memcpy(codeBk,InputBuf,PWD_LEN_MAX);
														printf("first code: %s",codeBk);
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please enter the password again":"������������"),UNBREAK);
														FirstCodeOk=1;
												}
												else
												{
														printf("second code: %s",InputBuf);
														if(strcmp((const char*)InputBuf,(const char*)codeBk)==0)
														{
																switch(AddPassWord(NORMAL,codeBk,NULL,NULL))
																{
																		case PWD_ADD_SUCESS:
																		{
																				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),UNBREAK);
																				newStart=1;
																				break;
																		}
																		case PWD_ADD_CODE_EXIST:
																				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password already exists":"�����Ѵ���"),UNBREAK);
																				break;
																		case PWD_ADD_MEM_FULL:
																		case PWD_ADD_TIME_MEM_FULL:
																				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password library is ful":"���������"),UNBREAK);
																				MenuNow=Menu_KeyPadAddUsers;
																				break;
																		case PWD_ADD_TYP_WRONG:
																				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add failed":"���ʧ��"),UNBREAK);
																				break;
																}
																FirstCodeOk=0;
																memset(codeBk,0,PWD_LEN_MAX);
														}
														else
														{
																AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
																AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
														}
														
												}
												memset(InputBuf,0,InputIdx);
												InputIdx=0;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:
										if(KeyBk.Numb>='0' && KeyBk.Numb<='9')
										{
												if(KeyBk.Flag!=0)
												{
														LedVal.ledval&=~(1<<KeyBit);
														AudioPlayVoice(GetVolIndex("��"),BREAK);
														InputBuf[InputIdx++]=KeyBk.Numb;
														if(InputIdx>=INPUT_MAX)InputIdx=0;
												}
												else
												{
														LedVal.ledval|=(1<<KeyBit);
												}
										}
										break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //����������ɺ�10Sδ�������˳����˵�
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadAddUsers_Code && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//���һ���������Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadAddUsers_OnetimeCode(void)
{
	  uint8_t codeBk[PWD_LEN_MAX]={0};                  //�ݴ��һ�����������
		
		uint8_t FirstCodeOk=0;
		uint32_t VolFinishTime=0;
		
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		
		UartFace_PowerOff();
		
		InputIdx=0;
		memset(InputBuf,0,INPUT_MAX);
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please enter password":"����������"),BREAK);       
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Hash key to confirm":"ȷ���밴#�ż�"),UNBREAK);     //ȷ��
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);     //����

		LedVal.ledval=LED_MASK_ADDUSER_CODE;
	
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_ADDUSER_CODE;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_x:               //���� 
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit); 
												AudioPlayVoice(GetVolIndex("��"),BREAK);
																			
												if(InputIdx>0)
												{
														InputIdx--;
												}
												else 
												{
														MenuNow=Menu_KeyPadAddUsers; 
												}
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_c:
										if(KeyBk.Flag!=0)
										{
												InputBuf[InputIdx]=0;
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												if(InputIdx>7)              //�������
												{
														AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
														AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
												}
												else if(FirstCodeOk==0)    //��һ����������
												{
														memcpy(codeBk,InputBuf,InputIdx);
														printf("first code: %s",codeBk);
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please enter the password again":"������������"),UNBREAK);
														FirstCodeOk=1;
												}
												else
												{
														printf("second code: %s",InputBuf);
														if(strcmp((const char*)InputBuf,(const char*)codeBk)==0)
														{
																if(!CheckOnetimeCodeExist(codeBk))
																{
																		memcpy(OneTimeCode.OneCode,codeBk,InputIdx);
																		RTC_GetTime((tm*)&OneTimeCode.StartTime);
																		IntFlashWrite((uint8_t*)&OneTimeCode,ONETIME_CODE_ADDR,sizeof(OneTimeCode_t));
																		SysConfig.Bits.OTCode=1;
																		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),UNBREAK);       //���
																		MenuNow=Menu_KeyPadAddUsers; //�����ϲ�
																}
																else
																{
																		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add failed":"���ʧ��"),UNBREAK); 
																}
														}
														else
														{
																AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
																AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
														}
												}
												memset(InputBuf,0,InputIdx);
												InputIdx=0;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:
										if(KeyBk.Numb>='0' && KeyBk.Numb<='9')
										{
												if(KeyBk.Flag!=0)
												{
														LedVal.ledval&=~(1<<KeyBit);
														AudioPlayVoice(GetVolIndex("��"),BREAK);
														InputBuf[InputIdx++]=KeyBk.Numb;
														if(InputIdx>=INPUT_MAX)InputIdx=0;
												}
												else
												{
														LedVal.ledval|=(1<<KeyBit);
												}
										}
										break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //����������ɺ�10Sδ�������˳����˵�
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadAddUsers_OnetimeCode && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//��ӿ�Ƭ�Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadAddUsers_Card(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint8_t freeslot=0;
		uint8_t newStart=1;
		uint32_t addcardStime=GetTimerCount();
		CardStat_t addStat={0};
		InputIdx=0;
		
		UartFace_PowerOff();

		LedVal.ledval=LED_MASK_ADDUSER_CARD;           //����ʾ#�ż�
	
		while(1)
		{
				if(newStart==1 && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						freeslot=GetCardFreeSlot();

						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add card":"��ӿ�Ƭ"),BREAK);       //��ӿ�Ƭ        
						AudioPlayVoice(freeslot/10+GetVolIndex(SysConfig.Bits.Language?"zero":"0"),UNBREAK); 
						AudioPlayVoice(freeslot%10+GetVolIndex(SysConfig.Bits.Language?"zero":"0"),UNBREAK);
						
						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
						newStart=0;
				}
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_ADDUSER_CARD; 
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_x:                    //��������û�����
										if(KeyBk.Flag!=0)
										{
												printf("key x down\n");
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadAddUsers; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)     //����������ˢ�����и���  
				{
						if(addStat.StTime==0)                        //��ӿ�Ƭ300msɨһ��
						{
								addStat.StTime=GetTimerCount();
								switch(CardAddHandle(ADD_NORMAL))
								{
										case ADD_CARD_SUCESS:
										{
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),BREAK);
												if(RemoteAdd.type.card!=1)
												{
														addcardStime=GetTimerCount();
														newStart=1;
												}
												else
												{
														newStart=0;
														MenuNow=Menu_KeyPadNormal; 
														KeyPadMode=KEYPAD_NORMAL;
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),UNBREAK);
												}
												break;
										}
										case ADD_CARD_FULL:
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"card library is full":"��Ƭ������"),BREAK);
												MenuNow=Menu_KeyPadAddUsers;      //�����ϲ�
												break;
										case ADD_CARD_EXIST:
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"card already exists":"��Ƭ�Ѵ���"),BREAK); 
												break;
										case ADD_CARD_FAIL:
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add failed":"���ʧ��"),BREAK);
												break;
										default:break;
								}
						}
						else if(GetTimerElapse(addStat.StTime)>CARD_ADD_INTVAL)
						{
								addStat.StTime=0;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if(addcardStime>0 && GetTimerElapse(addcardStime)>30000)
				{
						printf("add card timeout\n");  
						addcardStime=0;  
						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Operation timed out":"������ʱ"),BREAK);
						MenuNow=Menu_KeyPadAddUsers; 
				}
				if(MenuNow!=Menu_KeyPadAddUsers_Card && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(RemoteAdd.type.all!=1)RemoteAdd.type.all=0;
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//���ָ���Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadAddUsers_Fp(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint8_t addRet=0;
		uint8_t addfinish=0;
		uint32_t addFpStime=GetTimerCount();
		uint32_t addIntval=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
	
		FpAdd.LdFpAddStat=FP_ADD_READ_USER;
		FpAdd.StTime=0;
		FpAdd.GenCharCnt=0;
		FpAdd.VolStime=0;
		FpAdd.AlreadGetIMG=0;
		FpAdd.ComRetry=0;
	
		LedVal.ledval=LED_MASK_ADDUSER_FP;             //����ʾ#�ż�
	
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_ADDUSER_FP; 
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_x:                    //��������û�����
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadAddUsers; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				if(addfinish==0)
				{
						if(addIntval==0 && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
						{
								addRet=FpAddHandle(RemoteAdd.type.fp==2?ADD_COERCE:ADD_NORMAL);
								if(addRet==0)
								{
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),UNBREAK);
										addfinish=0;
										if(RemoteAdd.type.fp==0)
										{
												addFpStime=GetTimerCount();
												FpAdd.LdFpAddStat=FP_ADD_READ_USER;
												FpAdd.StTime=0;
												FpAdd.GenCharCnt=0;
												FpAdd.VolStime=0;
												FpAdd.ComRetry=0;
										}
										else
										{
												addfinish=1;
												MenuNow=Menu_KeyPadNormal; 
												KeyPadMode=KEYPAD_NORMAL;
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),UNBREAK);
										}
								}
								else if(addRet==1)
								{
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add failed":"���ʧ��"),UNBREAK);
										addfinish=1;
										MenuNow=Menu_KeyPadAddUsers; 
								}
								addIntval=GetTimerCount();
						}
						else if(GetTimerElapse(addIntval)>50)         //���50MS������״̬��
						{
								addIntval=0;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if(addFpStime>0 && GetTimerElapse(addFpStime)>60000)
				{
						addFpStime=0;  
						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Operation timed out":"������ʱ"),BREAK);
						addfinish=1;
						MenuNow=Menu_KeyPadAddUsers; 
				}
				if(MenuNow!=Menu_KeyPadAddUsers_Fp && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(RemoteAdd.type.all!=1)RemoteAdd.type.all=0;
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//��������Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadAddUsers_Face(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint8_t addstart=1;
		uint8_t freeslot=0;
		uint32_t addFaceStime=GetTimerCount();
		uint8_t addintval=0;
		
		InputIdx=0;
	
		FaceAdd.StTime=0;
		FaceAdd.VolFristTime=0;
		FaceAdd.Retry=0;
		FaceAdd.AddStat=FACEADD_READUSER;
		//��λ����ģ�飺��Ϊ����ģ��ʶ��ʱ��Ƚϳ���������ʶ������ж��������������������
		FaceReady.Ready=0;
		UartFace_TxQueueReset();
		UartFace_PowerOn();
		
		LedVal.ledval=LED_MASK_ADDUSER_FACE;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_ADDUSER_FACE;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_x:                    //��������û�����
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadAddUsers; 
												addstart=0;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				if(addstart==1)
				{
						if(addintval==0 && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
						{
								addintval=GetTimerCount();
								switch(FaceAddHandle(ADD_NORMAL))
								{
										case OP_READY:
												freeslot=GetOrbFaceFreeSlot(); 
												if(freeslot>=ORB_FACE_MAX)
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Face library is full":"����������"),BREAK);
														MenuNow=Menu_KeyPadAddUsers; 
														addstart=0;
														break;
												}
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add face":"�������"),BREAK);                    //�������    
												AudioPlayVoice(freeslot/10+GetVolIndex(SysConfig.Bits.Language?"zero":"0"),UNBREAK); 
												AudioPlayVoice(freeslot%10+GetVolIndex(SysConfig.Bits.Language?"zero":"0"),UNBREAK);
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
												FaceAdd.AddStat=FACEADD_RUN;
												break;
										case OP_SUCCESS:
										{
												//������ʾ
												if((LPM_GetStopModeValue()&LPM_AUDIO_ID)!=0)break;
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),BREAK);
												
												LockAddUser(PERIP_FACE_INDEX,freeslot);												
												
												//��ʼ��һ�����
												if(RemoteAdd.type.face==0)
												{
														FaceAdd.StTime=0;
														FaceAdd.VolFristTime=0;
														FaceAdd.Retry=0;
														FaceAdd.AddStat=FACEADD_READUSER; 
														
														addFaceStime=GetTimerCount();   //�����һ�����¼�ʱ
														FaceReady.Ready=0;
												}
												else
												{
														MenuNow=Menu_KeyPadNormal; 
														addstart=0;
														KeyPadMode=KEYPAD_NORMAL;
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),UNBREAK);
												}
												break;
										}
										case OP_FAIL:
												if((LPM_GetStopModeValue()&LPM_AUDIO_ID)!=0)break;
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please look at the camera":"��ֱ������ͷ"),UNBREAK);       
												FaceAdd.AddStat=FACEADD_RUN;
												FaceAdd.StTime=0;
												break;
										case OP_TIMEOUT:
												FaceAdd.StTime=0;
												FaceAdd.VolFristTime=0;
												FaceAdd.Retry=0;
												FaceAdd.AddStat=FACEADD_READUSER;
												
												FaceReady.Ready=0;
												UartFace_TxQueueReset();
												UartFace_PowerReset();
												break;
										default:break;
								}
						}
						else if(GetTimerElapse(addintval)>50)
						{
								addintval=0;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if(addFaceStime>0 && GetTimerElapse(addFaceStime)>50000)
				{
						printf("add face timeout\n");  
						addFaceStime=0;  
						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Operation timed out":"������ʱ"),BREAK);
						MenuNow=Menu_KeyPadAddUsers; 
				}
				if(MenuNow!=Menu_KeyPadAddUsers_Face && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(RemoteAdd.type.all!=1)RemoteAdd.type.all=0;
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//�޸Ĺ���Ա�û�ѡ�����
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadModifyAdmUsers(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
	
#ifndef NO_FACE_MODE		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Change administer face":"�޸Ĺ�������"),BREAK);     //�޸�
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"one":"1"),UNBREAK);     //1
#endif
#ifndef NO_FACE_MODE
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Change administer fingerprint":"�޸Ĺ���ָ��"),UNBREAK);     //�޸�
#else
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Change administer fingerprint":"�޸Ĺ���ָ��"),BREAK);     //�޸�
#endif	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);    //2
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Change administer password":"�޸Ĺ�������"),UNBREAK);     //�޸�
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"three":"3"),UNBREAK);    //3
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Change administer card":"�޸Ĺ���Ƭ"),UNBREAK);     //�޸�
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"four":"4"),UNBREAK);     //4
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
				
	
		LedVal.ledval=LED_MASK_MODIFY_ADMIN;
	
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_MODIFY_ADMIN;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
#ifndef NO_FACE_MODE	
								case KEY_1:               //�޸Ĺ���Ա����
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
												MenuNow=Menu_KeyPadModifyAdmUsers_Face;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
#endif
								case KEY_2:               //�޸Ĺ���Աָ��
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
												MenuNow=Menu_KeyPadModifyAdmUsers_Fp;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_3:               //�޸Ĺ���Ա����
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
												MenuNow=Menu_KeyPadModifyAdmUsers_Password;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_4:               //�޸Ĺ���Ա��
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
												MenuNow=Menu_KeyPadModifyAdmUsers_Card;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //���� 
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit); 
												MenuNow=Menu_KeyPadConfigSelect;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //�����������
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadModifyAdmUsers && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//�޸Ĺ���Աָ���Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadModifyAdmUsers_Fp(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint8_t addRet=0;
		uint8_t addfinish=0;
		uint32_t addFpStime=GetTimerCount();
		uint32_t addIntval=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
	
		FpAdd.LdFpAddStat=FP_ADD_READ_USER;
		FpAdd.StTime=0;
		FpAdd.GenCharCnt=0;
		FpAdd.VolStime=0;
		FpAdd.AlreadGetIMG=0;
		FpAdd.ComRetry=0;
	
		LedVal.ledval=LED_MASK_ADDUSER_FP;             //����ʾ#�ż�
	
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_ADDUSER_FP; 
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_x:                    //��������û�����
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadModifyAdmUsers; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				if(addfinish==0)                 //�����ɱ�־
				{
						if(addIntval==0)
						{
								addRet=FpAddHandle(ADD_ADMIN);
								if(addRet==0)
								{
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),UNBREAK);
										addfinish=1;
										MenuNow=Menu_KeyPadModifyAdmUsers; 
								}
								else if(addRet==1)
								{
										AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add failed":"���ʧ��"),UNBREAK);
										addfinish=1;
										MenuNow=Menu_KeyPadModifyAdmUsers; 
								}
								addIntval=GetTimerCount();
						}
						else if(GetTimerElapse(addIntval)>50)  //���50MS������״̬��
						{
								addIntval=0;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if(addFpStime>0 && GetTimerElapse(addFpStime)>60000)
				{
						addFpStime=0;  
						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Operation timed out":"������ʱ"),BREAK);
						MenuNow=Menu_KeyPadModifyAdmUsers; 
				}
				if(MenuNow!=Menu_KeyPadModifyAdmUsers_Fp && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)break;
		}
}
/******************************************************************************/
/*
//�޸Ĺ���Ա��Ƭ�Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadModifyAdmUsers_Card(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t addcardStime=GetTimerCount();
		CardStat_t addStat={0};
		InputIdx=0;
		
		UartFace_PowerOff();
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add card":"��ӿ�Ƭ"),BREAK);       //��ӿ�Ƭ        
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"zero":"0"),UNBREAK); 
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"zero":"0"),UNBREAK);
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
		
		LedVal.ledval=LED_MASK_ADDUSER_CARD;           //����ʾ#�ż�
	
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_ADDUSER_CARD; 
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_x:                    //��������û�����
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadModifyAdmUsers; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)     //����������ˢ�����и���  
				{
						if(addStat.StTime==0)
						{
								addStat.StTime=GetTimerCount();
								switch(CardAddHandle(ADD_ADMIN))
								{
										case ADD_CARD_SUCESS:
										case ADD_CARD_EXIST:
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),BREAK);
												MenuNow=Menu_KeyPadModifyAdmUsers;      //�����ϲ�
												break;
										case ADD_CARD_FULL:
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"card library is full":"��Ƭ������"),BREAK);
												MenuNow=Menu_KeyPadModifyAdmUsers;      //�����ϲ�
												break;
										case ADD_CARD_FAIL:
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add failed":"���ʧ��"),BREAK);
												break;
										default:break;
								}
						}
						else if(GetTimerElapse(addStat.StTime)>CARD_ADD_INTVAL)
						{
								addStat.StTime=0;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if(addcardStime>0 && GetTimerElapse(addcardStime)>30000)
				{
						printf("add card timeout\n");  
						addcardStime=0;  
						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Operation timed out":"������ʱ"),BREAK);
						MenuNow=Menu_KeyPadModifyAdmUsers; 
				}
				if(MenuNow!=Menu_KeyPadModifyAdmUsers_Card && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)break;
		}
}
/******************************************************************************/
/*
//�޸Ĺ���Ա�����Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadModifyAdmUsers_Password(void)
{
		uint8_t codeBk[PWD_LEN_MAX]={0};                  //�ݴ��һ�����������
		
		uint8_t FirstCodeOk=0;
		uint8_t chkok=0,i=0;
		uint32_t VolFinishTime=0;
		
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		
		InputIdx=0;
		memset(InputBuf,0,INPUT_MAX);
		
		UartFace_PowerOff();
		

		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please enter password":"����������"),BREAK);      //����
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Hash key to confirm":"ȷ���밴#�ż�"),UNBREAK);
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
		
		LedVal.ledval=LED_MASK_ADDUSER_CODE;
	
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_ADDUSER_CODE;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_x:               //���� 
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit); 
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												if(InputIdx>0)
												{
														InputBuf[--InputIdx]=0;
												}
												else 
												{
														MenuNow=Menu_KeyPadModifyAdmUsers; 
												}
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_c:
										if(KeyBk.Flag!=0)
										{
												InputBuf[InputIdx]=0;
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												if(InputIdx>PWD_LEN_MAX-1)              //�������
												{
														AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
														AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
												}
												else if(FirstCodeOk==0)    //��һ����������
												{
														memcpy(codeBk,InputBuf,InputIdx);
														printf("first code: %s",codeBk);
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please enter the password again":"������������"),UNBREAK);
														FirstCodeOk=1;
												}
												else
												{
														printf("second code: %s",InputBuf);
														for(i=0;i<strlen((char*)codeBk);i++)
														{
																if(codeBk[i]!=InputBuf[i])break;
														}
														if(i<strlen((char*)codeBk))
														{
																AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
																AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
														}
														else
														{
																chkok=1;
														}
												}
												memset(InputBuf,0,INPUT_MAX);
												InputIdx=0;
												
												if(chkok==0)break;
												switch(AddPassWord(ADMIN,codeBk,NULL,NULL))
												{
														case PWD_ADD_SUCESS:
														{
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),UNBREAK);
																MenuNow=Menu_KeyPadModifyAdmUsers; //�����ϲ�
														}
																break;
														case PWD_ADD_MEM_FULL:
														case PWD_ADD_TIME_MEM_FULL:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Password library is full":"���������"),UNBREAK);
																MenuNow=Menu_KeyPadModifyAdmUsers;
																break;
														case PWD_ADD_CODE_EXIST:
														case PWD_ADD_TYP_WRONG:
																AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add failed":"���ʧ��"),UNBREAK);
																break;
												}
												chkok=0;
												FirstCodeOk=0;
												memset(codeBk,0,PWD_LEN_MAX);
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:
										if(KeyBk.Numb>='0' && KeyBk.Numb<='9')
										{
												if(KeyBk.Flag!=0)
												{
														LedVal.ledval&=~(1<<KeyBit);
														AudioPlayVoice(GetVolIndex("��"),BREAK);
														InputBuf[InputIdx++]=KeyBk.Numb;
														if(InputIdx>=INPUT_MAX)InputIdx=0;
												}
												else
												{
														LedVal.ledval|=(1<<KeyBit);
												}
										}
										break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //����������ɺ�10Sδ�������˳����˵�
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								MenuNow=Menu_KeyPadModifyAdmUsers;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Operation timed out":"������ʱ"),BREAK);
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadModifyAdmUsers_Password && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)break;
		}
}
/******************************************************************************/
/*
//�޸Ĺ���Ա�����ӽ���
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadModifyAdmUsers_Face(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint8_t admfaceDelok=0;
		uint32_t addFaceStime=GetTimerCount();
		uint32_t faceintval=0;
		
		InputIdx=0;
		
		FaceReady.Ready=0;
		UartFace_TxQueueReset();
		UartFace_PowerOn();
	
		FaceDel.DelStat=FACEDEL_READUSER;
		FaceDel.Retry=0;
		FaceDel.StTime=0;
	
		FaceAdd.StTime=0;
		FaceAdd.VolFristTime=0;
		FaceAdd.Retry=0;
		FaceAdd.AddStat=FACEADD_READUSER;
	
		LedVal.ledval=LED_MASK_ADDUSER_FACE;
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_ADDUSER_FACE;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_x:                    //��������û�����
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadModifyAdmUsers; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				if(admfaceDelok==0)
				{
						if(faceintval==0)
						{
								switch(FaceDelHandle(0))
								{
										case OP_SUCCESS:
												admfaceDelok=1;
												faceintval=0;
												break;
										case OP_FAIL:
												FaceDel.DelStat=FACEDEL_RUN;
												FaceDel.StTime=0;
												FaceDel.Retry=0;
												break;
										case OP_TIMEOUT:
												FaceReady.Ready=0;
												UartFace_TxQueueReset();
												UartFace_PowerReset();
												FaceDel.DelStat=FACEDEL_READUSER;
												FaceDel.Retry=0;
												FaceDel.StTime=0;
												break;
								}
								faceintval=GetTimerCount();
						}
						else if(GetTimerElapse(faceintval)>100)
						{
								faceintval=0;
						}
				}
				else if(admfaceDelok==1)
				{
						if(faceintval==0)
						{
								switch(FaceAddHandle(ADD_ADMIN))
								{
										case OP_READY:
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please look at the camera":"��ֱ������ͷ"),UNBREAK);  
												FaceAdd.AddStat=FACEADD_RUN;
												break;
										case OP_SUCCESS:
												//������ʾ
												if((LPM_GetStopModeValue()&LPM_AUDIO_ID)!=0)break;
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add succeed":"��ӳɹ�"),BREAK);
												admfaceDelok=2;
												MenuNow=Menu_KeyPadModifyAdmUsers; 
												break;
										case OP_FAIL:
												if((LPM_GetStopModeValue()&LPM_AUDIO_ID)!=0)break;
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please look at the camera":"��ֱ������ͷ"),UNBREAK);       
												FaceAdd.AddStat=FACEADD_RUN;
												break;
										case OP_TIMEOUT:
												if((LPM_GetStopModeValue()&LPM_AUDIO_ID)!=0)break;
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Operation timed out":"������ʱ"),BREAK);
												admfaceDelok=2;
												MenuNow=Menu_KeyPadModifyAdmUsers;
												break;
										default:break;
								}
								faceintval=GetTimerCount();
						}
						else if(GetTimerElapse(faceintval)>100)
						{
								faceintval=0;
						}
				}
				Key_Scan();
				Uart_RtxFrameHandle();
				Audio_RunningHandle();
				Led_UpdateVal();
				if(addFaceStime>0 && GetTimerElapse(addFaceStime)>50000)
				{
						printf("add face timeout\n");  
						addFaceStime=0;  
						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Operation timed out":"������ʱ"),BREAK);
						MenuNow=Menu_KeyPadModifyAdmUsers; 
				}
				if(MenuNow!=Menu_KeyPadModifyAdmUsers_Face && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)break;
		}
}
/******************************************************************************/
/*
//ɾ���û�ѡ��˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadDelUsers(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
	
		UartFace_PowerOff();
	
		InputIdx=0;
#ifndef NO_FACE_MODE		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete face":"ɾ������"),BREAK);      //ɾ������
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"one":"1"),UNBREAK);     //1
#endif
#ifndef NO_FACE_MODE	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete fingerprint":"ɾ��ָ��"),UNBREAK);    //ɾ��ָ�� 
#else
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete fingerprint":"ɾ��ָ��"),BREAK);    //ɾ��ָ�� 
#endif
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);    //2
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete password":"ɾ������"),UNBREAK);    //ɾ������ 
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);    //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"three":"3"),UNBREAK);    //3
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete card":"ɾ����Ƭ"),UNBREAK);     //ɾ����Ƭ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"four":"4"),UNBREAK);     //4
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete one-time password":"ɾ��һ��������"),UNBREAK);     //ɾ��
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"five":"5"),UNBREAK);     //5
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete all users":"ɾ�������û�"),UNBREAK);     //ɾ�������û�
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"six":"6"),UNBREAK);     //6
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
	
		LedVal.ledval=LED_MASK_DELUSER;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_DELUSER;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
#ifndef NO_FACE_MODE	
								case KEY_1:               //ɾ�������û�
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadDelUsers_Face; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
#endif
								case KEY_2:               //ɾ��ָ���û�
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadDelUsers_Fp; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_3:               //ɾ�������û�
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadDelUsers_Code; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_4:               //ɾ�����û� 
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadDelUsers_Card; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_5:               //ɾ��һ��������
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadDelUsers_OnetimeCode;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_6:               //ɾ�������û�
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadDelUsers_All;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadConfigSelect; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //�����������
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadDelUsers && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//ɾ��һ���������Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadDelUsers_OnetimeCode(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
	
		UartFace_PowerOff();
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Hash key to confirm":"ȷ���밴#�ż�"),BREAK);
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
		
		LedVal.ledval=LED_MASK_DELUSER_ONETIMECODE;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_DELUSER_ONETIMECODE;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_c:               //ȷ��
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete succeed":"ɾ���ɹ�"),UNBREAK);
												
												memset((uint8_t*)&OneTimeCode,0xFF,sizeof(OneTimeCode_t));  
												IntFlashWrite((uint8_t*)&OneTimeCode,ONETIME_CODE_ADDR,sizeof(OneTimeCode_t));
												SysConfig.Bits.OTCode=0;
												MenuNow=Menu_KeyPadDelUsers;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadDelUsers;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:
										break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //����������ɺ�10Sδ�������˳����˵�
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadDelUsers_OnetimeCode && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//ɾ�������Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadDelUsers_Face(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t id=0,i=0,j=0;
		uint32_t VolFinishTime=0;
		uint8_t DelId=0xFF;
		uint32_t delintval=0;
		uint8_t remoteDel=0;
		
		InputIdx=0;
	
		if(RemoteDel.type.face==1)
		{
				DelId=RemoteDel.id;
				RemoteDel.type.face=0;	
				remoteDel=1;
		}
		else
		{
				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please enter face user number":"�����������û����"),BREAK);        //������
				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Hash key to confirm":"ȷ���밴#�ż�"),UNBREAK);
				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
		}
	
		LedVal.ledval=LED_MASK_DELUSER_FACE;
		
		//��λ����ģ�飺��Ϊ����ģ��ʶ��ʱ��Ƚϳ���������ʶ������ж��������������������
		UartFace_TxQueueReset();
		
		FaceReady.Ready=0;

		UartFace_PowerOn();
		
		FaceDel.DelStat=FACEDEL_READUSER;
		FaceDel.StTime=0;
		FaceDel.Retry=0;
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_DELUSER_FACE;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_c:               //ȷ��
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												InputBuf[InputIdx]=0;
												printf("input str:%s \n",InputBuf);
												id=0;
												for(i=InputIdx,j=0;i>0;i--,j++)
												{
														id+=(InputBuf[j]-'0')*pow(10,i-1);
												}
												InputIdx=0;
												printf("del num:%02d \n",id);
												if(id<100)           //0-99
												{
														DelId=id;
												}
												else
												{
														AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
														AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
												}
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadDelUsers;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:
										if(KeyBk.Numb>='0' && KeyBk.Numb<='9')
										{
												if(KeyBk.Flag!=0)
												{
														LedVal.ledval&=~(1<<KeyBit);
														AudioPlayVoice(GetVolIndex("��"),BREAK);
														InputBuf[InputIdx++]=KeyBk.Numb;
														if(InputIdx>=INPUT_MAX)InputIdx=0;
												}
												else
												{
														LedVal.ledval|=(1<<KeyBit);
												}
										}
										break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				//ɾ����ID
				if(DelId!=0xFF)
				{
						if(delintval==0)
						{
								delintval=GetTimerCount();
								switch(FaceDelHandle(DelId))
								{
										case OP_SUCCESS:
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete succeed":"ɾ���ɹ�"),BREAK);
												DelId=0xFF;
												if(remoteDel==1)
												{
														KeyPadMode=KEYPAD_NORMAL;
														MenuNow=Menu_KeyPadNormal;
												}
												else
												{
														MenuNow=Menu_KeyPadDelUsers;
												}
												break;
										case OP_FAIL:
												FaceDel.DelStat=FACEDEL_RUN;
												FaceDel.StTime=0;
												FaceDel.Retry=0;
												break;
										case OP_TIMEOUT:
												FaceReady.Ready=0;
												UartFace_PowerReset();
												UartFace_TxQueueReset();
												FaceDel.DelStat=FACEDEL_READUSER;
												FaceDel.Retry=0;
												FaceDel.StTime=0;
												break;
								}
						}
						else if(GetTimerElapse(delintval)>50)
						{
								delintval=0;
						}
				}
				//����������ɺ�10Sδ�������˳����˵�
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadDelUsers_Face && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//ɾ��ָ���Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadDelUsers_Fp(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t id=0,i=0,j=0;
		uint8_t DelId=0xFF,DelRet=0;
		uint8_t remoteDel=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;

		UartFace_PowerOff();
		
		if(RemoteDel.type.fp==1)
		{
				DelId=RemoteDel.id;
				RemoteDel.type.fp=0;
				remoteDel=1;
		}
		else
		{
				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please enter fingerprint user number":"������ָ���û����"),BREAK);        //������
				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Hash key to confirm":"ȷ���밴#�ż�"),UNBREAK);
				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
		}
		
		LedVal.ledval=LED_MASK_DELUSER_FP;
		
		FpDel.LdFpDelStat=FP_DEL_READ_USER;
		FpDel.StTime=0;
		FpDel.ComRetry=0;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_DELUSER_FP;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_c:               //ȷ��
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												InputBuf[InputIdx]=0;
												printf("input str:%s \n",InputBuf);
												id=0;
												for(i=InputIdx,j=0;i>0;i--,j++)
												{
														id+=(InputBuf[j]-'0')*pow(10,i-1);
												}
												InputIdx=0;
												printf("del num:%02d \n",id);
												if(id<100)        //0-99,����Ա�û�0�е��������޸�
												{
														DelId=id;
												}
												else
												{
														AudioPlayVoice(GetVolIndex("��"),BREAK);
														AudioPlayVoice(GetVolIndex("��"),UNBREAK);
												}
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadDelUsers;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:
										if(KeyBk.Numb>='0' && KeyBk.Numb<='9')
										{
												if(KeyBk.Flag!=0)
												{
														LedVal.ledval&=~(1<<KeyBit);
														AudioPlayVoice(GetVolIndex("��"),BREAK);
														InputBuf[InputIdx++]=KeyBk.Numb;
														if(InputIdx>=INPUT_MAX)InputIdx=0;
												}
												else
												{
														LedVal.ledval|=(1<<KeyBit);
												}
										}
										break;
						}
				}
				if(DelId!=0xFF)
				{
						DelRet=FpDelHandle(DelId);
						if(DelRet==FP_SUCCESS)
						{
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete succeed":"ɾ���ɹ�"),BREAK);
							
								DelId=0xFF;
								if(remoteDel==1)
								{
										KeyPadMode=KEYPAD_NORMAL;
										MenuNow=Menu_KeyPadNormal;
								}
								else
								{
										MenuNow=Menu_KeyPadDelUsers;
								}
						}
						if(DelRet==FP_FAIL)
						{
								DelId=0xFF;
								if(remoteDel==1)
								{
										KeyPadMode=KEYPAD_NORMAL;
										MenuNow=Menu_KeyPadNormal;
								}
								else
								{
										MenuNow=Menu_KeyPadDelUsers;
								}
						}
				}
				Key_Scan();
				Uart_RtxFrameHandle();
				Audio_RunningHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //����������ɺ�10Sδ�������˳����˵�
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadDelUsers_Fp && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//ɾ�������Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadDelUsers_Code(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t id=0,i=0,j=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please enter password user number":"�����������û����"),BREAK);        //������
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Hash key to confirm":"ȷ���밴#�ż�"),UNBREAK);
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
	
		LedVal.ledval=LED_MASK_DELUSER_CODE;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_DELUSER_CODE;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_c:               //ȷ��
										if(KeyBk.Flag!=0)
										{
												uint8_t ret=0;
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												InputBuf[InputIdx]=0;
												printf("input str:%s \n",InputBuf);
												id=0;
												for(i=InputIdx,j=0;i>0;i--,j++)
												{
														id+=(InputBuf[j]-'0')*pow(10,i-1);
												}
												InputIdx=0;
												printf("del num:%02d \n",id);
												ret=DelPassWord(id);
												if(ret==PWD_DEL_SUCCESS || PWD_DEL_CODE_NOT_EXIST)
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete succeed":"ɾ���ɹ�"),UNBREAK);
												}
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit);
												MenuNow=Menu_KeyPadDelUsers;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:
										if(KeyBk.Numb>='0' && KeyBk.Numb<='9')
										{
												if(KeyBk.Flag!=0)
												{
														LedVal.ledval&=~(1<<KeyBit);
														AudioPlayVoice(GetVolIndex("��"),BREAK);
														InputBuf[InputIdx++]=KeyBk.Numb;
														if(InputIdx>=INPUT_MAX)InputIdx=0;
												}
												else
												{
														LedVal.ledval|=(1<<KeyBit);
												}
										}
										break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //����������ɺ�10Sδ�������˳����˵�
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadDelUsers_Code && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//ɾ����Ƭ�Ӳ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadDelUsers_Card(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t id=0,i=0,j=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please enter card user number":"�����뿨�û����"),BREAK);        //������
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Hash key to confirm":"ȷ���밴#�ż�"),UNBREAK);
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
	
		LedVal.ledval=LED_MASK_DELUSER_CARD;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_DELUSER_CARD;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_c:               //ȷ��
										if(KeyBk.Flag!=0)
										{
												uint8_t ret=0;
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit);
												InputBuf[InputIdx]=0;
												printf("input str:%s \n",InputBuf);
												id=0;
												for(i=InputIdx,j=0;i>0;i--,j++)
												{
														id+=(InputBuf[j]-'0')*pow(10,i-1);
												}
												InputIdx=0;
												printf("del num:%02d \n",id);
												ret=DelCard(id);
												
												if(ret==DEL_CARD_SUCCESS || ret==DEL_CARD_NOEXIST)
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete succeed":"ɾ���ɹ�"),UNBREAK);
												}
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												LedVal.ledval&=~(1<<KeyBit);
												MenuNow=Menu_KeyPadDelUsers;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:
										if(KeyBk.Numb>='0' && KeyBk.Numb<='9')
										{
												if(KeyBk.Flag!=0)
												{
														LedVal.ledval&=~(1<<KeyBit);
														AudioPlayVoice(GetVolIndex("��"),BREAK);
														InputBuf[InputIdx++]=KeyBk.Numb;
														if(InputIdx>=INPUT_MAX)InputIdx=0;
												}
												else
												{
														LedVal.ledval|=(1<<KeyBit);
												}
										}
										break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //����������ɺ�10Sδ�������˳����˵�
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadDelUsers_Card && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//ɾ�������û��˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadDelUsers_All(void)          //ɾ�������룬��Ƭ��ָ�ƣ�����
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
		uint8_t DeFpOk=0,DeFaceOk=0;
		uint8_t DelConfirm=0;
		uint8_t remoteDel=0;
		InputIdx=0;
		
		if(RemoteDel.type.all==1)
		{
				DelConfirm=1;
				DelPassWord(0xFF);       //ɾ������
				DelCard(0xFF);           //ɾ����Ƭ
				RemoteDel.type.all=0;
				remoteDel=1;
		}
		else
		{
				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Hash key to confirm":"ȷ���밴#�ż�"),UNBREAK);
				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
		}
		
		//��λ����ģ�飺��Ϊ����ģ��ʶ��ʱ��Ƚϳ���������ʶ������ж��������������������   
#ifndef NO_FACE_MODE	
		FaceReady.Ready=0;
		UartFace_PowerOn();
		UartFace_TxQueueReset();
		FaceDel.DelStat=FACEDEL_READUSER;
#endif	 
		LedVal.ledval=LED_MASK_DELUSER_ALL;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_DELUSER_ALL;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_c:               //ȷ��
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												DelConfirm=1;
												DelPassWord(0xFF);       //ɾ������
												DelCard(0xFF);           //ɾ����Ƭ
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadDelUsers;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:
										break;
						}
				}
				if(DelConfirm==1)
				{
						if(DeFpOk==0)
						{
								if(FpDelHandle(0xFF)==FP_SUCCESS)
								{
										DeFpOk=1;
								}
						}
#ifndef NO_FACE_MODE	
						if(DeFaceOk==0)
						{
								switch(FaceDelHandle(0xFF))
								{
										case OP_SUCCESS:
												printf("face del ok\n");
												DeFaceOk=1;
												break;
										case OP_FAIL:
												FaceDel.DelStat=FACEDEL_RUN;
												FaceDel.StTime=0;
												FaceDel.Retry=0;
												break;
										case OP_TIMEOUT:
												FaceReady.Ready=0;
												UartFace_PowerReset();
												UartFace_TxQueueReset();
												FaceDel.DelStat=FACEDEL_READUSER;
												FaceDel.Retry=0;
												FaceDel.StTime=0;
												break;
								}
						}
#endif
						if(
#ifndef NO_FACE_MODE	
							DeFaceOk==1 && 
#endif							
							DeFpOk==1)
						{
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete succeed":"ɾ���ɹ�"),BREAK);
								
								if(remoteDel==1)
								{
										KeyPadMode=KEYPAD_NORMAL;
										MenuNow=Menu_KeyPadNormal;
								}
								else
								{
										MenuNow=Menu_KeyPadDelUsers;
								}
								DeFaceOk=0;
								DeFpOk=0;
								DelConfirm=0;
						}
				}
				Key_Scan();
				Uart_RtxFrameHandle();
				Audio_RunningHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //����������ɺ�10Sδ�������˳����˵�
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadDelUsers_All && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}

/******************************************************************************/
/*
//ɾ��ָ���û���������ָ�ƣ���������Ƭ�����룬ÿ�����5��
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadDelUsers_select(void)          
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t delStartTime=0;
		uint8_t DeSum=0;
		uint8_t idx=0;
		InputIdx=0;
		
		FaceReady.Ready=0;
		UartFace_PowerOn();
		UartFace_TxQueueReset();
		FaceDel.DelStat=FACEDEL_READUSER;
		LedVal.ledval=LED_MASK_WAKE;
		Led_UpdateVal();
		while(1)
		{
				for(;idx<RemoteDelSelect.sum;idx++)  
				{
						if(RemoteDelSelect.id[idx].u8[0]==0x02)  //ָ��
						{
								if(FpDelHandle(RemoteDelSelect.id[idx].u8[1])==FP_SUCCESS)
								{
										RemoteDelSelect.id[idx].u8[0]=0xFF;    //�ɹ�����ʱ
										DeSum++;
								}
								if(delStartTime==0)
								{
										delStartTime=GetTimerCount();
								}							
								else if(GetTimerElapse(delStartTime)>5000)
								{
										delStartTime=0;
										RemoteDelSelect.id[idx].u8[0]=0xFF;    //�ɹ�����ʱ
								}
								break;
						}
						else if(RemoteDelSelect.id[idx].u8[0]==0x04)  //��Ƭ
						{
								DelCard(RemoteDelSelect.id[idx].u8[1]);           //ɾ����Ƭ
								RemoteDelSelect.id[idx].u8[0]=0xFF;
								DeSum++;
						}
						else if(RemoteDelSelect.id[idx].u8[0]==0x01)  //����
						{
								if(FaceDelHandle(RemoteDelSelect.id[idx].u8[1])==OP_SUCCESS)
								{
										RemoteDelSelect.id[idx].u8[0]=0xFF;
										DeSum++;
								}
								if(delStartTime==0)
								{
										delStartTime=GetTimerCount();
								}							
								else if(GetTimerElapse(delStartTime)>5000)
								{
										delStartTime=0;
										RemoteDelSelect.id[idx].u8[0]=0xFF;    //�ɹ�����ʱ
								}
								break;
						}
						else if(RemoteDelSelect.id[idx].u8[0]==0x03)   //����
						{
								DelPassWord(RemoteDelSelect.id[idx].u8[1]);       //ɾ������
								RemoteDelSelect.id[idx].u8[0]=0xFF;
								DeSum++;
						}
						else             //��Ч�������һ��
						{
								continue;
						}
				}
				if(idx==RemoteDelSelect.sum)
				{
						if(DeSum>=RemoteDelSelect.sum)  //ɾ�����
						{
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete succeed":"ɾ���ɹ�"),BREAK);
						}
						else
						{
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Operation timed out":"������ʱ"),BREAK);
						}
						KeyPadMode=KEYPAD_NORMAL;
						MenuNow=Menu_KeyPadNormal;
						idx=0xFF;
				}
				Key_Scan();
				Uart_RtxFrameHandle();
				Audio_RunningHandle();
				if(MenuNow!=Menu_KeyPadDelUsers_select && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}


/******************************************************************************/
/*
//�������ý���
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadConfNet(void) 
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Add network":"�������"),BREAK);     //�������
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"one":"1"),UNBREAK);     //1
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Delete network":"ɾ������"),UNBREAK);     //ɾ������
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);     //2
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
			
		LedVal.ledval=LED_MASK_NETCONF;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_NETCONF;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_1:               //�������
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												uint8_t temp=1;
												UartBack_SendCmdData(CONFIG_NET,&temp,1,1,1000);  
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_2:               //ɾ������
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												uint8_t temp=2;
												UartBack_SendCmdData(CONFIG_NET,&temp,1,1,1000);
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadConfigSelect;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //����������ɺ�10Sδ�������˳����˵�
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadConfNet && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//ϵͳ���ý���
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadSysConf(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Advanced security settings":"�߼���ȫ����"),BREAK);       
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"one":"1"),UNBREAK);     //1
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"volume settings":"��������"),UNBREAK);     //��������
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);     //2

		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Ultra power saving mode":"��ʡ��ģʽ"),UNBREAK);     //��ʡ��ģʽ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"three":"3"),UNBREAK);     //4
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"keep open mode":"����ģʽ"),UNBREAK);     //��ʡ��ģʽ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"four":"4"),UNBREAK);     //4

		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Human body induction settings":"�����Ӧ����"),UNBREAK);     //��ʡ��ģʽ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"five":"5"),UNBREAK);     //4
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Serial number query":"���кŲ�ѯ"),UNBREAK);     //SN�Ų�ѯ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"six":"6"),UNBREAK);     //5
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
				
		LedVal.ledval=LED_MASK_SYSCONF;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_SYSCONF;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_1:               //�߼���ȫģʽ
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadSysConf_Security;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_2:               //��������
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadSysConf_Volume;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_3:               //��ʡ��ģʽ 
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadSysConf_PowerSave;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_4:               //����ģʽ
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadSysConf_KeepOpen;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_5:               //PIR����
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadSysConf_PirSet;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_6:               //SN�Ų�ѯ
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												AudioPlayNumString(SN);
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadConfigSelect;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //�����������
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadSysConf && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//˫��֤ģʽ���ý���
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadSysConf_Security(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime;
	
		InputIdx=0;
	
		UartFace_PowerOff();
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn on two-factor authentication":"����˫�ؿ���"),BREAK);     
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"one":"1"),UNBREAK);      //1
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn off two-factor authentication":"�ر�˫�ؿ���"),UNBREAK);     //�ر�
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);     //2
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
				
		LedVal.ledval=LED_MASK_SYSCONF_SECURITY;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_SYSCONF_SECURITY;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_1:               //����˫����֤
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												if(SysConfig.Bits.MutiIdy!=1)
												{
														SysConfig.Bits.MutiIdy=1;
												}
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn on two-factor authentication":"����˫�ؿ���"),BREAK); 
												MenuNow=Menu_KeyPadSysConf;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_2:               //�ر�˫����֤
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												if(SysConfig.Bits.MutiIdy!=0)
												{
														SysConfig.Bits.MutiIdy=0;
												}
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn off two-factor authentication":"�ر�˫�ؿ���"),BREAK); 
												MenuNow=Menu_KeyPadSysConf;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
								case KEY_c:               //ȷ��
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadSysConf;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //�����������
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadSysConf_Security && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//�������ò˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadSysConf_Volume(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();

		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Low volume":"������"),BREAK);       //������
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"one":"1"),UNBREAK);      //1
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"High volume":"������"),UNBREAK);     //������
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);     //2
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Mute":"����"),UNBREAK);     //����
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"three":"3"),UNBREAK);     //3
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
				
		LedVal.ledval=LED_MASK_SYSCONF_VOLUME;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_SYSCONF_VOLUME;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_1:               //������
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
											
												if(SysConfig.Bits.Volume!=1)
												{
														SysConfig.Bits.Volume=1;
														Audio_SetVolume(SysConfig.Bits.Volume);
												}
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Low volume":"������"),BREAK);  
												MenuNow=Menu_KeyPadSysConf;
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_2:               //������
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												if(SysConfig.Bits.Volume!=2)
												{
														SysConfig.Bits.Volume=2;
														Audio_SetVolume(SysConfig.Bits.Volume);
												}
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"High volume":"������"),BREAK); 
												MenuNow=Menu_KeyPadSysConf; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_3:               //����:ֻ���ڷ����ý���
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioHalSetVolume(5); 
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Mute":"����"),BREAK);  
												if(SysConfig.Bits.Volume!=0)
												{
														SysConfig.Bits.Volume=0;
												}
												MenuNow=Menu_KeyPadSysConf; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK); 
												MenuNow=Menu_KeyPadSysConf; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //�����������
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadSysConf_Volume && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)   //����������ɲŷ���
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//�������ò˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadSysConf_Language(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();

//		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"����":"Chinese"),BREAK);       
//		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"�밴":"Please press"),UNBREAK);     
//		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"1":"one"),UNBREAK);      
//		
//		AudioPlayVoice(GetVolIndex("English"),UNBREAK);     
//		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);       
//		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);     
	
		AudioPlayVoice(GetVolIndex("����"),BREAK);       
		AudioPlayVoice(GetVolIndex("�밴"),UNBREAK);     
		AudioPlayVoice(GetVolIndex("1"),UNBREAK);      
		
		AudioPlayVoice(GetVolIndex("English"),UNBREAK);     
		AudioPlayVoice(GetVolIndex("Please press"),UNBREAK);       
		AudioPlayVoice(GetVolIndex("two"),UNBREAK);  
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
				
		LedVal.ledval=LED_MASK_SYSCONF_LANGUAGE;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_SYSCONF_LANGUAGE;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_1:               //����
								case KEY_2:               //Ӣ��
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												SysConfig.Bits.Language=((KeyBk.Numb==KEY_1)?0:1);
												MenuNow=Menu_KeyPadConfigSelect; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadConfigSelect; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //�����������
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadSysConf_Language && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//��ʡ��ģʽ���ò˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadSysConf_PowerSave(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn On":"��"),BREAK);       //����
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Ultra power saving mode":"��ʡ��ģʽ"),UNBREAK);     //��ʡ��ģʽ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"one":"1"),UNBREAK);      //1
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn Off":"�ر�"),UNBREAK);     //�ر�
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Ultra power saving mode":"��ʡ��ģʽ"),UNBREAK);     //��ʡ��ģʽ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);     //2
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
				
		LedVal.ledval=LED_MASK_SYSCONF_POWERSAVE;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_SYSCONF_POWERSAVE;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_1:               //����
								case KEY_2:               //Ӣ��
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												SysConfig.Bits.SupPowSave=(KeyBk.Numb==KEY_1)?1:0;
												uint8_t temp=SysConfig.Bits.SupPowSave?1:2;       //1������2�ر� 
												UartBack_SendCmdData(CTRL_POWERSAVE,&temp,1,1,1000);        //��������ָ��       //1������2�ر� 
												if(SysConfig.Bits.SupPowSave)
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn On":"��"),UNBREAK);
														if(SysConfig.Bits.PirOn==1)
														{
																SysConfig.Bits.PirOn=0;
																PowerOffPir();
														}
												}
												else
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn Off":"�ر�"),UNBREAK);
														if(SysConfig.Bits.PirOn==0)
														{
																SysConfig.Bits.PirOn=1;
																SysConfig.Bits.Volume=2;
																Audio_SetVolume(SysConfig.Bits.Volume);
																PowerOnPir();
														}
												}
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Ultra power saving mode":"��ʡ��ģʽ"),UNBREAK);
												MenuNow=Menu_KeyPadSysConf; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadSysConf; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //�����������
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadSysConf_PowerSave && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//����ģʽ���ò˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadSysConf_KeepOpen(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn On":"��"),BREAK);       //����
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"keep open mode":"����ģʽ"),UNBREAK);     //��ʡ��ģʽ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"one":"1"),UNBREAK);      //1
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn Off":"�ر�"),UNBREAK);     //�ر�
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"keep open mode":"����ģʽ"),UNBREAK);     //��ʡ��ģʽ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);     //2
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
				
		LedVal.ledval=LED_MASK_SYSCONF_KEEPOPEN;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_SYSCONF_KEEPOPEN;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_1:               //����
								case KEY_2:               //Ӣ��
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												SysConfig.Bits.KeepOpen=(KeyBk.Numb==KEY_1)?1:0;
												uint8_t temp=SysConfig.Bits.KeepOpen?1:2;                  //1������2�ر� 
												UartBack_SendCmdData(CTRL_KEEPOPEN,&temp,1,1,1000);        //����ģʽ����ָ��       //1������2�ر� 
												if(SysConfig.Bits.KeepOpen)
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn On":"��"),UNBREAK);
												}
												else
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn Off":"�ر�"),UNBREAK);
												}
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"keep open mode":"����ģʽ"),UNBREAK);
												MenuNow=Menu_KeyPadSysConf; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadSysConf; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //�����������
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadSysConf_KeepOpen && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}
/******************************************************************************/
/*
//�����Ӧ���ò˵�
input:none
output:none
return:none
*/
/******************************************************************************/
void Menu_KeyPadSysConf_PirSet(void)
{
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		uint32_t VolFinishTime=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
	
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn On":"��"),BREAK);       //����
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Human body induction":"�����Ӧ"),UNBREAK);     //��ʡ��ģʽ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"one":"1"),UNBREAK);      //1
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn Off":"�ر�"),UNBREAK);     //�ر�
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Human body induction":"�����Ӧ"),UNBREAK);     //��ʡ��ģʽ
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please press":"�밴"),UNBREAK);     //�밴
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"two":"2"),UNBREAK);     //2
		
		AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Press Asterisk key to return":"�����밴*�ż�"),UNBREAK);
				
		LedVal.ledval=LED_MASK_SYSCONF_PIRSET;
		
		while(1)
		{
				if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
				{
						LedVal.ledval=LED_MASK_SYSCONF_PIRSET;
						KeyBk=KeyStat;
						KeyBit=GetKeyLedBit(KeyBk.Numb);
						switch(KeyBk.Numb)
						{
								case KEY_1:               //����
								case KEY_2:               //Ӣ��
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												SysConfig.Bits.PirOn=(KeyBk.Numb==KEY_1)?1:0;
												if(SysConfig.Bits.PirOn)
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn On":"��"),UNBREAK);
														PowerOnPir();
												}
												else
												{
														AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Turn Off":"�ر�"),UNBREAK);
														PowerOffPir();
												}
												AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Human body induction":"�����Ӧ"),UNBREAK);
												MenuNow=Menu_KeyPadSysConf; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								case KEY_x:               //����  
										if(KeyBk.Flag!=0)
										{
												LedVal.ledval&=~(1<<KeyBit);
												AudioPlayVoice(GetVolIndex("��"),BREAK);
												MenuNow=Menu_KeyPadSysConf; 
										}
										else
										{
												LedVal.ledval|=(1<<KeyBit);
										}
										break;
								default:break;
						}
				}
				Key_Scan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if((LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)        //�����������
				{
						if(VolFinishTime==0)
						{
								VolFinishTime=GetTimerCount();
						}							
						else if(GetTimerElapse(VolFinishTime)>10000)
						{
								KeyPadMode=KEYPAD_NORMAL;
								AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Exit management mode":"�˳�����ģʽ"),BREAK);
								MenuNow=Menu_KeyPadNormal;
						}
				}
				else
				{
						VolFinishTime=0;
				}
				if(MenuNow!=Menu_KeyPadSysConf_PirSet && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}


/******************************************************************************/
/*
//ATE���Բ˵�
input:none
output:none
return:none
*/
/******************************************************************************/
uint8_t AteStat=0;
uint8_t AteSubStat=0;

void Menu_KeyPadAteTest(void)
{
		static uint8_t ledbit[14]={4,5,6,9,10,11,14,0,1,2,3,8,12,13};  //1 2 3 6 9 # bat bell * 7 4 5 8 0
		uint8_t tptdat[10]={0};
		
		
		KeyStat_t KeyBk=KeyStat;
		uint8_t KeyBit=0;
		
		uint32_t stime=0;
		uint8_t i=0;
	
		InputIdx=0;
	
		UartFace_PowerOff();
	
		AudioPlayVoice(GetVolIndex("�밴ѡ���Ӧ����"),BREAK);  
	
		LedVal.ledval=LED_MASK_ATE_ALL_ON;
		
		while(1)
		{
				switch(AteStat)
				{
						case 0:                          //���Եȴ����棬���е�ȫ�� 
								LedVal.ledval=LED_MASK_ATE_ALL_ON;
								break;
						case 1:                          //��ȫ����ȫ������ˮ
								if(EPORT_ReadGpioData(PIR_POWER_PIN)==Bit_RESET)
								{
										PowerOffPir();
								}
								switch(AteSubStat)
								{
										case 0:      //ȫ��
												LedVal.ledval=LED_MASK_ATE_ALL_ON;
												stime=GetTimerCount();
												AteSubStat++;
												break;
										case 1:      //ȫ��
												if(GetTimerElapse(stime)>2000)
												{
														LedVal.ledval=0;
														AteSubStat++;
												}
												break;
										case 2:
												if(GetTimerElapse(stime)>500)
												{
														LedVal.ledval=0;
														AteSubStat++;
														stime=0;
												}
												break;
										case 3:
												if(stime==0)
												{
														LedVal.ledval=(1<<ledbit[i++]);
														stime=GetTimerCount();
														if(i>=14)
														{
																i=0;
																AteSubStat++;
														}
												}
												else if(GetTimerElapse(stime)>300)
												{
														stime=0;
												}
												break;
										case 4:
												AteSubStat=0;
												break;
								}
								break;
						case 2:         //������������
								if(EPORT_ReadGpioData(PIR_POWER_PIN)==Bit_RESET)
								{
										PowerOffPir();
								}
								if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
								{
										KeyBk=KeyStat;
										LedVal.ledval=LED_MASK_ATE_TOUCH;
										KeyBit=GetKeyLedBit(KeyBk.Numb);
										if(KeyBk.Flag!=0)
										{			
												tptdat[0]=AteStat;       //������
												tptdat[1]=KeyBk.Numb;    //����ֵ
												UartBack_SendCmdData(ATE_UPLINK,tptdat,2,1,1000);
										}
										switch(KeyBk.Numb)
										{
												case KEY_bell:
														if(KeyBk.Flag!=0)
														{
																LedVal.ledval&=~(1<<KeyBit);
																AudioPlayVoice(GetVolIndex("B"),BREAK);
														}
														else
														{
																LedVal.ledval|=(1<<KeyBit);
														}
														break;
												case KEY_c:
														if(KeyBk.Flag!=0)
														{
																LedVal.ledval&=~(1<<KeyBit);
																AudioPlayVoice(GetVolIndex("C"),BREAK);
														}
														else
														{
																LedVal.ledval|=(1<<KeyBit);
														}
														break;
												case KEY_x:
														if(KeyBk.Flag!=0)
														{
																LedVal.ledval&=~(1<<KeyBit);
																AudioPlayVoice(GetVolIndex("X"),BREAK);
														}
														else
														{
																LedVal.ledval|=(1<<KeyBit);
														}
														break;
												default:
														if(KeyBk.Flag!=0)
														{
																AudioPlayVoice(GetVolIndex("0")+KeyBk.Numb-0x30,BREAK);
																LedVal.ledval&=~(1<<KeyBit);
														}
														else
														{
																LedVal.ledval|=(1<<KeyBit);
														}
														break;
										}
								}
								break;
						case 3:            //RGB�Ʋ���
								if(EPORT_ReadGpioData(PIR_POWER_PIN)!=Bit_RESET)
								{
										PowerOnPir();
								}
								switch(AteSubStat)
								{
										case 0:
												if(stime==0)
												{
														PirLed_SetBrightness(2,100);  
														stime=GetTimerCount();
												}
												else if(GetTimerElapse(stime)>500)
												{
														stime=0;
														PirLed_SetBrightness(2,0); 
														AteSubStat++;
												}
												break;
										case 1:
												if(stime==0)
												{
														PirLed_SetBrightness(1,100);  
														stime=GetTimerCount();
												}
												else if(GetTimerElapse(stime)>500)
												{
														stime=0;
														PirLed_SetBrightness(1,0); 
														AteSubStat++;
												}
												break;
										case 2:
												if(stime==0)
												{
														PirLed_SetBrightness(0,100);  
														stime=GetTimerCount();
												}
												else if(GetTimerElapse(stime)>500)
												{
														stime=0;
														PirLed_SetBrightness(0,0); 
														AteSubStat++;
												}
												break;
										case 3:
												if(stime==0)
												{
														PirLed_SetBrightness(0,100); 
														PirLed_SetBrightness(1,100); 
														PirLed_SetBrightness(2,100);  
														stime=GetTimerCount();
												}
												else if(GetTimerElapse(stime)>500)
												{
														stime=0;
														PirLed_SetBrightness(0,0); 
														PirLed_SetBrightness(1,0); 
														PirLed_SetBrightness(2,0);  
														AteSubStat=0;
												}
												break;
								}
								break;
						case 4:       //�����ߵͲ���
								if(EPORT_ReadGpioData(PIR_POWER_PIN)==Bit_RESET)
								{
										PowerOffPir();
								}
								switch(AteSubStat)
								{
										case 0:
												if(stime==0)
												{
														Audio_SetVolume(2); 
														AudioPlayVoice(GetVolIndex("������"),BREAK);
														stime=GetTimerCount();
												}
												else if(GetTimerElapse(stime)>1000)
												{
														stime=0;
														PirLed_SetBrightness(2,0); 
														AteSubStat++;
												}
												break;
										case 1:
												if(stime==0)
												{
														Audio_SetVolume(1); 
														AudioPlayVoice(GetVolIndex("������"),BREAK);
														stime=GetTimerCount();
												}
												else if(GetTimerElapse(stime)>1000)
												{
														stime=0;
														PirLed_SetBrightness(1,0); 
														AteSubStat=0;
												}
												break;
								}
								break;
						case 5:                 //�������
								if(EPORT_ReadGpioData(PIR_POWER_PIN)==Bit_RESET)
								{
										PowerOffPir();
								}
								if(KeyBk.Flag!=KeyStat.Flag || KeyBk.Numb!=KeyStat.Numb)
								{
										KeyBk=KeyStat;
										LedVal.ledval=LED_MASK_ATE_DOORBELL;
										KeyBit=GetKeyLedBit(KeyBk.Numb);
										switch(KeyBk.Numb)
										{
												case KEY_bell:
														if(KeyBk.Flag!=0)
														{
																LedVal.ledval&=~(1<<KeyBit);
																tptdat[0]=AteStat;       //������
																tptdat[1]=KeyBk.Numb;    //����ֵ
																UartBack_SendCmdData(ATE_UPLINK,tptdat,2,1,1000);
														}
														else
														{
																LedVal.ledval|=(1<<KeyBit);
														}
														break;
												default:break;
										}
								}
								break;
						case 6:           //����SET���ز���
						{
								static uint8_t TampKeyValBk=0xFF,SetKeyValBk=0xFF;
								if(EPORT_ReadGpioData(PIR_POWER_PIN)==Bit_RESET)
								{
										PowerOffPir();
								}
								if(TampKeyValBk!=EPORT_ReadGpioData(KEY_TAMPER_PIN))
								{
										DelayMS(20);
										if(TampKeyValBk!=EPORT_ReadGpioData(KEY_TAMPER_PIN))
										{
												TampKeyValBk=EPORT_ReadGpioData(KEY_TAMPER_PIN);
												tptdat[0]=AteStat;       //������
												tptdat[1]=TampKeyValBk;    //����ֵ
												UartBack_SendCmdData(ATE_UPLINK,tptdat,2,1,1000);
										}
								}
								if(SetKeyValBk!=EPORT_ReadGpioData(KEY_BACK_PIN))
								{
										DelayMS(20);
										if(SetKeyValBk!=EPORT_ReadGpioData(KEY_BACK_PIN))
										{
												SetKeyValBk=EPORT_ReadGpioData(KEY_BACK_PIN);
												tptdat[0]=AteStat;       //������
												tptdat[1]=SetKeyValBk;    //����ֵ
												UartBack_SendCmdData(ATE_UPLINK,tptdat,2,1,1000);
										}
								}
								break;
						}
						case 7:          //PIR����
						{
								static uint8_t PirValBk=0xFF;  
								if(EPORT_ReadGpioData(PIR_POWER_PIN)!=Bit_RESET)
								{
										PowerOnPir();
								}
								if(PirValBk!=CPM_ReadGpioData(CPM_WAKEUP_PIN))
								{
										DelayMS(20);
										if(PirValBk!=CPM_ReadGpioData(CPM_WAKEUP_PIN))
										{
												PirValBk=CPM_ReadGpioData(CPM_WAKEUP_PIN);
												tptdat[0]=AteStat;       //������
												tptdat[1]=PirValBk;    //����ֵ
												UartBack_SendCmdData(ATE_UPLINK,tptdat,2,1,1000);
										}
								}
								break;
						}
						case 8:        //��ѹ����
								if(stime==0)
								{
										tptdat[0]=AteStat;       //������
										tptdat[1]=batvolatge;    //����ֵ
										tptdat[2]=batvolatge>>8;    //����ֵ
										UartBack_SendCmdData(ATE_UPLINK,tptdat,3,0,1000);
										stime=GetTimerCount();
								}
								else if(GetTimerElapse(stime)>1000)
								{
										stime=0;
								}
								break;
						case 9:         //��ӿ�Ƭ
						{
								static uint8_t id=0;
								
								switch(AteSubStat)
								{
										case 0:
												AudioPlayVoice(GetVolIndex("��ӿ�Ƭ"),BREAK);
												id=GetCardFreeSlot();
												AteSubStat++;
												stime=0;
												break;
										case 1:
												if(stime==0)
												{
														if((LPM_GetStopModeValue()&LPM_AUDIO_ID)!=0)break;
														uint8_t ret=CardAddHandle(ADD_NORMAL);
														if(ret==ADD_CARD_SUCESS)
														{
																AudioPlayVoice(GetVolIndex("��ӳɹ�"),UNBREAK);
																tptdat[0]=AteStat;       //������
																tptdat[1]=1;             //��ӳɹ���־
																tptdat[2]=id;            //��ӳɹ��Ŀ���ID 
																UartBack_SendCmdData(ATE_UPLINK,tptdat,3,0,1000);
														}
														else if(ret==ADD_CARD_EXIST)
														{
																AudioPlayVoice(GetVolIndex("��Ƭ�Ѵ���"),UNBREAK);
																tptdat[0]=AteStat;       //������
																tptdat[1]=1;             //��ӳɹ���־
																tptdat[2]=0xFF;          //FF��ʶ��Ƭ�Ѵ���
																UartBack_SendCmdData(ATE_UPLINK,tptdat,3,0,1000);
														}
														stime=GetTimerCount();
												}
												else if(GetTimerElapse(stime)>500)
												{
														stime=0;
												}
												break;
								}
								break;
						}
						case 10:         //ʶ��Ƭ  
						{
								static uint8_t id=0;
								
								if(stime==0)
								{
										id=CardAteSearchHandle();
										if(id<=CARD_MAX_NUM)         //��⵽��Ƭ
										{
												tptdat[0]=AteStat;       //������
												tptdat[1]=2;             //��ӳɹ���־
												tptdat[2]=id;            //��ӳɹ��Ŀ���ID 
												UartBack_SendCmdData(ATE_UPLINK,tptdat,3,0,1000);
										}
										stime=GetTimerCount();
								}
								else if(GetTimerElapse(stime)>500)
								{
										stime=0;
								}
								break;
						}
						case 11:         //���ָ��
								switch(AteSubStat)
								{
										case 0:
												AudioPlayVoice(GetVolIndex("���ָ��"),BREAK);
												if(FpPowerDown==1)
												{
														FpPowerDown=0;
														UartFp_DatStrInit();
														UartFp_Init();
												}
												FpTest.AddStat=FP_TEST_ADD_READ_USER;
												FpTest.ScanStat=FP_TEST_SCAN_GET_IMG;
												FpTest.AlreadGetIMG=0;
												FpTest.ComRetry=0;
												FpTest.GenCharCnt=0;
												FpTest.Stime=0;
												FpTest.VolStime=0;
												AteSubStat++;
												break;
										case 1:
												if((LPM_GetStopModeValue()&LPM_AUDIO_ID)!=0)break;
												switch(FpTest_Add())
												{
														case FP_SUCCESS:
																AudioPlayVoice(GetVolIndex("��ӳɹ�"),UNBREAK);
																AteSubStat++;
																tptdat[0]=AteStat;       //������
																tptdat[1]=1;             //��ӳɹ���־
																UartBack_SendCmdData(ATE_UPLINK,tptdat,2,0,1000);
																break;
														case FP_FAIL:
																AudioPlayVoice(GetVolIndex("���ʧ��"),UNBREAK);
																AteSubStat++;
																tptdat[0]=AteStat;       //������
																tptdat[1]=2;             //���ʧ�ܱ�־
																UartBack_SendCmdData(ATE_UPLINK,tptdat,2,0,1000);
																break;
														default:break;
												}
										case 2:break;			
								}
								break;
						case 12:         //��ָ֤�� 
								switch(AteSubStat)
								{
										case 0:
												AudioPlayVoice(GetVolIndex("�밴��ָ"),BREAK);
												if(FpPowerDown==1)
												{
														FpPowerDown=0;
														UartFp_DatStrInit();
														UartFp_Init();
												}
												FpTest.AddStat=FP_TEST_ADD_READ_USER;
												FpTest.ScanStat=FP_TEST_SCAN_GET_IMG;
												FpTest.AlreadGetIMG=0;
												FpTest.ComRetry=0;
												FpTest.GenCharCnt=0;
												FpTest.Stime=0;
												FpTest.VolStime=0;
												AteSubStat++;
												break;
										case 1:
												if((LPM_GetStopModeValue()&LPM_AUDIO_ID)!=0)break;
												switch(FpTest_Scan())
												{
														case FP_SUCCESS:
																AudioPlayVoice(GetVolIndex("ָ����"),UNBREAK);						//��������
																AteSubStat++;
																tptdat[0]=AteStat;       //������
																tptdat[1]=1;             //ʶ��ɹ���־
																UartBack_SendCmdData(ATE_UPLINK,tptdat,2,0,1000);
																break;
														case FP_FAIL:
																AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
																AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
																AteSubStat++;
																tptdat[0]=AteStat;       //������
																tptdat[1]=2;             //ʶ��ʧ�ܱ�־
																UartBack_SendCmdData(ATE_UPLINK,tptdat,2,0,1000);
																break;
														default:break;
												}
												break;
										case 2:break;												
								}
								break;
						case 13:              //�������
								switch(AteSubStat)
								{
										case 0:                   //��ʼ
												AudioPlayVoice(GetVolIndex("�������"),BREAK);
												printf("face test start\n");
												FaceReady.Ready=0;
												UartFace_TxQueueReset();
												UartFace_PowerReset();
												FaceAdd.StTime=0;
												FaceAdd.VolFristTime=0;
												FaceAdd.Retry=0;
												FaceAdd.AddStat=FACEADD_READUSER;
												AteSubStat++;
												break;
										case 1:                    //������� 
												if((LPM_GetStopModeValue()&LPM_AUDIO_ID)!=0)break;
												switch(FaceTest_Add())
												{
														case OP_READY: 
																AudioPlayVoice(GetVolIndex("��ֱ������ͷ"),UNBREAK);
																FaceAdd.AddStat=FACEADD_RUN;
																break;
														case OP_SUCCESS:
																AudioPlayVoice(GetVolIndex("��ӳɹ�"),UNBREAK);
																AteSubStat++;
																tptdat[0]=AteStat;       //������
																tptdat[1]=1;             //��ӳɹ���־
																UartBack_SendCmdData(ATE_UPLINK,tptdat,2,0,1000);
																break;
														case OP_FAIL:
																AudioPlayVoice(GetVolIndex("���ʧ��"),UNBREAK);       
																AteSubStat++;
																tptdat[0]=AteStat;       //������
																tptdat[1]=2;             //���ʧ�ܱ�־
																UartBack_SendCmdData(ATE_UPLINK,tptdat,2,0,1000);
																break;
														case OP_TIMEOUT:
																AudioPlayVoice(GetVolIndex("������ʱ"),UNBREAK);  
																AteSubStat++;
																tptdat[0]=AteStat;       //������
																tptdat[1]=3;             //��ӳ�ʱ��־
																UartBack_SendCmdData(ATE_UPLINK,tptdat,2,0,1000);
																break;
														default:break;
												}
												break;
										case 2:break;
								}
								break;
						case 14:              //��֤����
								switch(AteSubStat)
								{
										case 0:                   //��ʼ
												AudioPlayVoice(GetVolIndex("��ֱ������ͷ"),BREAK);
												printf("face test start\n");
												FaceReady.Ready=0;
												UartFace_TxQueueReset();
												UartFace_PowerReset();
												AteSubStat++;
												break;
										case 1:                    //������� 
												if((LPM_GetStopModeValue()&LPM_AUDIO_ID)!=0)break;
												switch(FaceTest_Scan())
												{
														case OP_SUCCESS:
																AudioPlayVoice(GetVolIndex("ָ����"),UNBREAK);		
																AteSubStat++;
																tptdat[0]=AteStat;       //������
																tptdat[1]=1;             //ʶ��ɹ���־
																UartBack_SendCmdData(ATE_UPLINK,tptdat,2,0,1000);
																break;
														case OP_FAIL:
																AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
																AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
																AteSubStat++;
																tptdat[0]=AteStat;       //������
																tptdat[1]=2;             //ʶ��ɹ���־
																UartBack_SendCmdData(ATE_UPLINK,tptdat,2,0,1000);
																break;
														default:break;
												}
												break;
										case 2:break;
								}
								break;
						case 15:           //��̬�������� 
								if(FpPowerDown!=1)    //�ȴ�ָ������
								{
										LdFpStatHandle();
								}
								//�ȴ����ݷ��͸�ATE���԰���ɺ��������͹���
								else if((LPM_GetStopModeValue()&LPM_BACKLOCK_TX_ID)==0) 
								{
										while(1)
										{
												ATE_PowerDownDeinit();   //����͹���
										}
								}
								break;
						default:break;
				}
				Key_AteScan();
				Audio_RunningHandle();
				Uart_RtxFrameHandle();
				Led_UpdateVal();
				if(MenuNow!=Menu_KeyPadAteTest && (LPM_GetStopModeValue()&LPM_AUDIO_ID)==0)
				{
						if(MenuNow==Menu_KeyPadNormal)LedVal.ledval=LED_MASK_NORMAL;
						break;
				}
		}
}


/******************************************************************************/
/*
//��ȡ������Ӧ��LEDλ��
input: key -- �������
output:none
return:bitK -- ledλ��
*/
/******************************************************************************/
uint8_t GetKeyLedBit(uint8_t key)
{
		uint8_t bitK=0;
		switch(key)
		{
				case KEY_0:                    //5
						bitK=13;
						break;
				case KEY_1:
						bitK=4;
						break;
				case KEY_2:
						bitK=5;
						break;
				case KEY_3:
						bitK=6;
						break;
				case KEY_4:
						bitK=3;
						break;
				case KEY_5:
						bitK=8;
						break;
				case KEY_6:
						bitK=9;
						break;
				case KEY_7:  
						bitK=2;
						break;
				case KEY_8:
						bitK=12;
						break;
				case KEY_9:
						bitK=10;
						break;
				case KEY_x:
						bitK=1;
						break;
				case KEY_c:
						bitK=11;
						break;
				case KEY_bell:
						bitK=0;
						break;
		}
		return bitK;
}








