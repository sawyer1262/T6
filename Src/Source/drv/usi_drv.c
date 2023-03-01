// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : usi_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "usi_drv.h"
#include "delay.h"
#include "cpm_drv.h"
#include "debug.h"
#include "libRegOpt.h"
#include "iccemv.h"

//#define BCTC_CONTACT_TEST	//BCTC�Ӵ�������Ҫ�򿪴˺�
#ifdef BCTC_CONTACT_TEST
	#define DATA_EPORT	0	//����7816����EPORT�����ӷ�ʽ
#endif

/******************************************************************************
 *  Global variables defined
 ******************************************************************************/

USI_TypeDef * g_USIReg = USI;


/*******************************************************************************
* Function Name  : USI_ConfigGpio
* Description    : USI���ó�GPIO��;
* Input          : - USIx    : USI�Ļ���ַ��USI1����USI2
*                  - USI_PINx��USI��Ӧ��PIN�ţ�ȡֵUSI_RST��USI_CLK��USI_DAT
*                  - GPIO_Dir������GPIO����   GPIO_OUTPUT�����  GPIO_INPUT������
*
* Output         : None
* Return         : None
******************************************************************************/
void USI_ConfigGpio(USI_TypeDef * USIx, USI_PIN USI_PINx, UINT8 GPIO_Dir)
{
	/* Check the parameters */
	assert_param(IS_USI_PINx(USI_PINx));
	assert_param(IS_GPIO_DIR_BIT(GPIO_Dir));
    
    if(USIx == USI2)
    {
        CHIP_RESET->RCR &= ~(0x00800000);
        USIx->USIPCR = 0;
        *(uint32_t *)0x40000004 = 0x77770001; //����IOCTRL USI 
        CPM_PWRCR_OptBits((0x03<<25),RESET);
        CPM->CPM_SLPCFGR = (CPM->CPM_SLPCFGR & ~0x0000FF00)|0x00003300;
        while((CPM->CPM_PWRSR & 0x04) != 0x04);
    }

	USIx->USIPCR |= 0xC0;	//config gpio

	if (GPIO_Dir == GPIO_OUTPUT)
	{
		USIx->USIDDR |= (1<<USI_PINx);//output
	}
	else if (GPIO_Dir == GPIO_INPUT)
	{
		USIx->USIDDR &= (~(1<<USI_PINx));//input
	}
}


/*******************************************************************************
* Function Name  : USI_ReadGpioData
* Description    : ��ȡUSI_PINx��Ӧ���ŵĵ�ƽ
* Input          : - USIx    : USI�Ļ���ַ��USI1����USI2
*                  - USI_PINx��USI��Ӧ��PIN�ţ�ȡֵUSI_RST��USI_CLK��USI_DAT
*
* Output         : None
* Return         : Bit_SET:�ߵ�ƽ  Bit_RESET���͵�ƽ
******************************************************************************/
UINT8 USI_ReadGpioData(USI_TypeDef * USIx, USI_PIN USI_PINx)
{
	UINT8 bitstatus = 0x00;

	/* Check the parameters */
	assert_param(IS_USI_PINx(USI_PINx));

	bitstatus = USIx->USIPDR;
	if (bitstatus &(Bit_SET<<USI_PINx))
		bitstatus = Bit_SET;
	else
		bitstatus = Bit_RESET;

	return bitstatus;
}


/*******************************************************************************
* Function Name  : USI_WriteGpioData
* Description    : ����USI_PINx��Ӧ���ŵĵ�ƽ
* Input          : - USIx    : USI�Ļ���ַ��USI1����USI2
*                  - USI_PINx��SPI��Ӧ��PIN�ţ�ȡֵUSI_RST��USI_CLK��USI_DAT
*                  - bitVal�����õĵ�ƽ��Bit_SET������Ϊ�ߵ�ƽ  Bit_RESET������Ϊ�͵�ƽ
*
* Output         : None
* Return         : None
******************************************************************************/
void USI_WriteGpioData(USI_TypeDef * USIx, USI_PIN USI_PINx, UINT8 bitVal)
{
	/* Check the parameters */
	assert_param(IS_USI_PINx(USI_PINx));

	if (bitVal == Bit_SET)
		USIx->USIPDR |= (Bit_SET<<USI_PINx);
	else
		USIx->USIPDR &= (~(Bit_SET<<USI_PINx));
}


/*******************************************************************************
* Function Name  : USI_Init
* Description    : USI��ʼ��
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void  USI_Init(void)
{
	//CHIP_RESET->RCR &= ~CRE;
}

/*******************************************************************************
* Function Name  : TransmitByte
* Description    : ����һ���ֽ�����
* Input          : �����͵��ֽ�data
*
* Output         : None
* Return         : None
******************************************************************************/
void TransmitByte(UINT8 data)
{
    while((g_USIReg->USISR & USISR_TDRE_MASK)==0);
    g_USIReg->USITDR = data;
	
    while((g_USIReg->USISR & USISR_TC_MASK)==0);		//ÿ���ַ��ĵ�11��etu����ʱ��ɣ������⵽error����Ȼ�����¸��ַ�
}

/*******************************************************************************
* Function Name  : ReceiveByte
* Description    : ����һ���ֽ�����
* Input          : None
*
* Output         : None
* Return         : ���ؽ��յ����ֽ�
******************************************************************************/
UINT8 ReceiveByte(void)
{
    UINT8 data;

    while((g_USIReg->USISR & USISR_RDRF_MASK) != USISR_RDRF_MASK);
    data = g_USIReg->USIRDR;
    return data;
}


/*******************************************************************************
* Function Name  : USI_DeInit
* Description    : USI������ʼ��
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void USI_DeInit(void)
{
	SC_RSTSET(0); 	//RST ����
	DelayMS(1);

//	g_USIReg->USIPDR &= 0xfd;                   //isoclk L
//	g_USIReg->USIPCR |= 0x40;	                //clk��ΪGPIOģʽ������CLKֹͣ���м�ʮus�ĸߵ�ƽ
	g_USIReg->USICSR |= 0x40;                   //����ʱ��ֹͣ��Ϊ�͵�ƽ
	DelayNETU(1);
	g_USIReg->USICSR &= 0x7F;                   //disable cclk
	g_USIReg->USICR1 = g_USIReg->USICR1 & 0xef;	//�ر�7816ģ��

	//IO��CLK��RST�����0�������ͨ�����������VCC����
	g_USIReg->USIPCR = 0xC7;		            //IO/CLK���GPIO
	g_USIReg->USIDDR = 0x07; 	                //isoclk, isorst, isodat output
	g_USIReg->USIPDR = 0x00;		            //���0
}

/*******************************************************************************
* Function Name  : USIMasterInit
* Description    : USI������ʼ��
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void USIMasterInit(void)
{
	unsigned char ret,i;

	//������ʱ�ӣ�����CLK������һ��ʱ������ʱ�ӣ������Ϲ淶
	g_USIReg->USICSR = g_ips_clk/4000000-1;  //fclk = fips/(ccps+1); fclk = 4M

	g_USIReg->USICSR |= 0x80;       //enable cclk

	g_USIReg->USIPDR &= 0xfe;       //isorst L
	g_USIReg->USIDDR = 0x03;        //isoclk, isorst output

	g_USIReg->USIBDR = 0x02;        // 372 F/D
	g_USIReg->USIIER = 0x0;         //disable interrupt

	g_USIReg->USIPCR = 0x07;		//IO/CLK���USIģʽ
	g_USIReg->USIPCR |= 0x20;       //isodat is open-drain
	g_USIReg->USICR1 = 0x14;		//ʹ��7816ģ��
	g_USIReg->USICR1 |= 0x80;	    //��SBλ��Ч��־�����������SBֻ����stopģʽ���յ���ʼλ�Ż���1

	//Clear RDR
	ret = g_USIReg->USISR;
	g_USIReg->USISR = ret; //clear ATR
	ret = g_USIReg->USIRDR;

	for(i=0;i<8;i++)		// 20131226,���ִ��1CF110.02�����ִ��1CF110.03��ȡATRʧ�����⣬ԭ����110.02�෢����N���ֽڣ��Ա����ڻ�������
	{
		//FIFO���Ϊ8�ֽڣ�so��8�ֽڼ��ɡ�����ͨ��USISR�ж��Ƿ�������
		ret = g_USIReg->USIRDR;
	}
}

/*******************************************************************************
* Function Name  : USIMasterHotInit
* Description    : USI������������ʼ��
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void USIMasterHotInit(void)
{
	UINT8 ret,i;
	
	ret = ret;

	g_USIReg->USIBDR = 0x02; // 372 F/D
	g_USIReg->USIIER = 0x0; //disable interrupt

	for(i=0;i<8;i++)		// 20131226,���ִ��1CF110.02�����ִ��1CF110.03��ȡATRʧ�����⣬ԭ����110.02�෢����N���ֽڣ��Ա����ڻ�������
	{
		//FIFO���Ϊ8�ֽڣ�so��8�ֽڼ��ɡ�����ͨ��USISR�ж��Ƿ�������
		ret = g_USIReg->USIRDR;
	}
}

/*******************************************************************************
* Function Name  : USI_OpenWTC
* Description    : USI��֡�ȴ�ʱ��
* Input          : �ȴ�ʱ��etu
*
* Output         : None
* Return         : None
******************************************************************************/
void USI_OpenWTC(UINT32 etu)
{
	g_USIReg->USICR1 &= ~0x02;		// WTEN�رգ�ͬʱ���Wait TimeOut Flag

	g_USIReg->USIWTRH = (UINT8)(etu>>16);	//���õȴ�ʱ��
	g_USIReg->USIWTRM = (UINT8)(etu>>8);
	g_USIReg->USIWTRL = (UINT8)etu;

	g_USIReg->USICR1 |= 0x02;		// WTENʹ��
}

/*******************************************************************************
* Function Name  : Card2_Power_On
* Description    : �������ϵ�
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void Card2_Power_On(void)
{
//	UINT32 temp;
	
	CPM->CPM_SLPCFGR &= ~(0x1<<15);
//	CPM->CPM_PWRCR &= ~(0x1<<26);
	CPM_PWRCR_OptBits((0x1<<26),RESET);

#if 0
	//�ȴ�card vcc����ȶ�
	while((CPM->CPM_PWRSR & 0x02) != 0x02);
#else
	DelayMS(2);
	if ((CPM->CPM_PWRSR & 0x02) != 0x02)	//����·���������û���ȶ����VCC���µ�
	{
//		CPM->CPM_PWRCR |= (0x1<<26);
		CPM_PWRCR_OptBits((0x1<<26),SET);
		CPM->CPM_SLPCFGR |= (0x1<<15);		
	}
#endif	
}

/*******************************************************************************
* Function Name  : Card2_Power_Off
* Description    : �������µ�
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void Card2_Power_Off(void)
{
//	CPM->CPM_PWRCR |= (0x1<<26);
	CPM_PWRCR_OptBits((0x1<<26), SET);
	CPM->CPM_SLPCFGR |= (0x1<<15);

	//�ȴ�card ldo��Դ�ͷ����
	DelayMS(5);
}

/*******************************************************************************
* Function Name  : Card2_Power_Switch
* Description    : ��ѹ�����л�
* Input          : mode ��ѹ�л�ģʽ
*
* Output         : None
* Return         : None
******************************************************************************/
void Card2_Power_Switch(UINT8 mode)
{
	Card2_Power_Off();

	CPM->CPM_SLPCFGR &= 0xffffcfff;

	CPM->CPM_SLPCFGR |= (mode<<12);

	Card2_Power_On();
}

/*******************************************************************************
* Function Name  : Card3_Power_Off
* Description    : �������µ�
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
//void Card3_Power_Off(void)
//{
//	CPM->CPM_PWRCR |= (0x1<<25);
//	CPM->CPM_SLPCFGR |= (0x1<<11);

//	//�ȴ�card ldo��Դ�ͷ����
//	DelayMS(5);
//}

/*******************************************************************************
* Function Name  : Card3_Power_On
* Description    : �������ϵ�
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
//void Card3_Power_On(void)
//{
//	CPM->CPM_SLPCFGR &= ~(0x1<<11);
//	CPM->CPM_PWRCR &= ~(0x1<<25);

//#if 0
//	//�ȴ�card vcc����ȶ�
//	while((CPM->CPM_PWRSR & 0x04) != 0x04);
//#else
//	DelayMS(2);
//	if ((CPM->CPM_PWRSR & 0x04) != 0x04)	//����·���������û���ȶ����VCC���µ�
//	{
//		CPM->CPM_PWRCR |= (0x1<<25);
//		CPM->CPM_SLPCFGR |= (0x1<<11);
//	}
//#endif	
//}

/*******************************************************************************
* Function Name  : Card3_Power_Switch
* Description    : ��ѹ�����л�
* Input          : mode ��ѹ�л�ģʽ
*
* Output         : None
* Return         : None
******************************************************************************/
//void Card3_Power_Switch(unsigned char mode)
//{
//	Card3_Power_Off();

//	CPM->CPM_SLPCFGR &= 0xfffffcff;

//	CPM->CPM_SLPCFGR |= (mode<<8);

//	Card3_Power_On();
//}









