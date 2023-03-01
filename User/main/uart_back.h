#ifndef  __UART_BACK_H_
#define  __UART_BACK_H_




#include <stdint.h>



typedef struct{
		uint8_t Lock;                      //��״̬
	//	uint8_t Door;                      //��״̬ 
}LockStat_t;



#define      NT_BACK_PIN              EPORT_PIN3


#define      UART_BACK_INDEX          0
#define      UART_BACK_SCI            SCI1

#define      UART_BACK_TX_Q_SIZE        20            //���ݷ��Ͷ��д�С
#define      UART_BACK_WAKE_SIZE        10            //����ǰ�����ӻ������ݵĳ��ȣ�����ʵ�ʵ�������
//#define      RTX_BACK_BYTE_TIMEOUT      5           //�ֽ����ݷ��ͽ��ճ�ʱ���

#define      GMT_LOCK_HEAD_8_H          0xAA         //Э��֡ͷ��8λ
#define      GMT_LOCK_HEAD_8_L          0x55         //Э��֡ͷ��8λ
#define      GMT_LOCK_FRAME_MIN         7            //Э��֡��С����



 
//�����PCͨѶָ�

typedef enum{
		GET_VER                                 = 0x01,
		GET_VER_ACK                             = 0x02,
		REQ_LOCK_STAT                           = 0x03,
		REQ_LOCK_STAT_ACK                       = 0x04,
		CTRL_LOCK                               = 0x05,
		CTRL_LOCK_ACK                           = 0x06,
		LOCK_STAT_REPORT                        = 0x07,
		LOCK_STAT_REPORT_ACK                    = 0X08,
		REQ_TIME                                = 0X09,
		REQ_TIME_ACK                            = 0X0A,
		USER_CHG_REPORT                         = 0X0B,
		USER_CHG_REPORT_ACK                     = 0X0C,
		CONFIG_NET                              = 0X0D,
		CONFIG_NET_ACK                          = 0X0E,
		NET_STAT_REPORT                         = 0X0F,
		NET_STAT_REPORT_ACK                     = 0X10,
		CTRL_KEEPOPEN                           = 0X11,
		CTRL_KEEPOPEN_ACK                       = 0X12,
		CTRL_FAST_SLEEP                         = 0X13,
		CTRL_FAST_SLEEP_ACK                     = 0X14,
		CTRL_POWERSAVE                          = 0X15,
		CTRL_POWERSAVE_ACK                      = 0X16,
		CHILDLOCK_STAT_REPORT                   = 0X20,
		CHILDLOCK_STAT_REPORT_ACK               = 0X21,
		PRIV_STAT_REPORT                        = 0X22,
		PRIV_STAT_REPORT_ACK                    = 0X23,
		SYSCONFIG_REPORT                        = 0X24,
		SYSCONFIG_REPORT_ACK                    = 0X25,
		ALARM_REPORT                            = 0X26,
		ALARM_REPORT_ACK                        = 0X27,
		
		SET_SN_NUMBER                           = 0XA0,
		SET_SN_NUMBER_ACK                       = 0XA1,
		SET_BOOT_MODE                           = 0XA2,
		SET_BOOT_MODE_ACK                       = 0XA3,
		GET_SN_NUMBER                           = 0XA4,
		GET_SN_NUMBER_ACK                       = 0XA5,
		GET_OP_RECORD                           = 0XA6,
		GET_OP_RECORD_ACK                       = 0XA7,
		CLEAR_ACT_FLAG                          = 0XA8,
		CLEAR_ACT_FLAG_ACK                      = 0XA9,
		
		READ_CFG                                = 0xB0,               //��ȡϵͳ������
		READ_CFG_ACK                            = 0xB1,               //Ӧ��
		APP_MANAGE_NORMAL_USER                  = 0xB2,               //APP��ӻ�ɾ���û�
		APP_MANAGE_NORMAL_USER_ACK              = 0xB3, 
		APP_MANAGE_ADMIN_CODE                   = 0xB4,               //APP��ӻ�ɾ���û�
		APP_MANAGE_ADMIN_CODE_ACK               = 0xB5,
		SET_CFG                                 = 0xB6,               //дϵͳ������
		SET_CFG_ACK                             = 0xB7,               //Ӧ��
		OTA_START                               = 0xBE,               //����OTA���䣬����ESP32ģ��ͨ����崫��ǰ��
		OTA_START_ACK                           = 0xBF,               //ǰ��ͨ�����Ӧ��ESPģ��   
		OTA_GET_FAME                            = 0xC0,               //ǰ��ͨ�������ESP32����OTA���ݰ�  
		OTA_GET_FAME_ACK                        = 0xC1,               //ESP32ͨ�������ǰ��Ӧ��OTA���ݰ� 
		
		
		//ATE����
		ATE_START                               = 0xF0,
		ATE_START_ACK                           = 0xF1,
		ATE_DOWNLINK                            = 0xF2,
		ATE_DOWNLINK_ACK                        = 0xF3,
		ATE_UPLINK                              = 0xF4,
		ATE_UPLINK_ACK                          = 0xF5,
		ATE_STOP                                = 0xF6,
		ATE_STOP_ACK                            = 0xF7,
}GmtCmd_t;

typedef union{
		uint8_t u8[4];
		uint32_t u32;
}uT32_t;

typedef union{
		uint8_t u8[2];
		uint16_t u16;
}uT16_t;


typedef union{
		uint8_t face;
		uint8_t fp;
		uint8_t code;
		uint8_t card;
		uint8_t all;
		uint8_t select;
}RemoteType_t;


typedef struct{
		RemoteType_t type;
		uint8_t id;
}RemoteDel_t;

typedef struct{
		RemoteType_t type;
}RemoteAdd_t;


typedef struct{
		RemoteType_t type;
		uint8_t sum;
		uT16_t id[25];          //5�࣬ÿ�����5��
}RemoteDelSelect_t;

typedef struct{
		uint8_t Running;                   //OTA״̬���Ƿ���������
		uint8_t GotFrame;                  //�ɹ���ȡһ֡����
		uint8_t TimeoutCount;              //��ʱ���� 
		uint16_t NextPktNum;               //��һ��BIN�ļ����
		uint32_t TimeStart;                //��ʱ������ֵ 
}OtaInfo_t;



extern LockStat_t LockStat;
extern RemoteDel_t RemoteDel;
extern RemoteAdd_t RemoteAdd;
extern RemoteDelSelect_t RemoteDelSelect;

void UartBack_DatStrInit(void);
void UartBack_Init(void);
void UartBack_Deinit(void);
void UartBack_RtxTimeoutoutHandle(void);
void UartBack_SendNbytes(uint8_t* buf,uint16_t len);
void UartBack_RxDataHandle(void);
void UartBack_RtxTimeoutoutHandle(void); 
void UartBack_DataSendHandle(void);
uint8_t UartBack_SendCmdData(uint8_t cmd,uint8_t *buf,uint8_t len,uint8_t ack,uint32_t timeout);
void EPORT_GINT3_callback(void);
void CheckFirmwareUpdate(void);





#endif

