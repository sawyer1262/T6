#ifndef __UART_FACE_H_
#define __UART_FACE_H_


#include <stdint.h>




#define       FACE_POWERON_DELAY   500 
#define       FACE_SCAN_INTVAL     500               //����ɨ��
#define       FACE_SCAN_TIMEOUT    2000
#define       ORB_FACE_MAX         100                //�����������
#define       ORB_FACE_TX_Q_SIZE   10
#define       ORB_TX_FAIL_RTY      3 
#define       FACE_ADD_TIMEOUT_MAX 60000
#define       FACE_VCC_PIN         I2C_SDA


/***********************************������������ѡ��*****************************************/
//�������ڲ��ԣ�
//#define 	UART_FACE_TEST

/***********************************������������ѡ��*****************************************/
#define        UART_FACE_INDEX      1
#define        UART_FACE_SCI        SCI2

#define        ORB_HEAD_16          0xAEF5
#define        ORB_HEAD_8_H         0xAE
#define        ORB_HEAD_8_L         0xF5

#define        ORB_HEAD_IDX         0
#define        ORB_SRC_IDX         (ORB_HEAD_IDX+2)
#define        ORB_TYPE_IDX        (ORB_SRC_IDX+1)
#define        ORB_LEN_IDX         (ORB_TYPE_IDX+1)
#define        ORB_DAT_IDX         (ORB_LEN_IDX+2)


typedef enum{
		HOST=0x01,
		SLAVE
}ObFrameSrc_t;

typedef enum{
		CMD=0x01,
		DATA,
		QUERY,
		ERR,
		UPDATE=0x05,
		IMG
}ObFrameType_t;

//ע���ʶ���������صĴ����
typedef enum{
		ORBBEC_ERR_NOT_INIT=0x00,                   //δ��ʼ�����ʼ��ʧ�� 
		ORBBEC_ERR_LICENSE,                         //δ��Ȩ  
		ORBBEC_ERR_INTERNAL,                        //SDK�ڲ�����
		ORBBEC_ERR_NO_FACE,                         //δ��⵽����
	
		ORBBEC_ERR_FACE_DISTANCE_NEAR,              //��������
		ORBBEC_ERR_FACE_DISTANCE_FAR,               //������Զ
		ORBBEC_ERR_FACE_YAW_LEFT,                   //����̫ƫ��
		ORBBEC_ERR_FACE_YAW_RIGHT,                  //����̫ƫ��
		ORBBEC_ERR_FACE_PITCH_TOP,                  //����ƫ��
		ORBBEC_ERR_FACE_PITCH_BOTTOM,               //����ƫ��
		ORBBEC_ERR_FACE_ROLL_CLOCKWISE,             //����˳ʱ����ת����
		ORBBEC_ERR_FACE_ROLL_ANTI_CLOCKWISE,        //������ʱ����ת����
		ORBBEC_ERR_FACE_NEAR_LEFT,                  //�����������̫��
		ORBBEC_ERR_FACE_NEAR_RIGHT,                 //�������б���̫��
		ORBBEC_ERR_FACE_NEAR_TOP,                   //�������ϱ���̫��
		ORBBEC_ERR_FACE_NEAR_BOTTOM,                //�������±���̫��
	
		ORBBEC_ERR_FACE_LIVE=0x11,                       //�ǻ�������
		ORBBEC_ERR_DB_FULL=0x13,                        //���ݿ�����
		ORBBEC_ERR_NOT_IN_DB=0x14,                      //δ�����ݿ���
		ORBBEC_ERR_BAD_ARGUMENTS=0x15,                   //�����������ȷ
		ORBBEC_ERR_FACE_ID_EXIST,                   //��ע������ID�Ѵ���
		ORBBEC_ERR_FACE_DEPTH_QUALITY=0x18,              //������Ȳ�����
		ORBBEC_ERR_FACE_OCCLUSION=0x1A,                  //�������ڵ�
		//0x20-0x24,��������λ��
		READSENSE_ERR_NOFACE=0xF3                      //���Э���ĵ�
}FaceRegisterErrCode_t;

typedef struct{
		uint8_t Ack;
		uint8_t Cmd;
		uint8_t Stat;
		uint8_t ErrC;
}OrbFaceAckStr_t;


typedef enum{
		FACE_IDLE=0,                    //����
		FACE_CHECK_SENSOR,              //��֤������
		FACE_IDY,                       //ʶ������
}FaceIdyStat_t;

typedef enum{
		FACEADD_READUSER=0,             //�ȴ�Ӧ��
		FACEADD_READY,
    FACEADD_RUN,
		FACEADD_OK,
		FACEADD_FAIL,
		FACEADD_TIMEOUT
}FaceAddStat_t;

typedef enum{
		FACEDEL_READUSER=0,             //�ȴ�Ӧ��
    FACEDEL_RUN,
		FACEDEL_OK,
		FACEDEL_FAIL,
		FACEDEL_TIMEOUT
}FaceDelStat_t;

typedef enum{
		WAIT_RESP=0,                    //�ȴ�Ӧ��
		OP_READY,
		OP_SUCCESS,                     //��ӳɹ�
		OP_FULL,                        //����
		OP_FAIL,                        //���ʧ��
		OP_TIMEOUT                      //��ʱ��Ӧ��       
}FaceAck_t;

typedef union{
		uint8_t IdyReady;
		uint8_t AddReady;
		uint8_t DelReady;
		uint8_t Ready;
}FaceReady_t;

typedef struct{
		FaceIdyStat_t FsStat;
		uint8_t IdyRetry;
		uint32_t StTime;
}FaceIdy_t;

typedef struct{
		FaceAddStat_t AddStat;
		uint8_t Retry;
		uint32_t StTime;
		uint32_t VolFristTime;
}FaceAdd_t;

typedef struct{
		FaceDelStat_t DelStat;
		uint8_t Retry;
		uint32_t StTime;
}FaceDel_t;


typedef struct{
		uint8_t Sum;                     //��Ч��������
		uint8_t IndexTab[13];            //�������ݴ洢����
}FaceUserStr_t;

typedef struct{
		uint32_t StTime;
}FaceTest_t;


/************************************************************************************************************/
/*
���ڻ���������

������ ����λ ��żУ�� ֹͣλ  ��������
115200   8       ��      1      ��


һ����ѯ�豸��Ϣ���ָ��

�����ͣ�ORB_QUERY

����1���������ݣ�

0x55-----�㷨��׼����ɣ��豸�ϵ���������ϱ��������ѯ��������ȷ����ʱ��ģ�鷢�Ͳ�������
0x57-----�ϱ�ע������е��м�״̬��һ��5֡���ݣ�ǰ4֡0x57��֡��ţ���5֡0x01�ӳɹ�ʧ�ܱ�־����������ǿ��������
0x50-----��ѯ��ǰ�豸����ID���������д洢������ID

//���в�������ʹ��
0x56-----�ϱ���������״̬��1Sһ�Σ��˹�����ʱ���ã�
0x60-----��ѯϵͳ�豸�汾��
0x61-----��ѯ�㷨������汾��
0x62-----��ѯ����ͷģ��汾��
0x64-----��ѯchipID
0x65-----��ѯDeviceKey
0x66-----�����ض���С�ڴ�ֱ�������������ã�

���������豸��������

�����ͣ�ORB_CMD

����1�����������ͣ�

0x01-----����������ڼ����ϱ��м�״̬��������10�Σ��ͽ��1��
0x02-----ɾ���������������ID��FFΪɾ����������
0x40-----ʶ����������������ʶ�������������Χ1-5��Ĭ��5��


0x51-----��ʾʶ��ʶ��ɹ�����FF
0x70-----����WIFI��Ϣ������wifiģ��Ĳž��д˹��ܣ�������Ʒ�����д�ģ��
0x71-----����WIFI��Դ������wifiģ��Ĳž��д˹��ܣ�������Ʒ�����д�ģ��
0x73-----ץȡһ��JPEGѹ�����ץ��ͼƬ
0x77-----��֪����ͼƬ��ץ�ĵ�ͼƬ���ļ�����Ϣ���˴�Э���ϱ��������⣬�������ҹ�ͨ


�����豸�ϵ��ʼ���������

bit 0������ 3D ģ�� I2C ͨ���쳣��
bit 1�������㷨 license �쳣��
bit 2�������㷨 weight �쳣��
bit 3��DB error -1�������������ݿ�����޷���ʼ�� SDK��6.9.0 ֮��İ汾֧�֡�
bit 4�����������㷨�������쳣��

���Կ��ܵ��쳣��:					
0x1 �� ֻ�� 3D ģ���쳣
0x3 �� license �쳣��ͬʱ������ 3D ģ����쳣
0x5 �� weight �쳣��ͬʱ������ 3D ģ����쳣
0x8 �������������ݿ��쳣�����Ե�������
0x9�� �������ݿ� DB �쳣��ͬʱ������ 3D ģ����쳣


docklight�����������£�

//�������
2021/12/3 11:26:42.053 [TX] - F5 AE 01 01 02 00 01 06 52 FE 
2021/12/3 11:26:42.543 [RX] - F5 AE 02 01 03 00 57 01 03 FC FD         //��һ֡���
                              F5 AE 02 01 03 00 57 02 03 FB FD         //�ڶ�֡���   
															F5 AE 02 01 03 00 57 03 03 FA FD         //����֡���
															F5 AE 02 01 03 00 57 04 03 F9 FD         //����֡���
															F5 AE 02 01 03 00 57 05 07 F4 FD         //����֡���
															F5 AE 02 01 03 00 57 06 08 F2 FD         //����֡���
															F5 AE 02 01 03 00 01 01 06 4F FE 
//�м�״̬֡���������1-10�������Ժ����м�״̬��ֻ������

//ʶ������
2021/12/3 11:27:44.384 [TX] - F5 AE 01 01 02 00 40 05 14 FE 
2021/12/3 11:27:45.709 [RX] - F5 AE 02 01 03 00 40 01 06 10 FE 
//ɾ������
2021/12/3 13:32:13.533 [TX] - F5 AE 01 01 02 00 02 05 52 FE 
2021/12/3 13:32:13.655 [RX] - F5 AE 02 01 02 00 02 01 55 FE 
//��ȡ����ID
2021/12/3 13:32:19.926 [TX] - F5 AE 01 03 01 00 50 08 FE 
2021/12/3 13:32:20.024 [RX] - F5 AE 02 03 06 00 50 01 02 03 04 06 F2 FD 
//��ѯӲ���汾
2021/12/3 13:35:18.675 [TX] - F5 AE 01 03 01 00 60 F8 FD 
2021/12/3 13:35:18.746 [RX] - F5 AE 02 03 04 00 60 02 00 00 F2 FD 
//��ѯ����汾
2021/12/3 13:35:47.938 [TX] - F5 AE 01 03 01 00 61 F7 FD 
2021/12/3 13:35:48.008 [RX] - F5 AE 02 03 04 00 61 07 00 18 D4 FD 
//��ѯ����״̬����ָ����Ӧ�𣬿��������ⳡ�ϲ�ʹ��
2021/12/3 13:35:44.076 [TX] - F5 AE 01 03 01 00 01 57 FE 
//��ѯ����ͷ�汾
2021/12/3 13:37:55.415 [TX] - F5 AE 01 03 01 00 62 F6 FD 
2021/12/3 13:37:55.474 [RX] - F5 AE 02 03 04 00 62 02 07 05 E4 FD 
//��ѯоƬID
2021/12/3 13:38:13.660 [TX] - F5 AE 01 03 01 00 64 F4 FD 
2021/12/3 13:38:13.752 [RX] - F5 AE 02 03 09 00 64 31 2D 0F 35 38 32 35 32 78 FC 
//ץ��ͼƬ��ͼƬΪ����ͼ���˹�����ʱ����
2021/12/3 13:49:47.846 [TX] - F5 AE 01 01 02 00 73 00 E6 FD 
2021/12/3 13:49:48.501 [RX] - F5 AE 02 01 06 00 77 B3 23 00 D0 FA 3D FB   //Ӧ��ͼƬ��С��У��
															F5 AE 02 04 00 04 FF D8 FF E0 ......        //ʡ�Ժź�Ϊץ�ĵ�ͼƬ���ݣ�		
//ע������������Ϊ04�����ܲ��ԣ�����ʹ�øù��ܣ�Ӧ��±Ⱥ˶�
*/                              
/************************************************************************************************************/


extern FaceAdd_t FaceAdd;
extern FaceIdy_t FaceIdy;
extern FaceDel_t FaceDel;
extern OrbFaceAckStr_t OrdFace;
extern FaceReady_t FaceReady;
extern FaceUserStr_t FaceUser;
extern uint8_t FaceIdyLock;




void UartFace_DatStrInit(void);
void UartFace_PowerOn(void);
void UartFace_PowerOff(void);
void UartFace_PowerReset(void);
void UartFace_Init(void);
void UartFace_Deinit(void);
void UartFace_RtxTimeoutoutHandle(void);
void UartFace_SendNbytes(uint8_t* buf,uint16_t len);
void UartFace_RxDataHandle(void);
void UartFace_RtxTimeoutoutHandle(void); 
void UartFace_DataSendHandle(void);
void UartFace_TxQueueReset(void);

void UartFace_SendCmdData(ObFrameType_t type,uint8_t *buf,uint8_t len,uint8_t ack,uint32_t timeout);



void OrbAddFace(uint8_t id);
void OrbIdfyFace(void);
void OrbReadFaceID(void);
void OrbFaceScanHandle(void);

uint8_t GetOrbFaceFreeSlot(void);
uint8_t FaceAddHandle(uint8_t type);
uint8_t FaceDelHandle(uint8_t id);
void OrbFaceUpdateUser(uint8_t type,uint8_t id);



uint8_t FaceTest_Add(void);
uint8_t FaceTest_Scan(void);

#endif

