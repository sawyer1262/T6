// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : pwm_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef PWM_DRV_H_
#define PWM_DRV_H_

#include "pwm_reg.h"

#define PWM        ((PWM_TypeDef *)(PWM_BASE_ADDR))

//PWM_PORT DEFINITION
#define PWM_PORT0		0
#define PWM_PORT1		1
#define PWM_PORT2		2
#define PWM_PORT3		3

#define IS_PWM_PORTx(port) (((port) == PWM_PORT0) || \
                            ((port) == PWM_PORT1) || \
                            ((port) == PWM_PORT2) || \
                            ((port) == PWM_PORT3))


//PWM Clock divider
#define PWM_CLK_DIV_1   4
#define PWM_CLK_DIV_2   0
#define PWM_CLK_DIV_4   1
#define PWM_CLK_DIV_8   2
#define PWM_CLK_DIV_16  3
#define IS_PWM_CLK_DIV(div) (((div) == PWM_CLK_DIV_1) || \
                             ((div) == PWM_CLK_DIV_2) || \
                             ((div) == PWM_CLK_DIV_4) || \
                             ((div) == PWM_CLK_DIV_8) || \
                             ((div) == PWM_CLK_DIV_16))


#define PWM_PULLUP_EN   1
#define PWM_PULLUP_DIS  0

#define IS_PWM_PULLUP_STA(sta) (((sta) == PWM_PULLUP_EN) || \
                                 ((sta) == PWM_PULLUP_DIS))

/******************************************************************************
* Function Name  : PWM_Output_IntEnable
* Description    : PWM�����ж�ʹ��
* Input          : - - PWMx : PWM channel��where x can be 0, 1, 2, 3 to select the PWM peripheral.
*
* Output         : None
* Return         : None
 ******************************************************************************/
extern void PWM_Output_IntEnable(UINT8 PWMx);

/*******************************************************************************
* Function Name  : PWM_OutputInit
* Description    : PWM�����ʼ�����������
* Input          : - PWMx : PWM channel��where x can be 0, 1, 2, 3 to select the PWM peripheral.
*                  - PWM_Prescaler: PWMԤ��Ƶֵ
*                  - PWM_Csr��Clock Source Selection
*                             ȡֵ��PWM_CLK_DIV_1��PWM_CLK_DIV_16��PWM_CLK_DIV_8��PWM_CLK_DIV_4��PWM_CLK_DIV_2
*                  - PWM_Period��PWM Counter/Timer Loaded Value, data range : 65535~0 (Unit : 1 PWM clock cycle)
*                                One PWM cycle width = PWM_Period + 1
*                  - PWM_Width��PWM Comparator Register Value;data range : 65535~0 (Unit : 1 PWM clock cycle)
*                               used to determine PWM output duty ratio
*                               PWM_Width >= PWM_Period : PWM output is always high
*                               PWM_Width >= PWM_Period : PWM output high = (PWM_Width + 1) unit
*                               PWM_Width = 0 : PWM output high = 1 unit
*                  - PWM_CHxINV_Flg��Timer  Inverter ON/OFF flag
*                                    1 = Inverter ON
*                                    0 = Inverter OFF
*
* Output         : None
* Return         : None
*******************************************************************************/
extern void PWM_OutputInit(UINT8 PWMx, UINT8 PWM_Prescaler, UINT8 PWM_Csr, UINT16 PWM_Period,  UINT16 PWM_Width, UINT8 PWM_CHxINV_Flg);

/*******************************************************************************
* Function Name  : PWM_InputInit
* Description    : PWM�����ʼ�����ɴ����ж�
* Input          : - - PWMx : PWM channel��where x can be 0, 1, 2, 3 to select the PWM peripheral.
*                  - PWM_Prescaler: PWMԤ��Ƶֵ
*                  - PWM_Csr��Clock Source Selection
*                             ȡֵ��PWM_CLK_DIV_1��PWM_CLK_DIV_16��PWM_CLK_DIV_8��PWM_CLK_DIV_4��PWM_CLK_DIV_2
*                  - PWM_Period��PWM Counter/Timer Loaded Value, data range : 65535~0 (Unit : 1 PWM clock cycle)
*                                One PWM cycle width = PWM_Period + 1
*
* Output         : None
* Return         : None
*******************************************************************************/
extern void PWM_InputInit(UINT8 PWMx, UINT8 pwm_prescaler, UINT8 pwm_csr,UINT16 pwm_period);

/*******************************************************************************
* Function Name  : PWM_ISR
* Description    : PWM�жϴ�����
* Input          : - None
*
* Output         : None
* Return         : None
*******************************************************************************/
extern void PWM_ISR(void);

/*******************************************************************************
* Function Name  : PWM_Start
* Description    : PWM��
* Input          : - PWMx : PWM channel��where x can be 0, 1, 2, 3 to select the PWM peripheral.
*
* Output         : None
* Return         : None
*******************************************************************************/
extern void PWM_Start(UINT8 PWMx);

/*******************************************************************************
* Function Name  : PWM_Stop
* Description    : PWM�ر�
* Input          : - PWMx : PWM channel��where x can be 0, 1, 2, 3 to select the PWM peripheral.
*
* Output         : None
* Return         : None
*******************************************************************************/
extern void PWM_Stop(UINT8 PWMx);


/*******************************************************************************
* Function Name  : PWM_ConfigGpio
* Description    : PWM���ó�GPIO��;
* Input          : - PWMx: PWM channel��where x can be 0, 1, 2 to select the PWM peripheral.
*
*                  - SPI_Dir������GPIO����   GPIO_OUTPUT�����  GPIO_INPUT������
*                  - PullUp_En��������Ӧ�Ľ�����ʹ��
*                               PWM_PULLUP_EN:  enable����
*                               PWM_PULLUP_DIS: disable����
*
* Output         : None
* Return         : None
******************************************************************************/
INT8 PWM_ConfigGpio(UINT8 PWMx, UINT8 GPIO_Dir, UINT8 PullUp_En);


/*******************************************************************************
* Function Name  : PWM_ReadGpioData
* Description    : ��ȡPWMx��Ӧ���ŵĵ�ƽ
* Input          : - PWMx: PWM channel��where x can be 0, 1, 2 to select the PWM peripheral.
*                  - PWM_Port: 0,1,2,3
*
* Output         : None
* Return         : Bit_SET:�ߵ�ƽ  Bit_RESET���͵�ƽ  -1��fail
******************************************************************************/
INT8 PWM_ReadGpioData(UINT8 PWMx);

/*******************************************************************************
* Function Name  : PWM_WriteGpioData
* Description    : ����PWMx��Ӧ���ŵĵ�ƽ
* Input          : - PWMx: PWM channel��where x can be 0, 1, 2 to select the PWM peripheral.
*                  - PWM_Port: 0,1,2,3
*                  - bitVal�����õĵ�ƽ��Bit_SET������Ϊ�ߵ�ƽ  Bit_RESET������Ϊ�͵�ƽ
*
* Output         : None
* Return         : 0: ���óɹ�    other������ʧ��
******************************************************************************/
INT8 PWM_WriteGpioData(UINT8 PWMx, UINT8 bitVal);


#endif /* PWM_DRV_H_ */
