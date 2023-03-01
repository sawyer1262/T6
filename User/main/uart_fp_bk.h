#ifndef  __UART_FP_H_
#define  __UART_FP_H_



#include <stdint.h>


#define        FP_TX_Q_SIZE            5
#define        FP_REG_PRESS_TIMES      8         //ע��ʱ��ѹ��ָ����




#define      UART_FP_INDEX          2
#define      UART_FP_SCI            SCI3

//ָ��ģ��MCU�����Դ
#define      FP_MCU_VCC_PIN      I2C_SDA
#define      FP_TOUCHOUT_PIN     I2C_SCL
//ָ��ģ�鴫����������ʱ��һֱ����


//��ͷ
#define        FP_HEAD              0xEF01
#define        FP_HEAD_H            0xEF
#define        FP_HEAD_L            0x01

//����ʾ
typedef enum{
		FP_CMD=0x01,                    // ָ���,�������͵�ָ�� 
		FP_DAT_CONTINUED,               // ���ݰ����к������ݰ� 
		FP_ACK=0x07,                    // Ӧ���
		FP_DAT_END,                     // ���һ�����ݰ� 
}FpPktType_t;

//ָ��ģ����Ϣ
typedef struct{
		uint16_t SysStat;                //ϵͳ״̬�Ĵ���ֵ
		uint16_t SenType;                //����������
		uint16_t MaxFpSum;               //ָ���������
		uint16_t ActFpSum;               //��Чָ��ģ������
		uint16_t SecLevel;               //��ȫ�ȼ�
		uint32_t DevAddr;                //�豸��ַ
		uint16_t PktSize;                //���ݰ���С
		uint16_t BaudRate;               //���ڲ�����
}LdFpStr_t;


//ָ��ע��״̬��
typedef enum{
		FP_REG_DONE=0,                   //���
		FP_REG_WAIT_FG_DOWN,             //�ȴ���ָ����
		FP_REG_GET_IMG,                  //��ȡͼ��
		FP_REG_GET_IMG_WAIT_ACK,         //�ȴ���ȡͼ��Ӧ��
		FP_REG_GEN_CHAR,                 //ͼ����������
		FP_REG_GEN_CHAR_WAIT_ACK,        //�ȴ���������Ӧ��  
		FP_REG_WAIT_FG_UP,               //�ȴ���ָ̧��
		FP_REG_REG_MODEL,                //�ϲ�����
		FP_REG_REG_MODEL_WAIT_ACK,       //�ȴ��ϲ�����Ӧ��
		FP_REG_STORE_FP,                 //����ָ������
		FP_REG_STORE_FP_WAIT_ACK         //�ȴ�����ָ��Ӧ��
}LdFpRegStat_t;

typedef struct{
		LdFpRegStat_t Stat;              //״̬ 
		uint8_t Ack;                     //����Ӧ��״̬
		uint8_t CharCount;               //������������ 
		uint32_t StartTime;              //��ʼʱ�� 
}LdFpRegStr_t;

typedef enum{
		FP_IDY_DONE=0,                   //���
		FP_IDY_WAIT_FG_DOWN,             //�ȴ���ָ����
		FP_IDY_GET_IMG,                  //��ȡͼ��
		FP_IDY_GET_IMG_WAIT_ACK,         //�ȴ���ȡͼ��Ӧ��
		FP_IDY_GEN_CHAR,                 //ͼ����������
		FP_IDY_GEN_CHAR_WAIT_ACK,        //�ȴ���������Ӧ��  
		FP_IDY_SEACH_FP,                 //����ָ�ƿ�
		FP_IDY_SEACH_FP_WAIT_ACK         //�ȴ�����ָ�ƿ����
}LdFpIdyStat_t;

typedef struct{
		LdFpIdyStat_t Stat;              //״̬ 
		uint8_t Ack;                     //����Ӧ��״̬
		uint32_t StartTime;              //��ʼʱ�� 
}LdFpIdyStr_t;


//֡�ṹ
#define    FP_HEAD_IDX        0
#define    FP_ADDR_IDX        (FP_HEAD_IDX+2)
#define    FP_TYPE_IDX        (FP_ADDR_IDX+4)
#define    FP_LEN_IDX         (FP_TYPE_IDX+1)


//////////////////���󷵻���////////////////////
#define PS_OK                	0x00         //OK
#define PS_COMM_ERR          	0x01         //���ݰ����մ���
#define PS_NO_FINGER         	0x02         //��������û����ָ
#define PS_GET_IMG_ERR       	0x03         //¼��ָ��ͼ��ʧ��
#define PS_FP_TOO_DRY        	0x04         //��ʾָ��ͼ��̫�ɡ�̫��������������
#define PS_FP_TOO_WET       	0x05         //��ʾָ��ͼ��̫ʪ��̫��������������
#define PS_FP_DISORDER       	0x06         //��ʾָ��ͼ��̫�Ҷ�����������
#define PS_LITTLE_FEATURE    	0x07         //��ʾָ��ͼ����������������̫�٣������̫С��������������
#define PS_NOT_MATCH         	0x08         //��ʾָ�Ʋ�ƥ�� 
#define PS_NOT_SEARCHED      	0x09         //��ʾû������ָ��
#define PS_MERGE_ERR         	0x0a         //��ʾ�����ϲ�ʧ��
#define PS_ADDRESS_OVER      	0x0b         //��ʾ����ָ�ƿ�ʱ��ַ��ų���ָ�ƿⷶΧ
#define PS_READ_ERR          	0x0c         //��ʾ��ָ�ƿ��ģ��������Ч
#define PS_UP_TEMP_ERR       	0x0d         //��ʾ�ϴ�����ʧ��
#define PS_RECV_ERR          	0x0e         //��ʾģ�鲻�ܽ��պ������ݰ�
#define PS_UP_IMG_ERR        	0x0f         //��ʾ�ϴ�ͼ��ʧ��  
#define PS_DEL_TEMP_ERR      	0x10         //��ʾɾ��ģ��ʧ�� 
#define PS_CLEAR_TEMP_ERR    	0x11         //��ʾ���ָ�ƿ�ʧ��
#define PS_SLEEP_ERR         	0x12         //��ʾ���ܽ���͹���״̬
#define PS_INVALID_PASSWORD  	0x13         //��ʾ�����ȷ
#define PS_RESET_ERR         	0x14         //��ʾϵͳ��λʧ�� 
#define PS_INVALID_IMAGE     	0x15         //��ʾ��������û����Чԭʼͼ��������ͼ��
#define PS_UPDATE_ERROR     	0x16         //��ʾ��������ʧ��
#define PS_HANGOVER_UNREMOVE 	0X17         //��ʾ����ָ�ƻ����βɼ�֮����ָû���ƶ���
#define PS_FLASH_ERR         	0x18         //��ʾ��д FLASH ���� 
#define PS_TRNG_ERR         	0x19         //���������ʧ��
#define PS_INVALID_REG       	0x1a         //��Ч�Ĵ����� 
#define PS_REG_CONTENT_ERR    0x1b         //�Ĵ����趨���ݴ����
#define PS_NOTEADDR_ERR      	0x1c         //���±�ҳ��ָ������ 
#define PS_PORT_ERR      		  0x1d         //�˿ڲ���ʧ��
#define PS_ENROLL_ERR        	0x1e         //�Զ�ע�ᣨenroll��ʧ��
#define PS_LIB_FULL_ERR      	0x1f         //ָ�ƿ���
#define PS_DEVICE_ADDR_ERR   	0x20         //�豸��ַ����
#define PS_MUST_VERIFY_PWD   	0x21         //�������� 
#define PS_TMPL_NOT_EMPTY    	0x22         //ָ��ģ��ǿ�
#define PS_TMPL_EMPTY    	 	  0x23         //ָ��ģ��Ϊ��
#define PS_LIB_EMPTY_ERR      0x24         //ָ�ƿ�Ϊ��
#define PS_TMPL_NUM_ERR		 	  0x25         //¼��������ô���
#define PS_TIME_OUT			 	    0x26         //��ʱ
#define PS_FP_DUPLICATION	 	  0x27         //ָ���Ѵ��� 
#define PS_RELATE         		0x28         //ָ��ģ���й���
#define PS_SENSOR_ERR         0x29         //��������ʼ��ʧ��


//ͨ��ָ��
#define PS_GetImage				    0x01         //��֤�û�ȡͼ��	
#define PS_GenChar            0x02         //��������
#define PS_Match              0x03         //��ȷ�ȶ���öָ������
#define PS_Search             0x04         //����ָ��
#define PS_RegModel           0x05         //�ϲ�����������ģ�壩 
#define PS_StoreChar          0x06         //����ģ��
#define PS_LoadChar           0x07         //����ģ��
#define PS_UpChar             0x08         //�ϴ�������ģ��
#define PS_DownChar           0x09         //����������ģ��
#define PS_UpImage				    0x0a         //�ϴ�ͼ��
#define PS_DownImage          0x0b         //����ͼ��
#define PS_DeletChar          0x0c         //ɾ��ģ��
#define PS_Empty              0x0d         //���ָ�ƿ� 
#define PS_WriteReg           0x0e         //дϵͳ�Ĵ���
#define PS_ReadSysPara        0x0f         //��ϵͳ��������
#define PS_SetPwd             0x12         //���ÿ���
#define PS_VfyPwd							0x13         //��֤����
#define PS_GetRandomCode      0x14         //���������
#define PS_SetChipAddr        0x15         //����оƬ��ַ
#define PS_ReadINFpage				0x16         //�� flash ��Ϣҳ
#define PS_Port_Contro        0x17         //�˿ڿ���
#define PS_WriteNotepad       0x18         //д���±�
#define PS_ReadNotepad        0x19         //�����±�
#define PS_BurnCode				    0x1a         //��дƬ�� FLASH
#define PS_GenBinImage        0x1c         //����ϸ��ָ��ͼ��
#define PS_ValidTempleteNum	  0x1d         //����Чģ�����
#define PS_ReadIndexTable     0x1f         //��ȡģ�������� 
#define PS_GetEnrollImage     0x29         //ע���û�ȡͼ��
#define PS_AutoEnroll         0x31         //�Զ�ע��ģ��
#define PS_AutoIdentify       0x32         //�Զ���ָ֤��
#define PS_Sleep              0x33         //����ָ��
#define PS_ReadUUID           0x34         //��оƬΨһ���к�
#define PS_HandShake		      0x35         //����ָ��




/***********************************************************************/
/*
//ָ��ģ��Ĭ�ϵ�ַFFFFFFFF��������57600

������ ����λ ��żУ�� ֹͣλ  ��������
57600    8       ��      1       ��


**************************************ע��**********************************************
ע�����ָ�ƴ������ж��ź�TOUCH-OUT:ֻ��������ָ���Żᴥ�����͵��ߣ���������һֱΪ��
������ָ�Ƿ��뿪��Ҫ�ٴδ������뷢����ָ�������ȷ�Ĵ���ʽ���ǣ��ȷ�����ָ��յ���
���źžͿ�ʼ��������������ߴ���������Ҫ������ָ֤�ƣ�ע��ָ��ʱ�������������ָ�Ƿ�
�£�Ȼ��ɼ�ͼ��
**************************************ע��**********************************************


��Ҫ���ܣ�

1.���ָ��
2.ɾ��ָ��
3.��ָ֤��

//��������
2021/12/16 09:12:23.585 [RX] - 55                //�ϵ�����                                                                  
2021/12/16 09:15:04.397 [TX] - EF 01 FF FF FF FF 01 00 03 0F 00 13        //��ϵͳ��Ϣ 
2021/12/16 09:15:04.413 [RX] - EF 01 FF FF FF FF 07 00 13 00 00 0C 2E C8 00 64 00 03 FF FF FF FF 00 02 00 06 05 87 

			00 0C                    //״̬�Ĵ���
			2E C8                    //����������
			00 64                    //ָ�ƿ���������Ŵ�0-99
			00 03                    //��ȫ�ȼ�
			FF FF FF FF              //�豸��ַ 
			00 02                    //���ݰ���С��0-32,1-62,2-128,3-258
			00 06                    //�����ʣ�9600*N=9600*6=57600

//�Զ�ע�᣺�Զ�ע���������
1.>2�ΰ�ѹ��ע��ɹ�
2021/12/16 10:11:09.817 [TX] - EF 01 FF FF FF FF 01 00 08 31 00 01 02 00 4B 00 88    //
2021/12/16 10:11:09.830 [RX] - EF 01 FF FF FF FF 07 00 05 00 00 00 00 0C             //ָ����ȷ
2021/12/16 10:11:11.488 [RX] - EF 01 FF FF FF FF 07 00 05 00 01 01 00 0E             //��һ�β�ͼ�ɹ�
2021/12/16 10:11:11.775 [RX] - EF 01 FF FF FF FF 07 00 05 00 02 01 00 0F             //��һ�����������ɹ�
2021/12/16 10:11:12.846 [RX] - EF 01 FF FF FF FF 07 00 05 00 03 01 00 10             //��ָ�뿪����һ��¼��ɹ� 
2021/12/16 10:11:13.964 [RX] - EF 01 FF FF FF FF 07 00 05 00 01 02 00 0F             //�ڶ��β�ͼ�ɹ� 
2021/12/16 10:11:14.268 [RX] - EF 01 FF FF FF FF 07 00 05 00 02 02 00 10             //�ڶ������������ɹ�
															 EF 01 FF FF FF FF 07 00 05 00 04 F0 01 00             //�ϳ�ģ��
															 EF 01 FF FF FF FF 07 00 05 00 05 F1 01 02             //��ע����
															 EF 01 FF FF FF FF 07 00 05 00 06 F2 01 04             //ģ��洢���
															 
2.->ע�ᳬʱ��8S��ʱ
2021/12/16 10:18:37.927 [RX] - EF 01 FF FF FF FF 07 00 05 00 00 00 00 0C 
2021/12/16 10:18:45.860 [RX] - EF 01 FF FF FF FF 07 00 05 26 01 01 00 34 


//����Чģ�����
2021/12/16 11:01:20.793 [TX] - EF 01 FF FF FF FF 01 00 03 1D 00 21 
2021/12/16 11:01:20.805 [RX] - EF 01 FF FF FF FF 07 00 05 00 00 01 00 0D 




//���ָ�����̣���ȡͼ����������������һ�㣬6-8�Ρ�����Ҫ��ȷ�ȶԣ������ļ������ж����������д4����δ���£�
//¼ָ��ʱ��ָ��2�Σ�ÿ�β�4��ͼ��һ����8������ֵ�������ļ����棬��Ӧ��PS_GenChar�����BufferIDΪ1-8��Ȼ��
//ͨ��8����������ָ�����ݣ�����ָ�ƿ⡣

2021/12/16 14:55:16.131 [TX] - EF 01 FF FF FF FF 01 00 03 01 00 05                        //��һ�λ�ȡͼ��
2021/12/16 14:55:16.315 [RX] - EF 01 FF FF FF FF 07 00 03 00 00 0A                        //���سɹ���־ 
2021/12/16 14:55:19.023 [TX] - EF 01 FF FF FF FF 01 00 04 02 01 00 08                     //��һ��ͼ����������
2021/12/16 14:55:19.319 [RX] - EF 01 FF FF FF FF 07 00 03 00 00 0A                        //���سɹ���־
2021/12/16 14:55:25.948 [TX] - EF 01 FF FF FF FF 01 00 03 01 00 05                        //�ڶ��λ�ȡͼ��
2021/12/16 14:55:26.141 [RX] - EF 01 FF FF FF FF 07 00 03 00 00 0A                        //���سɹ���־   
2021/12/16 14:55:28.411 [TX] - EF 01 FF FF FF FF 01 00 04 02 02 00 09                     //�ڶ���ͼ����������
2021/12/16 14:55:28.713 [RX] - EF 01 FF FF FF FF 07 00 03 00 00 0A                        //���سɹ���־
//2021/12/16 14:55:31.161 [TX] - EF 01 FF FF FF FF 01 00 03 03 00 07                        //��ȷ�Ա���öָ��������ģ�鳧��˵��Ҫ��ȷ�Ա�ָ������
//2021/12/16 14:55:31.189 [RX] - EF 01 FF FF FF FF 07 00 05 00 01 13 00 20                  //���ضԱȽ��
2021/12/16 14:56:01.662 [TX] - EF 01 FF FF FF FF 01 00 03 05 00 09                        //�ϲ���������
2021/12/16 14:56:01.674 [RX] - EF 01 FF FF FF FF 07 00 03 00 00 0A                        //���ؽ��
2021/12/16 14:56:07.061 [TX] - EF 01 FF FF FF FF 01 00 06 06 01 00 00 00 0E               //�洢ָ������
2021/12/16 14:56:07.119 [RX] - EF 01 FF FF FF FF 07 00 03 00 00 0A                        //���ؽ��

//ˢָ������

2021/12/16 15:33:25.430 [TX] - EF 01 FF FF FF FF 01 00 03 01 00 05                        //��ȡͼ��
2021/12/16 15:33:25.620 [RX] - EF 01 FF FF FF FF 07 00 03 00 00 0A                        //���سɹ���־
2021/12/16 15:33:34.574 [TX] - EF 01 FF FF FF FF 01 00 04 02 01 00 08                     //����ָ������ 
2021/12/16 15:33:34.869 [RX] - EF 01 FF FF FF FF 07 00 03 00 00 0A                        //����ָ������Ӧ�� 
2021/12/16 15:33:36.835 [TX] - EF 01 FF FF FF FF 01 00 08 04 01 00 01 00 10 00 1F         //����ָ�ƿ�
2021/12/16 15:33:36.882 [RX] - EF 01 FF FF FF FF 07 00 07 00 00 01 00 50 00 5F            //�����������

//ɾ��ָ��

2021/12/16 11:08:08.695 [TX] - EF 01 FF FF FF FF 01 00 07 0C 00 01 00 01 00 16 
2021/12/16 11:08:08.761 [RX] - EF 01 FF FF FF FF 07 00 03 00 00 0A 

//���ָ�ƿ�

2021/12/16 11:10:06.391 [TX] - EF 01 FF FF FF FF 01 00 03 0D 00 11 
2021/12/16 11:10:06.461 [RX] - EF 01 FF FF FF FF 07 00 03 00 00 0A 

*/
/***********************************************************************/
















void UartFp_DatStrInit(void);
void UartFp_Init(void);
void UartFp_Deinit(void);
void UartFp_RtxTimeoutoutHandle(void);
void UartFp_SendNbytes(uint8_t* buf,uint16_t len);
void UartFp_RxDataHandle(void);
void UartFp_RtxTimeoutoutHandle(void); 
void UartFp_DataSendHandle(void);


void UartFp_SendCmdData(FpPktType_t type,uint8_t *buf,uint16_t len,uint8_t ack,uint32_t timeout);

void FpMcuPowerIoInit(void);
void FpMcuPowerOn(void);
void FpMcuPowerOff(void);


void Fp_ReadActFpSum(void);
void Fp_RegNewFp(void);
void Fp_IdyNewFp(void);
void FpRegNewFpStatHandle(void);
void FpIdyNewFpStatHandle(void);

#endif

