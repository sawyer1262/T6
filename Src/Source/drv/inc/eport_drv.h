// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : eport_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef EPORT_DRV_H_
#define EPORT_DRV_H_

#include "memmap.h"
#include "eport_reg.h"

#define EPORT   ((EPORT_TypeDef *)(EPORT_BASE_ADDR))
#define EPORT1  ((EPORT_TypeDef *)(EPORT1_BASE_ADDR))
#define EPORT2  ((EPORT_TypeDef *)(EPORT2_BASE_ADDR))
#define EPORT3  ((EPORT_TypeDef *)(EPORT3_BASE_ADDR))
#define EPORT4  ((EPORT_TypeDef *)(EPORT4_BASE_ADDR))

extern unsigned char eport_isr_mark;

typedef enum
{
	EPORT_PULLDOWN,
	EPORT_PULLUP,
	EPORT_DISPULL 
} EPORT_PULL_MODE;

typedef enum
{
	/*EPORT0*/
	EPORT_PIN0 = 0,
	EPORT_PIN1,
	EPORT_PIN2,
	EPORT_PIN3,
	EPORT_PIN4,
	EPORT_PIN5,
	EPORT_PIN6,
	EPORT_PIN7,

	/*EPORT1*/
	EPORT_PIN8,
	EPORT_PIN9,
	EPORT_PIN10,
	EPORT_PIN11,
	EPORT_PIN12,
	EPORT_PIN13,
	EPORT_PIN14,
	EPORT_PIN15,
	
		/*EPORT2*/
	EPORT_PIN16,
	EPORT_PIN17,
	EPORT_PIN18,
	EPORT_PIN19,
	EPORT_PIN20,
	EPORT_PIN21,
	EPORT_PIN22,
	EPORT_PIN23,
	
		/*EPORT3*/
	EPORT_PIN24,
	EPORT_PIN25,
	EPORT_PIN26,
	EPORT_PIN27,
	EPORT_PIN28,
	EPORT_PIN29,
	EPORT_PIN30,
	EPORT_PIN31,
	
		/*EPORT4*/
	EPORT_PIN32,
	EPORT_PIN33,
	EPORT_PIN34,
	EPORT_PIN35,
	EPORT_PIN36,
	EPORT_PIN37,
	EPORT_PIN38,
	EPORT_PIN39,

}EPORT_PINx;

#define IS_EPORT_PINx(PIN) (((PIN) == EPORT_PIN0) || \
		                    ((PIN) == EPORT_PIN1) || \
		                    ((PIN) == EPORT_PIN2) || \
		                    ((PIN) == EPORT_PIN3) || \
		                    ((PIN) == EPORT_PIN4) || \
		                    ((PIN) == EPORT_PIN5) || \
		                    ((PIN) == EPORT_PIN6) || \
												((PIN) == EPORT_PIN7) || \
		                    ((PIN) == EPORT_PIN8) || \
		                    ((PIN) == EPORT_PIN9) || \
		                    ((PIN) == EPORT_PIN10)|| \
		                    ((PIN) == EPORT_PIN11)|| \
		                    ((PIN) == EPORT_PIN12)|| \
												((PIN) == EPORT_PIN13)|| \
		                    ((PIN) == EPORT_PIN14)|| \
		                    ((PIN) == EPORT_PIN15)|| \
												((PIN) == EPORT_PIN16)|| \
		                    ((PIN) == EPORT_PIN17)|| \
		                    ((PIN) == EPORT_PIN18)|| \
		                    ((PIN) == EPORT_PIN19)|| \
		                    ((PIN) == EPORT_PIN20)|| \
												((PIN) == EPORT_PIN21)|| \
		                    ((PIN) == EPORT_PIN22)|| \
		                    ((PIN) == EPORT_PIN23)|| \
												((PIN) == EPORT_PIN24)|| \
		                    ((PIN) == EPORT_PIN25)|| \
		                    ((PIN) == EPORT_PIN26)|| \
		                    ((PIN) == EPORT_PIN27)|| \
		                    ((PIN) == EPORT_PIN28)|| \
												((PIN) == EPORT_PIN29)|| \
		                    ((PIN) == EPORT_PIN30)|| \
		                    ((PIN) == EPORT_PIN31)|| \
												((PIN) == EPORT_PIN32)|| \
		                    ((PIN) == EPORT_PIN33)|| \
		                    ((PIN) == EPORT_PIN34)|| \
		                    ((PIN) == EPORT_PIN35)|| \
		                    ((PIN) == EPORT_PIN36)|| \
												((PIN) == EPORT_PIN37)|| \
		                    ((PIN) == EPORT_PIN38)|| \
		                    ((PIN) == EPORT_PIN39)   )


#define LEVEL_H        1
#define LEVEL_L        0

#define EPORT_INPUT    0
#define EPROT_OUTPUT   1

typedef enum
{
    LOW_LEVEL_INT = 0,
    HIGH_LEVEL_INT,
    RISING_EDGE_INT,
    FALLING_EDGE_INT,
    RISING_FALLING_EDGE_INT,

}EPORT_INT_MODE;

#define IS_EPORT_INT_MODE(MODE) (((MODE) == LOW_LEVEL_INT) || \
		                             ((MODE) == HIGH_LEVEL_INT) || \
		                             ((MODE) == RISING_EDGE_INT) || \
		                             ((MODE) == FALLING_EDGE_INT) || \
                                 ((MODE) == RISING_FALLING_EDGE_INT))


/*******************************************************************************
* Function Name  : EPORT_Init
* Description    : EPORT��ʼ��
* Input          : -PINx   �ж����ź�
*                  -IntMode �ж�ģʽ
*
* Output         : None
* Return         : None
******************************************************************************/
extern void EPORT_Init(EPORT_PINx PINx,EPORT_INT_MODE IntMode);


/*******************************************************************************
* Function Name  : EPORT_ConfigGpio
* Description    : EPORT���ó�GPIO��;
* Input          : - EPORT_PINx: EPORT Pin��where x can be 0~7 to select the EPORT peripheral.
*                  - GpioDir������GPIO����   GPIO_OUTPUT�����  GPIO_INPUT������
*
* Output         : None
* Return         : None
******************************************************************************/
extern INT8  EPORT_ConfigGpio( EPORT_PINx GpioNo, UINT8 GpioDir);


/*******************************************************************************
* Function Name  : EPORT_WriteGpioData
* Description    : ����EPORT_PINx��Ӧ���ŵĵ�ƽ
* Input          : - EPORT_PINx: EPORT Pin��where x can be 0~7 to select the EPORT peripheral.
*                  - bitVal�����õĵ�ƽ��Bit_SET������Ϊ�ߵ�ƽ  Bit_RESET������Ϊ�͵�ƽ
*
* Output         : None
* Return         : 0: ���óɹ�    other������ʧ��
******************************************************************************/
extern void  EPORT_WriteGpioData(EPORT_PINx GpioNo, UINT8 bitVal);


/*******************************************************************************
* Function Name  : EPORT_ReadGpioData
* Description    : ��ȡEPORT_PINx��Ӧ���ŵĵ�ƽ
* Input          : - EPORT_PINx: EPORT Pin��where x can be 0~7 to select the EPORT peripheral.
*
* Output         : None
* Return         : Bit_SET:�ߵ�ƽ  Bit_RESET���͵�ƽ  -1��fail
******************************************************************************/
extern INT8  EPORT_ReadGpioData(EPORT_PINx GpioNo);


/*******************************************************************************
* Function Name  : EPORT_ITTypeConfig
* Description    : ����EPORT_PINx�ŵ��жϴ�����ʽ����ʹ���ж�
* Input          : - EPORT_PINx: EPORT Pin��where x can be 0~7 to select the EPORT peripheral.
*                  - IntMode:�жϴ�����ʽ
*                            LOW_LEVEL_INT: �͵�ƽ����
*                            HIGH_LEVEL_INT: �ߵ�ƽ����
*                            RISING_EDGE_INT:�����ش���
*                            FALLING_EDGE_INT:�½��ش���
*                            RISING_FALLING_EDGE_INT:�����ػ��½��ش���
*
* Output         : None
* Return         : None
******************************************************************************/
extern void EPORT_ITTypeConfig(EPORT_PINx IntNo, EPORT_INT_MODE IntMode);



/*******************************************************************************
* Function Name  : EPORT_ITConfig
* Description    : EPORT�ж�ʹ�ܿ���
* Input          : - EPORT_PINx: EPORT Pin��where x can be 0~7 to select the EPORT peripheral.
*                  - NewState��new state of the specified EPORT interrupts.
*                              This parameter can be: ENABLE or DISABLE.
*
* Output         : None
* Return         : None
******************************************************************************/
extern void EPORT_ITConfig(EPORT_PINx IntNo, FunctionalState NewState);


/*******************************************************************************
* Function Name  : EPORT_PullupConfig
* Description    : EPORT����ʹ�ܿ���
* Input          : - EPORT_PINx: EPORT Pin��where x can be 0~7 to select the EPORT peripheral.
*                  - NewState��new state of the specified EPORT Pullup.
*                              This parameter can be: ENABLE or DISABLE.
*
* Output         : None
* Return         : None
******************************************************************************/
extern void EPORT_PullupConfig(EPORT_PINx IntNo, FunctionalState NewState);

extern uint32_t EPORT_PullConfig(const EPORT_PINx IntNo, const EPORT_PULL_MODE eport_pull_mode);

#endif /* EPORT_DRV_H_ */
