#include "card.h"
#include "timer.h"
#include "sky1311_drv.h"
#include "sky1311t.h"
#include "iso14443.h"
#include "string.h"
#include "lpm.h"
#include "lock_config.h"
#include "audio.h"
#include "keypad_menu.h"
#include "key.h"
#include "i2c_drv.h"
#include "uart_fp.h"
#include "uart_back.h"
#include "uart.h"
#include "led.h"
#include "rtc.h"
#include "lock_record.h"


CardStat_t CdStat; 
CardBaseInfo_t CardBaseInfo;
CardInfo_t CardInfo={0};
CardUser_t CardUser[CARD_MAX_NUM];

uint8_t isCardInit=0;


/******************************************************************************/
/*
//ˢ��IO��ʼ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void CardIoInit(void)
{
		if(isCardInit==1)return;
	
		CardUserInit();
		RFID_PortInit();
		
		isCardInit=1;
}
/******************************************************************************/
/*
//ˢ����ʼ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void CardInit(void)
{
		if(isCardInit==0)CardIoInit();
		SKY1311_ENABLE();
		sky1311Init();	
		typeAOperate();
		DelayMS(1);
}
/******************************************************************************/
/*
//ˢ������
input:   none
output   none
return   none 
*/
/******************************************************************************/
void CardDeInit(void)
{
	//	if(isCardInit==0)return;
		RFID_PortDeInit();
		isCardInit=0;
}
/******************************************************************************/
/*
//Ѱ��--500msһ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void CardSearchHandle(void)
{
		uint8_t tmpBuf[10]={0};
		
		if(SysConfig.Bits.CardLock==1)return;                                //��������
		if(LPM_GetStopModeValue()==0)return;                                 // 
		
		//��������ˢ��
		CardInit();
		//���ý���
		if(piccWakeupA(CardBaseInfo.ATQA)==Ok) 
		{
				uint8_t *uid=&CardBaseInfo.UID[0];
				uint8_t sel=SEL1;
				uint8_t isOk=0;
				CardBaseInfo.UIDSize=CardBaseInfo.ATQA[0]>>6;
				CardBaseInfo.SAK=0;
				do{
						if(piccAntiA(sel,1,uid)==Ok) 
						{
								uint8_t checkSum = uid[0] ^ uid[1] ^ uid[2] ^ uid[3];
								if(checkSum == uid[4])
								{
										if(piccSelectA(sel,uid,tmpBuf)==Ok)
										{
												CardBaseInfo.SAK=tmpBuf[0];
												if(CardBaseInfo.UIDSize!=0 && uid[0] == 0x88)
												{
														for(uint8_t i=0;i<3;i++)
														{
																uid[i] = uid[i+1];
														}
														uid += 3;
														sel += 2;
												}
												else
												{
														isOk=1;	
														printf("card id: [0]=0x%02x,[1]=0x%02x,[2]=0x%02x,[3]=0x%02x \n",uid[0],uid[1],uid[2],uid[3]);
														printf("card sak: 0x%02x\n",CardBaseInfo.SAK);
														printf("card atqa: [0]=0x%02x,[1]=0x%02x\n",CardBaseInfo.ATQA[0],CardBaseInfo.ATQA[1]);
												}
										}
								}
						}
				}while((CardBaseInfo.SAK&0x04)!=0);
				
				if(isOk==1 && SysConfig.Bits.Activate==0)
				{
						AudioPlayVoice(GetVolIndex("ָ����"),BREAK);						//��������
						AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please activate the product":"�뼤���Ʒ"),UNBREAK);
						PirLed_SetStat(LOGO_OPEN_SUCCESS);
						
						UserControlLock(CTL_OPEN_LOCK,CARD_OPEN,0xFF);
				}
				else if(isOk==1 && mifare1_ReadTest(CardBaseInfo.UID,NULL)==Ok)
				{
						uint8_t idyokidx=0;
						
						CardUser_t CardUserTmp={0};
						CardUserTmp.Card.Uid[0]=CardBaseInfo.UID[0];
						CardUserTmp.Card.Uid[1]=CardBaseInfo.UID[1];
						CardUserTmp.Card.Uid[2]=CardBaseInfo.UID[2];
						CardUserTmp.Card.Uid[3]=CardBaseInfo.UID[3];
						
						idyokidx=IdfCard(&CardUserTmp);
						
						if(idyokidx<CARD_MAX_NUM)      //0-99
						{
								tmpBuf[0]=0;
								LockUpdatePeripLockStat(PERIP_CARD_INDEX,0);
							
								if(SysConfig.Bits.MutiIdy==1)
								{
										if(MutiIdyItem&(~IDY_CARD))        //��������֤�ɹ�+������֤�ɹ�,˫����֤�ɹ�
										{	
												MutiIdyItem=0;
												AudioPlayVoice(GetVolIndex("ָ����"),BREAK);						//��������
												PirLed_SetStat(LOGO_OPEN_SUCCESS);
												UserControlLock(CTL_OPEN_LOCK,CARD_OPEN,idyokidx);
										}
										else                              //�ÿ���֤�ɹ���־ 
										{
												MutiIdyItem|=IDY_CARD;
												PirLed_SetStat(LOGO_OPEN_SUCCESS_MUTIL);
										}
								}
								else
								{
										AudioPlayVoice(GetVolIndex("ָ����"),BREAK);						//��������
										PirLed_SetStat(LOGO_OPEN_SUCCESS);
										UserControlLock(CTL_OPEN_LOCK,CARD_OPEN,idyokidx);
								}
						}
						else      //GMT��������δ���
						{
								AudioPlayVoice(GetVolIndex("�쿨��"),BREAK);       //��֤ʧ��
								AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
								PirLed_SetStat(LOGO_OPEN_FAIL);
								LockUpdatePeripLockStat(PERIP_CARD_INDEX,1);
						}
				}
				else if(isOk==1)         //��Ч������GMT����
				{
						AudioPlayVoice(GetVolIndex("�쿨��"),BREAK);       //��֤ʧ��
						AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
						PirLed_SetStat(LOGO_OPEN_FAIL);
					  LockUpdatePeripLockStat(PERIP_CARD_INDEX,1);
				}
		}
		sky1311Reset();
}
/******************************************************************************/
/*
//��ӿ�Ƭ
input:   type:��ӿ�����
             00   ---- ����Ա
             0-99 ---- ��ͨ��
output   none
return   ��ӿ�Ƭ����״̬ 
            
             FF--δ��⵽��
						 ADD_CARD_SUCESS            0   --��ӳɹ�            
						 ADD_CARD_FULL              1   --��Ƭ������
						 ADD_CARD_EXIST             2   --��Ƭ�Ѵ��� 
						 ADD_CARD_FAIL              3   --���ʧ�� 
*/
/******************************************************************************/
uint8_t CardAddHandle(uint8_t type)
{
		CardBaseInfo_t CdInfo={0};
		uint8_t addok=0xFF;
		
		CardInit();
		if(piccWakeupA(CardBaseInfo.ATQA)==Ok) 
		{
				uint8_t tmpBuf[10]={0};
				uint8_t *uid=&CdInfo.UID[0];
				uint8_t sel=SEL1;
				uint8_t isOk=0;
				CdInfo.UIDSize=CdInfo.ATQA[0]>>6;
				CdInfo.SAK=0;
				do{
						if(piccAntiA(sel,1,uid)==Ok) 
						{
								uint8_t checkSum = uid[0] ^ uid[1] ^ uid[2] ^ uid[3];
								if(checkSum == uid[4])
								{
										if(piccSelectA(sel,uid,tmpBuf)==Ok)
										{
												CdInfo.SAK=tmpBuf[0];
												if(CdInfo.UIDSize!=0 && uid[0] == 0x88)
												{
														for(uint8_t i=0;i<3;i++)
														{
																uid[i] = uid[i+1];
														}
														uid += 3;
														sel += 2;
												}
												else
												{
														isOk=1;
												}
										}
								}
						}
				}while((CdInfo.SAK&0x04)!=0);
				
				if(isOk==1)
				{
						if(mifare1_ReadTest(CdInfo.UID,NULL)==Ok)
						{
								CardUser_t CardUserTmp={0};
								
								CardUserTmp.Card.Uid[0]=CdInfo.UID[0];
								CardUserTmp.Card.Uid[1]=CdInfo.UID[1];
								CardUserTmp.Card.Uid[2]=CdInfo.UID[2];
								CardUserTmp.Card.Uid[3]=CdInfo.UID[3];
								
								CardUserTmp.Card.Roll[0]=0;
								CardUserTmp.Card.Roll[1]=0;
								CardUserTmp.Card.Roll[2]=0;
								CardUserTmp.Card.Roll[3]=1;
								addok=AddCard(&CardUserTmp,type);
						}
						else 
						{
								addok=ADD_CARD_FAIL;
						}
				}
		}
		sky1311Reset();
		return addok;
}

/******************************************************************************/
/*
//��ȡ���û�
input:   none
output   none
return   none 
*/
/******************************************************************************/
void CardUserInit(void)
{
		uint8_t i=0;
		//��⿨�û��Ƿ��Ѿ���ʼ��
		
		IntFlashRead(CardInfo.CardBuf,CARD_INFO_ADDR,sizeof(CardInfo_t));  
		if(CardInfo.Card.InitFlag!=0xAA)
		{
				CardInfo.Card.InitFlag=0xAA;
				CardInfo.Card.CardSum=0;
				CardInfo.Card.Rsv1=0;
				CardInfo.Card.Rsv2=0;
				IntFlashWrite(CardInfo.CardBuf,CARD_INFO_ADDR,sizeof(CardInfo_t)); 
				//дĬ������0
				CardUser_t CardUserTmp={0};
				for(i=0;i<CARD_MAX_NUM;i++)
				{
						IntFlashWrite(CardUserTmp.CardBuf,CARD_DATA_ADDR+i*sizeof(CardUser_t),sizeof(CardUser_t));  
				}
		}
		//�����û�����������Ч������
		CardInfo.Card.CardSum=0;     //��λ�����������¼���
		for(i=0;i<CARD_MAX_NUM;i++)
		{
				IntFlashRead(CardUser[i].CardBuf,CARD_DATA_ADDR+i*sizeof(CardUser_t),sizeof(CardUser_t)); 
				if(!CheckCardInvalid(&CardUser[i]))   //����Ч
				{
						CardInfo.Card.CardSum++;
				}					
		}
}

/******************************************************************************/
/*
//��⿨���Ƿ���Ч
input:   usr---���û�
output   none
return   0--����Ч   1--��Ч��
*/
/******************************************************************************/
uint8_t CheckCardInvalid(CardUser_t *usr)
{
		uint8_t i=0;
		for(i=0;i<8;i++)
		{
				if(usr->CardBuf[i]!=0)return 0; 
		}
		return 1;
}
/******************************************************************************/
/*
//��ȡ���õĿ��û�slot
input:   none
output   none
return   ����slot
*/
/******************************************************************************/
uint8_t GetCardFreeSlot(void)
{
		uint8_t i=0;
	
		for(i=0;i<CARD_MAX_NUM;i++)
		{
				if(CheckCardInvalid(&CardUser[i]))
				{
						return i;
				}
		}
		return i;
}
/******************************************************************************/
/*
//��ӿ�Ƭ
input:   card --- ���û�����Ϣ
         type --- ������ ��ADD_ADMIN������Ա��  ADD_NORMAL����ͨ��
output   none
return   
				 ADD_CARD_SUCESS            0       --��ӳɹ�     
				 ADD_CARD_FULL              1       --��Ƭ����
				 ADD_CARD_EXIST             2       --��Ƭ�Ѵ���
				 ADD_CARD_FAIL              3	      --���ʧ��  
*/
/******************************************************************************/
uint8_t AddCard(CardUser_t* card,uint8_t type)
{
		uint8_t i=GetCardFreeSlot();      //
		uint8_t j=0,k=0;     

		//��ӹ���Ա��
		if(type==ADD_ADMIN)                        //��ӹ���Ա����ֱ�Ӹ���
		{
				CardUser[0]=*card;
				IntFlashWrite(CardUser[0].CardBuf,CARD_DATA_ADDR,sizeof(CardUser_t));  
				
				LockAddUser(PERIP_CARD_INDEX,0);	
				return ADD_CARD_SUCESS;
		}
		//��Ƭ������
		if(i==CARD_MAX_NUM)return ADD_CARD_FULL;   //�ǹ���Ա����������
		//��⿨Ƭ�����Ƿ��Ѵ���
		for(j=0;j<CARD_MAX_NUM;j++)
		{
				for(k=0;k<4;k++)         //ֻƥ��4�ֽڿ���
				{
						if(CardUser[j].Card.Uid[k]!=card->Card.Uid[k])
						{
								break;           //���Ų�ͬ
						}
				}
				if(k==4)break;           //�����Ѵ��� 
		}
		if(j<CARD_MAX_NUM)return ADD_CARD_EXIST;       //���Ѵ���
		//�洢����
		CardUser[i]=*card;
		IntFlashWrite(CardUser[i].CardBuf,CARD_DATA_ADDR+i*sizeof(CardUser_t),sizeof(CardUser_t));
		
		LockAddUser(PERIP_CARD_INDEX,i);	

		return ADD_CARD_SUCESS;
}
/******************************************************************************/
/*
//ɾ����Ƭ
input:   id1 --- ��ID

output   none
return   
				 DEL_CARD_SUCCESS           0   --ɾ���ɹ�
         DEL_CARD_NOEXIST           1   --��Ƭ������
*/
/******************************************************************************/
uint8_t DelCard(uint8_t id1)
{
		uint8_t id=id1;
		//���Ų�����
		if(id>=CARD_MAX_NUM && id!=0xFF)return DEL_CARD_NOEXIST;
		//ɾ�����п�
		if(id==0xFF)           //ɾ�����п�Ƭ
		{
				//ɾ��
				printf("carduser size: 0x%02x\n",sizeof(CardUser));
				memset(CardUser,0,sizeof(CardUser));
				IntFlashWrite((uint8_t*)&CardUser,CARD_DATA_ADDR,sizeof(CardUser));
				LockDelUser(PERIP_CARD_INDEX,id);
				//ɾ�����¶�
				CardUserInit();
		}
		else                 //ɾ�����ſ�Ƭ
		{
				//�ж�Ҫɾ���Ŀ�Ƭ�Ƿ����
				if(CheckCardInvalid(&CardUser[id]))return DEL_CARD_NOEXIST;
				//ɾ����Ƭ
				memset(CardUser[id].CardBuf,0,sizeof(CardUser_t));
				IntFlashWrite(CardUser[id].CardBuf,CARD_DATA_ADDR+id*sizeof(CardUser_t),sizeof(CardUser_t));
			
				LockDelUser(PERIP_CARD_INDEX,id);
		}
		return DEL_CARD_SUCCESS;
}
/******************************************************************************/
/*
//����ָ�����ڿ��е�ID��
input:   card --- ����Ϣ

output   none
return   
				 n------- ����ID��
*/
/******************************************************************************/
uint8_t IdfCard(CardUser_t* card)
{
		uint8_t j=0,k=0;
		for(j=0;j<CARD_MAX_NUM;j++)
		{
				for(k=0;k<4;k++)
				{
						if(CardUser[j].Card.Uid[k]!=card->Card.Uid[k])
						{
								break;  
						}
				}
				if(k==4)return j;                    //UID���
		}
		return j;
}


/******************************************************************************/
/*
//��ƬATE����
input:   card --- ����Ϣ

output   none
return   
				 n------- ����ID��
*/
/******************************************************************************/
uint8_t CardAteSearchHandle(void)
{
		uint8_t tmpBuf[10]={0};
		uint8_t idyokidx=0xFF;
		
		CardInit();
		//���ý���
		if(piccWakeupA(CardBaseInfo.ATQA)==Ok) 
		{
				uint8_t *uid=&CardBaseInfo.UID[0];
				uint8_t sel=SEL1;
				uint8_t isOk=0;
				CardBaseInfo.UIDSize=CardBaseInfo.ATQA[0]>>6;
				CardBaseInfo.SAK=0;
				do{
						if(piccAntiA(sel,1,uid)==Ok) 
						{
								uint8_t checkSum = uid[0] ^ uid[1] ^ uid[2] ^ uid[3];
								if(checkSum == uid[4])
								{
										if(piccSelectA(sel,uid,tmpBuf)==Ok)
										{
												CardBaseInfo.SAK=tmpBuf[0];
												if(CardBaseInfo.UIDSize!=0 && uid[0] == 0x88)
												{
														for(uint8_t i=0;i<3;i++)
														{
																uid[i] = uid[i+1];
														}
														uid += 3;
														sel += 2;
												}
												else
												{
														isOk=1;	
														printf("card id: [0]=0x%02x,[1]=0x%02x,[2]=0x%02x,[3]=0x%02x \n",uid[0],uid[1],uid[2],uid[3]);
														printf("card sak: 0x%02x\n",CardBaseInfo.SAK);
														printf("card atqa: [0]=0x%02x,[1]=0x%02x\n",CardBaseInfo.ATQA[0],CardBaseInfo.ATQA[1]);
												}
										}
								}
						}
				}while((CardBaseInfo.SAK&0x04)!=0);
				
				if(isOk==1 && mifare1_ReadTest(CardBaseInfo.UID,NULL)==Ok)
				{
						CardUser_t CardUserTmp={0};
						CardUserTmp.Card.Uid[0]=CardBaseInfo.UID[0];
						CardUserTmp.Card.Uid[1]=CardBaseInfo.UID[1];
						CardUserTmp.Card.Uid[2]=CardBaseInfo.UID[2];
						CardUserTmp.Card.Uid[3]=CardBaseInfo.UID[3];
						
						idyokidx=IdfCard(&CardUserTmp);
						
						if(idyokidx<CARD_MAX_NUM)      //0-99
						{
								AudioPlayVoice(GetVolIndex("ָ����"),BREAK);						//��������
						}
						else      //GMT��������δ���
						{
								AudioPlayVoice(GetVolIndex("�쿨��"),BREAK);       //��֤ʧ��
								AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
						}
				}
				else if(isOk==1)         //��Ч������GMT����
				{
						idyokidx=CARD_MAX_NUM;
						AudioPlayVoice(GetVolIndex("�쿨��"),BREAK);       //��֤ʧ��
						AudioPlayVoice(GetVolIndex("�쿨��"),UNBREAK);
				}
		}
		sky1311Reset();
		return idyokidx;
}




