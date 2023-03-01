#ifndef  __LPM_H_
#define  __LPM_H_



#include <stdint.h>


typedef enum
{                                     
		LPM_POWERON_ID            =(1 << 0),             //�ϵ�ȴ�
		LPM_KEYPAD_ID             =(1 << 1),             //���̴���
		LPM_CARD_ID               =(1 << 2),             //ˢ�����
		LPM_BACKLOCK_RX_ID        =(1 << 3),             //������պ�������
		LPM_BACKLOCK_TX_ID        =(1 << 4),             //���巢������
		LPM_BTN_PIR_ID            =(1 << 5),             //�����LED����
		LPM_AUDIO_ID              =(1 << 6),             //����ID
		LPM_BTN_BACK_ID           =(1 << 7),             //��尴��
		LPM_BATCHK_ID             =(1 << 8),             //��ؼ��
		LPM_FP_ID                 =(1 << 9),             //ָ�ƴ���
		LPM_FACE_ID               =(1 << 10),            //��������
		LPM_TAMP_ID               =(1 << 11),            //���𱨾�
		LPM_OTA_ID                =(1 << 12)             //OTA����ID    
}LPM_Id_t;


typedef enum
{
		LPM_Enable=0,
		LPM_Disable,
}LPM_SetMode_t;



/********************************************************************************************/
/*

����оƬ�ֲᣬ�͹��Ľ�ʹ��CPM_PowerOff_1p5ģʽ����ģʽ����Դ�����£�

	1>SPI(SS3PAD)�͵�ƽ
	2>USI1
  3>I2C1
	4>EPORT0:EPORT00-07
	5>TSI TOUCH
	6>RTC��ʱ����
	7>POR

������Ҫ�û��ѣ�
	1>���UART����:�����һ���жϽţ��ݶ�33��-GINT3
	2>PIR����:PIR-H1:�ݶ�13��WAKEUP
//  3>TSI TOUCH����     ����ҪIO��  
//	4>ָ�Ƽ�⣺TOUCHOUT����⵽��ָΪ�ߵ�ƽ���ѡ�
	7>��尴����KEY_BACK����������ģʽ�������ݶ�34�ţ��͵�ƽ����
	8>���𰴼���Leverage���ݶ�36��
	�ܵĻ���IO��4����NT_BACK,PIR-H1,KEY_BACK,LEVERAGE
	
	


�͹��Ĵ���

������������£�����������򿪵ȴ�����������ʱ���л��ѵĽӿ���4��:PIR-H1��⻽�ѣ����ͨѶ����NT_BACK,���ð�������KEY_BACK,���𱨾�LEVERAGE����



TODO��

��һ����ܷ��߲����»������ţ�ȷ���ܻ��ѡ�

J5���ӷ���
	
*/
/********************************************************************************************/


extern uint32_t ForcePowerDownCount;

void LPM_EnterLowPower(void);
void LPM_SetStopMode(LPM_Id_t id, LPM_SetMode_t mode);
uint32_t LPM_GetStopModeValue(void);
void LPM_ResettopModeValue(void);
void ATE_PowerDownDeinit(void);



#endif

