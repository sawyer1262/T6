#ifndef __LED_H_                           
#define __LED_H_   
   

#include "eport_drv.h"
#include "i2c_drv.h"
#include "spi_drv.h"
#include "lock_config.h"



#define     PIR_LED_LIGHT_MAX         65535
#define     PIR_LED_LIGHT_MIN         0


//����LED��0-14
#define     LED_POWER_PIN     I2C_SCL
#define     LED_SCL_PIN       EPORT_PIN11
#define     LED_SDA_PIN       EPORT_PIN10
#define     LED_LE_PIN        EPORT_PIN9
#define     LED_OE_PIN        EPORT_PIN26            //OE��POWER����
//����PIR: R G B
#define     PWM_R_PIN         PWM_PORT2
#define     PWM_G_PIN         PWM_PORT1
#define     PWM_B_PIN         PWM_PORT0

//�˵�LED��ʾMASK
#define     LED_MASK_WAKE                      0x0001                   //����MASK
#define     LED_MASK_NORMAL                    0x3F7F//0xFEFC                   //������MASK
#define     LED_MASK_CONFIG_SEL                0X037A//0x5eC0                   //���˵�ѡ�����
#define     LED_MASK_NETCONF                   0X0032//0x4c00                   //�������ý���
#define     LED_MASK_SYSCONF                   0X037A//0x5e00                   //ϵͳ���ý���

#ifndef     NO_FACE_MODE                                                //�Ƿ�֧������ģ��               
#define     LED_MASK_ADDUSER                   0X017A//0x5e80                   //����û����� 
#define     LED_MASK_DELUSER                   0X037A//0x5ec0                   //ɾ���û�����
#define     LED_MASK_MODIFY_ADMIN              0X007A//0x5e00                   //�޸Ĺ���Ա���� 
#else
#define     LED_MASK_ADDUSER                   0X016A//0x5680
#define     LED_MASK_DELUSER                   0X036A//0x56c0
#define     LED_MASK_MODIFY_ADMIN              0X006A//0x5e00                   //�޸Ĺ���Ա���� 
#endif
//ɾ���û�
#define     LED_MASK_DELUSER_ONETIMECODE       0X0802//0x4010                   //ɾ��һ�����������
#define     LED_MASK_DELUSER_FACE              0X3F7E//0x7EFC                   //ɾ����������
#define     LED_MASK_DELUSER_FP                0X3F7E//0x7EFC                   //ɾ��ָ�ƽ���
#define     LED_MASK_DELUSER_ALL               0X0802//0x4010                   //ɾ�������û�����  
#define     LED_MASK_DELUSER_CODE              0X3F7E//0x7EFC                   //ɾ���������
#define     LED_MASK_DELUSER_CARD              0X3F7E//0x7EFC                   //ɾ����Ƭ����
//����û�
#define     LED_MASK_ADDUSER_FACE              0X0002//0x4000                   //�����������        
#define     LED_MASK_ADDUSER_CODE              0X3F7E//0x7EFC                   //����������   
#define     LED_MASK_ADDUSER_CARD              0X1102//0x4088                   //��ӿ�Ƭ����
#define     LED_MASK_ADDUSER_FP                0X0002//0x4000                   //���ָ�� 
//ϵͳ����
#define     LED_MASK_SYSCONF_SECURITY          0X0032//0x4c00                   //�߼���ȫ���ý���
#define     LED_MASK_SYSCONF_VOLUME            0X0072//0x4e00                   //�������ý���
#define     LED_MASK_SYSCONF_LANGUAGE          0X0032//0x4c00                   //�������ý���
#define     LED_MASK_SYSCONF_POWERSAVE         0X0032//0x4c00                   //��ʡ��ģʽ����
#define     LED_MASK_SYSCONF_KEEPOPEN          0X0032//0x4c00                   //����ģʽ����
#define     LED_MASK_SYSCONF_PIRSET            0X0032//0x4c00                   //PIR����ʹ�ܽ���

//ATE����
#define     LED_MASK_ATE_ALL_ON                0X7F7F
#define     LED_MASK_ATE_TOUCH                 0X6F7F
#define     LED_MASK_ATE_DOORBELL              0X0001
#define     LED_MASK_ATE_ADDCARD               0X1100

//LOGO��״̬
typedef enum{
		LOGO_OFF=0,
		LOGO_WAKEUP,
		LOGO_OPEN_SUCCESS,
		LOGO_OPEN_SUCCESS_MUTIL,                 //������֤��һ�γɹ�  
		LOGO_OPEN_FAIL,
		LOGO_COMMU_FAIL
}LogoLedStat_t;
//LOGO��RGB��PWMͨ��״̬
typedef struct{
	uint8_t state;
	uint8_t val;
	uint32_t sttime;
}LedChl_t;
//LED��λ
//�°�MCU��TSI0(#)��TSI10(8)������
typedef union{
		struct{
				uint16_t BELL:1;                       //BELL 
				uint16_t ASTERISK:1;                   //3
				uint16_t NUM7:1;                       //6
				uint16_t NUM4:1;                       //9
				uint16_t NUM1:1;                       //#
				uint16_t NUM2:1;                       //0
				uint16_t NUM3:1;                       //*
				uint16_t RSV1:1;                       //null
				uint16_t NUM5:1;                       //8
				uint16_t NUM6:1;                       //7 
				uint16_t NUM9:1;                       //4 
				uint16_t HASH:1;                       //1
				uint16_t NUM8:1;                       //5 
				uint16_t NUM0:1;                       //2
				uint16_t BAT:1;                       //LOCK
				uint16_t VIRTU:1;                      //NULL
		}bits;
		uint16_t ledval;
}LedVal_t;



extern LedVal_t LedVal;



void Led_Init(void);
void Led_DeInit(void);
void Led_UpdateVal(void);
void PirLed_SetBrightness(uint8_t chl,uint8_t percent);
uint8_t PirLed_GetBrightness(uint8_t chl);
void PirLed_Handle(void);
uint8_t PirLed_SetStat(LogoLedStat_t stat);
void Led_WakeupDeInit(void);

#endif



