#include "lpm.h"
#include "cpm_drv.h"
#include "key.h"
#include "debug.h"
#include "cache_drv.h"
#include "reset_drv.h"
#include "led.h"
#include "card.h"
#include "uart.h"  
#include "audio.h"
#include "uart_face.h"
#include "uart_fp.h"
#include "uart_back.h"
#include "bat.h"
#include "delay.h"
#include "pci_drv.h"

volatile uint32_t StopModeDisable = 0;                    //�͹���״̬
uint32_t ForcePowerDownCount=0;                           //ǿ�����߼���
uint32_t LpmErrorCount=0;
uint8_t HwDeinit=0;
/******************************************************************************/
/*
//������Χģ��͹���״̬
input:id-----��Ҫ���õ�ģ��ID
			mode---LPM_Disable:ģ�鹤��δ��ɣ����ܽ���͹���
						 LPM_Enable:ģ����У����Խ���͹���
output:none
*/
/******************************************************************************/
void LPM_SetStopMode(LPM_Id_t id, LPM_SetMode_t mode)    
{
    __disable_irq();
    switch(mode)
    {
        case LPM_Disable:
            StopModeDisable |= (uint32_t)id;
            break;
        case LPM_Enable:
            StopModeDisable &= ~(uint32_t)id;
            break;
        default:break;
    }
    __enable_irq();
}
/******************************************************************************/
/*
//��ȡ��ǰ�豸��Χ�͹���״̬
input:none
output:��ǰ�͹�����Ϣ
*/
/******************************************************************************/
uint32_t LPM_GetStopModeValue(void)
{
		uint32_t retDat=0;
		retDat=StopModeDisable;
    return retDat;
}

/******************************************************************************/
/*
//��λ�͹���ֵ
input:none
output:
*/
/******************************************************************************/
void LPM_ResettopModeValue(void)
{
		__disable_irq();
		StopModeDisable=0;
		__enable_irq();
}

/******************************************************************************/
/*
//�첽��ʱ����ʱ��������
CPM_HANDLEWAKEUP_ASYCTIMER_CYCLE����ʱʱ�䣬��Ϊ��λ
input:none
output:
*/
/******************************************************************************/
void AsycTimerInit(void)
{
		PCI_Init(EXTERNAL_CLK_SEL);
		PCI->PCI_CR &= ~(0xffu << 24); // NVSRAM don`t Clear
		PCI_InitWakeup();              // PCI reinit.
}
/******************************************************************************/
/*
//����͹�����Χ����
input:none
output:none
*/
/******************************************************************************/
void HW_DeInit(void)
{
		if(HwDeinit==1)return;
		SSI_FLASH_DeInit();      //FLASH����PD
		TampSetButtonDeInit();   //SET��TAMPER�������� 
		Key_DeInit();            //��������SLEEP
		Led_DeInit();            //��LED
		CardDeInit();            //�ر�ˢ�� 
		Audio_DeInit();          //�ر�����  
		Uart_Deinit();           //�ر���Ӧ����ģ��
		AsycTimerInit();         //��ʼ���첽��ʱ����������5s����
		PCI_AsyncTC_Init(CPM_HANDLEWAKEUP_ASYCTIMER_CYCLE*2000);
		HwDeinit=1;
}

void ATE_PowerDownDeinit(void)
{
		//ATE����ģʽ��ֻ��ָ�ƻ��ѣ����߶ϵ����ϵ�
		SysConfig.Bits.PirOn=0;  //��PIR��������PIR����
		SSI_FLASH_DeInit();      //FLASH����PD
		Led_DeInit();            //��LED
		CardDeInit();            //�ر�ˢ�� 
		Audio_DeInit();          //�ر�����  
		UartFace_Deinit();       //������Դ����IO
		UartFp_Deinit();         //ָ�Ƶ�Դ����IO
		//����ʼ�����������ѣ���ֹ���⻽����΢����
		UART_ConfigGpio(UART_BACK_SCI,UART_RX,GPIO_INPUT);         //RX���룬Ĭ������
		UART_ConfigGpio(UART_BACK_SCI,UART_TX,GPIO_OUTPUT);        //TX����� 
		UART_WriteGpioData(UART_BACK_SCI,UART_TX,Bit_SET);
		CPM_PowerOff_1p5();      //wakeup�����ܻ��ѣ�Ҳ��ָ���ܻ���
}

/******************************************************************************/
/*
//ѭ������Ƿ����͹���
input:none
output:none
*/
/******************************************************************************/
void LPM_EnterLowPower(void)
{                    
		//�ϵ���ʱ10S����͹���
		if((LPM_GetStopModeValue()&LPM_POWERON_ID)==LPM_POWERON_ID && GetTimerElapse(0)>10000)   //�ϵ���ʱ10S���͹���
		{
				LPM_SetStopMode(LPM_POWERON_ID,LPM_Enable);
		}
		//��OTAģʽ50Sǿ������
		if((StopModeDisable&LPM_OTA_ID)!=LPM_OTA_ID && (StopModeDisable&LPM_KEYPAD_ID)!=LPM_KEYPAD_ID 
			&& (StopModeDisable&LPM_FP_ID)!=LPM_FP_ID)
		{
				if(ForcePowerDownCount==0)                                       //��ʼǿ�����߼���
				{
						ForcePowerDownCount=GetTimerCount();
				}
				else if(GetTimerElapse(ForcePowerDownCount)>50000)               //�����泬ʱ50S��ǿ������
				{
						//ForcePowerDownCount=0;                                     //����0
						printf("StopModeDisable: 0x%08x\n",StopModeDisable);
						StopModeDisable=0;
				}
		}
		else
		{
				ForcePowerDownCount=0;
		}
		//WAKEUP����Ϊ�Ͳ��ܽ���͹���
		if(StopModeDisable == 0 && CPM_ReadGpioData(CPM_WAKEUP_PIN)==0)  
		{         
				uint16_t trycount=0;
				while(1)
				{
						HW_DeInit();
						//����͹��ģ������ʱWAKEUP����Ϊ�ߵĻ����ͽ����˵͹��ģ�5�ν����˵͹��ľ�����
						printf("goto POFF1.5... %08x\r\n", CPM->CPM_PADWKINTCR);
						CPM_PowerOff_1p5();
						if(++trycount>=5)
						{
								trycount=0;
								Set_POR_Reset();
						}
				}
		}
		else if(StopModeDisable == 0)     //wakeup���ų���Ϊ��   
		{
				if(LpmErrorCount == 0)
				{
						LpmErrorCount=GetTimerCount();
				}
				else if(GetTimerElapse(LpmErrorCount)>10000)    //wakeup ����Ϊ�ߣ���ʱ10Sֱ�����ߡ�
				{
						LpmErrorCount=0;
						HW_DeInit();
						Set_POR_Reset();
				}
		}
		else if(StopModeDisable>0)
		{
				LpmErrorCount=0;
		}
}














