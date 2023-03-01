// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : dmac_drv.c
// Version      : V0.1
//NOTE          :MCCʹ����DMAC1���жϣ������ʹ��MCC�������ֻ��ʹ��DMAC1�Ĳ�ѯ��ʽ������DMAC2�����ڸ�����
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "memmap.h"
#include "dmac_drv.h"
#include "spi_drv.h"
#include "debug.h"
#include "main.h"
#include "ssi_drv.h"

#ifdef MCC_DEMO
#include "mcc_drv.h"
#endif
#ifdef SSI_DEMO
#include "ssi_drv.h"
#endif

#ifdef SPI_DEMO
#include "spi_drv.h"
#endif

#ifdef DAC_DEMO
#include "dac_drv.h"
#endif

#ifdef DAC_AUDIO_DEMO
#include "audio_drv.h"
#endif

__IO unsigned int datatemp=0;
extern void MCC_DMA_ISR(void);
extern volatile UINT8 uart_idle_flag ;
// extern volatile UART_TypeDef* pregspointer ;		//SCI registers pointer
/*DMA channel base address*/

DMA_CHANNEL_REG *sci_dma_channel[2][DMAC_CHNUM] = {{(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR        ),(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0x58 ),\
													  (DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0xB0 ),(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0x108)},
                                                    {(DMA_CHANNEL_REG *)(DMA2_BASE_ADDR     ),(DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0x58 ),\
													  (DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0xB0 ),(DMA_CHANNEL_REG *)(DMA2_BASE_ADDR + 0x108)}};//global struct variable for for Channel registers

DMA_CONTROL_REG *m_dma_control ;

DMA_CHANNEL_REG *m_dma_channel[DMAC_CHNUM] = {(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR        ),(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0x58 ),\
													  (DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0xB0 ),(DMA_CHANNEL_REG *)(DMA1_BASE_ADDR + 0x108)};//global struct variable for for Channel registers
/*DMA config base address */
DMA_CONTROL_REG *m_dma_control = (DMA_CONTROL_REG*)(DMA1_BASE_ADDR+0x2C0);//global struct variable for for DMAC registers


//DMA_CHANNEL_REG *m_dma_channel[4];//global struct variable for for Channel registers
//DMA_CONTROL_REG *m_dma_control;//global struct variable for for DMAC registers
volatile UINT32 dma_isr_flag =0;
volatile UINT32 dma_isr_errflag = 0;
static DMA_LLI g_dma_lli_rx;
														
/*******************************************************************************
* Function Name  : DMA1_IRQHandler
* Description    : dma2�жϴ�����
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void DMA1_IRQHandler(void)
{
#ifdef MCC_DMA_INT
		MCC_DMA_ISR();
#endif
	
	//complet int
	if(m_dma_control->DMA_STATTFR & 0x0f)//dma done
	{
#ifdef SSI_DMA_INT
		SSI_DMA_ISR();
#endif
        
#ifdef SPI_DMA_INT
        SPI_DMA_ISR();
#endif
	}
	
	if(m_dma_control->DMA_STATERR & 0x0f)//dma error
	{
		dma_isr_errflag = m_dma_control->DMA_STATERR & 0x0f;
		m_dma_control->DMA_CLRERR = dma_isr_errflag;
	}
	
  m_dma_control->DMA_CLRTFR = m_dma_control->DMA_STATTFR;
	m_dma_control->DMA_CLRBLOCK=m_dma_control->DMA_STATBLOCK;
	m_dma_control->DMA_CLRSRC=m_dma_control->DMA_STATSRC;
	m_dma_control->DMA_CLRDST=m_dma_control->DMA_STATDST;
	m_dma_control->DMA_CLRERR=m_dma_control->DMA_STATERR;
}
//#endif														
/*******************************************************************************
* Function Name  : DMA2_IRQHandler
* Description    : dma2�жϴ�����
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void DMA2_IRQHandler(void)
{
#ifdef DAC_AUDIO_DMA_INT	
	if(m_dma_control[AUDIO_DMA_CH].DMA_STATTFR & (1<<AUDIO_DMA_CH))
	{
		//Only Handle TF IRQ
		AudioISRHandler();
	}
#else
	//complet int
	if(m_dma_control->DMA_STATTFR & 0x0f)//dma done
	{
#ifdef SSI_DMA_INT
		SSI_DMA_ISR();
#endif

#ifdef SPI_DMA_INT
        SPI_DMA_ISR();
#endif
		//dma_isr_flag |= m_dma_control->DMA_STATTFR & 0x0f;
		//m_dma_control->DMA_CLRTFR = dma_isr_flag;
#ifdef DAC_DMA_INT	
		DAC_DMA_ISR();
#endif

	}
#endif
    if(m_dma_control->DMA_STATERR & 0x0f)//dma error
    {
        dma_isr_errflag = m_dma_control->DMA_STATERR & 0x0f;
        m_dma_control->DMA_CLRERR = dma_isr_errflag;
    }

    m_dma_control->DMA_CLRTFR = m_dma_control->DMA_STATTFR;
    m_dma_control->DMA_CLRBLOCK=m_dma_control->DMA_STATBLOCK;
    m_dma_control->DMA_CLRSRC=m_dma_control->DMA_STATSRC;
    m_dma_control->DMA_CLRDST=m_dma_control->DMA_STATDST;
    m_dma_control->DMA_CLRERR=m_dma_control->DMA_STATERR;
}


/*******************************************************************************
* Function Name  : DMA_REG_Init
* Description    : DMA�Ĵ�����ʼ��
* Input          : - dmac_base_addr: DMA����ַ
*
* Output         : None
* Return         : None
******************************************************************************/
void DMA_REG_Init(UINT32 dmac_base_addr)
{
	m_dma_control    = (DMA_CONTROL_REG*)(dmac_base_addr+0x2c0);
	m_dma_channel[0] = (DMA_CHANNEL_REG*)(dmac_base_addr);
	m_dma_channel[1] = (DMA_CHANNEL_REG*)(dmac_base_addr+0x58);
	m_dma_channel[2] = (DMA_CHANNEL_REG*)(dmac_base_addr+0xB0);
	m_dma_channel[3] = (DMA_CHANNEL_REG*)(dmac_base_addr+0x108);

	//m_dma_control->DMA_MASKBLOCK = 0xf0f;
	//interrupt_setup(0x24, isr_dma);
	m_dma_control->DMA_CONFIG = 0x01;
}

void DMA_ADC_Tran(UINT8 channel, UINT32 dest,UINT32 length)
{
	m_dma_control->DMA_CONFIG = 1;
	m_dma_channel[channel]->DMA_SADDR = 0x4002004c;
	m_dma_channel[channel]->DMA_DADDR = dest;
	m_dma_channel[channel]->DMA_CTRL = SNOCHG|DIEC|P2M_DMA|DWIDTH_W|SWIDTH_W;
	m_dma_channel[channel]->DMA_CTRL_HIGH = length;
	
	m_dma_channel[channel]->DMA_CFG = 0;
	m_dma_channel[channel]->DMA_CFG_HIGH = 6<<7;
	
	m_dma_control->DMA_MASKTFR = CHANNEL_UMASK(channel);
	m_dma_control->DMA_CHEN = CHANNEL_ENABLE(channel);
}

void DMA_DAC_Tran(UINT8 channel, UINT32 src,UINT32 length)
{
	//m_dma_control->DMA_CONFIG = 1;
	m_dma_channel[channel]->DMA_SADDR = src;
	m_dma_channel[channel]->DMA_DADDR = 0x40021004;
	m_dma_channel[channel]->DMA_CTRL = SIEC|DNOCHG|M2P_DMA|DWIDTH_HW|SWIDTH_HW|INTEN;
	m_dma_channel[channel]->DMA_CTRL_HIGH = length;
	
	m_dma_channel[channel]->DMA_CFG = 0;
	m_dma_channel[channel]->DMA_CFG_HIGH = DST_PER_DAC;
	
	m_dma_control->DMA_MASKTFR = CHANNEL_UMASK(channel);
	m_dma_control->DMA_CHEN = CHANNEL_WRITE_ENABLE(channel)|CHANNEL_ENABLE(channel);
}
void DMA_dis(UINT8 n)
{
	m_dma_channel[n]->DMA_CFG|=1<<8;
	m_dma_control->DMA_CHEN |=CHANNEL_WRITE_ENABLE(n);
	m_dma_control->DMA_CHEN &=~CHANNEL_ENABLE(n);
	m_dma_channel[n]->DMA_CFG&=~(1<<8);
	m_dma_control->DMA_CONFIG=~1;	
}
void dma_lli_reg_init(UINT8 n,DMA_LLI *dma_lli)
{
	m_dma_control->DMA_CONFIG = 1;
	
	m_dma_channel[n]->DMA_SADDR = dma_lli->src_addr;
	m_dma_channel[n]->DMA_DADDR = dma_lli->dst_addr;
	m_dma_channel[n]->DMA_LLP = (unsigned int)dma_lli;
	m_dma_channel[n]->DMA_CTRL = dma_lli->control0;
	m_dma_channel[n]->DMA_CTRL_HIGH =  dma_lli->len ;
	
	m_dma_channel[n]->DMA_CFG = 0;
	m_dma_channel[n]->DMA_CFG_HIGH = 8<<11;
	
	m_dma_control->DMA_MASKTFR = CHANNEL_UMASK(n);
	m_dma_control->DMA_CHEN =CHANNEL_WRITE_ENABLE(n)|CHANNEL_ENABLE(n);

}

/*******************************************************************************
* Function Name  : dma_m2mtran
* Description    : dma���ͺ���
* Input          : - channel: ͨ����
*				   - width: ���ݿ��
*				   - src �� ��ʼ��ַ
*				   - dest �� Ŀ�ĵ�ַ
*				   - length �� �������ݳ���
*
* Output         : None
* Return         : FALSE - ʧ��
*                  TRUE  - �ɹ�
******************************************************************************/
bool dma_m2mtran(UINT8 channel,UINT8 width,UINT32 src, UINT32 dest,UINT32 length)
{
	UINT32 bitwidth = 0;
	if(width == WIDTH_BYTE)
	{
		bitwidth = DWIDTH_B|SWIDTH_B;
	}
	else if(width == WIDTH_HALFWORD)
	{
		bitwidth = DWIDTH_HW|SWIDTH_HW;
		if(length & 1)
		{
			return FALSE;
		}
		length >>= 1;
	}
	else
	{
		bitwidth = DWIDTH_W|SWIDTH_W;
		if(length & 3)
		{
			return FALSE;
		}
		length >>= 2;
	}
	if(length > 0xFFF)
	{
		return FALSE;
	}
	m_dma_control->DMA_CONFIG = 1;

	m_dma_channel[channel]->DMA_SADDR = src;
	m_dma_channel[channel]->DMA_DADDR = dest;
	m_dma_channel[channel]->DMA_CTRL = DIEC|SIEC|bitwidth|DBSIZE_4|SBSIZE_4;
	m_dma_channel[channel]->DMA_CTRL_HIGH = (length&0x00000FFF) ;  //��󳤶�Ϊ0x0FFF

	//enable dma channel
	m_dma_control->DMA_MASKTFR = CHANNEL_UMASK(channel);
	m_dma_control->DMA_CHEN |= CHANNEL_WRITE_ENABLE(channel)|CHANNEL_ENABLE(channel);

	while((m_dma_control->DMA_RAWTFR & CHANNEL_STAT(channel)) != CHANNEL_STAT(channel));

	m_dma_control->DMA_CLRTFR = CHANNEL_STAT(channel);

	m_dma_control->DMA_CHEN = 0;
	m_dma_control->DMA_CONFIG = 0;

	return TRUE;
}
/*******************************************************************************
* Function Name  : dma_uartTram
* Description    : uart DMA���ʹ�����
* Input          :  - n��DMAͨ��
*                   - src: ����Դ��ַ
*                   -len:�������ݳ���
*
* Output         : None
* Return         : STATUS
******************************************************************************/
UINT16 dma_uartTram(UART_TypeDef *UARTx,UINT8 n, UINT8 *src,UINT32 len)
{
    UINT32 timeout = UART_TIMEOUT(1000);
    UINT16 ret = STATUS_OK;
    UINT8 dmaNum = 0;

    if(UARTx == SCI1)
    {
        m_dma_control = (DMA_CONTROL_REG*)(DMA1_BASE_ADDR+0x2C0);
    }
    else if(UARTx == SCI2)
    {
        m_dma_control = (DMA_CONTROL_REG*)(DMA2_BASE_ADDR+0x2C0);
        dmaNum = 1;
    }
    else
    {
        m_dma_control = (DMA_CONTROL_REG*)(DMA1_BASE_ADDR+0x2C0);
    }
	m_dma_control->DMA_CHEN &= (~(CHANNEL_WRITE_ENABLE(n)|CHANNEL_ENABLE(n)));////DMA disable 
	m_dma_control->DMA_CONFIG = DMACEN;
#ifdef DAM_INTERRUPT
	interrupt_setup(0x24, 0,dmac_isr);
#endif
	sci_dma_channel[dmaNum][n]->DMA_SADDR = (UINT32)src;
    if(UARTx == SCI1)
    {
    	sci_dma_channel[dmaNum][n]->DMA_DADDR = SCI1_BASE_ADDR+0x06;
    }
    else if(UARTx == SCI2)
    {
    	sci_dma_channel[dmaNum][n]->DMA_DADDR = SCI2_BASE_ADDR+0x06;
    }
    else
    {
    	sci_dma_channel[dmaNum][n]->DMA_DADDR = SCI3_BASE_ADDR+0x06;
    }

#ifdef DAM_INTERRUPT
	sci_dma_channel[dmaNum][n]->DMA_CTRL = INTEN|DNOCHG|SIEC|M2P_DMA;
#else
	sci_dma_channel[dmaNum][n]->DMA_CTRL = DNOCHG|SIEC|M2P_DMA;
#endif

	sci_dma_channel[dmaNum][n]->DMA_CTRL_HIGH =  len;
	sci_dma_channel[dmaNum][n]->DMA_CFG &= SRC_SOFT;
    if(UARTx == SCI1)
    {
     	sci_dma_channel[dmaNum][n]->DMA_CFG_HIGH = DST_PER_SCI1_TX; 
    }
    else if(UARTx == SCI2)
    {
     	sci_dma_channel[dmaNum][n]->DMA_CFG_HIGH = DST_PER_SCI2_TX; 
    }
    else
    {
     	sci_dma_channel[dmaNum][n]->DMA_CFG_HIGH = DST_PER_SCI3_TX; 
    }

    m_dma_control->DMA_MASKTFR = CHANNEL_UMASK(n);
	m_dma_control->DMA_CHEN = (CHANNEL_WRITE_ENABLE(n)|CHANNEL_ENABLE(n));
#ifdef DAM_INTERRUPT
	while(dma_int!= n+1)
    {
        if(!timeout)
        {
            ret = STATUS_UART_TIMEOUT;
            break;
        }
        timeout--;
    }
	dma_int = 0;
#else
	while((m_dma_control->DMA_RAWTFR & CHANNEL_ENABLE(n)) != CHANNEL_ENABLE(n))
    {
        if(!timeout)
        {
            ret = STATUS_UART_TIMEOUT;
            break;
        }
        timeout--;
	}
	m_dma_control->DMA_CLRTFR = CHANNEL_ENABLE(n);
#endif
	m_dma_control->DMA_CHEN = 0;
	m_dma_control->DMA_CONFIG = 0;

    return ret;
}

/*******************************************************************************
* Function Name  : DMA_uartRec
* Description    : DMA���մ�����
* Input          :  - n��DMAͨ��
*                   - dest: ��������buf
*                   -len :�������ݳ���
*
* Output         : None
* Return         : STATUS
******************************************************************************/
UINT16 dma_uartRec(UART_TypeDef *UARTx,UINT8 n, UINT8 *dest,UINT32 len)
{
    UINT8 dmaNum = 0;
    UINT16 ret = STATUS_OK;
    UINT32 timeout = UART_TIMEOUT(1000);

    if(UARTx == SCI1)
    {
        m_dma_control = (DMA_CONTROL_REG*)(DMA1_BASE_ADDR+0x2C0);
    }
    else if(UARTx == SCI2)
    {
        m_dma_control = (DMA_CONTROL_REG*)(DMA2_BASE_ADDR+0x2C0);
        dmaNum =1;
    }
    else
    {
        m_dma_control = (DMA_CONTROL_REG*)(DMA1_BASE_ADDR+0x2C0);
    }
	m_dma_control->DMA_CHEN &= (~(CHANNEL_WRITE_ENABLE(n)|CHANNEL_ENABLE(n)));////DMA disable 
    datatemp = m_dma_control->DMA_CHEN;
	m_dma_control->DMA_CONFIG = DMACEN;
#ifdef DAM_INTERRUPT
	interrupt_setup(0x24, 0,dmac_isr);
#endif
    if(UARTx == SCI1)
    {
     	sci_dma_channel[dmaNum][n]->DMA_SADDR = SCI1_BASE_ADDR+0x06;
    }
    else if(UARTx == SCI2)
    {
        sci_dma_channel[dmaNum][n]->DMA_SADDR = SCI2_BASE_ADDR+0x06;
    }
    else
    {
     	sci_dma_channel[dmaNum][n]->DMA_SADDR = SCI3_BASE_ADDR+0x06;
    }
 
	sci_dma_channel[dmaNum][n]->DMA_DADDR = (UINT32)dest;
#ifdef DAM_INTERRUPT
	sci_dma_channel[dmaNum][n]->DMA_CTRL = INTEN|DIEC|SNOCHG|P2M_DMA;
#else
	sci_dma_channel[dmaNum][n]->DMA_CTRL = DIEC|SNOCHG|P2M_DMA;
#endif
	sci_dma_channel[dmaNum][n]->DMA_CTRL_HIGH =  len;
	sci_dma_channel[dmaNum][n]->DMA_CFG &= DST_SOFT;
    if(UARTx == SCI1)
    {
    	sci_dma_channel[dmaNum][n]->DMA_CFG_HIGH = SRC_PER_SCI1_RX;  
    }
    else if(UARTx == SCI2)
    {
    	sci_dma_channel[dmaNum][n]->DMA_CFG_HIGH = SRC_PER_SCI2_RX;   
    }
    else
    {
    	sci_dma_channel[dmaNum][n]->DMA_CFG_HIGH = SRC_PER_SCI3_RX;
    }

	m_dma_control->DMA_MASKTFR = CHANNEL_UMASK(n);
	m_dma_control->DMA_CHEN = (CHANNEL_WRITE_ENABLE(n)|CHANNEL_ENABLE(n));
    datatemp = m_dma_control->DMA_CHEN;
#ifdef DAM_INTERRUPT
	while(dma_int!= n+1)
    {
        if(!timeout)
        {
            ret = STATUS_UART_TIMEOUT;
            break;
        }
        timeout--;
    }
	dma_int = 0;
	m_dma_control->DMA_CHEN = 0;
	m_dma_control->DMA_CONFIG = 0;	
#else
	uart_idle_flag = 0;
	while((m_dma_control->DMA_RAWTFR & CHANNEL_ENABLE(n)) != CHANNEL_ENABLE(n))
	{
		if(uart_idle_flag == 1)
		{
			uart_idle_flag = 0;
			break;
		}
        if(!timeout)
        {
            ret = STATUS_UART_TIMEOUT;
            break;
        }
        timeout--;
	}
	m_dma_control->DMA_CLRTFR = CHANNEL_ENABLE(n); 
	m_dma_control->DMA_CHEN = 0;
	m_dma_control->DMA_CONFIG = 0;

#endif

    return ret;

}
/*******************************************************************************
* Function Name  : dma_spitran
* Description    : spi dma���ͺ���
* Input          : - spiid: SPI ID��
*				   - psend: �������ݵ�ַ
*				   - precv ���������ݵ�ַ
*				   - length �� �������ݳ���
*				   - binten : �Ƿ����ж�ģʽ
*
* Output         : None
* Return         : None
******************************************************************************/
UINT16 dma_spitran(SPI_TypeDef *SPIx, UINT8* psend, UINT8* precv, UINT32 length, BOOL binten)
{
	UINT32 timeout = SPI_TIMEOUT_MS(1000);
    UINT8 spiid;
    UINT16 ret = STATUS_OK;

//	binten = FALSE;
    
    if(SPIx == SPI1)
    {
        spiid = 0;
    }
    else if(SPIx == SPI2)
    {
        spiid = 1;
    }
    else if(SPIx == SPI3)
    {
        spiid = 2;
    }
    else
	{
		return STATUS_ID_ERR;
	}

	m_dma_control->DMA_CONFIG = 1;
	//Tx
	m_dma_channel[DMACCH0]->DMA_SADDR = (UINT32)psend;
	m_dma_channel[DMACCH0]->DMA_DADDR = SPI1_BASE_ADDR+(spiid<<12) + 0x12;
	if(binten == TRUE)
	{
		m_dma_channel[DMACCH0]->DMA_CTRL = INTEN|DNOCHG|SIEC|M2P_DMA;
	}
	else
	{
		m_dma_channel[DMACCH0]->DMA_CTRL = DNOCHG|SIEC|M2P_DMA;
	}
	m_dma_channel[DMACCH0]->DMA_CTRL_HIGH = length ;  //��󳤶�Ϊ0x0FFF
	m_dma_channel[DMACCH0]->DMA_CFG = (HS_SEL_SRC_SOFT);
	if(SPIx == SPI3)
	{
		m_dma_channel[DMACCH0]->DMA_CFG_HIGH = DST_PER_SPI_TX(5);
	}
	else
	{
		m_dma_channel[DMACCH0]->DMA_CFG_HIGH = DST_PER_SPI_TX(spiid);
	}

	//Rx
	m_dma_channel[DMACCH1]->DMA_SADDR = SPI1_BASE_ADDR+(spiid<<12) + 0x12;
	m_dma_channel[DMACCH1]->DMA_DADDR = (UINT32)precv;
	if(binten == TRUE)
	{
		m_dma_channel[DMACCH1]->DMA_CTRL = INTEN|SNOCHG|DIEC|P2M_DMA;
	}
	else
	{
		m_dma_channel[DMACCH1]->DMA_CTRL = SNOCHG|DIEC|P2M_DMA;
	}
	m_dma_channel[DMACCH1]->DMA_CTRL_HIGH = length ;  //��󳤶�Ϊ0x0FFF
	m_dma_channel[DMACCH1]->DMA_CFG = (HS_SEL_DST_SOFT);
	if(SPIx == SPI3)
	{
		m_dma_channel[DMACCH1]->DMA_CFG_HIGH = SRC_PER_SPI_RX(4);
	}
	else
	{
		m_dma_channel[DMACCH1]->DMA_CFG_HIGH = SRC_PER_SPI_RX(spiid+3);
	}

	//enable dma channel
	m_dma_control->DMA_MASKTFR = CHANNEL_UMASK(DMACCH0)|CHANNEL_UMASK(DMACCH1);
	m_dma_control->DMA_CHEN = CHANNEL_WRITE_ENABLE(DMACCH0)|CHANNEL_ENABLE(DMACCH0)|CHANNEL_WRITE_ENABLE(DMACCH1)|CHANNEL_ENABLE(DMACCH1);
	if(binten == TRUE)
	{
		while(1)
		{
			if(dma_isr_flag & (CHANNEL_STAT(DMACCH0)|CHANNEL_STAT(DMACCH1)) )
			{
				dma_isr_flag = 0;
				break;
			}
			if(0 == timeout)
			{
				ret = STATUS_TIMEOUT;
                break;
			}
			timeout --;
		}
	}
	else
	{
		while((m_dma_control->DMA_RAWTFR & (CHANNEL_STAT(DMACCH0) | CHANNEL_STAT(DMACCH1))) != (CHANNEL_STAT(DMACCH0) | CHANNEL_STAT(DMACCH1)))
		{
			if(0 == timeout)
			{
				ret = STATUS_TIMEOUT;
                break;
			}
			timeout --;
		}

		m_dma_control->DMA_CLRTFR = (CHANNEL_STAT(DMACCH0) | CHANNEL_STAT(DMACCH1));
	}
    
	m_dma_control->DMA_CHEN = 0;
	m_dma_control->DMA_CONFIG = 0;
    
    return ret;
}

/*******************************************************************************
* Function Name  : dma_spi_LLIReceive
* Description    : spi dma������պ���
* Input          : -n     ��dmaͨ����
* 				   - spiid: SPI ID��
*				   - precv ���������ݵ�ַ
*				   - length �� �������ݳ���
*
* Output         : None
* Return         : None
******************************************************************************/
void dma_spi_LLIReceive(DMA_CHANNEL n,UINT8 spiid, UINT8 *dest,UINT32 length)
{
	//init
	g_dma_lli_rx.src_addr = SPI1_BASE_ADDR+(spiid<<12) + 0x12;
	g_dma_lli_rx.dst_addr = (UINT32)dest;
	g_dma_lli_rx.len = length;
#ifdef MCC_DMA_LLI_INT_EN
	g_dma_lli_rx.control0 = DMA_IE | SNC | DI | LLP_DST_EN | LLP_SRC_EN | P2M_DMA;
#else
	g_dma_lli_rx.control0 = SNC | DI | LLP_DST_EN | LLP_SRC_EN | P2M_DMA;
#endif
	g_dma_lli_rx.next_lli = (UINT32)&g_dma_lli_rx;

	m_dma_control->DMA_CONFIG = DMACEN;

	m_dma_channel[n]->DMA_SADDR = g_dma_lli_rx.src_addr;
	m_dma_channel[n]->DMA_DADDR = g_dma_lli_rx.dst_addr;

	m_dma_channel[n]->DMA_CTRL =  g_dma_lli_rx.control0;
	m_dma_channel[n]->DMA_CTRL_HIGH = g_dma_lli_rx.len;

	m_dma_channel[n]->DMA_LLP = (UINT32)&g_dma_lli_rx;//++

	m_dma_channel[n]->DMA_CFG &= SRC_HARD;
    if(spiid == 2)
    {
        m_dma_channel[n]->DMA_CFG_HIGH = SRC_PER_SPI_RX(4);
    }
    else
    {
        m_dma_channel[n]->DMA_CFG_HIGH = SRC_PER_SPI_RX(spiid+3);
    }
//	m_dma_channel[n]->DMA_CFG_HIGH = SRC_PER_SPI_RX(spiid);

#ifdef MCC_DMA_LLI_INT_EN
	m_dma_control->DMA_MASKBLOCK |=  CHANNEL_UMASK(n);
#endif
	m_dma_control->DMA_MASKTFR |= CHANNEL_UMASK(n);
	m_dma_control->DMA_CHEN |= CHANNEL_WRITE_ENABLE(n)|CHANNEL_ENABLE(n);//DMA enable
}

/*******************************************************************************
* Function Name  : dma_channle_stop
* Description    : ֹͣDMAͨ������
* Input          : -n     ��dmaͨ����
*
* Output         : None
* Return         : None
******************************************************************************/
void dma_channle_stop(DMA_CHANNEL n)
{
	/*suspend channel*/
	m_dma_channel[n]->DMA_CFG |= (1<<8);
	/*waite FIFO empty*/
	while(((m_dma_channel[n]->DMA_CFG) & (1 << 9)) == 0);
	/*disable channel*/
	m_dma_control->DMA_CHEN =(( m_dma_control->DMA_CHEN|CHANNEL_WRITE_ENABLE(n))&(~CHANNEL_ENABLE(n)));
	/*clear suspend channel*/
	m_dma_channel[n]->DMA_CFG &= ~(1<<8);
}

/*******************************************************************************
* Function Name  : dma_getLength
* Description    : ��ȡDMA���������ݳ���
* Input          : -n     ��dmaͨ����
*
* Output         : None
* Return         : ��ǰ�������ݳ���
******************************************************************************/
UINT32  dma_getLength(DMA_CHANNEL n)
{
	UINT32 addr;

	addr = m_dma_channel[n]->DMA_DADDR;

	return (addr - g_dma_lli_rx.dst_addr);
}


static void dma_reg_init(UINT32 dma_base_addr)
{
	UINT8 n = 0;
	UINT32 ch_addr[4] = {0x0000, 0x0058, 0x00b0, 0x0108};

	m_dma_control = (DMA_CONTROL_REG*)(dma_base_addr+0x2c0);

	for(n =0; n<4; n++)
	{
		m_dma_channel[n] = (DMA_CHANNEL_REG*)(dma_base_addr+ch_addr[n]);
	}
}

/*******************************************************************************
* Function Name  : DMA_Init
* Description    : DMA��ʼ��
* Input          : UINT32 dma_base_addr 
*
* Output         : None
* Return         : None
******************************************************************************/
void DMA_Init(UINT32 dma_base_addr)
{
	dma_reg_init(dma_base_addr);
	//NVIC_Init(3, 3, DMA1_IRQn, 2);//���ڴ�����dma�ж�
}
UINT32 DMA_GetRecvLen(UART_TypeDef *UARTx,UINT8 n)
{
    UINT8 dmaNum=0;

    if(UARTx == SCI2)
    {
       dmaNum=1; 
    }

	return sci_dma_channel[dmaNum][n]->DMA_CTRL_HIGH;
}
