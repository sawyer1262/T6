#include "audio_demo.h"
#include "eport_drv.h"
#include "audio.h"
#include "audio_service.h"
#include "lpm.h"
#include "timer.h"
#include "i2c_drv.h"
#include "lock_config.h"
#include "flash.h"
#include "delay.h"
#include "dac_drv.h"
#include "led.h"
#include "keypad_menu.h"

/******************************************************************************/
/*
//����������ɻص�����
input:   number -- ��ǰ��������number
output   none
return   none
*/
/******************************************************************************/
void Audio_Donecallback(uint32_t number)
{
		if(number==0)
		{
				if(MenuNow==Menu_KeyPadNormal && SysConfig.Bits.SupPowSave==1) //���˵�
				{
						if(SysConfig.Bits.Volume!=0)SysConfig.Bits.Volume=0;
				}
				LPM_SetStopMode(LPM_AUDIO_ID,LPM_Enable);
		}
}

/******************************************************************************/
/*
//�������ų�ʼ��
input:   none
output   none
return   none
*/
/******************************************************************************/
void Audio_Init(void)
{
		GenFadeDat();
		AudioTimerInit(mVoiceFreq);     //��ʼ����ʱ��
		Audio_PowerOn();
		AudioServiceInit(Audio_Donecallback);
		AudioPowerOnPlayMute();
}
/******************************************************************************/
/*
//����������������
input:   none
output   none
return   none
*/
/******************************************************************************/
void Audio_SetVolume(uint8_t sel)
{
		uint8_t vol=0;
		
		if(sel==0)
		{
				vol=0;
		}
		else if(sel==1)
		{
				vol=5;
		}
		else if(sel==2)
		{
				vol=10;
		}
		AudioHalSetVolume(vol);
}

#if 0
void DelayuS(vu32 delayuS)
{
	uint32_t k=0;
	while(delayuS --)
	{
			for(k=0;k<30;k++){;}
	}
}

uint8_t decDat[13312*4]={0};

void AudioTest(void)
{
		uint32_t i=0;
		uint32_t len=13312;
		int16_t dat=0;
		
		I2C_ConfigGpio(I2C3,LED_POWER_PIN,GPIO_OUTPUT);
		I2C_WriteGpioData(I2C3,LED_POWER_PIN,Bit_RESET);
	
		I2C_ConfigGpio(I2C2,AUDIO_EN_PIN,GPIO_OUTPUT);
		Audio_PowerOn();
		DAC_Init(LEFTALIGNED_12BITS, TRIGGER_SOFTWARE, DET_ON_LOW); 
		AudioDecoderInit();
		while(1)         //������������  
		{
				dat=AudioDecodeProc( SoundData[i] &0x0F );
			  Send_DAC_data((dat<<4)+0x8000); 
				DelayuS(62);		
			
				dat=AudioDecodeProc( SoundData[i] >> 4 );
			  Send_DAC_data((dat<<4)+0x8000);	
				DelayuS(62);	
				i++;

			
				if(i >= 13312)
				{
						printf("audio finish!\n");
						i = 0;
					  AudioDecoderInit();
				}
		}
}
#endif
/******************************************************************************/
/*
//�������ų���
input:   none
output   none
return   none
*/
/******************************************************************************/
void Audio_DeInit(void)
{
		Audio_PowerOff();
}
/******************************************************************************/
/*
//�������Ŵ򿪵�Դ
input:   none
output   none
return   none
*/
/******************************************************************************/
void Audio_PowerOn(void)
{
		I2C_ConfigGpio(I2C2,AUDIO_EN_PIN,GPIO_OUTPUT);
		DelayMS(5);
		I2C_WriteGpioData(I2C2,AUDIO_EN_PIN,Bit_SET);
}
/******************************************************************************/
/*
//�������Źرյ�Դ
input:   none
output   none
return   none
*/
/******************************************************************************/
void Audio_PowerOff(void)
{
		I2C_ConfigGpio(I2C2,AUDIO_EN_PIN,GPIO_OUTPUT);
		DelayMS(5);
		I2C_WriteGpioData(I2C2,AUDIO_EN_PIN,Bit_RESET);
}
/******************************************************************************/
/*
//��������
input:   num -- �������
         mode -- ����ģʽ����ϻ򲻴��
output   none
return   none
*/
/******************************************************************************/
void AudioPlayVoice(uint16_t num,PlayMode_t mode)
{
//		printf("audio num:0x%04x\n",num);
		if(num==0xFFFF || num==0)return;            //δ�ҵ���Ӧ�����ţ��˳�
		LPM_SetStopMode(LPM_AUDIO_ID,LPM_Disable);
		AudioServicePlay(num,mode);                    //
}
/******************************************************************************/
/*
//��������״̬����
input:   none
output   none
return   none
*/
/******************************************************************************/
void Audio_RunningHandle(void)
{
		AudioServiceRouting();
}
/******************************************************************************/
/*
//���������ַ���
input:   str -- �����ŵ�����
output   none
return   none
*/
/******************************************************************************/
void AudioPlayNumString(char* str)
{
		char *ptr=str;
	
		while(*ptr)
		{
				if(*ptr>='0' && *ptr<='9')
				{
						AudioPlayVoice(*ptr-0x30+GetVolIndex(SysConfig.Bits.Language?"zero":"0"),UNBREAK);
				}
				if(*ptr>='A' && *ptr<='Z')
				{
						AudioPlayVoice(*ptr-0x41+GetVolIndex("A"),UNBREAK);
				}
//				else
//				{
//						AudioPlayVoice(GetVolIndex("������"),BREAK);
//						break;
//				}
				ptr++;
		}
}























