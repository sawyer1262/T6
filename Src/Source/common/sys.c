#include "sys.h"
#include "type.h"
#include "cpm_drv.h"
#include "tc_drv.h"
#include "delay.h"
#include "libRegOpt.h"
#include "clk_switch_drv.h"
#include "cache_drv.h"

UINT32 g_sys_clk; //ϵͳʱ��ȫ�ֱ���
UINT32 g_ips_clk;
UINT32 g_trim_clk = OSC_160M_HZ;

//����������ƫ�Ƶ�ַ
//NVIC_VectTab:��ַ
//Offset:ƫ����
void NVIC_SetVectorTable(UINT32 NVIC_VectTab, UINT32 Offset)
{
    SCB->VTOR = NVIC_VectTab | (Offset & (UINT32)0xFFFFFE00); //����NVIC��������ƫ�ƼĴ���,VTOR��9λ����,��[8:0]������
}
//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5��
void NVIC_PriorityGroupConfig(UINT8 NVIC_Group)
{
    UINT32 temp, temp1;
    temp1 = (~NVIC_Group) & 0x07; //ȡ����λ
    temp1 <<= 8;
    temp = SCB->AIRCR;  //��ȡ��ǰ������
    temp &= 0X0000F8FF; //�����ǰ����
    temp |= 0X05FA0000; //д��Կ��
    temp |= temp1;
    SCB->AIRCR = temp; //���÷���
}
//����NVIC
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~3
//ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
//�黮��:
//��0:0λ��ռ���ȼ�,3λ��Ӧ���ȼ�
//��1:1λ��ռ���ȼ�,2λ��Ӧ���ȼ�
//��2:2λ��ռ���ȼ�,1λ��Ӧ���ȼ�
//��3:3λ��ռ���ȼ�,0λ��Ӧ���ȼ�

//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����
void NVIC_Init(UINT8 NVIC_PreemptionPriority, UINT8 NVIC_SubPriority, UINT8 NVIC_Channel, UINT8 NVIC_Group)
{
    UINT32 temp;
    NVIC_PriorityGroupConfig(NVIC_Group); //���÷���
    temp = NVIC_PreemptionPriority << (3 - NVIC_Group);
    temp |= NVIC_SubPriority & (0x07 >> NVIC_Group);
    temp &= 0x7;                                             //ȡ����λ
    NVIC->ISER[NVIC_Channel / 32] |= 1 << NVIC_Channel % 32; //ʹ���ж�λ(Ҫ����Ļ�,����ICER��ӦλΪ1����)
    NVIC->IP[NVIC_Channel] |= temp << 5;                     //������Ӧ���ȼ����������ȼ�
}
////�ⲿ�ж����ú���
////ֻ���GPIOA~I;������PVD,RTC,USB_OTG,USB_HS,��̫�����ѵ�
////����:
////GPIOx:0~8,����GPIOA~I
////BITx:��Ҫʹ�ܵ�λ;
////TRIM:����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
////�ú���һ��ֻ������1��IO��,���IO��,���ε���
////�ú������Զ�������Ӧ�ж�,�Լ�������
//void Ex_NVIC_Config(UINT8 GPIOx,UINT8 BITx,UINT8 TRIM)
//{
//UINT8 EXTOFFSET=(BITx%4)*4;
//RCC->APB2ENR|=1<<14;  //ʹ��SYSCFGʱ��
//SYSCFG->EXTICR[BITx/4]&=~(0x000F<<EXTOFFSET);//���ԭ�����ã�����
//SYSCFG->EXTICR[BITx/4]|=GPIOx<<EXTOFFSET;//EXTI.BITxӳ�䵽GPIOx.BITx
////�Զ�����
//EXTI->IMR|=1<<BITx;//����line BITx�ϵ��ж�(���Ҫ��ֹ�жϣ��򷴲�������)
//if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx���¼��½��ش���
//if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx���¼��������ش���
//}

//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI

void WDT_Open_Clk_Gate(void)
{
		CPM->CPM_IPSCGTCR |= (1<<1);  //open WDT IPS clk gate
}

//�������ģʽ
void Sys_Standby(void)
{
    SCB->SCR |= 1 << 2; //ʹ��SLEEPDEEPλ (SYS->CTRL)
    //RCC->APB1ENR|=1<<28;//ʹ�ܵ�Դʱ��
    //PWR->CSR|=1<<8;     //����WKUP���ڻ���
    //PWR->CR|=1<<2;      //���Wake-up ��־
    //PWR->CR|=1<<1;      //PDDS��λ
		CPM->CPM_IPSCGTCR &= ~(1<<1);  //close WDT IPS clk gate
    if ((CPM->CPM_SLPCFGR & 0x80000000) == 0)
    {
        //CPM->CPM_PWRCR &=~ 0x0c;//disable LVD if enter lp/poff1/poff1.5 mode.
        CPM_PWRCR_OptBits(0x0c, RESET);
    }
    else
    {
        //CPM->CPM_PWRCR |= 0x0c;//enable LVD if enter poff2 mode.
        CPM_PWRCR_OptBits(0x0c, SET);
    }
    WFI_SET(); //ִ��WFIָ��,�������ģʽ
        //CPM->CPM_PWRCR |= 0x0c;//enable LVD after wake-up from lowpower.
		CPM->CPM_IPSCGTCR |= (1<<1);  //open WDT IPS clk gate
    CPM_PWRCR_OptBits(0x0c, SET);
}
//ϵͳ��λ
void Sys_Soft_Reset(void)
{
    SCB->AIRCR = 0X05FA0000 | (UINT32)0x04;
}

void Sys_Init(void)
{
    //trimʱ�ӡ�����ʱ�ӷ�Ƶ��
    Sys_Clk_Init(OSC_120M_HZ, SYS_CLK_DIV_1, IPS_CLK_DIV_2);

    //����EFLASH����ģ��
    DCACHE_Init(cacheThrough, cacheThrough, cacheOff, cacheOff);
    ICACHE_Init(cacheThrough, cacheThrough, cacheOff, cacheOff);

    CPM_ClearPADWKINTCR();      //���Ĭ���жϻ���Դ

    //CPM_VCC5V_Bypass(); //оƬ3.3V��1.8V����ʱ���Թ���Ҫ��ߣ�����򿪴˽ӿڣ��Թ���Ҫ�󲻸߿��Բ���; оƬ5V����ʱ�������δ˽ӿ�,������ܻ�оƬ������
}

void WFI_SET(void)
{
    __ASM volatile("wfi");
}
//�ر������ж�(���ǲ�����fault��NMI�ж�)
void INTX_DISABLE(void)
{
    __ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
    __ASM volatile("cpsie i");
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(UINT32 addr)
{
    MSR MSP, r0 //set Main Stack value
                 BX r14
}

