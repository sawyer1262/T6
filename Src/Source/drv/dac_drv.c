// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : dac_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "dac_drv.h"
#include "dac_reg.h"
#include "pit32_drv.h"
#include "eport_drv.h"
#include "type.h"
#include "debug.h"

unsigned char *data_buf;
unsigned int data_len;
unsigned int data_s;
unsigned char data_en=0;

#define DAC_DMA_TRANS

void DAC_Systick_ISR(void)
{
	if(data_en)
	{
		if(data_s<data_len)
		{
			Send_DAC_data(data_buf[data_s]);
			data_s++;
		}
		else
		{
			SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk; 
			data_en=0;	
		}
	}
}

static __inline void dac_enable(void);
static __inline void sw_trig(void);

static __inline void dac_enable(void)
{
	DAC->DAC_CR |= DACEN;
}

static __inline void sw_trig(void)
{
	DAC->DAC_SWTR |= SW_TRIG;
}

/*******************************************************************************
* Function Name  : DAC_Init
* Description    : DAC��ʼ��
* Input          : -data_format:
*                      LEFTALIGNED_8BITS��8bits�����
*                      LEFTALIGNED_12BITS��12bits�����
*                      RIGHTALIGNED_8BITS��8bits�Ҷ���
*                      RIGHTALIGNED_12BITS��12bits�Ҷ���
*                  -trigger_select:
*                      TRIGGER_SOFTWARE���������
*                      TRIGGER_EXTERNAL���ⲿ����(SS_SPI6)
*                      TRIGGER_PIT����ʱ������(PIT2)
*                  -external_mode:
*                      DET_ON_RISING���ⲿ������ʽΪ������
*                      DET_ON_FALLING���ⲿ������ʽΪ�½��ش���
*                      DET_ON_BOTH���ⲿ������ʽΪ˫�ش���
*                      DET_ON_HIGH���ⲿ������ʽΪ�ߵ�ƽ����
*                      DET_ON_LOW���ⲿ������ʽΪ�͵�ƽ����
* Output         : None
* Return         : None
******************************************************************************/
void DAC_Init(DAC_DATA_FORMAT data_format, DAC_TRIGGER_SEL trigger_select, DAC_EXT_TMOD external_mode)
{
	UINT32 cr_reg;
	
	if (LEFTALIGNED_8BITS == data_format)
	{
		cr_reg |= ALIGN;
	}
	else if (LEFTALIGNED_12BITS == data_format)
	{
		cr_reg |= (ALIGN | RES);
	}
	else if (RIGHTALIGNED_12BITS == data_format)
	{
		cr_reg |= RES;
	}
	else
		cr_reg = 0;
	
	cr_reg &= ~(TSEL_MASK | EXT_TMOD_MASK);
	cr_reg |= ((trigger_select << TSEL_SHIFT) | (external_mode << EXT_TMOD_SHIFT));
#ifdef DAC_DMA_TRANS
	cr_reg |= DMAEN;
#endif
	
	DAC->DAC_CR = cr_reg;
	DAC->DAC_TRIMR &= ~(0x1000000);
	
	dac_enable();
}
void DAC_close(void)
{
	DAC->DAC_CR &=~DACEN;
}
extern UINT32 g_ips_clk;
/*******************************************************************************
* Function Name  : DAC_Start
* Description    : DAC����
* Input          : -trigger_select:
*                      TRIGGER_SOFTWARE���������
*                      TRIGGER_EXTERNAL���ⲿ����(SS_SPI6)
*                      TRIGGER_PIT����ʱ������(PIT2)
* Output         : None
* Return         : None
******************************************************************************/
void DAC_Start(DAC_TRIGGER_SEL trigger_select,unsigned int rate)
{
	float us;
	if (TRIGGER_SOFTWARE == trigger_select)
		sw_trig();
	else if (TRIGGER_PIT == trigger_select)
	{
		us=1000000/(float)rate;
		PIT32_Init(PIT2, PIT32_CLK_DIV_1, g_ips_clk/1000000*us,FALSE);//config PIT2
	}
	else
	{
		NVIC_Init(3, 3, EPORT1_0_IRQn, 2);
		eport_isr_mark = 0;
		EPORT_ITTypeConfig(EPORT_PIN8, RISING_EDGE_INT);
		EPORT_ConfigGpio(EPORT_PIN8, GPIO_INPUT);
		while(0 == eport_isr_mark);
	}
}

/*******************************************************************************
* Function Name  : DAC_Wait_Load_Done
* Description    : DAC�ȴ��������
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void DAC_Wait_Load_Done(void)
{
	while(0 == (DAC->DAC_FSR & LD_DONE));
}

void DAC_Wait_Load_Cear(void)
{
   DAC->DAC_FSR|=LD_DONE;
	 while(DAC->DAC_FSR&LD_DONE)
	 {
	    ;
	 }	 
}	

void DAC_SW_Trig(void)
{
	sw_trig();
}

void Send_DAC_data(unsigned char data)
{
	DAC_Write_Data(data);
	DAC->DAC_SWTR |= SW_TRIG;
	DAC_Wait_Load_Done();
}

void DAC_Write_Data(UINT16 data)
{
	DAC->DAC_DR = data;
}

void DAC_Trim(void)
{
	DAC->DAC_TRIMR &= ~(0x1f);
	DAC->DAC_TRIMR |= 0x80;
	DAC->DAC_TRIMR |= 0x0e;
	DAC->DAC_TRIMR &= ~(0x80);
}
