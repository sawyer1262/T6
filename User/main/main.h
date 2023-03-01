

#ifndef _TEST_DEMO_H_
#define _TEST_DEMO_H_

//////////////////////////////////////////////
//              test demo define            //
//////////////////////////////////////////////
//#define ADC_DEMO
//#define ALGO_DEMO
//#define APDU_DEMO
//#define BCTC_DEMO
//#define CCM_DEMO
#define DAC_AUDIO_DEMO
//#define DAC_DEMO
//#define DMA_DEMO
//#define DSP_DEMO
//#define EDMA_DEMO
//#define EFLASH_DEMO
//#define EPORT_DEMO
//#define GETSN_DEMO
//#define GPIO_DEMO
//#define I2C_DEMO
//#define LOCK_DISJTAG_DEMO
//#define LOWPOWER_DEMO
//#define PCI_DEMO
//#define PIT32_DEMO
//#define PWM_DEMO
//#define RESET_DEMO
//#define RTC_DEMO			//PAD_AVDD_BBAT�ܽ���Ҫ��3.3V��ѹ
//#define RTC_TIMESTAMP_DEMO		//PAD_AVDD_BBAT�ܽ���Ҫ��3.3V��ѹ
//#define SPI_DEMO
#define SSI_DEMO
//#define STARTUP_DEMO
//#define SUBC0_DEMO
//#define SYSTICK_DEMO
//#define TC_DEMO
#define TSI_DEMO
//#define UART_DEMO
//#define USB_DEMO
//#define WDT_DEMO
//#define XIP_DEMO

///////////////////////////////////////////////////
//            demo hander                        //
///////////////////////////////////////////////////

#ifdef ADC_DEMO
	#include "adc_demo.h"
#endif

#ifdef ALGO_DEMO
   #include "algo_demo.h"
#endif

#ifdef APDU_DEMO
   #include "apdu_demo.h"
#endif

#ifdef BCTC_DEMO
	#include "icc_demo.h"
#endif

#ifdef LOWPOWER_DEMO
   #include "lowpower_demo.h"
#endif

#ifdef CCM_DEMO
	 #include "ccm_demo.h"
#endif

#ifdef DAC_AUDIO_DEMO
#include "audio_demo.h"
#endif

#ifdef DAC_DEMO
	 #include "dac_demo.h"
#endif

#ifdef DMA_DEMO
   #include "DMA_demo.h"
#endif

#ifdef DSP_DEMO
	 #include "dsp_demo.h"
#endif

#ifdef EDMA_DEMO
   #include "EDMA_demo.h"
#endif

#ifdef EFLASH_DEMO
   #include "eflash_demo.h"
#endif

#ifdef EPORT_DEMO
   #include "eport_demo.h"
#endif

#ifdef GETSN_DEMO
	#include "get_sn_demo.h"
#endif

#ifdef GPIO_DEMO
	#include "gpio_demo.h"
#endif

#ifdef I2C_DEMO
   #include "i2c_demo.h"
#endif

#ifdef LOCK_DISJTAG_DEMO
   #include "lock_disjtag_demo.h"
#endif


#ifdef PCI_DEMO
	#include "pci_demo.h"
#endif

#ifdef PIT32_DEMO
	#include "pit32_demo.h"
#endif

#ifdef PWM_DEMO
	#include "pwm_demo.h"
#endif

#ifdef RESET_DEMO
	#include "reset_demo.h"
#endif

#ifdef RTC_DEMO
	#include "rtc_demo.h"
#endif

#ifdef RTC_TIMESTAMP_DEMO
	#include "time_stamp_api.h"
#endif

#ifdef SPI_DEMO
   #include "spi_demo.h"
#endif

#ifdef SSI_DEMO
   #include "ssi_demo.h"
#endif

#ifdef STARTUP_DEMO
   #include "startup_demo.h"
#endif

#ifdef SUBC0_DEMO
   #include "sub_c0_demo.h"
#endif

#ifdef SYSTICK_DEMO
	#include "systick_demo.h"
#endif

#ifdef TC_DEMO
   #include "tc_demo.h"
#endif

#ifdef TSI_DEMO
	#include "tsi_demo.h"
#endif

#ifdef UART_DEMO
   #include "uart_demo.h"
#endif

#ifdef USB_DEMO
   #include "usb_demo.h"
#endif

#ifdef WDT_DEMO
   #include "wdt_demo.h"
#endif

#ifdef XIP_DEMO
	 #include "xip_demo.h"
#endif








//#include "audio_drv.h"
//#include "dmac_drv.h"
//#include "dac_drv.h"
//#include "pci_drv.h"
//#include "cpm_drv.h"
//#include "pit32_drv.h"
//#include "pit32_reg.h"

//#include "led.h"     
#include "timer.h"
//#include "bat.h"
//#include "debug.h"
//#include "audio.h"
#include "flash.h"


/*****************************************************************************/
/*
todo:
1.������Χ��AD,DA,PWM,SPI FLASH��ˢ����               //����ɣ�����ˢ���ǿ�ȱ2����У���ʱֻ���˶�доƬ����
2.UARTָ�ƣ�������������ָ�ƶ��Ǵ���ͨѶ���������´��ڵ��շ����������OK��
3.�����Χģ��ĵ��ԣ�ȷ����Ӳ��IO��
4.Ŀǰ���ڵ������ǣ���2��LED�ڲ��������������Լ�USB��¼���������⡣
5.ˢ����ʹ���Զ��쿨����Ϊˢ���ʹ��������и��ţ���Ҫ��ʱ����





*/
/*****************************************************************************/



#endif

