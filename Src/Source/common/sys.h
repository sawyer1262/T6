#ifndef __SYS_H__
#define __SYS_H__	 

#include "ccm4202s.h" 
#include "type.h"

//0,��֧��OS
//1,֧��OS
#define SYSTEM_SUPPORT_OS		0		//����ϵͳ�ļ����Ƿ�֧��OS
///////////////////////////////////////////////////////////////////////////////////

//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).M4ͬM3����,ֻ�ǼĴ�����ַ����.
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 


extern UINT32 g_sys_clk;	//ϵͳʱ��ȫ�ֱ���
extern UINT32 g_ips_clk;
extern UINT32 g_trim_clk;

////////////////////////////////////////////////////////////////////////////////// 

extern void Sys_Soft_Reset(void);      							//ϵͳ��λ
extern void Sys_Standby(void);         							//����ģʽ 	
extern void NVIC_SetVectorTable(UINT32 NVIC_VectTab, UINT32 Offset);	//����ƫ�Ƶ�ַ
extern void NVIC_PriorityGroupConfig(UINT8 NVIC_Group);			//����NVIC����
extern void NVIC_Init(UINT8 NVIC_PreemptionPriority,UINT8 NVIC_SubPriority,UINT8 NVIC_Channel,UINT8 NVIC_Group);//�����ж�
extern void Ex_NVIC_Config(UINT8 GPIOx,UINT8 BITx,UINT8 TRIM);				//�ⲿ�ж����ú���(ֻ��GPIOA~I)
extern void Sys_Init(void);

//����Ϊ��ຯ��
extern void WFI_SET(void);		//ִ��WFIָ��
extern void INTX_DISABLE(void);//�ر������ж�
extern void INTX_ENABLE(void);	//���������ж�
extern void MSR_MSP(UINT32 addr);	//���ö�ջ��ַ 
#endif		/*__SYS_H__*/











