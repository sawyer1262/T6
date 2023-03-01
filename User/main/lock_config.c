#include "lock_config.h"           
#include "flash.h"
#include "audio.h"
#include "password.h"
#include <string.h>
#include "card.h"
#include "lock_record.h" 
#include "key.h"
#include "eport_drv.h"
#include "bat.h"

//��Ʒ�ͺ�;Ӳ���汾;����汾;��������;����beta��release
//ͨ���汾�����жϲ�Ʒ�汾���Լ���Ʒ����


#ifdef NO_FACE_MODE   
//const char ver[40]="T6W_H2.0_S1.0_230224_release";                       //Ӳ���汾2.0�����°汾4202EоƬ

const char ver[40]="T6MW_H2.0_S1.0_230224_release";        //è�۲�������
//const char ver[40]="T6VW_H2.0_S1.0_230224_release";        //���Ӳ�������

#else
const char ver[40]="T6FW_H2.0_S1.0_230224_release";                      //Ӳ���汾2.0�����°汾4202EоƬ
//const char ver[40]="T6FV_H2.0_S1.0_230224_release";        //���Ӵ�����
//const char ver[40]="T6MV_H2.0_S1.0_230224_release";        //è�۴�����
#endif

SysConfig_t SysConfig={0},SysConfigBk={0};
PeripLock_t PeripLock={0},PeripLockBk={0};
TamperAlarm_t TamperAlarm={0};
LowBatAlarm_t LowBatAlarm={0};
NetTime_t NetTime={0};
IapInfo_t IapInfo={0};
CoerceFp_t CoerceFp={0};
// DG   22    17        M5       000016    085105
//����  ��  �ڼ���  �ͺţ�2-4��  �ڼ�̨     SN��
char SN[SN_MAX_LEN]={0};  
char SnNum[7]={'7','8','6','9','3','0',0}; 
uint8_t MutiIdyItem=0;
uint8_t SnInvalid=0;

/******************************************************************************/
/*
2023/02/24
1.���°�MCU�������°���ɰ�IO������������������Ӳ���Ե������Բ����ݣ�Ӳ���汾��Ϊ3.0
2.�޸�����˵��˳�ָ�Ʋ�ʧ�ܵ�BUG


*/
/******************************************************************************/
/*
//��������SN��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void UpdateLockSn(void)
{
		uint8_t i=0,snStart=0;
	
		SnInvalid=0;
		//�����к�����
		IntFlashRead((uint8_t*)SN,SN_ADDR,SN_MAX_LEN);  
		//�ҳ���Ч���к�λ��
		for(i=31;i>0;i--)
		{
				if(SN[i]!=0)
				{
						snStart=i-5;
						break;
				}
		}
		//��ȡ6�ֽ����к�
		for(i=snStart;i<snStart+6;i++)
		{
				SnNum[i-snStart]=SN[i];
				if(SN[i]<'0' || SN[i]>'9')
				{
						SnInvalid=1;
				}
		}
		//��֤�������Ƿ���Ч
		printf("SN: %s",SnNum);
}

/******************************************************************************/
/*
//��ϵͳ������Ϣ
input:   none
output   none
return   none 
*/
/******************************************************************************/
void LockReadSysConfig(void)
{
		uint8_t i=0;  
		//��SN
		UpdateLockSn();
		//���豸������
		IntFlashRead(SysConfig.B8,SYS_CONFIG_ADDR,sizeof(SysConfig_t));           
		if(SysConfig.Bits.Flag!=0xA)        //1010  
		{
				SysConfig.Bits.Activate=0;
				SysConfig.Bits.Language=0;
				SysConfig.Bits.CardLock=0;
				SysConfig.Bits.ChildLock=0;
				SysConfig.Bits.FaceLock=0;
				SysConfig.Bits.MutiIdy=0;
				SysConfig.Bits.FpLock=0;
				SysConfig.Bits.SupPowSave=0;
				SysConfig.Bits.TampAct=1;
				SysConfig.Bits.KeyPADLock=0;
				SysConfig.Bits.KeepOpen=0;
				SysConfig.Bits.Volume=1;              //0-0,1-5,2-10
				SysConfig.Bits.OTCode=0;
				SysConfig.Bits.PrivMode=0;
				SysConfig.Bits.PirOn=0;               //Ĭ�ϴ�PIR
				SysConfig.Bits.DiSel=0;
				SysConfig.Bits.TimeZone=8;
				SysConfig.Bits.Rsv=0;                 //����ֵ0
				SysConfig.Bits.Flag=0xA;
				IntFlashWrite(SysConfig.B8,SYS_CONFIG_ADDR,sizeof(SysConfig_t)); 
		}	
		SysConfigBk=SysConfig;
		SysConfig.Bits.Activate=1;
		if(SysConfig.Bits.Activate==0)SysConfig.Bits.PirOn=0;
		printf("timezone:%d\n",SysConfig.Bits.TimeZone);
		//��������
		Audio_SetVolume(SysConfig.Bits.Volume);
		//����������أ��˴�û�ж������ϵ����������������ѭ����ÿ�����и���ȥ�ˡ�
		IntFlashRead(PeripLock.PripBuf,SYS_PERIP_LOCK_ADDR,sizeof(PeripLock_t)); 
		if(PeripLock.Perip.Flag!=PERIPHLOCK_FLAG)  
		{
				for(i=0;i<4;i++)
				{
						PeripLock.Perip.FailTime[i]=0;
				}
				for(i=0;i<4;i++)
				{
						PeripLock.Perip.FailStart[i].day=0;
						PeripLock.Perip.FailStart[i].hour=0;
						PeripLock.Perip.FailStart[i].minute=0;
						PeripLock.Perip.FailStart[i].second=0;
						PeripLock.Perip.LockStart[i].day=0;
						PeripLock.Perip.LockStart[i].hour=0;
						PeripLock.Perip.LockStart[i].minute=0;
						PeripLock.Perip.LockStart[i].second=0;
				}
				PeripLock.Perip.Flag=PERIPHLOCK_FLAG;
				IntFlashWrite(PeripLock.PripBuf,SYS_PERIP_LOCK_ADDR,sizeof(PeripLock_t)); 
		}
		PeripLockBk=PeripLock;
		//����ص�ѹ
		IntFlashRead(LowBatAlarm.BatBuf,BAT_CHK_ADDR,sizeof(LowBatAlarm_t)); 
		if(LowBatAlarm.Bat.Flag!=0xAA)
		{
				LowBatAlarm.Bat.Flag=0xAA;
				LowBatAlarm.Bat.Value=0;
				LowBatAlarm.Bat.Day=0;
				LowBatAlarm.Bat.Hour=0;
				LowBatAlarm.Bat.Minute=0;
				LowBatAlarm.Bat.Second=0; 
				IntFlashWrite(LowBatAlarm.BatBuf,BAT_CHK_ADDR,sizeof(LowBatAlarm_t));   //��ʼ��
		}
		if(LowBatAlarm.Bat.Value>0)     //�б����ѹ
		{
				mytm_t tm={0};
				tm.day=LowBatAlarm.Bat.Day;
				tm.hour=LowBatAlarm.Bat.Hour;
				tm.minute=LowBatAlarm.Bat.Minute;
				tm.second=LowBatAlarm.Bat.Second;
				if(GetPeripLockTimeElapse(&tm)<=0)    //�����ϵ�ȫ����0
				{
						LowBatAlarm.Bat.Value=0;
						LowBatAlarm.Bat.Day=0;
						LowBatAlarm.Bat.Hour=0;
						LowBatAlarm.Bat.Minute=0;
						LowBatAlarm.Bat.Second=0; 
				}
				BatPerCent=GetVoltagePercent(LowBatAlarm.Bat.Value);   //���ѻ��ϵ������ǰ��¼�ĵ�ص�ѹ����,��Ȼ�ڴ��������ݵ�APPʱ�����0
		}	
		//������ʱ��
		IntFlashRead(NetTime.NTBuf,NET_TIME_ADDR,sizeof(NetTime_t)); 
		if(NetTime.NT.Flag!=0xAA)          //��ʼ������ʱ��ṹ
		{
				NetTime.NT.Stat=0;
				NetTime.NT.Day=0;
				NetTime.NT.Hour=0;
				NetTime.NT.Minute=0;
				NetTime.NT.Second=0;
				NetTime.NT.Flag=0xAA;
				IntFlashWrite(NetTime.NTBuf,NET_TIME_ADDR,sizeof(NetTime_t)); 
		}
		else if(NetTime.NT.Stat==1)       //�Ѿ���ȡ��ʱ�䣬�ж��Ƿ��������ϵ�
		{
				mytm_t tm={0};
				tm.day=NetTime.NT.Day;
				tm.hour=NetTime.NT.Hour;
				tm.minute=NetTime.NT.Minute;
				tm.second=NetTime.NT.Second;
				if(GetPeripLockTimeElapse(&tm)<=0)    //�����ϵ�ȫ����0
				{
						NetTime.NT.Stat=0;
						NetTime.NT.Day=0;
						NetTime.NT.Hour=0;
						NetTime.NT.Minute=0;
						NetTime.NT.Second=0;
						NetTime.NT.Flag=0xAA;
						IntFlashWrite(NetTime.NTBuf,NET_TIME_ADDR,sizeof(NetTime_t));
				}
		}
//		for(i=0;i<8;i++)printf("data:0x%02x ",NetTime.NTBuf[i]);
//		printf("nt: day:%02d,hour:%02d,min:%02d,sec:%02d\n",NetTime.NT.Day,NetTime.NT.Hour,NetTime.NT.Minute,NetTime.NT.Second);
		//��в��ָ�Ʊ�ʶ
		IntFlashRead(CoerceFp.CoerceFpBuf,COERCE_FP_ADDR,sizeof(CoerceFp_t)); 
		if(CoerceFp.CFP.Flag1!=0xAA && CoerceFp.CFP.Flag2!=0x55 && CoerceFp.CFP.Flag3!=0xAA)
		{
				CoerceFp.CFP.Flag1=0xAA;CoerceFp.CFP.Flag2=0x55;CoerceFp.CFP.Flag3=0xAA;
				for(i=0;i<13;i++)
				{
						CoerceFp.CFP.CoerceFp[i]=0;
				}
				IntFlashWrite(CoerceFp.CoerceFpBuf,COERCE_FP_ADDR,sizeof(CoerceFp_t)); 
		}
		//���ⲿFLASH�û�������¼���ݰ�ͷ
		ExFlashRead(UserOperaHead.OpHeadBuf,USER_RECORD_HEAD_ADDR,sizeof(UserOperaHead_t));
		if(UserOperaHead.OpHead.Flag!=0xAA55 || UserOperaHead.OpHead.WriteIndex>=USER_RECORD_MAX_NUM)
		{
				printf("user record init\n");
				UserOperaHead.OpHead.Flag=0xAA55;
				UserOperaHead.OpHead.WriteIndex=0;
				UserOperaHead.OpHead.RecordSum=0;
				ExFlashWrite(UserOperaHead.OpHeadBuf,USER_RECORD_HEAD_ADDR,sizeof(UserOperaHead_t));
		}
		printf("user record sum: 0x%02x\n",UserOperaHead.OpHead.WriteIndex);
}

/******************************************************************************/
/*
//дϵͳ������Ϣ
input:   none
output   none
return   none 
*/
/******************************************************************************/
void LockWriteSysConfig(void)
{
		if(SysConfigBk.B32!=SysConfig.B32)
		{
				if(SysConfigBk.Bits.Volume!=SysConfig.Bits.Volume)
				{
						Audio_SetVolume(SysConfig.Bits.Volume);
				}
				LockModifySysConfigOpera(SysConfigBk.B32,SysConfig.B32);
				SysConfigBk.B32=SysConfig.B32;
				IntFlashWrite(SysConfig.B8,SYS_CONFIG_ADDR,sizeof(SysConfig_t)); 
		}
}
/******************************************************************************/
/*
//д������״̬��Ϣ
input:   none
output   none
return   none 
*/
/******************************************************************************/
void LockWritePerpLcokInfo(void)
{
		uint8_t i=0;
		for(i=0;i<sizeof(PeripLock_t);i++)
		{
				if(PeripLockBk.PripBuf[i]!=PeripLock.PripBuf[i])
				{
						PeripLockBk=PeripLock;
						IntFlashWrite(PeripLock.PripBuf,SYS_PERIP_LOCK_ADDR,sizeof(PeripLock_t)); 
						return;
				}
		}
}
/***************************************************************************/
/*
-----------------���ײ���������-------------------
. �ȵ���SN�����ַ���λ�ã�1~3������2~4������5~6����
. ����һ�����������1~9��
. �������ַ���ֵ���������� 
. ��10ȡ��
. ȥ����һ������ ���������ȡ��
. ���µ����ֲ���������

--------------------------����-------------------------
SN�� 786930
.����λ�� -> 697803
.���������-> 2
.��� -> [6+2,9+2,7+2��8+2��0+2��3+2]
.ȡ�� -> [8��1��9��0��2��5 ]
.ȥ����һ������ ���������ȡ�� ->[2,1,9,0,2,5]
.���������� 219025
*/


/****************************************************************************/

/******************************************************************************/
/*
//������У��
input:   ActCode1--������6�ֽ��ַ�
output   none
return   1-OK 0-NOK 
*/
/******************************************************************************/
uint8_t CheckSnActiveCode(char *ActCode1) 
{
		uint8_t i=0;
		char newSn[7]={0};
		uint8_t arad=ActCode1[0];        //��һ��Ϊ�����
		//����˳��
		newSn[0]=SnNum[2]-'0';
		newSn[1]=SnNum[3]-'0';
		newSn[2]=SnNum[0]-'0';
		newSn[3]=SnNum[1]-'0';
		newSn[4]=SnNum[5]-'0';
		newSn[5]=SnNum[4]-'0';
		
		for(i=0;i<6;i++)
		{
				newSn[i]+=(arad-'0');         //�������
				newSn[i]%=10;
				newSn[i]+='0';
		}
		newSn[0]=arad;
		
		printf("actcode: %s",newSn);
		
		
		for(i=0;i<6;i++)
		{
				if(newSn[i]!=ActCode1[i])
				{
						break;
				}
		}
		if(i<6)return 0;
		
		return 1;
}
/******************************************************************************/
/*
//δ����״̬��ʾ
input:   none
output   none
return   none 
*/
/******************************************************************************/
void SysActCheck(void)
{
		static uint8_t AlarmAct=0;
	
		if(SysConfig.Bits.Activate==0 && AlarmAct==0 && GetTimerElapse(0)>1000)
		{
				AlarmAct=1;
				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Please activate the product":"�뼤���Ʒ"),UNBREAK);
		}
}

/******************************************************************************/
/*
//�ָ���������
input:   none
output   none
return   none 
*/
/******************************************************************************/
void FactoryReset(void)
{
		uint16_t i=0;

		//sysconfig����
//		SysConfig.Bits.Activate=0; ����ϵͳ������Ϣ��������ԭ��Ĭ������
		SysConfig.Bits.Language=0;
		SysConfig.Bits.CardLock=0;
		SysConfig.Bits.ChildLock=0;
		SysConfig.Bits.FaceLock=0;
		SysConfig.Bits.MutiIdy=0;
		SysConfig.Bits.FpLock=0;
		SysConfig.Bits.SupPowSave=0;
		SysConfig.Bits.TampAct=1;
		SysConfig.Bits.KeyPADLock=0;
		SysConfig.Bits.KeepOpen=0;
		SysConfig.Bits.Volume=1;              //0-0,1-5,2-10
		SysConfig.Bits.OTCode=0;
		SysConfig.Bits.PrivMode=0;
#ifdef NO_FACE_MODE
		SysConfig.Bits.PirOn=0;               //Ĭ�Ϲر�PIR
#else
		SysConfig.Bits.PirOn=1;               //Ĭ�ϴ�PIR
		if(SysConfig.Bits.Activate==0)SysConfig.Bits.PirOn=0;
#endif
		SysConfig.Bits.DiSel=0;
		SysConfig.Bits.TimeZone=8;            //Ĭ�϶�����
		SysConfig.Bits.Rsv=0;                 //����ֵ0
		SysConfig.Bits.Flag=0xA;
		IntFlashWrite(SysConfig.B8,SYS_CONFIG_ADDR,sizeof(SysConfig_t)); 
		//����������Ϣ
		for(i=0;i<4;i++)
		{
				PeripLock.Perip.FailTime[i]=0;
		}
		for(i=0;i<4;i++)
		{
				PeripLock.Perip.FailStart[i].day=0;
				PeripLock.Perip.FailStart[i].hour=0;
				PeripLock.Perip.FailStart[i].minute=0;
				PeripLock.Perip.FailStart[i].second=0;
				PeripLock.Perip.LockStart[i].day=0;
				PeripLock.Perip.LockStart[i].hour=0;
				PeripLock.Perip.LockStart[i].minute=0;
				PeripLock.Perip.LockStart[i].second=0;
		}
		PeripLock.Perip.Flag=PERIPHLOCK_FLAG;
		IntFlashWrite(PeripLock.PripBuf,SYS_PERIP_LOCK_ADDR,sizeof(PeripLock_t)); 
		//��һ��������
		memset((uint8_t*)&OneTimeCode,0,sizeof(OneTimeCode_t));   
		IntFlashWrite((uint8_t*)&OneTimeCode,ONETIME_CODE_ADDR,sizeof(OneTimeCode_t));	
		//�������Ϣ
		TamperAlarm.Tamp.Flag=0xAA;
		TamperAlarm.Tamp.Time=10;                                    //Ĭ��10S���� 
		TamperAlarm.Tamp.Stat=EPORT_ReadGpioData(KEY_TAMPER_PIN);    //����״̬������
		TamperAlarm.Tamp.Day=0;
		TamperAlarm.Tamp.Hour=0;
		TamperAlarm.Tamp.Minute=0;
		TamperAlarm.Tamp.Second=0; 
		IntFlashWrite(TamperAlarm.TampBuf,TAMPER_ADDR,sizeof(TamperAlarm_t));
		//���ص�ѹ�����Ϣ
		LowBatAlarm.Bat.Flag=0xAA;
		LowBatAlarm.Bat.Value=0;
		LowBatAlarm.Bat.Day=0;
		LowBatAlarm.Bat.Hour=0;
		LowBatAlarm.Bat.Minute=0;
		LowBatAlarm.Bat.Second=0; 
		IntFlashWrite(LowBatAlarm.BatBuf,BAT_CHK_ADDR,sizeof(LowBatAlarm_t));   //��ʼ��
		//�������ʱ���
		NetTime.NT.Stat=0;
		NetTime.NT.Day=0;
		NetTime.NT.Hour=0;
		NetTime.NT.Minute=0;
		NetTime.NT.Second=0;
		NetTime.NT.Flag=0xAA;
		IntFlashWrite(NetTime.NTBuf,NET_TIME_ADDR,sizeof(NetTime_t)); 
		//��в��
		CoerceFp.CFP.Flag1=0xAA;CoerceFp.CFP.Flag2=0x55;CoerceFp.CFP.Flag3=0xAA;
		for(i=0;i<13;i++)
		{
				CoerceFp.CFP.CoerceFp[i]=0;
		}
		IntFlashWrite(CoerceFp.CoerceFpBuf,COERCE_FP_ADDR,sizeof(CoerceFp_t));
		//���û�����
		memset(Pwd.PwdGenStr[0].PwdBuf,0,PWD_MAX*sizeof(PwdGenStr_t));    //�������
		IntFlashWrite(Pwd.PwdGenStr[0].PwdBuf,PWD_DATA_ADDR,PWD_MAX*sizeof(PwdGenStr_t));
		//��ʱЧ����
		memset(Pwd.PwdTimStr[0].PwdTimBuf,0,PWD_TIM_MAX*sizeof(PwdExtStr_t));
		IntFlashWrite(Pwd.PwdTimStr[0].PwdTimBuf,PWD_TIM_DATA_ADDR,PWD_TIM_MAX*sizeof(PwdExtStr_t));
		//�忨
		memset(CardUser,0,sizeof(CardUser));
		IntFlashWrite((uint8_t*)&CardUser,CARD_DATA_ADDR,sizeof(CardUser));
}




















