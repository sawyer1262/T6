#include "uart_drv.h"
#include "uart.h"
#include "uart_fp.h"
#include "uart_face.h"
#include "uart_back.h"
#include "lock_config.h"


tUartDriver UartDrv[UART_NUM]={0};

/******************************************************************************/
/*
//���������ṹ���ʼ��
input:   none
output   none
return   none
*/
/******************************************************************************/
void Uart_DriverInit(void)
{
		//���
		UartDrv[UART_BACK_INDEX].StrInit=UartBack_DatStrInit;
		UartDrv[UART_BACK_INDEX].HardInit=UartBack_Init;
		UartDrv[UART_BACK_INDEX].HardDeinit=UartBack_Deinit;
		UartDrv[UART_BACK_INDEX].RxFrameOP=UartBack_RxDataHandle;
		UartDrv[UART_BACK_INDEX].RtxTimeoutOP=UartBack_RtxTimeoutoutHandle;
		UartDrv[UART_BACK_INDEX].SendNBytes=UartBack_SendNbytes;
		UartDrv[UART_BACK_INDEX].SendQueueOP=UartBack_DataSendHandle;
		UartDrv[UART_BACK_INDEX].StrInit();        //�ṹ���ʼ��
		//����
#ifndef NO_FACE_MODE	    
		UartDrv[UART_FACE_INDEX].StrInit=UartFace_DatStrInit;   
		UartDrv[UART_FACE_INDEX].HardInit=UartFace_Init;
		UartDrv[UART_FACE_INDEX].HardDeinit=UartFace_Deinit;
		UartDrv[UART_FACE_INDEX].RxFrameOP=UartFace_RxDataHandle;
		UartDrv[UART_FACE_INDEX].RtxTimeoutOP=UartFace_RtxTimeoutoutHandle;    
		UartDrv[UART_FACE_INDEX].SendNBytes=UartFace_SendNbytes;
		UartDrv[UART_FACE_INDEX].SendQueueOP=UartFace_DataSendHandle;
		UartDrv[UART_FACE_INDEX].StrInit();        //�ṹ���ʼ��   
#endif
		//ָ��
		UartDrv[UART_FP_INDEX].StrInit=UartFp_DatStrInit;
		UartDrv[UART_FP_INDEX].HardInit=UartFp_Init;
		UartDrv[UART_FP_INDEX].HardDeinit=UartFp_Deinit;
		UartDrv[UART_FP_INDEX].RxFrameOP=UartFp_RxDataHandle;
		UartDrv[UART_FP_INDEX].RtxTimeoutOP=UartFp_RtxTimeoutoutHandle;
		UartDrv[UART_FP_INDEX].SendNBytes=UartFp_SendNbytes;
		UartDrv[UART_FP_INDEX].SendQueueOP=UartFp_DataSendHandle;
		UartDrv[UART_FP_INDEX].StrInit();        //�ṹ���ʼ��
}
/******************************************************************************/
/*
//���ڳ�ʼ��
input:   none
output   none
return   none
*/
/******************************************************************************/
void Uart_Init(void)
{
		uint8_t i=0;
	
		for(i=0;i<UART_NUM;i++)
		{
				if(UartDrv[i].HardInit!=0)
				{
						UartDrv[i].HardInit();       //Ӳ����ʼ��
				}
		}
}
/******************************************************************************/
/*
//���ڳ���
input:   none
output   none
return   none
*/
/******************************************************************************/
void Uart_Deinit(void)
{
		uint8_t i=0;
	
		for(i=0;i<UART_NUM;i++)
		{
				if(UartDrv[i].HardDeinit!=0)
				{
						UartDrv[i].HardDeinit();       //Ӳ����ʼ��
				}
		}
}
/******************************************************************************/
/*
//������������
input:   none
output   none
return   none
*/
/******************************************************************************/
void Uart_RtxFrameHandle(void)
{
		uint8_t i=0;
	
		for(i=0;i<UART_NUM;i++)
		{
				if(UartDrv[i].SendQueueOP!=0)
				{
						UartDrv[i].SendQueueOP();           //���Ͷ��д���
				}
				if(UartDrv[i].RxFrameOP!=0)
				{
						UartDrv[i].RxFrameOP();             //��������֡����
				}
				if(UartDrv[i].RtxTimeoutOP!=0)
				{
						UartDrv[i].RtxTimeoutOP();         //���ͽ��ճ�ʱ����
				}
		}
}























