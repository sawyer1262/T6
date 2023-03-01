#include "adc_drv.h"
#include "debug.h"
#include "bat.h"
#include "audio.h"
#include "led.h"
#include "lpm.h"
#include "timer.h"
#include "lock_config.h"
#include "uart_back.h"

uint8_t  BatLow=0;                          //�͵�ѹ��־
uint16_t batvolatge=0;
uint8_t BatPerCent=0;
uint16_t BatRaw[BAT_RAW_MAX]={0};           //��ѹADֵ����
uint16_t PowerOnCheckBatDelay=0;

const uint16_t BatPercentCalTab[11]={6500,6840,7020,7160,7240,7300,7360,7460,7580,7740,7940};



#define   ADC_REF       33					  //�ο���ѹ3.3V
#define   BAT_ADC_CHL 	ADCCH_1       //��ؼ��ADCͨ��




//�ϵ��ڵ���֮ǰ�����µ�ص�ѹ
//��ص�ѹһֱ�⣬����������̫�������ֱ�Ӷ���������ǰ�����ϴεõ�ѹֵ��
//�������ص�ѹ����̫�죬1����ֻ�������һ������ֵ������0.1V������ѹ��������
//�����˲�,����ǰҪ�ѵ�ѹֵ����ROM
/*****************************************************************************/
/* 
   ﮵�ص�ص������ص�ѹ���Զ��ձ��
11	 100%----4.20V
10�� 90%-----3.97V
09�� 80%-----3.87V
08�� 70%-----3.79V
07�� 60%-----3.73V
06�� 50%-----3.68V
05�� 40%-----3.65V
04�� 30%-----3.62V
03�� 20%-----3.58V
02�� 10%-----3.51V
01�� 5%------3.42V
00�� 0%------3.00V

	 �ɵ�ص�ص������ص�ѹ���Զ��ձ��
*/
/******************************************************************************/

/******************************************************************************/
/*
//����ص�ѹ--100msһ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void Bat_GetVoltage(void)
{
		uint32_t resVC=0,i=0;
		
		resVC = ADC_GetConversionValue(BAT_ADC_CHL)&0xFFFF;         //Vref���Žӵ�3.3V
		
	//	resVC = 323*ADC_REF*resVC/4095+30;                          //(33/4095)*res;y=kx+b
//		resVC = 26*resVC/10+30; 
//			resVC+=500;                                             //���0.9����Ϊ����0.9Vƫִ 
		
		for(i=(BAT_RAW_MAX-1);i>0;i--)
		{
				BatRaw[i]=BatRaw[i-1];
		}
		BatRaw[0]=resVC;
}

/******************************************************************************/
/*
//��ȡ��ذٷֱ�
input:   none
output   none
return   none 
*/
/******************************************************************************/
uint8_t GetVoltagePercent(uint16_t volt)
{
		uint8_t retc=0;
		uint8_t i=0;
		for(i=0;i<11;i++)
		{
				if(volt<BatPercentCalTab[i])break;
		}
		if(i==0)
		{
				retc=0;
		}
		else if(i<11)
		{
				retc=(uint8_t)((uint16_t)(volt-BatPercentCalTab[i-1])*10/(BatPercentCalTab[i]-BatPercentCalTab[i-1]))+(i-1)*10;
		}
		else 
		{
				retc=100;
		}
		return retc;
}

/******************************************************************************/
/*
//�����ص�ѹ��1Sһ��
input:   none
output   none
return   none 
*/
/******************************************************************************/
void Bat_PrintVoltage(void)
{
		uint16_t i=0,min=0,max=0;
		uint16_t numsum=0,numcal=0;
		uint32_t volsum=0;
		uint16_t batval=0;
		tm rtc_times={0};
		static uint16_t lowCount=0;
		
		min=max=BatRaw[0];            
		
		for(i=0;i<BAT_RAW_MAX;i++)
		{
				if(BatRaw[i]==0)break;
				if(BatRaw[i]<min)min=BatRaw[i];
				if(BatRaw[i]>max)max=BatRaw[i];
		}
		if(i==0)return;             //������δ�����ݣ������ϵ���ʱ5S
		numcal=numsum=i;
		for(i=0;i<numsum;i++)
		{
				if(BatRaw[i]<(max-BAT_CHK_GAP))      //�����仯����ֵ����
				{
						numcal--;
				}
				else
				{
						volsum+=BatRaw[i];
				}
		}
		if(numcal==0)return;        //������û����Ч����
		batval=volsum/numcal;
		batvolatge = 26*batval/10; 
//		printf("numsum:%2d;numcal:%2d\n",numsum,numcal);
//		printf("batval:%4d\n",batval);
//		printf("batvoltage:%4d\n",batvolatge);
		
		if(LowBatAlarm.Bat.Value==0)                    //�״��ϵ�
		{
				RTC_GetTime(&rtc_times);               
				LowBatAlarm.Bat.Day=rtc_times.day;
				LowBatAlarm.Bat.Hour=rtc_times.hour;
				LowBatAlarm.Bat.Minute=rtc_times.minute;
				LowBatAlarm.Bat.Second=rtc_times.second;
				LowBatAlarm.Bat.Value=batvolatge;
				IntFlashWrite(LowBatAlarm.BatBuf,BAT_CHK_ADDR,sizeof(LowBatAlarm_t));
		}
		
		if(batvolatge<(LowBatAlarm.Bat.Value-100))     //���糬��0.1V���ж�ʱ���Ƿ񳬹�1����
		{
				mytm_t tm={0};
				tm.day=LowBatAlarm.Bat.Day;
				tm.hour=LowBatAlarm.Bat.Hour;
				tm.minute=LowBatAlarm.Bat.Minute;
				tm.second=LowBatAlarm.Bat.Second;
				if(GetPeripLockTimeElapse(&tm)>60)
				{
						RTC_GetTime(&rtc_times);               
						LowBatAlarm.Bat.Day=rtc_times.day;
						LowBatAlarm.Bat.Hour=rtc_times.hour;
						LowBatAlarm.Bat.Minute=rtc_times.minute;
						LowBatAlarm.Bat.Second=rtc_times.second;
						if(batvolatge<LowBatAlarm.Bat.Value-150)
						{
								LowBatAlarm.Bat.Value-=150;
						}
						else
						{
								LowBatAlarm.Bat.Value=batvolatge;
						}
						IntFlashWrite(LowBatAlarm.BatBuf,BAT_CHK_ADDR,sizeof(LowBatAlarm_t));
				}
		}
		//�����ذٷֱ�
		BatPerCent=GetVoltagePercent(LowBatAlarm.Bat.Value);
		//printf("vol percent:%02d\n",BatPerCent);
		if(batvolatge<LIBAT_LOW_VAL)       //��ѹ,����һ���⣬��ֹ��Ϊ���ߵ�Դ������
		{
				if(++lowCount<3)return;
		}

		else
		{
				lowCount=0;
		}
		if(LowBatAlarm.Bat.Value<LIBAT_LOW_VAL && batvolatge<LIBAT_LOW_VAL)       //��ѹ,����һ���⣬��ֹ��Ϊ���ߵ�Դ������
		{
				if(BatLow==0)
				{
						uint8_t temp[4]={1,batvolatge%256,batvolatge/256,BatPerCent};
						BatLow=1;
						UartBack_SendCmdData(ALARM_REPORT,temp,4,1,1000);
				}
		}
}






















