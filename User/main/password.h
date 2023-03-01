#ifndef  __PASSWORD_H_
#define  __PASSWORD_H_


#include <stdint.h>
#include "lock_config.h"
#include "rtc_drv.h"


//���룺100�����루��в�����룩

#define      PWD_LEN_MAX               13
#define      PWD_LEN_MIN               6 
#define      PWD_TIM_LEN_MAX           16

//������뷵��
#define      PWD_ADD_SUCESS            0x00         //��ӳɹ�
#define      PWD_ADD_TYP_WRONG         0x01         //���ʹ���
#define      PWD_ADD_MEM_FULL          0x02         //���������
#define      PWD_ADD_TIME_MEM_FULL     0x03         //��ʱЧ���������
#define      PWD_ADD_CODE_EXIST        0x04         //�����Ѵ���
//ɾ�����뷵�� 
#define      PWD_DEL_SUCCESS           0x00         //ɾ������ɹ�
#define      PWD_DEL_CODE_NOT_EXIST    0x01         //���벻����
//ʶ�����뷵��
#define      PWD_IDY_ONETIME_CODE      0xFE         //һ��������
#define      PWD_IDY_CODE_NOT_EXIST    0xFF         //���벻���� 
//0-99                                              //����ʶ��ɹ���������

typedef enum{
		INVALID,                     //��Ч����
		NORMAL,                      //��ͨ����
		ADMIN,                       //����Ա����
		COERCE,                      //в������              
		VISITOR,                     //�ÿ����� 
}PwdType_t;

typedef enum{
		INACTIVE,                     //��Ч����
		PWD_TIME_CYC,                 //�ÿ����루�������룩
		PWD_TIME_FEQ                  //�������
}PwdTimType_t;


#pragma pack(push) 	//�������״̬
#pragma pack(1)		//�趨Ϊ1�ֽڶ���

typedef struct{
		mytm_t StartTime;                 //һ�������뿪ʼʱ��
		uint8_t OneCode[7];               //����
}OneTimeCode_t;


//ͨ������ṹ
typedef union{
		struct{
				uint8_t Type;                //�������ͣ���ͨ������в������
				uint8_t Idx;                 //�����ʱЧ���룬idx������ʱЧ�ṹ�е�λ�� 
				uint8_t Buf[PWD_LEN_MAX+1];  //�������ݣ�6-12λ������0
		}Pwd;
		uint8_t PwdBuf[PWD_LEN_MAX+3];   //���룺Ϊ�˴�4�ֽڶ���
}PwdGenStr_t;

//ʱЧ�ṹ
typedef union{
		struct{
				uint8_t Type;                //PWD_TIME_CYC  
				uint8_t Week;
				uint8_t StartHour;
				uint8_t StartMin;
				uint8_t StopHour;
				uint8_t StopMin;
				uint32_t TimeOutUtc;         //ʧЧʱ���
		}PwdTimCyc;
		struct{
				uint8_t Type;                //PWD_TIME_FEQ
				uint8_t Times;
				uint32_t TimeOutUtc;         //ʧЧʱ���
		}PwdTimeFreq;
		uint8_t PwdTimBuf[PWD_TIM_LEN_MAX];
}PwdExtStr_t;
#pragma pack(pop)//�ָ�����״̬

typedef union{
		struct{
				uint16_t PwdSum;
				uint16_t PwdTimSum;
		}Item;
		uint8_t ItemBuf[4];
}PwdInfo_t;


typedef struct{
		PwdInfo_t PwdInfo;                   //������Ϣ
		PwdGenStr_t PwdGenStr[PWD_MAX];      //���뼯��    
		PwdExtStr_t PwdTimStr[PWD_TIM_MAX];  //����ʱЧ����
}PwdStr_t;



extern PwdStr_t Pwd;
extern OneTimeCode_t OneTimeCode;


void PassWordInit(void);
uint8_t CheckPwdVail(uint8_t* pPwd,uint8_t nLen);
uint8_t CheckPwdTimVail(PwdExtStr_t *time);
uint8_t GetPassWordFreeSlot(void);
uint8_t IdyPassWord(uint8_t *code);
uint8_t AddPassWord(uint8_t type,uint8_t *code,PwdExtStr_t *ext,uint8_t *slot);
uint8_t DelPassWord(uint8_t slot);
void DelAllPassword(void);
uint8_t CheckOneTimeCodeValid(OneTimeCode_t* onecode);
uint8_t GetCodeWithoutX(char *in,char *out);
uint8_t CheckAdminCode(void);
uint8_t CheckOnetimeCodeExist(uint8_t *code);
void CheckTempCodeValid(void);


#endif


