#ifndef  __LOCK_RECORD_H_
#define  __LOCK_RECORD_H_


#include <stdint.h>
#include "lock_config.h"


typedef enum{
		CTL_OPEN_LOCK       = 0,
		CTL_CLOSE_LOCK,
    CTL_DOOR_BELL	
}LcokCtlType_t;


//�û������洢
typedef enum{
			//ϵͳ�������
			LANG_SET         = 1,           //��������
			ACTIVATE_SET     = 2,           //��Ʒ����
			KEEP_OPEN_SET    = 3,           //����ģʽ����         //��ʱû�������ͽ��棬����
			CHILD_LOCK_SET   = 4,           //ͯ������  
			MUTILIDY_SET     = 5,           //˫��֤ģʽ���� 
			POWERSAVE_SET    = 6,           //��ʡ��ģʽ
			TAMPER_SET       = 7,           //�������� 
			FACE_LOCK_CHG    = 8,           //����ģ������  
			FP_LOCK_CHG      = 9,           //ָ��ģ������
			KEYPAD_LOCK_CHG  = 10,          //��������
			CARD_LOCK_CHG    = 11,          //ˢ������
			VOLUME_SET       = 12,          //�������� 
			OTCODE_CHG       = 13,          //һ��������״̬
			PRIVMODE_SET     = 14,          //��˽ģʽ����
			PIR_SET          = 15,          //PIR����
			//����û�
			ADD_USER         = 16,          //����û�
			DEL_USER         = 17,          //ɾ���û�
			//����
			OPEN_LOCK        = 18,          //���� 
			//����
			DOOR_BELL        = 19,          //����   
			//���𴥷�    
			TAMP_TRIG        = 20,          //���𴥷�
			//�ָ���������
			FACTORY_RESET    = 21,           //�ָ���������
			
			OPERA_RESV        
}UserOperaType_t;

typedef union{
		struct{
				uint8_t isChinese;            //��Ӣ�ı�ʶ
		}LAN;
		struct{
				uint8_t isActivate;           //����
		}ACT;
		struct{
				uint8_t isEnKeepOpen;         //������ʶ 
		}KP;
		struct{
				uint8_t isEnChildLock;        //ͯ������
		}CL;
		struct{
				uint8_t isEnMutiIdy;          //˫����֤ 
		}MI;
		struct{
				uint8_t isEnPowSave;          //��ʡ��ģʽ
		}PS;
		struct{
				uint8_t isEnTamp;             //���𿪹�ʹ��״̬�ı�   
		}TP;
		struct{
				uint8_t isTampTrig;           //���𴥷�  
		}TPTRIG;
		struct{
				uint8_t isFaceLock;           //����ʶ������
		}FACEL;
		struct{
				uint8_t isFpLock;             //ָ��ģ������ 
		}FPL;
		struct{
				uint8_t isKeypadLock;         //��������   
		}KEYL;
		struct{
				uint8_t isCardLock;           //ˢ������  
		}CARDL;
		struct{
				uint8_t volume;               //��������
		}VOLU;
		struct{
				uint8_t isBell;               //���崥��  
		}DB;
		struct{
				uint8_t isFactoryReset;       //�ָ��������ô���
		}FR;
		struct{
				uint8_t isEnPrivMode;         //��˽ģʽ  
		}PRIV;
		struct{
				uint8_t isEnPir;              //pir���� 
		}PIR;
		struct{
				uint8_t isAdd;                //��ӻ�ɾ��
		}OTCODE;                          //һ��������  
		struct{
				uint8_t Type;                 //���ͣ�������ָ�ƣ����룬ˢ��
				uint8_t Dat[13];              //����ID��ָ��ID������12λ+0��ˢ��4�ֽڿ���
		}ADUSR;
		struct{
				uint8_t Type;                 //���ͣ�������ָ�ƣ����룬ˢ��
				uint8_t Dat[13];              //����ID��ָ��ID������12λ+0��ˢ��4�ֽڿ���
		}DELUSR;
		struct{
				uint8_t Type;                 //���ͣ�������ָ�ƣ����룬ˢ��
				uint8_t Dat[13];              //����ID��ָ��ID������12λ+0��ˢ��4�ֽڿ���
		}MODADM;
		struct{
				uint8_t Type;                 //���ͣ�������ָ�ƣ����룬ˢ��
				uint8_t Dat[13];               //����ID��ָ��ID������12λ+0��ˢ��4�ֽڿ���
		}OPLOCK;
		uint8_t OpaDatBuf[14];            //�14�ֽ� 
}UserOperaDat_t;


typedef union{
		struct{
				UserOperaType_t OpType;       //��������
				UserOperaDat_t OpDat;         //����������� 
				mytm_t OpTime;                 //����ʱ��
		}OpDet;
		uint8_t OpDetBuf[20];  
}UserOpera_t;   

typedef union{
		struct{
				uint16_t Flag;                //��ʼ����־ 
				uint16_t WriteIndex;          //����д��ָ��
				uint32_t RecordSum;           //��¼�������������500����Ȼֻ����500��
		}OpHead;
		uint8_t OpHeadBuf[8];
}UserOperaHead_t;



extern UserOperaHead_t UserOperaHead;


uint8_t LockWriteUserOpera(UserOpera_t *oprea);
uint8_t LockReadUserOpera(void *out,uint16_t index);
uint8_t UserControlLock(uint8_t ctrlType,uint8_t usrType,uint8_t pIdx);
uint8_t LockAddUser(uint8_t usrType,uint8_t pIdx) ;
uint8_t LockDelUser(uint8_t usrType,uint8_t pIdx);
uint8_t LockFactoryResetOpera(void);
uint8_t LockTampTrigOpera(void);
uint8_t LockModifySysConfigOpera(uint32_t oldVal,uint32_t newVal);
void WriteUserOpera(void);


#endif



