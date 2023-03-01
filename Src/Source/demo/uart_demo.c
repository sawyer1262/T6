// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : uart_demo.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "uart_drv.h"
#include "type.h"
#include "debug.h"
#include "string.h"
#include "uart_demo.h"
#include "ioctrl_drv.h"
#include "cpm_drv.h"
#include "eport_drv.h"
#include "tc_drv.h"
#include "wdt_drv.h"
#include "dmac_drv.h"
#include "delay.h"

static UINT8 uart_data_buf[0x1000] = {0};

void Demo_UART_PinSwap(UART_TypeDef *UARTx)
{
	//GINT[5:0]���ž���SCI���ù��ܣ�����SCI���ù��ܺ�GINT��ת����Ϊ��Ӧ��SCI���ܣ�ͬʱ��SCI���Ž�ת���ɶ�Ӧ��GINT����(����������Դ)��
	if(UARTx == SCI1)
	{
		//SCI1 GINT0&GINT4 swap enable 
		IO_Ctrl_SCI_Swap(TX1_GINT4,TRUE);
		IO_Ctrl_SCI_Swap(RX1_GINT0,TRUE);
	}
	else if(UARTx == SCI2)
	{
		//SCI2 GINT3&GINT5 swap enable
		IO_Ctrl_SCI_Swap(TX2_GINT5,TRUE);
		IO_Ctrl_SCI_Swap(RX2_GINT3,TRUE);
	}
	else if(UARTx == SCI3)
	{
		//SCI3 GINT1&GINT2 swap enable
		IO_Ctrl_SCI_Swap(TX3_GINT1,TRUE);
		IO_Ctrl_SCI_Swap(RX3_GINT2,TRUE);
	}
	else
	{
		;
	}

	while(1);

}

void Demo_UART_CPUNormalTransfer(UART_TypeDef *UARTx)
{
    UART_InitTypeDef UART_InitStruct;
    UINT16 ret;

    printf("uart cpu normal transfer.\n");	

	//init uart
    UART_InitStruct.UART_BaudRate = 115200;
    UART_InitStruct.UART_Mode = UART_NORMAL_MODE;//����CPU��ͨ��ʽ
	UART_InitStruct.UART_Parity = UART_PARITY_NONE;
#if 1  //UART_StopBits = 1
	UART_InitStruct.UART_FrameLength = UART_DATA_FRAME_LEN_10BIT;
    UART_InitStruct.UART_StopBits = 1;
#else  //UART_StopBits = 2
    UART_InitStruct.UART_FrameLength = UART_DATA_FRAME_LEN_11BIT;
    UART_InitStruct.UART_StopBits = 2;
#endif
	
	UART_Init(UARTx, &UART_InitStruct);

    while(1)
    {
        ret = UART_ReceiveData(UARTx,uart_data_buf,16);//���ڽ�������
        if(STATUS_OK != ret)
        {
            printf("uart receive failed, ret = %04x\n",ret);
            continue;	
        }
		ret = UART_SendData(UARTx,uart_data_buf,16);//���ͽ�������
        if(STATUS_OK != ret)
        {
            printf("uart send failed, ret = %04x\n",ret);
            continue;	
        }
    }

}

void Demo_UART_CPUIntTransfer(UART_TypeDef *UARTx)
{
    UART_InitTypeDef UART_InitStruct;
    UINT8 temp = 0;
    UINT16 ret;
    UINT32 i = 0,j = 0;
    UINT32 receive_len  = 0;

    printf("uart cpu interrupt transfer.\n");	

	//init uart
    UART_InitStruct.UART_BaudRate = 115200;
    UART_InitStruct.UART_Mode = UART_INT_MODE;//����CPU�жϷ�ʽ
	UART_InitStruct.UART_Parity = UART_PARITY_NONE;
#if 1  //UART_StopBits = 1
	UART_InitStruct.UART_FrameLength = UART_DATA_FRAME_LEN_10BIT;
    UART_InitStruct.UART_StopBits = 1;
#else  //UART_StopBits = 2
    UART_InitStruct.UART_FrameLength = UART_DATA_FRAME_LEN_11BIT;
    UART_InitStruct.UART_StopBits = 2;
#endif
	
	UART_Init(UARTx, &UART_InitStruct);
    
    receive_len = 16;

    while(1)
    {
        ret = UART_RecvByte(UARTx, &temp);
       
        if(STATUS_OK == ret)  //�յ�һ���ֽ�����
        {
            uart_data_buf[i++] = temp;

            if(i == receive_len)
            {
                ret = UART_SendData(UARTx, uart_data_buf,i);
                if(STATUS_OK != ret)
                {
                    printf("uart send failed, ret = %04x\n",ret);
                }
                i = 0;
                j = 0;
            }
        }
        else if(STATUS_NULL == ret) //û������
        {
            if(j++ > 5000000) //�ӳٴ�ӡ
            {
                j = 0;
                printf("uart no data received.\n");
            }
        }
        else  //�������ݴ���
        {
            printf("uart receive failed, ret = %04x\n",ret);
        }
        
    }

}

void Demo_UART_DMA_Transfer(UART_TypeDef *UARTx)
{
    UART_InitTypeDef UART_InitStruct;
    UINT8 dma_channel;
    UINT16 ret;
    UINT32 len = 0;

    printf("uart DMA transfer.\n");

	//init uart
    UART_InitStruct.UART_BaudRate = 115200;
    UART_InitStruct.UART_Mode = UART_DMA_MODE;//����DMA��ͨ��ʽ
	UART_InitStruct.UART_Parity = UART_PARITY_NONE;
#if 1  //UART_StopBits = 1
	UART_InitStruct.UART_FrameLength = UART_DATA_FRAME_LEN_10BIT;
    UART_InitStruct.UART_StopBits = 1;
#else  //UART_StopBits = 2
    UART_InitStruct.UART_FrameLength = UART_DATA_FRAME_LEN_11BIT;
    UART_InitStruct.UART_StopBits = 2;
#endif
	
	UART_Init(UARTx, &UART_InitStruct);

    while(1)
    {
        dma_channel = 0;
		ret = UART_DMAReceiveData(UARTx,dma_channel,uart_data_buf, DMA_MAX_RECV_LEN);	//���ղ��������ݣ����4095B
        if(STATUS_OK != ret)
        {
             printf("uart receive failed, ret = %04x\n",ret);
            continue;
        }
		len = DMA_GetRecvLen(UARTx,dma_channel);
        ret = UART_DMASendData(UARTx,dma_channel,uart_data_buf, len);
        if(STATUS_OK != ret)
        {
            printf("uart send failed, ret = %04x\n",ret);
            continue;	
        }
    }

}

void UART_Demo(void)
{
    UART_TypeDef *uart_type_id;
	uart_type_id = SCI1;
	    
	printf("uart demo...\r\n");		//����SCI1 
    
	//UART CPU����ģʽ
    Demo_UART_CPUNormalTransfer(uart_type_id);

	//UART CPU�ж�ģʽ
//    Demo_UART_CPUIntTransfer(uart_type_id);

	//UART DMAģʽ��ʹ����UART idleģʽ�����Խ��ղ��������ݣ����4095B
//    Demo_UART_DMA_Transfer(uart_type_id);

	//UART �ܽŸ����л�demo
//    Demo_UART_PinSwap(uart_type_id);
	
}




