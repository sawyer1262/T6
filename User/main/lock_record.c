#include "lock_record.h"
#include "lock_config.h"
#include "uart_back.h"
#include "string.h"
#include "password.h"
#include "card.h"
#include "flash.h"
#include "bat.h"
#include "time_cal.h"
#include "uart_face.h"
#include "uart_fp.h"


//�û�������¼ͷ
UserOperaHead_t UserOperaHead={0};
UserOpera_t UserOperaBk;
UserOpera_t UserOpera;


/******************************************************************************/
/*
//�û�������
input:   
				 ctrlType-��������

				 CTL_OPEN_LOCK  =0
				 CTL_CLOSE_LOCK =1
         CTL_DOOR_BELL	=2

	       usrType-�û�����:

		     FACE      =1
				 FP        =2
				 CODE      =3
				 CARD      =4
				 
         pidx-�û����

				 0-------����Ա
         1-99----��ͨ�û�
				 FE------һ���������û�������ֻ������뿪��PERIP_CODE_INDEX��
         FF------��ʾ�û�
				 
output   none
return   OK---1   NOK----0   
*/
/******************************************************************************/
uint8_t UserControlLock(uint8_t ctrlType,uint8_t usrType,uint8_t pIdx) 
{
		uint8_t temp[3]={0};
		UserOpera_t opt={OPERA_RESV,0};
		
		temp[0]=ctrlType;temp[1]=usrType;temp[2]=pIdx;
		
		if(ctrlType==CTL_OPEN_LOCK && usrType==CODE_OPEN)
		{
				if(Pwd.PwdGenStr[pIdx].Pwd.Type==COERCE)
				{
						temp[1]=5;       //в������
				}
		}
		else if(ctrlType==CTL_OPEN_LOCK && usrType==FP_OPEN)
		{
				if(CoerceFp.CFP.CoerceFp[pIdx/8]&=(1<<(pIdx%8)))
				{
						temp[1]=6;       //в��ָ��
				}
		}
		UartBack_SendCmdData(CTRL_LOCK,temp,3,1,1000);
		
		switch(ctrlType)
		{
				case CTL_OPEN_LOCK:
						opt.OpDet.OpType=OPEN_LOCK;
						opt.OpDet.OpDat.OPLOCK.Type=usrType;
						opt.OpDet.OpDat.OPLOCK.Dat[0]=pIdx;
						break;
				case CTL_DOOR_BELL:        
						opt.OpDet.OpType=DOOR_BELL;
						opt.OpDet.OpDat.DB.isBell=1;
						break;
				case CTL_CLOSE_LOCK:        //�Զ�����û�й���
				default:return 0;
		}
		LockWriteUserOpera(&opt);
		return 1;
}

/******************************************************************************/
/*
//����û�:����������������û���Ҳ������APPԶ������û�����Ҫ������
input:   
				 usrType-�û�����
         pIdx   -�û�ID
output   none
return   OK---1   NOK----0 
*/
/******************************************************************************/
uint8_t LockAddUser(uint8_t usrType,uint8_t pIdx) 
{
		UserOpera_t opt={OPERA_RESV,0};
		
		opt.OpDet.OpType=ADD_USER;
		opt.OpDet.OpDat.ADUSR.Type=usrType;
		if(usrType==PERIP_CODE_INDEX)
		{
				memcpy(&opt.OpDet.OpDat.ADUSR.Dat,&Pwd.PwdGenStr[pIdx].PwdBuf[2],PWD_LEN_MAX);  //����
		}
		else if(usrType==PERIP_CARD_INDEX)
		{
				opt.OpDet.OpDat.ADUSR.Dat[0]=pIdx;   //���
				memcpy(&opt.OpDet.OpDat.ADUSR.Dat[1],CardUser[pIdx].CardBuf,8);  //����
		}
		else
		{
				opt.OpDet.OpDat.ADUSR.Dat[0]=pIdx;   //���
		}
		LockWriteUserOpera(&opt);
		
		uint8_t cdat[3]={0};
		cdat[0]=0x01;
		cdat[1]=usrType;
		cdat[2]=pIdx;
		UartBack_SendCmdData(USER_CHG_REPORT,cdat,3,1,1000);        
		return 1;
}
/******************************************************************************/
/*
//ɾ���û�:������������ɾ���û���Ҳ������APPԶ��ɾ���û�
input:   
				 usrType-�û�����
         pIdx   -�û�ID
output   none
return   OK---1   NOK----0 
*/
/******************************************************************************/
uint8_t LockDelUser(uint8_t usrType,uint8_t pIdx) 
{
		UserOpera_t opt={OPERA_RESV,0};
										
		opt.OpDet.OpType=DEL_USER;
		opt.OpDet.OpDat.DELUSR.Type=usrType;
		opt.OpDet.OpDat.DELUSR.Dat[0]=pIdx;
		LockWriteUserOpera(&opt);
		
		uint8_t cdat[3]={0};
		cdat[0]=0x02;
		cdat[1]=usrType;
		cdat[2]=pIdx;
		UartBack_SendCmdData(USER_CHG_REPORT,cdat,3,1,1000);  
		return 1;
}

/******************************************************************************/
/*
//�ָ��������ò�����¼
input: none  
return OK---1   NOK----0
*/
/******************************************************************************/
uint8_t LockFactoryResetOpera(void)
{
		UserOpera_t opt={OPERA_RESV,0};
		opt.OpDet.OpType=FACTORY_RESET;
		opt.OpDet.OpDat.FR.isFactoryReset=1;
		LockWriteUserOpera(&opt);
		
		uint8_t cdat[3]={0};
		cdat[0]=0x02;
		cdat[1]=0XFF;
		cdat[2]=0XFF;
		UartBack_SendCmdData(USER_CHG_REPORT,cdat,3,1,1000);  
		return 1;
}

/******************************************************************************/
/*
//���𴥷���¼
input: none
return OK---1   NOK----0
*/
/******************************************************************************/
uint8_t LockTampTrigOpera(void)
{
		UserOpera_t opt={OPERA_RESV,0};
		opt.OpDet.OpType=TAMP_TRIG;
		opt.OpDet.OpDat.TPTRIG.isTampTrig=1;
		LockWriteUserOpera(&opt);
		
		uint8_t temp=2;
		UartBack_SendCmdData(ALARM_REPORT,&temp,3,1,1000);
		return 1;
}

/******************************************************************************/
/*
//�޸�ϵͳ���ò�����¼
input: none  
return OK---1   NOK----0
*/
/******************************************************************************/
uint8_t LockModifySysConfigOpera(uint32_t oldVal,uint32_t newVal)
{
		SysConfig_t oldv={0},newv={0};
		UserOpera_t opt={OPERA_RESV,0};
		
		oldv=(SysConfig_t)oldVal;
		newv=(SysConfig_t)newVal;
		
		if(oldVal==newVal)return 0;
		
		//������Ϣ
		if(oldv.Bits.Activate!=newv.Bits.Activate)
		{
				opt.OpDet.OpType=ACTIVATE_SET;
				opt.OpDet.OpDat.ACT.isActivate=newv.Bits.Activate;
				LockWriteUserOpera(&opt);
		}
		//���Ըı�
		if(oldv.Bits.Language!=newv.Bits.Language)
		{
				opt.OpDet.OpType=LANG_SET;
				opt.OpDet.OpDat.LAN.isChinese=newv.Bits.Language;
				LockWriteUserOpera(&opt);
		}
		//��˽ģʽ
		if(oldv.Bits.PrivMode!=newv.Bits.PrivMode)
		{
				opt.OpDet.OpType=PRIVMODE_SET;
				opt.OpDet.OpDat.PRIV.isEnPrivMode=newv.Bits.PrivMode;
				LockWriteUserOpera(&opt);
		}
		//ͯ������
		if(oldv.Bits.ChildLock!=newv.Bits.ChildLock)
		{
				opt.OpDet.OpType=CHILD_LOCK_SET;
				opt.OpDet.OpDat.CL.isEnChildLock=newv.Bits.ChildLock;
				LockWriteUserOpera(&opt);
		}
		//��ʡ��ģʽ
		if(oldv.Bits.SupPowSave!=newv.Bits.SupPowSave)
		{
				opt.OpDet.OpType=POWERSAVE_SET;
				opt.OpDet.OpDat.PS.isEnPowSave=newv.Bits.SupPowSave;
				LockWriteUserOpera(&opt);
		}
		//˫��֤ģʽ
		if(oldv.Bits.MutiIdy!=newv.Bits.MutiIdy)
		{
				opt.OpDet.OpType=MUTILIDY_SET;
				opt.OpDet.OpDat.MI.isEnMutiIdy=newv.Bits.MutiIdy;
				LockWriteUserOpera(&opt);
		}
		//����ʹ��
		if(oldv.Bits.TampAct!=newv.Bits.TampAct)
		{
				opt.OpDet.OpType=TAMPER_SET;
				opt.OpDet.OpDat.TP.isEnTamp=newv.Bits.TampAct;
				LockWriteUserOpera(&opt);
		}
		//����ģʽ
		if(oldv.Bits.KeepOpen!=newv.Bits.KeepOpen)
		{
				opt.OpDet.OpType=KEEP_OPEN_SET;
				opt.OpDet.OpDat.KP.isEnKeepOpen=newv.Bits.KeepOpen;
				LockWriteUserOpera(&opt);
		}
		//��������
		if(oldv.Bits.Volume!=newv.Bits.Volume)
		{
				opt.OpDet.OpType=VOLUME_SET;
				opt.OpDet.OpDat.VOLU.volume=newv.Bits.Volume;
				LockWriteUserOpera(&opt);
		}
		//������
		if(oldv.Bits.FaceLock!=newv.Bits.FaceLock)
		{
				opt.OpDet.OpType=FACE_LOCK_CHG;
				opt.OpDet.OpDat.FACEL.isFaceLock=1;
				LockWriteUserOpera(&opt);
		}
		//ָ����
		if(oldv.Bits.FpLock!=newv.Bits.FpLock)
		{
				opt.OpDet.OpType=FP_LOCK_CHG;
				opt.OpDet.OpDat.FPL.isFpLock=newv.Bits.FpLock;
				LockWriteUserOpera(&opt);
		}
		//������
		if(oldv.Bits.KeyPADLock!=newv.Bits.KeyPADLock)
		{
				opt.OpDet.OpType=KEYPAD_LOCK_CHG;
				opt.OpDet.OpDat.KEYL.isKeypadLock=newv.Bits.KeyPADLock;
				LockWriteUserOpera(&opt);
		}
		//��Ƭ��
		if(oldv.Bits.CardLock!=newv.Bits.CardLock)
		{
				opt.OpDet.OpType=CARD_LOCK_CHG;
				opt.OpDet.OpDat.CARDL.isCardLock=newv.Bits.CardLock;
				LockWriteUserOpera(&opt);
		}
		//���ɾ��һ��������
		if(oldv.Bits.OTCode!=newv.Bits.OTCode)
		{
				opt.OpDet.OpType=OTCODE_CHG;
				opt.OpDet.OpDat.OTCODE.isAdd=newv.Bits.OTCode;
				LockWriteUserOpera(&opt);
		}
		//PIR����
		if(oldv.Bits.PirOn!=newv.Bits.PirOn)
		{
				opt.OpDet.OpType=OTCODE_CHG;
				opt.OpDet.OpDat.PIR.isEnPir=newv.Bits.PirOn;
				LockWriteUserOpera(&opt);
		}
		
		tm RT={0};
		t_ctimes gt={0};
		RTC_GetTime(&RT);	//��ȡоƬRTCʱ��
		rtctime_to_mytime(&gt,RT);	//��ȡʱ���
		
		uint8_t dat[20]={0};
		dat[0]=newv.B8[0];
		dat[1]=newv.B8[1];
		dat[2]=newv.B8[2];
		dat[3]=newv.B8[3];
		dat[4]=BatPerCent;  
		dat[5]=(uint8_t)gt.years;
		dat[6]=(uint8_t)(gt.years>>8);
		dat[7]=gt.months;
		dat[8]=gt.days;
		dat[9]=gt.hours;
		dat[10]=gt.minutes;
		dat[11]=gt.seconds;
		dat[12]=gt.weeks;
		dat[13]=Pwd.PwdInfo.Item.PwdSum;
		dat[14]=Pwd.PwdInfo.Item.PwdTimSum;
		dat[15]=CardInfo.Card.CardSum;
		dat[16]=FaceUser.Sum;
		dat[17]=LdFp.ActFpSum;
		dat[18]=LockStat.Lock;
		dat[19]=TamperAlarm.Tamp.Time;
		UartBack_SendCmdData(SYSCONFIG_REPORT,dat,20,1,1000);   //�ϱ�ϵͳ����״̬
		return 1;
}

/******************************************************************************/
/*
//д�û�������¼
input:   
				 oprea-������¼��Ϣ
output   none
return   OK---1   NOK----0 
*/
/******************************************************************************/
uint8_t LockWriteUserOpera(UserOpera_t *oprea)
{
	//	UserOpera_t op=*oprea;
	  UserOpera=*oprea;
		tm rtc_times={0};
		RTC_GetTime(&rtc_times);
		UserOpera.OpDet.OpTime.day=rtc_times.day;
		UserOpera.OpDet.OpTime.hour=rtc_times.hour;
		UserOpera.OpDet.OpTime.minute=rtc_times.minute;
		UserOpera.OpDet.OpTime.second=rtc_times.second;
		
		return 1;
		
//		ExFlashWrite(UserOpera.OpDetBuf,USER_RECORD_DAT_ADDR+UserOperaHead.OpHead.WriteIndex*sizeof(UserOpera_t),sizeof(UserOpera_t));

//		if(UserOperaHead.OpHead.RecordSum<500)UserOperaHead.OpHead.RecordSum++;
//		UserOperaHead.OpHead.WriteIndex++;
//		if(UserOperaHead.OpHead.WriteIndex>=USER_RECORD_MAX_NUM)
//		{
//				UserOperaHead.OpHead.WriteIndex=0;
//		}
//		ExFlashWrite(UserOperaHead.OpHeadBuf,USER_RECORD_HEAD_ADDR,sizeof(UserOperaHead_t));
//		return 1;
}


void WriteUserOpera(void)
{
		uint8_t i=0;
	
		for(i=0;i<sizeof(UserOpera_t);i++)
		{
				if(UserOpera.OpDetBuf[i]!=UserOperaBk.OpDetBuf[i])
				{
						UserOperaBk=UserOpera;
					
						ExFlashWrite(UserOpera.OpDetBuf,USER_RECORD_DAT_ADDR+UserOperaHead.OpHead.WriteIndex*sizeof(UserOpera_t),sizeof(UserOpera_t));

						if(UserOperaHead.OpHead.RecordSum<500)UserOperaHead.OpHead.RecordSum++;
						UserOperaHead.OpHead.WriteIndex++;
						if(UserOperaHead.OpHead.WriteIndex>=USER_RECORD_MAX_NUM)
						{
								UserOperaHead.OpHead.WriteIndex=0;
						}
						ExFlashWrite(UserOperaHead.OpHeadBuf,USER_RECORD_HEAD_ADDR,sizeof(UserOperaHead_t));
						return;
				}
		}
}

/******************************************************************************/
/*
//���û�������¼
input:   
				 index-������¼������FFΪ����¼ͷ

output:  out--�����¼����

return OK---1   NOK----0
*/
/******************************************************************************/
uint8_t LockReadUserOpera(void *out,uint16_t index)
{
		if(index==0xFF)
		{
				UserOperaHead_t Head={0};
				ExFlashRead(Head.OpHeadBuf,USER_RECORD_HEAD_ADDR,sizeof(UserOperaHead_t));
				*((UserOperaHead_t*)out)=Head;
		}
		else if(index<USER_RECORD_MAX_NUM)
		{
				UserOpera_t Opera={OPERA_RESV,0};
				ExFlashRead(Opera.OpDetBuf,USER_RECORD_DAT_ADDR+index*sizeof(UserOpera_t),sizeof(UserOpera_t));
				*((UserOpera_t*)out)=Opera;
		}
		else return 0;
		
		return 1;
}














