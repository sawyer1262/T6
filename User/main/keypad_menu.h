#ifndef  __KEYPAD_MENU_H_
#define  __KEYPAD_MENU_H_


#include <stdint.h>

//��������������󳤶�
#define    INPUT_MAX         26
//��ǰ���в˵�
typedef void (*MenuFun)(void);

//����ģʽ������ģʽ
typedef enum{
		KEYPAD_NORMAL=0,              //����ģʽ
		KEYPAD_CONFIG                 //����ģʽ
}KeyMode_t;

extern MenuFun MenuNow;
extern KeyMode_t KeyPadMode;




extern uint8_t AteStat;
extern uint8_t AteSubStat;

//
uint8_t GetKeyLedBit(uint8_t key);
//Ĭ����ҳ�˵�
void Menu_KeyPadNormal(void);                      //���˵�
//�����Ӳ˵��˵�ѡ��
void Menu_KeyPadConfigSelect(void);                //ϵͳ����ѡ��
//ϵͳ�����Ӳ˵�
void Menu_KeyPadAddUsers(void);                    //����û� 
void Menu_KeyPadDelUsers(void);                    //ɾ���û�
void Menu_KeyPadModifyAdmUsers(void);              //�޸Ĺ���Ա�˻�
void Menu_KeyPadConfNet(void);                     //��������
void Menu_KeyPadSysConf(void);                     //ϵͳ����
//ϵͳ�����Ӳ˵�
void Menu_KeyPadSysConf_Security(void);            //��ȫģʽ����
void Menu_KeyPadSysConf_Volume(void);              //�������� 
void Menu_KeyPadSysConf_Language(void);            //��������
void Menu_KeyPadSysConf_PowerSave(void);           //��ʡ��ģʽ���� 
void Menu_KeyPadSysConf_KeepOpen(void);            //����ģʽ����
void Menu_KeyPadSysConf_PirSet(void);              //PIR��������
//����û��Ӳ˵���
void Menu_KeyPadAddUsers_Face(void);               //�������
void Menu_KeyPadAddUsers_Code(void);               //�������
void Menu_KeyPadAddUsers_Card(void);               //��ӿ�Ƭ  
void Menu_KeyPadAddUsers_Fp(void);                 //���ָ��
void Menu_KeyPadAddUsers_OnetimeCode(void);        //���һ�������� 
//ɾ���û��Ӳ˵�
void Menu_KeyPadDelUsers_Face(void);               //ɾ������
void Menu_KeyPadDelUsers_Fp(void);                 //ɾ��ָ�� 
void Menu_KeyPadDelUsers_Code(void);               //ɾ������ 
void Menu_KeyPadDelUsers_Card(void);               //ɾ����Ƭ
void Menu_KeyPadDelUsers_OnetimeCode(void);        //ɾ��һ��������
void Menu_KeyPadDelUsers_All(void);                //ɾ�������û� 
void Menu_KeyPadDelUsers_select(void);             //ɾ��ָ�����û�
//�޸Ĺ���Ա�˻��Ӳ˵�
void Menu_KeyPadModifyAdmUsers_Face(void);         //�޸Ĺ���Ա����
void Menu_KeyPadModifyAdmUsers_Fp(void);           //�޸Ĺ���Աָ�� 
void Menu_KeyPadModifyAdmUsers_Card(void);         //�޸Ĺ���Ա��
void Menu_KeyPadModifyAdmUsers_Password(void);     //�޸Ĺ���Ա���� 

void Menu_KeyPadAteTest(void);

#endif

