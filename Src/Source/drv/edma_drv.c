// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : edma_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "debug.h"
#include "cpm_drv.h"
#include "edma_drv.h"
#include "spi_drv.h"


volatile UINT8 g_Edmac0ChMinDoneFlag = 0;
volatile UINT8 g_Edmac1ChMinDoneFlag = 0;
/*******************************************************************************
* Function Name  : EDMA_ISR
* Description    : EDMA�жϴ���
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void ENCR_IRQHandler(void)
{
	if((EDMA_CH0->EDMACCSR)&(EDMAC_MINORDONE_MASK))
	{
		g_Edmac0ChMinDoneFlag = 1;
		EDMA_CH0->EDMACCSR |= EDMAC_MINORDONE_MASK;
//		printf("EDMA-CH0-INT\r\n");
	}
	if((EDMA_CH1->EDMACCSR)&(EDMAC_MINORDONE_MASK))
	{
		g_Edmac1ChMinDoneFlag = 1;
		EDMA_CH1->EDMACCSR |= EDMAC_MINORDONE_MASK;
//		printf("EDMA-CH1-INT\r\n");
	}
}

/*******************************************************************************
* Function Name  : EDMA_ISR
* Description    : EDMA�жϴ���
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void ENCR1_IRQHandler(void)
{
	if((EDMA_CH0->EDMACCSR)&(EDMAC_MINORDONE_MASK))
	{
		g_Edmac0ChMinDoneFlag = 1;
		EDMA_CH0->EDMACCSR |= EDMAC_MINORDONE_MASK;
		printf("EDMA-CH0-INT\r\n");
	}
	if((EDMA_CH1->EDMACCSR)&(EDMAC_MINORDONE_MASK))
	{
		g_Edmac1ChMinDoneFlag = 1;
		EDMA_CH1->EDMACCSR |= EDMAC_MINORDONE_MASK;
		printf("EDMA-CH1-INT\r\n");
	}
}

/*******************************************************************************
* Function Name  : EDMA_CHxTran
* Description    : EDMA ���䴦��
* Input          :  - EDMA_CHx�� where x can be 0 to 1 to select the EDMA peripheral��
*                   - sendAddr��EDMAC Read Buffer Address
*                   - recvAddr��EDMAC Write Buffer Address
*                   - ttpye:    Channel Transfer Type ȡֵ��EDMAC_TTYPE0~EDMAC_TTYPE3
*                   - pn��    Channel Periperal Number
*                   - len�� Channel Minor Transfer Data Sum(in byte)
*
* Output         : None
* Return         : STATUS
******************************************************************************/
UINT16 EDMA_CHx(EDMA_TypeDef *EDMA_CHx, UINT32 sendAddr,  UINT32 recvAddr, UINT32 ttpye, UINT32 pn, UINT32 len)
{
	UINT32 timeout = SPI_TIMEOUT_MS(1000);
	UINT16 ret = STATUS_OK;

	/* Check the parameters */
	assert_param(IS_EDMA_ALL_PERIPH(EDMA_CHx));//
	assert_param(IS_EDMA_TTYPE(ttpye));
	assert_param(IS_EDMA_EDMA_PN(pn));

//	printf("dma_sendAddr = %08x\r\n", sendAddr);
//	printf("dma_recvAddr = %08x\r\n", recvAddr);

	EDMA_CHx->EDMACCR = 0;
	EDMA_CHx->EDMACCR = ttpye|pn|EDMAC_CHVALID_MASK|0x04;

#ifdef EDMA_INT
	EDMA_CHx->EDMACCR |=  EDMAC_MINDONEIE_MASK;//enable edmac interrupt
#endif
	EDMA_CHx->EDMACRBAR = sendAddr;//(UINT32)(sendAddr&0x3FFFF);//send
	EDMA_CHx->EDMACWBAR = recvAddr;//(UINT32)(recvAddr&0x3FFFF);//recv
	EDMA_CHx->EDMACMINSUMR = len;
	EDMA_CHx->EDMACMAJSUMR = 0;

	EDMA_CHx->EDMACCSR |= EDMAC_EDMAEN_MASK;//enable edmac transfer
#ifdef EDMA_INT
	if (EDMA_CH0 == EDMA_CHx)
	{
		while(g_Edmac0ChMinDoneFlag == 0)
		{
			if(0 == timeout)
			{
				ret = STATUS_TIMEOUT;
                break;
			}
			timeout--;
		}
		g_Edmac0ChMinDoneFlag = 0;
	}
	else
	{
		while(g_Edmac1ChMinDoneFlag == 0)
		{
			if(0 == timeout)
			{
				ret = STATUS_TIMEOUT;
                break;
			}
			timeout--;
		}
		g_Edmac1ChMinDoneFlag = 0;
	}
#else
	while(((EDMA_CHx->EDMACCSR)&(EDMAC_MINORDONE_MASK))==0)// channelx done
	{
		if(0 == timeout)
		{
			ret = STATUS_TIMEOUT;
            break;
		}
		timeout--;
	}
//	EDMA_CHx->EDMACCSR |= EDMAC_MINORDONE_MASK; // clear done
#endif

	EDMA_CHx->EDMACCSR |= EDMAC_MINORDONE_MASK|EDMAC_MAJORDONE_MASK;
	EDMA_CHx->EDMACCSR &= ~EDMAC_EDMAEN_MASK;
    
    return ret;
}

/*******************************************************************************
* Function Name  : EDMA_CHx_SAMEDATA
* Description    : EDMA ����̶���ֵ
* Input          :  - EDMA_CHx�� where x can be 0 to 1 to select the EDMA peripheral��
*                   - sendAddr��EDMAC Read Buffer Address
*                   - recvAddr��EDMAC Write Buffer Address
*                   - ttpye:    Channel Transfer Type ȡֵ��EDMAC_TTYPE0~EDMAC_TTYPE3
*                   - pn��    Channel Periperal Number
*                   - len�� Channel Minor Transfer Data Sum(in byte)
*
* Output         : None
* Return         : None
******************************************************************************/
void EDMA_CHx_SAMEDATA(EDMA_TypeDef *EDMA_CHx, UINT32 sendAddr,  UINT32 recvAddr, UINT32 ttpye, UINT32 pn, UINT32 len)
{
	/* Check the parameters */
	assert_param(IS_EDMA_ALL_PERIPH(EDMA_CHx));//
	assert_param(IS_EDMA_TTYPE(ttpye));
	assert_param(IS_EDMA_EDMA_PN(pn));

	EDMA_CHx->EDMACCR = 0;
	EDMA_CHx->EDMACCR = ttpye|pn|EDMAC_CHVALID_MASK;
#ifdef EDMA_INT
	EDMA_CHx->EDMACCR |=  EDMAC_MINDONEIE_MASK;//enable edmac interrupt
#endif
	EDMA_CHx->EDMACRBAR = sendAddr;//(UINT32)(sendAddr&0x3FFFF);//send
	EDMA_CHx->EDMACRBAR |= 0x80000000;//��ַ������
	EDMA_CHx->EDMACWBAR = recvAddr;//(UINT32)(recvAddr&0x3FFFF);//recv
	EDMA_CHx->EDMACMINSUMR = len;
	EDMA_CHx->EDMACMAJSUMR = 0;

	EDMA_CHx->EDMACCSR |= EDMAC_EDMAEN_MASK;//enable edmac transfer
#ifdef EDMA_INT
	if (EDMA_CH0 == EDMA_CHx)
	{
		while(g_Edmac0ChMinDoneFlag == 0)
		{
			;
		}
		g_Edmac0ChMinDoneFlag = 0;
	}
	else
	{
		while(g_Edmac1ChMinDoneFlag == 0)
		{
			;
		}
		g_Edmac1ChMinDoneFlag = 0;
	}
#else
	while(((EDMA_CHx->EDMACCSR)&(EDMAC_MINORDONE_MASK))==0)// channelx done
	{
		;
	}
	EDMA_CHx->EDMACCSR |= EDMAC_MINORDONE_MASK; // clear done
#endif

	EDMA_CHx->EDMACCSR |= EDMAC_MINORDONE_MASK|EDMAC_MAJORDONE_MASK;
	EDMA_CHx->EDMACCSR &= ~EDMAC_EDMAEN_MASK;
}

/*******************************************************************************
* Function Name  : EDMA_Init
* Description    : EDMA��ʼ��
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void EDMA_Init(void)
{
	cpm_controlModuleClock(MODULE_CLK_EDMAC0, TRUE);
#ifdef EDMA_INT
	NVIC_Init(3, 3, ENCR_IRQn, 2);
#endif
}
