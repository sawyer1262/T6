#include "rtc.h"
#include "rtc_drv.h"
#include "debug.h"
#include "delay.h"
#include "timer.h"
#include "lock_config.h"
#include "flash.h"
#include "wdt_drv.h"
#include "password.h"
#include "audio.h"
#include "bat.h"
#include "uart_back.h"
#include "password.h"
#include "time_cal.h"


t_ctimes RtcSetTime={2022,04,17,3,18,20,55};               //RTC��ʼʱ��


//оƬ�ϵ��ʼʱ�䣺1972/12/31/00:00:00


/******************************************************************************/
/*
//RTC��ʼ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void RtcInit(void)
{
		t_ctimes gt={0};
		t_ctimes st = RtcSetTime;  
		tm rtc_times={0}; 
	
		RTC_Init(EXTERNAL_CLK_SEL);           //ʹ��RTC�ⲿ���� 
		
		RTC_GetTime(&rtc_times);	            //��ȡоƬRTCʱ��
		printf("Data: %04d %02d:%02d:%02d\r\n",rtc_times.day,rtc_times.hour,rtc_times.minute,rtc_times.second);
		
		rtctime_to_mytime(&gt,rtc_times);	    //ת�����Զ���ʱ��
		printf("Data: %04d %02d %02d %02d:%02d:%02d\r\n",gt.years,gt.months,gt.days,gt.hours,gt.minutes,gt.seconds);
		printf("weekday:%02d\n",gt.weeks);
		if(gt.years==1972)                    //оƬ��ʼʱ��ת����Ϊ1972��
		{
				printf("set time\n");
				//�����ϵ磬������־
				SysConfig.Bits.FaceLock=0;
				SysConfig.Bits.FpLock=0;
				SysConfig.Bits.CardLock=0;
				SysConfig.Bits.KeyPADLock=0;
				PeripLock.Perip.FailTime[0]=0;
				PeripLock.Perip.FailTime[1]=0;
				PeripLock.Perip.FailTime[2]=0;
				PeripLock.Perip.FailTime[3]=0;
				//����ʱ��
				mytime_to_rtc(st,&rtc_times);
				RTC_SetTime(rtc_times);
		}
}
/******************************************************************************/
/*
//����������֤ʧ�ܴ������˴�û�ж�FLASH����д��������������ǰ����FLASH��
input:   idx--������ţ�
						PERIP_FACE_INDEX        0-����
						PERIP_FP_INDEX          1-ָ��
						PERIP_CODE_INDEX        2-��������
						PERIP_CARD_INDEX        3-��Ƭ

				 stat--����״̬ 0-��0������1-����+1
output   none
return   none 
*/
/******************************************************************************/
void LockUpdatePeripLockStat(uint8_t idx,uint8_t stat)
{
		if(idx>PERIP_CARD_INDEX)return;
	
		if(stat==0)
		{
				PeripLock.Perip.FailTime[idx]=0;
				return;
		}
		//�ж������Ƿ�����������Ѿ������Ͳ�����
		switch(idx)
		{
				case PERIP_FACE_INDEX:
						if(SysConfig.Bits.FaceLock==1)return;
						break;
				case PERIP_FP_INDEX:
						if(SysConfig.Bits.FpLock==1)return;
						break;
				case PERIP_CODE_INDEX:
						if(SysConfig.Bits.KeyPADLock==1)return;
						break;
				case PERIP_CARD_INDEX:
						if(SysConfig.Bits.CardLock==1)return;
						break;
				default:return;
		}
		tm rtc_times={0};
		if(PeripLock.Perip.FailTime[idx]<5)PeripLock.Perip.FailTime[idx]++;
		if(PeripLock.Perip.FailTime[idx]==1)               //�״���֤ʧ�ܣ���¼ʱ��
		{
				RTC_GetTime(&rtc_times);
				PeripLock.Perip.FailStart[idx].day=rtc_times.day;
				PeripLock.Perip.FailStart[idx].hour=rtc_times.hour;
				PeripLock.Perip.FailStart[idx].minute=rtc_times.minute;
				PeripLock.Perip.FailStart[idx].second=rtc_times.second;
		}
		else if(PeripLock.Perip.FailTime[idx]>=5)         //�����涨�������������裬����¼������ʼʱ�� 
		{
				switch(idx)
				{
						case PERIP_FACE_INDEX:
								SysConfig.Bits.FaceLock=1;
								break;
						case PERIP_FP_INDEX:
								SysConfig.Bits.FpLock=1;
								break;
						case PERIP_CODE_INDEX:
								SysConfig.Bits.KeyPADLock=1;
								break;
						case PERIP_CARD_INDEX:
								SysConfig.Bits.CardLock=1;         //lock mask 0x00000780
								break;
						default:return;
				}
				RTC_GetTime(&rtc_times);
				PeripLock.Perip.FailTime[idx]=0;
				PeripLock.Perip.LockStart[idx].day=rtc_times.day;
				PeripLock.Perip.LockStart[idx].hour=rtc_times.hour;
				PeripLock.Perip.LockStart[idx].minute=rtc_times.minute;
				PeripLock.Perip.LockStart[idx].second=rtc_times.second;
				AudioPlayVoice(GetVolIndex(SysConfig.Bits.Language?"Too many fails, locked for 3 minutes":"��֤�����������,ϵͳ����3����"),UNBREAK); 
		}
}
/******************************************************************************/
/*
//������������״̬,1S���һ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void LockCheckPeripLockStat(void)
{
		int32_t elapset=0;
		
		if(b1sFlag!=1)return;                  //1S���һ��
		b1sFlag=0;
	
		//��ص�ѹ��������
		Bat_PrintVoltage();
		//��֤������
		if(SysConfig.Bits.FaceLock==1)
		{
				elapset=GetPeripLockTimeElapse(&PeripLock.Perip.LockStart[PERIP_FACE_INDEX]);
			
				if(elapset>180 || elapset<0)        //����3���ӽ��������������ϵ磬�Ӵ�����
				{
						SysConfig.Bits.FaceLock=0;
				}
		}
		else if(PeripLock.Perip.FailTime[PERIP_FACE_INDEX]>0)
		{
				elapset=GetPeripLockTimeElapse(&PeripLock.Perip.FailStart[PERIP_FACE_INDEX]);
				if(elapset>300 || elapset<0)        //5���ӳ�ʱ���������ϵ磬��λʧ�ܼ���
				{
						PeripLock.Perip.FailTime[PERIP_FACE_INDEX]=0; 
				}
		}
		//��ָ֤����
		if(SysConfig.Bits.FpLock==1)
		{
				elapset=GetPeripLockTimeElapse(&PeripLock.Perip.LockStart[PERIP_FP_INDEX]);
				if(elapset>180 || elapset<0)
				{
						SysConfig.Bits.FpLock=0;
				}
		}
		else if(PeripLock.Perip.FailTime[PERIP_FP_INDEX]>0)
		{
				elapset=GetPeripLockTimeElapse(&PeripLock.Perip.FailStart[PERIP_FP_INDEX]);
				if(elapset>300 || elapset<0)      //5������0
				{
						PeripLock.Perip.FailTime[PERIP_FP_INDEX]=0; 
				}
		}
		//��֤������
		if(SysConfig.Bits.KeyPADLock==1)
		{
				elapset=GetPeripLockTimeElapse(&PeripLock.Perip.LockStart[PERIP_CODE_INDEX]);
				if(elapset>180 || elapset<0)
				{
						SysConfig.Bits.KeyPADLock=0;
				}
		}
		else if(PeripLock.Perip.FailTime[PERIP_CODE_INDEX]>0)
		{
				elapset=GetPeripLockTimeElapse(&PeripLock.Perip.FailStart[PERIP_CODE_INDEX]);
				if(elapset>300 || elapset<0)      //5������0
				{
						PeripLock.Perip.FailTime[PERIP_CODE_INDEX]=0; 
				}
		}
		//��֤ˢ����
		if(SysConfig.Bits.CardLock==1)
		{
				elapset=GetPeripLockTimeElapse(&PeripLock.Perip.LockStart[PERIP_CARD_INDEX]);
				if(elapset>180 || elapset<0)
				{
						SysConfig.Bits.CardLock=0;
						printf("card Lock resume\n");
				}
		}
		else if(PeripLock.Perip.FailTime[PERIP_CARD_INDEX]>0)
		{
				elapset=GetPeripLockTimeElapse(&PeripLock.Perip.FailStart[PERIP_CARD_INDEX]);
				if(elapset>300 || elapset<0)      //5������0
				{
						printf("alapset: %02d\n",elapset);
						PeripLock.Perip.FailTime[PERIP_CARD_INDEX]=0; 
				}
		}
		//��֤һ��������ʱЧ
		if(SysConfig.Bits.OTCode==1)     //һ����24СʱʧЧ
		{
				elapset=GetPeripLockTimeElapse(&OneTimeCode.StartTime);
				if(elapset>86400 || elapset<0)          //24СʱʧЧ
				{
						SysConfig.Bits.OTCode=0;
						printf("onetime code fail\n");
				}
		}
		//��֤��ʱ�����Ƿ�ʧЧ
		CheckTempCodeValid();
}
/******************************************************************************/
/*
//��ȡ��ǰʱ����ָ��ʱ��֮���ʱ���

input:   stime--ָ��ʱ��
output   none
return   ʱ��
            -1 ----- ��ǰʱ��С��ָ��ʱ��
            >0 ----- ʱ���
*/
/******************************************************************************/
int32_t GetPeripLockTimeElapse(mytm_t *stime)
{
		tm rtc_times={0};
		uint32_t timNow=0;
		uint32_t timStart=stime->hour*3600+stime->minute*60+stime->second;
		
		RTC_GetTime(&rtc_times);
		
		timNow=rtc_times.hour*3600+rtc_times.minute*60+rtc_times.second;
		
		if(rtc_times.day>=stime->day)
		{
				timNow+=(rtc_times.day-stime->day)*86400;
				return (timNow-timStart);
		}
		else
		{
				return -1;
		}
}
/******************************************************************************/
/*
//�������ͬ��ʱ�䣬1S����һ��

input:   stime--ָ��ʱ��
output   none
return   ʱ��
            -1 ----- ��ǰʱ��С��ָ��ʱ��
            >0 ----- ʱ���
*/
/******************************************************************************/
void SnycNetTime(void)
{
		static uint8_t GetTimeCmdSend=0;
		mytm_t tm={0};
		
		if(NetTime.NT.Stat==0)                           //δͬ����ʱ��
		{
				if(GetTimeCmdSend==0)
				{
						GetTimeCmdSend=1;
						UartBack_SendCmdData(REQ_TIME,NULL,0,0,1000);     //��ȡʱ��ָ����ط�
				}
		}
		else if(GetTimeCmdSend==0)                       
		{
				tm.day=NetTime.NT.Day;
				tm.hour=NetTime.NT.Hour;
				tm.minute=NetTime.NT.Minute;
				tm.second=NetTime.NT.Second;
				if(GetPeripLockTimeElapse(&tm)>28800)                 //8Сʱ��ʱһ��
				{
						UartBack_SendCmdData(REQ_TIME,NULL,0,0,1000);
						GetTimeCmdSend=1;
				}
		}
}



