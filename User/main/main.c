#include "sys.h"
#include "debug.h"
#include "type.h"
#include "startup_demo.h"
#include "main.h"
#include "cpm_drv.h"
#include "common.h"


/*������������������������������������װ���ͼ����ŷ��䡪����������|
����       | QFN32 | QFN40 | QFN48 | QFN88 |
��������������������������������������������������������������������������������������|
SPI        |   2   |   1   |   3   |   3   |
��������������������������������������������������������������������������������������|
I2C        |   1   |   1   |   1   |   3   |   
��������������������������������������������������������������������������������������|
SSI        |   0   |   1   |   0   |   1   |
��������������������������������������������������������������������������������������|
UART       |   1   |   1   |   3   |   3   |
��������������������������������������������������������������������������������������|
ISO7816    |   2   |   0   |   0   |   1   |
��������������������������������������������������������������������������������������|
USB_OTG    |   1   |   1   |   1   |   1   |
��������������������������������������������������������������������������������������|
ADC        |   0   |   1   |   2   |   3   |
��������������������������������������������������������������������������������������|
DAC        |   0   |   0   |   1   |   1   |
��������������������������������������������������������������������������������������|
PWM(����)  |   3   |   4   |   2   |   4   |
��������������������������������������������������������������������������������������|
MCC        |   0   |   0   |   0   |   0   |
��������������������������������������������������������������������������������������|
TSI(����)  |   8   |   5   |  10   |  16   |
��������������������������������������������������������������������������������������|
EPORT(����)|  10   |  18   |  19   |  20   |
��������������������������������������������������������������������������������������|
SDIO(����) |   0   |   0   |   0   |   1   |
��������������������������������������������������������������������������������������|
C0 SUB I/O |   0   |   0   |   0   |   4   |
��������������������������������������������������������������������������������������|*/

/*������������������������������USB ֧���������������������������������|
��װ   | USB2.0 | USB1.1 |  host  | device |
��������������������������������������������������������������������������������������|
QFN32  | ������ |  ����  | ��֧�� |  ֧��  |
��������������������������������������������������������������������������������������|
QFN40  |  ����  |  ����  | ��֧�� |  ֧��  |
��������������������������������������������������������������������������������������|
QFN48  | ������ |  ����  | ��֧�� |  ֧��  |
��������������������������������������������������������������������������������������|
QFN88  | ������ |  ����  | ��֧�� |  ֧��  |
��������������������������������������������������������������������������������������|*/


#include "eport_drv.h"
#include "audio.h"
#include "led.h"
#include "key.h"
#include "keypad_menu.h"
#include "uart.h"
#include "uart_fp.h"
#include "password.h"
#include "lpm.h"
#include "uart_back.h"
#include "pci_drv.h"
#include "tc_drv.h"
#include "cache_drv.h"
#include "card.h"  
#include "uart_face.h"
#include "bat.h"
#include "delay.h"
#include "rtc.h"
#include "lock_config.h"
#include "wdt_drv.h"
#include "time_cal.h"


extern void IO_Latch_Clr(void);


void Reback_Boot(void)
{
#ifdef MY_DEBUG
//����ʱ�������գ��Է�оƬ����ס
	//	IO_Latch_Clr();
		EPORT_ConfigGpio(KEY_TAMPER_PIN,GPIO_INPUT);            
		EPORT_ConfigGpio(KEY_BACK_PIN,GPIO_INPUT);  
		EPORT_PullupConfig(KEY_BACK_PIN,ENABLE);            //ʹ������ 
    if(EPORT_ReadGpioData(KEY_TAMPER_PIN) == Bit_RESET)           //�����ɿ����ɿ�Ϊ��
    {
				DelayMS(20);
				if(EPORT_ReadGpioData(KEY_BACK_PIN) == Bit_RESET)      //��尴������ 
				{
						Demo_StartupConfig(Startup_From_ROM, ENABLE);
						while(1);
				}			
    }
#else
		DelayMS(20);
#endif
}

int main()
{
		e_CPM_wakeupSource_Status rst_source=*(e_CPM_wakeupSource_Status *)CPM_WAKEUPSOURCE_REG;         //��ȡ����Դ
		Sys_Init();                                                 //ϵͳ��ʼ��
		IO_Latch_Clr();
		Reback_Boot();
		UART_Debug_Init(SCI1,g_ips_clk,9600);                       //���Դ�ӡ��ʼ��
//		printf("CPM->CPM_PADWKINTCR: 0x%08x \n",CPM->CPM_PADWKINTCR);
//		printf("wakeup source: 0x%02x \n",rst_source);
//		printf("reset source: 0x%02x \n",Get_Reset_Status());
		switch(rst_source)
		{
					case WAKEUP_SOURCE_GIN3:                  //��崮�ڻ���
							LedVal.ledval=LED_MASK_WAKE;
#ifndef NO_FACE_MODE       //�����汾
									FaceIdyLock=1;       
#endif
							break;
//				case WAKEUP_SOURCE_WAKEUP:                //PIR����
//						LedVal.ledval=LED_MASK_WAKE;
//						break;
//				case WAKEUP_SOURCE_TSI:                   //������������
//						LedVal.ledval=LED_MASK_WAKE;
//						break;
//				case WAKEUP_SOURCE_GIN3:                  //��崮�ڻ���
//						LedVal.ledval=LED_MASK_WAKE;
//						break;
//				case WAKEUP_SOURCE_GIN4:                  //SET��������
//						LedVal.ledval=LED_MASK_WAKE;
//						break;
//				case WAKEUP_SOURCE_GIN5:                  //���𰴼�����
//						LedVal.ledval=LED_MASK_WAKE;
//						break;
				case WAKEUP_SOURCE_POR:                     //�ϵ縴λ�������еƣ����ϵ縴λֻ������ͼ��
						LedVal.ledval=LED_MASK_WAKE;
						PowerOnCheckBatDelay=5000; 
						break;
				default:
						LedVal.ledval=LED_MASK_WAKE;        
						PowerOnCheckBatDelay=0;
						break;
		}
		printf("allright here1\n");
		Led_Init();                         //�ȳ�ʼ��LED
		Timer_Init();                       //��ʱ����ʼ��
		SSI_FLASH_Init();                   //FLASH��ʼ��
		RtcInit();                          //RTC��ʼ�� 
		LockReadSysConfig();                //����ϵͳ������ 
		Audio_Init();                       //������ʼ�� 
		Uart_DriverInit();                  //����������ʼ��
		Uart_Init();                        //���ڳ�ʼ��
		TampSetButtonInit();                //����Ȱ�����ʼ��
		PassWordInit();                     //�����ʼ��    
		Key_Init();                         //����������ʼ��
		CardIoInit();                        //��ƬIO��ʼ��
		WDT_Init(0xffff);                   //���Ź���ʼ�� 
		WDT_FeedDog();                      //��ʼ��ι��
		Key_Scan();                         //�ϵ��һ��״̬����ֹ���ѵ������м��̵�
		MenuNow=Menu_KeyPadNormal;          //�������˵� 
		LPM_SetStopMode(LPM_POWERON_ID,LPM_Disable);   //�ϵ���ʱ5S
		printf("init ok\n");
		while(1)
		{
				MenuNow();                      //���в����˵�  
		}
}
