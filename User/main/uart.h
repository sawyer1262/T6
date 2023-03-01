#ifndef  __UART_H_
#define  __UART_H_


#include <stdint.h>


#define     UART_NUM            3
#define     UART_TX_MAX_LEN     100

typedef enum{
		ADD_NORMAL=0,                   //�����ͨ����
		ADD_ADMIN,                       //��ӹ���Ա
		ADD_COERCE
}AddType_t;


typedef struct sUartDriver
{
		void ( *StrInit )( void );                             //�ṹ���ʼ��
    void ( *HardInit )( void );                            //Ӳ��IO��ʼ��
    void ( *HardDeinit )( void );                          //Ӳ��IO�͹��Ĵ���
		void ( *SendNBytes )(uint8_t* buf,uint16_t len);       //����ָ���������� 
		void ( *RxFrameOP )(void);                             //�������ݴ���   
		void ( *RtxTimeoutOP )(void);                          //�շ���ʱ���
		void ( *SendQueueOP )(void);                           //���Ͷ��д���  
}tUartDriver;

typedef struct{
		uint8_t DatStat;                  //����״̬��0-�����ݣ�1-�����ݴ�����
		uint8_t Retry;                    //�ط�����
		uint8_t NeedAck;                  //�Ƿ���ҪӦ��
		uint16_t DatBufLen;               //�������ݳ��� 
		uint32_t Timeout;                 //��ʱʱ���趨ֵ 
		uint32_t TxTime;                  //���ݷ���ʱ��
		uint8_t DatBuf[UART_TX_MAX_LEN];  //�������ݻ���
}UartTxStr_t;


void Uart_Init(void);
void Uart_DriverInit(void);
void Uart_Deinit(void);
void Uart_RtxFrameHandle(void);














#endif

