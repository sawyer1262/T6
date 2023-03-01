#ifndef  __FLASH_CONFIG_H_
#define  __FLASH_CONFIG_H_


#include <stdint.h>
#include "rtc_drv.h"    


//�汾����
	
#define       NO_FACE_MODE             //�Ƿ������













#define     SN_MAX_LEN            32                      //22-24λ��Ԥ��32λ
#define     PWD_MAX               100                     //���100������
#define     PWD_TIM_MAX           50                      //�������룺��ʱЧ�ģ�������ʱ����������������

#define     OTA_FLAG              0x55aa55aa
#define     PERIPHLOCK_FLAG       OTA_FLAG

/********************************************************************************************/
//�������ͣ�
typedef enum{
		FACE_OPEN=1,
		FP_OPEN,
		CODE_OPEN,
		CARD_OPEN,
}LockOpenType_t;


/********************************************************************************************
*�ڲ�Flash ����
�ڲ�flash��ʼ|code|OTA code|�û�����|����

//EFLASH���û����ÿռ�0x08000000~0x0807EFFF����508KB�����4KB��ֹʹ�ã�֧�ַ�װ��ALL
*********************************************************************************************/
#define  EFSH_START_ADDR									  (0x08000000)                                                //EFLASH��ʼ��ַ 
#define  EFSH_PAGE_SIZE											(512)																												//EFLASHÿҳ��С
#define  EFSH_SIZE 													(0x80000-0x1000)  																					//EFLASH���ÿռ䣨���4KоƬ���ã�
/*********************************************************************************************/
/*�ڲ�falsh�洢���ݷ���Ͷ�Ӧ�Ŀռ��С����*/
#define  BOOT_SIZE                          (0x8000)                             //BOOTLOADER�����С����ʱԤ��32K 
#define  APP_SIZE                           (0x2A000)                            //APP��OTA�����С
#define  CODE_SIZE													(0x5C000)                            //1��boot��0x8000��һ��app��0x2A000��һ��backup��0x2A000
#define  SYS_DATA_SIZE											(0x800)                              //Ԥ��2Kϵͳ������Ϣ            
#define  SN_ADDR                            (0x79F00)                            //���кŴ��λ��ƫ����
/*�ڲ�falsh�洢���ݷ���Ͷ�Ӧ�Ŀռ��ַ����*/
#define  SYS_DATA_ADDR											(0+CODE_SIZE)                         //ϵͳ������Ϣ
#define  SYS_CONFIG_ADDR                     SYS_DATA_ADDR                        //LockInfo_t��2�ֽ�
#define  SYS_PERIP_LOCK_ADDR                (SYS_CONFIG_ADDR+4)                   //��������48�ֽ�
#define  ONETIME_CODE_ADDR                  (SYS_PERIP_LOCK_ADDR+48)              //һ��������:12�ֽ�
#define  TAMPER_ADDR                        (ONETIME_CODE_ADDR+12)                //������Ϣ��8�ֽ�
#define  BAT_CHK_ADDR                       (TAMPER_ADDR+8)                       //��ص�ѹ��Ϣ��8�ֽ�
#define  NET_TIME_ADDR                      (BAT_CHK_ADDR+8)                      //����ʱ��״̬��8�ֽ� 
#define  LINGER_TIME_ADDR                   (NET_TIME_ADDR+8)                     //�ǻ��������ʱ�䣺8�ֽ� 
#define  COERCE_FP_ADDR                     (LINGER_TIME_ADDR+8)                  //в��ָ�ƣ�16�ֽ�  

#define  IAP_INFO_ADDR                      (SYS_DATA_ADDR+2000)
#define  USER_DATA_ADDR											(SYS_DATA_ADDR+SYS_DATA_SIZE)         //�û���Ϣ��Ŀǰ�ܹ���4+1600+800+4+800=3208
//���룺
#define  PWD_INFO_ADDR                       USER_DATA_ADDR                       //������Ϣ����Ч��������ЧʱЧ���� 4�ֽ�
#define  PWD_DATA_ADDR                      (USER_DATA_ADDR+4)                    //100�����룬ÿ��16�ֽ�            16*100=1600�ֽ� 
#define  PWD_TIM_DATA_ADDR                  (PWD_DATA_ADDR+16*PWD_MAX)            //50��ʱЧ��ÿ��16�ֽ�             16*50=800 
//ˢ��
#define  CARD_INFO_ADDR                     (PWD_TIM_DATA_ADDR+16*PWD_TIM_MAX)    //����Ϣ��������                   4�ֽ�
#define  CARD_DATA_ADDR                     (CARD_INFO_ADDR+4)                    //���û�����100*8                  800


//�ⲿFlash ����
//64Mbit---�����1MΪ�û����ݣ��ݶ������洢�������
//����������û���ɾ���û���ϵͳ���ã��������Ѿ�����ϱ��ģ���˽ģʽ��ͯ�������ر�
#define  USER_RECORD_MAX_NUM               500

//�û�������¼500��
#define  USER_EXT_START_ADDR               0x700000                               //
#define  USER_RECORD_HEAD_ADDR             USER_EXT_START_ADDR
#define  USER_RECORD_DAT_ADDR             (USER_RECORD_HEAD_ADDR+8)


/********************************************************************************/


              

//ϵͳ���������
typedef union{
		struct{
				uint32_t Language:1;                      //��Ӣ�ı�ʶ 
				uint32_t Activate:1;                      //����״̬
				uint32_t KeepOpen:1;                      //����ģʽ
				uint32_t ChildLock:1;                     //ͯ��
				uint32_t MutiIdy:1;                       //˫��֤                 
				uint32_t SupPowSave:1;                    //��ʡ��ģʽ              
				uint32_t TampAct:1;                       //���𿪹�                
				uint32_t FaceLock:1;                      //������ģ��������ʶ
				uint32_t FpLock:1;                        //ָ��ģ��������ʶ
				uint32_t KeyPADLock:1;                    //����������ʶ
				uint32_t CardLock:1;                      //ˢ��������ʶ
				uint32_t Volume:2;                        //����                    
				uint32_t OTCode:1;                        //һ���������־         
				uint32_t PrivMode:1;                      //��˽ģʽ                
			  uint32_t PirOn:1;                         //pir����                  
				uint32_t DiSel:2;                         //������ѡ��
			  int32_t TimeZone:5;                       //ʱ��:-11��+12
				uint32_t Rsv:5;                           //Ԥ��5λ
				uint32_t Flag:4;                          //0101����ʼ����ʶ            
		}Bits;
		uint32_t B32;
		uint16_t B16[2];
		uint8_t  B8[4];
}SysConfig_t;

//�Դ�����������
#pragma pack(push) 	//�������״̬
#pragma pack(1)		//�趨Ϊ1�ֽڶ���
typedef struct {
	UINT16 day;
	UINT8  hour;
	UINT8  minute;
	UINT8  second;
}mytm_t;
#pragma pack(pop)//�ָ�����״̬


#define   PERIP_FACE_INDEX        0
#define   PERIP_FP_INDEX          1
#define   PERIP_CODE_INDEX        2
#define   PERIP_CARD_INDEX        3


typedef union{
		struct{
				uint8_t FailTime[4];           //��֤�������
				uint32_t Flag;                 //��ʼ����ʶ 
				mytm_t FailStart[4];           //�״�ʧ��ʱ��
				mytm_t LockStart[4];           //��������ʼʱ��  
		}Perip;
		uint8_t PripBuf[48];
}PeripLock_t;

//�����������
typedef union{
		struct{
				uint8_t Flag;
				uint8_t Stat;
				uint8_t Time;           
				uint8_t Hour;
				uint8_t Minute;
				uint8_t Second;
				uint16_t Day;
		}Tamp;
		uint8_t TampBuf[8];                
}TamperAlarm_t;
//��ؼ�����
typedef union{
		struct{
				uint8_t Flag;                //��ʶ
				uint8_t Hour;                //ʱ
				uint8_t Minute;              //��
				uint8_t Second;              //��
				uint16_t Day;                //����
				uint16_t Value;              //��ѹֵ
		}Bat;
		uint8_t BatBuf[8];                
}LowBatAlarm_t;
//����ʱ�����
typedef union{
		struct{
				uint8_t Flag;               //��ʼ����ʶ
				uint8_t Hour;               //ʱ
				uint8_t Minute;             //�� 
				uint8_t Second;             //�� 
				uint16_t Day;               //����
				uint16_t Stat;              //״̬���Ƿ�ɹ���ȡ������ʱ��    
		}NT;
		uint8_t NTBuf[8];
}NetTime_t;

typedef union{
		struct{
				uint8_t Flag1;               //��ʼ����ʶ
				uint8_t Flag2;
				uint8_t Flag3;
				uint8_t CoerceFp[13];
		}CFP;
		uint8_t CoerceFpBuf[16];
}CoerceFp_t;

//���������Ϣ
typedef union{
		struct{
				uint32_t binSum;                 //�����ļ��ܳ���
				uint32_t chkSum;                 //�����ļ�У��
				uint32_t stat;                   //����״̬��0-��������1-������
				uint32_t pktSum;                 //��ǰ��ȡ�����ݳ���  
				uint32_t flag;                   //��ʼ����־ 
		}Det;		
		uint8_t DetBuf[20];
}IapInfo_t;


extern uint8_t MutiIdyItem;
extern SysConfig_t SysConfig;
extern SysConfig_t SysConfigBk;
extern PeripLock_t PeripLock;
extern TamperAlarm_t TamperAlarm;
extern LowBatAlarm_t LowBatAlarm;
extern char SN[SN_MAX_LEN];
extern char SnNum[7];
extern const char ver[];
extern IapInfo_t IapInfo;
extern NetTime_t NetTime;
extern uint8_t SnInvalid;
extern CoerceFp_t CoerceFp;

void FactoryReset(void);
void LockReadSysConfig(void);
void LockWriteSysConfig(void);
void LockWritePerpLcokInfo(void);
uint8_t CheckSnActiveCode(char *ActCode1);
int32_t GetPeripLockTimeElapse(mytm_t *stime);
void SysActCheck(void);
void UpdateLockSn(void);






#endif




