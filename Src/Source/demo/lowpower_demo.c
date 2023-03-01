/**
 * @file lowpower_demo.c
 * @author ��ƷӦ�ò�
 * @brief 
 * @version 1.2
 * @date 2021-03-28
 * 1. ע�⣬����ǰҪ��eport 0��1��2���жϹ��ܣ��ٶ����һ���жϣ���������оƬ��normal sleep���������ó�Ϊ��ƽ������
 * @copyright Copyright (c) 2021
 * 
 */

#include "cpm_drv.h"
#include "debug.h"
#include "lowpower_demo.h"
#include "eport_drv.h"
#include "tc_drv.h"
#include "wdt_drv.h"
#include "delay.h"
#include "cache_drv.h"
#include "trng_drv.h"
#include "delay.h"
#include "main.h"

#ifndef LOWPOWER_CONFIG
#include "tsi_app.h"
#include "pci_drv.h"
#include "c0_drv.h"

/**
 * @brief: ģ��IO���Գ��򣬲��������Ļ��ѹ���
 */
void Lowpower_function_test(void)
{
    uint delayTime = 0;
    //1. ��ʼ��
    Init_Trng();
    // GetRandomWord();

    //2. EPORT����ܽ�����
    EPORT_ConfigGpio(EPORT_PIN2, GPIO_OUTPUT);  
    EPORT_ConfigGpio(EPORT_PIN3, GPIO_OUTPUT);  
    EPORT_ConfigGpio(EPORT_PIN4, GPIO_OUTPUT);  
    EPORT_ConfigGpio(EPORT_PIN5, GPIO_OUTPUT);  
    EPORT_ConfigGpio(EPORT_PIN23, GPIO_OUTPUT);  
    EPORT_ConfigGpio(EPORT_PIN24, GPIO_OUTPUT);  
    EPORT_ConfigGpio(EPORT_PIN25, GPIO_OUTPUT);  
    EPORT_ConfigGpio(EPORT_PIN26, GPIO_OUTPUT);  
    EPORT_ConfigGpio(EPORT_PIN27, GPIO_OUTPUT); 
    EPORT_ConfigGpio(EPORT_PIN28, GPIO_OUTPUT); 

    //3.
    EPORT_WriteGpioData(EPORT_PIN2, Bit_SET);
    delayTime = GetRandomWord() / 1000000;
    printf("delayTime: %d\n", delayTime);
    DelayMS(delayTime);
    EPORT_WriteGpioData(EPORT_PIN2, Bit_RESET);

    EPORT_WriteGpioData(EPORT_PIN3, Bit_SET);
    printf("delayTime: %d\n", delayTime);
    delayTime = GetRandomWord() / 100000;
    DelayMS(delayTime);
    EPORT_WriteGpioData(EPORT_PIN3, Bit_RESET);

    EPORT_WriteGpioData(EPORT_PIN4, Bit_SET);
    printf("delayTime: %d\n", delayTime);
    delayTime = GetRandomWord() / 1000000;
    DelayMS(delayTime);
    EPORT_WriteGpioData(EPORT_PIN4, Bit_RESET);

    EPORT_WriteGpioData(EPORT_PIN5, Bit_SET);
    printf("delayTime: %d\n", delayTime);
    delayTime = GetRandomWord() / 100000;
    DelayMS(delayTime);
    EPORT_WriteGpioData(EPORT_PIN5, Bit_RESET);

    EPORT_WriteGpioData(EPORT_PIN23, Bit_SET);
    printf("delayTime: %d\n", delayTime);
    delayTime = GetRandomWord() / 1000000;
    DelayMS(delayTime);
    EPORT_WriteGpioData(EPORT_PIN23, Bit_RESET);

    EPORT_WriteGpioData(EPORT_PIN23, Bit_SET);
    printf("delayTime: %d\n", delayTime);
    delayTime = GetRandomWord() / 100000;
    DelayMS(delayTime);
    EPORT_WriteGpioData(EPORT_PIN24, Bit_RESET);

    EPORT_WriteGpioData(EPORT_PIN24, Bit_SET);
    printf("delayTime: %d\n", delayTime);
    delayTime = GetRandomWord() / 1000000;
    DelayMS(delayTime);
    EPORT_WriteGpioData(EPORT_PIN25, Bit_RESET);

    EPORT_WriteGpioData(EPORT_PIN26, Bit_SET);
    printf("delayTime: %d\n", delayTime);
    delayTime = GetRandomWord() / 100000;
    DelayMS(delayTime);
    EPORT_WriteGpioData(EPORT_PIN27, Bit_RESET);

    EPORT_WriteGpioData(EPORT_PIN27, Bit_SET);
    printf("delayTime: %d\n", delayTime);
    delayTime = GetRandomWord() / 1000000;
    DelayMS(delayTime);
    EPORT_WriteGpioData(EPORT_PIN28, Bit_RESET);

    EPORT_WriteGpioData(EPORT_PIN28, Bit_SET);
    printf("delayTime: %d\n", delayTime);
    delayTime = GetRandomWord() / 100000;
    DelayMS(delayTime);
    EPORT_WriteGpioData(EPORT_PIN11, Bit_RESET);
}

/**
 * @brief: ����ǰ��Ҫ������ػ������ݣ�չʾ������������, LP���Ѳ���Ҫcpm_handlwakeup
 */
void Lowpower_wakeupSource_config(void)
{
    //ϵͳ��ʼ��, �˴���������˵��ʵ�ʿ��Խ�ʱ�ӳ�ʼ���������ⲿ
    Sys_Init();

    //1. c0�����߼�����
    drv_SubC0_init(hSubC0); //����C0���ѡ�GINT0/1/4/5��ƽ״̬��

    //2. TSI������������
    app_tsi_init(&htsi);
    for (uint32_t i = 0; i < 2000000; i++)
    {
        app_tsi_handle(&htsi);
        if (htsi.Touch.Current < TSI_MULTI_VALID) // �����ް���
        {
            printf("Touch Key is %d\r\n", htsi.Touch.Current);
        }
    }

    //3. EPORT��������
    EPORT_PullConfig(EPORT_PIN2, EPORT_PULLUP);
    EPORT_Init(EPORT_PIN2, FALLING_EDGE_INT);
    EPORT_PullConfig(EPORT_PIN3, EPORT_PULLUP);
    EPORT_Init(EPORT_PIN3, FALLING_EDGE_INT);
    EPORT_PullConfig(EPORT_PIN4, EPORT_PULLUP);
    EPORT_Init(EPORT_PIN4, FALLING_EDGE_INT);
    EPORT_PullConfig(EPORT_PIN5, EPORT_PULLUP);
    EPORT_Init(EPORT_PIN5, FALLING_EDGE_INT);

    //4. �첽��ʱ����������
    PCI_Init(EXTERNAL_CLK_SEL);
    PCI->PCI_CR &= ~(0xffu << 24); // NVSRAM don`t Clear
    //��������ǰ��Ҫ�����Ӧ���ж�λ��ȷ��������

    TC_selectClockSource(1); //���ߺ�ѡ���ⲿʱ��Դ
    PCI_AsyncTC_Init(2000);  //�����첽��ʱ��ʱ��

    //5. ����ǰ����TSI��C0����͹���
    drv_SubC0_sleep();    //����C0����͹���
    app_tsi_sleep(&htsi); //����TSI����͹���
}

/**
 * @brief: ���Ѻ���Ҫ���еĳ�ʼ�����������
 */
void Lowpower_wakeup_config(void)
{
    //1. cache init ����cache����
    DCACHE_Init(cacheOff, cacheThrough, cacheOff, cacheOff);
    ICACHE_Init(cacheOff, cacheThrough, cacheOff, cacheOff);

    //2. ���Ѻ���Ҫ���³�ʼ��TSI
    app_tsi_init(&htsi);
    for (uint32_t i = 0; i < 2000000; i++)
    {
        app_tsi_handle(&htsi);
        if (htsi.Touch.Current < TSI_MULTI_VALID) // �����ް���
        {
            printf("Touch Key is %d\r\n", htsi.Touch.Current);
        }
    }

    //3. ��ʼ��PCI���ֵ���Ĵ���
    PCI_InitWakeup(); // PCI reinit.
    drv_SubC0_wakeup();
}

/**
 * @brief PLP�͹�������ʾ������.
 * 1. LP�͹�������:
 *              CPU SRAM�����磻SRAM���Ա������ݣ�����֮��,����Ӷϵ㴦��ʼִ��.
 *              M4-42uA��ʵ�⣩; TSI-2uA; PCI-1.5uA; RTC-2.5uA
 * @pre: 
 * @Question: 
 *        1. ����жϻ���Դ����Ҫ���жϻ���Դ���߼���ƺû���Ԥ�����ͻ�
 *        2. ����LPģʽǰ��Ҫ���úû�����صĴ����߼� 
 * 
 */
void Lowpower_lp_demo(void)
{
    while (1)
    {
        Lowpower_wakeupSource_config();
        printf("enter LP,:CPM->CPM_PADWKINTCR = 0x%08x\n", CPM->CPM_PADWKINTCR);
        CPM_Sleep();
        printf("wakeup from LP, CPM->CPM_PADWKINTCR = 0x%08x\n", CPM->CPM_PADWKINTCR);
        Lowpower_wakeup_config();
    }
}

/**
 * @brief 
 * 
 * @return uint32_t 
 */
void Lowpower_goto_POFF1P0(void)
{
    /* ����AsycTimer��ʱ���� */
    PCI_AsyncTC_Init(2000);

    /* ����TSI����͹��� */
    app_tsi_sleep(&htsi);
    // printf_tsiReg();

    /* ����SubC0����͹��� */
    drv_SubC0_sleep();

    printf("goto POFF1.0... %08x\r\n", CPM->CPM_PADWKINTCR);
    CPM_PowerOff_1();
}

/**
 * @brief 
 * 
 * @return uint32_t 
 */
void Lowpower_reinit_fromPOFF1P0(void)
{
    UART_Debug_Init(SCI1, g_ips_clk, 115200);
    // WDT_Close();
    // TC_Close();

    DCACHE_Init(cacheOff, cacheThrough, cacheOff, cacheOff); // ����EFLASH����ģ��
    ICACHE_Init(cacheOff, cacheThrough, cacheOff, cacheOff);

    PCI_InitWakeup(); // PCI reinit.
    drv_SubC0_wakeup();
    IO_Latch_Clr();
    printf("\n\n\nwakeup from POFF1.0...\n");
}

/**
 * @brief POFF1.5�͹�������ʾ������.
 * 1. POFF1.5�͹�������:
 *              CPU��ȫ���磻����SRAM�޷��������ݣ�����֮��,�����Reset_Handler��ʼִ��.
 *              M4-6uA; TSI-2uA; PCI-2uA
 * @pre cpm_handleWakeup() ���cpm_handleWakeup()���ֻ���Դ.
 * 
 */
void Lowpower_poff1p0_demo(void)
{
    e_CPM_wakeupSource_Status rst_source = *(e_CPM_wakeupSource_Status *)CPM_WAKEUPSOURCE_REG;
    printf("\r\nReset source is:%d\r\n", rst_source);

    // if ((rst_source == WAKEUP_SOURCE_POR) || (rst_source == WAKEUP_SOURCE_NONE))
    {
        Sys_Init();

        PCI_Init(EXTERNAL_CLK_SEL);
        PCI->PCI_CR &= ~(0xffu << 24); // NVSRAM don`t Clear

        // do
        // {
        //     CPM->CPM_PADWKINTCR |= 0xFF;
        // } while (CPM->CPM_PADWKINTCR & 0xFF);

        TC_selectClockSource(1);

        drv_SubC0_init(hSubC0);
    }

    /* ����gint0~5�жϻ��� */
    EPORT_PullConfig(EPORT_PIN2, EPORT_PULLUP); // TODO Ϊʲô����֮�����EPORT�жϱȽ���
    EPORT_Init(EPORT_PIN2, FALLING_EDGE_INT);
    EPORT_PullConfig(EPORT_PIN3, EPORT_PULLUP);
    EPORT_Init(EPORT_PIN3, FALLING_EDGE_INT);
    EPORT_PullConfig(EPORT_PIN4, EPORT_PULLUP);
    EPORT_Init(EPORT_PIN4, FALLING_EDGE_INT);
    EPORT_PullConfig(EPORT_PIN5, EPORT_PULLUP);
    EPORT_Init(EPORT_PIN5, FALLING_EDGE_INT);

    app_tsi_init(&htsi);
    for (uint32_t i = 0; i < 2000000; i++)
    {
        app_tsi_handle(&htsi);
        if (htsi.Touch.Current < TSI_MULTI_VALID) // �����ް���
        {
            printf("Touch Key is %d\r\n", htsi.Touch.Current);
        }
    }

    while (1)
    {
        DelayMS(1000);
        Lowpower_goto_POFF1P0();

        Lowpower_reinit_fromPOFF1P0();

        /* device������reinit */
        app_tsi_init(&htsi);
        for (uint32_t i = 0; i < 2000000; i++)
        {
            app_tsi_handle(&htsi);
            if (htsi.Touch.Current < TSI_MULTI_VALID) // �����ް���
            {
                printf("Touch Key is %d\r\n", htsi.Touch.Current);
            }
        }
    }
}

/**
 * @brief POFF1.5�͹�������ʾ������.
 * 1. POFF1.5�͹�������:
 *              CPU��ȫ���磻����SRAM�޷��������ݣ�����֮��,�����Reset_Handler��ʼִ��.
 *              M4-6uA; TSI-2uA; PCI-2uA
 * @pre cpm_handleWakeup() ���cpm_handleWakeup()���ֻ���Դ.
 * 
 */
void Lowpower_poff1p5_demo(void)
{
    e_CPM_wakeupSource_Status rst_source = *(e_CPM_wakeupSource_Status *)CPM_WAKEUPSOURCE_REG;
    printf("\r\nReset source is:%d\r\n", rst_source);

    if ((rst_source == WAKEUP_SOURCE_POR) || (rst_source == WAKEUP_SOURCE_NONE))
    {
        Sys_Init();

        PCI_Init(EXTERNAL_CLK_SEL);
        PCI->PCI_CR &= ~(0xffu << 24); // NVSRAM don`t Clear

        // do
        // {
        //     CPM->CPM_PADWKINTCR |= 0xFF;
        // } while (CPM->CPM_PADWKINTCR & 0xFF);

        TC_selectClockSource(1);

        drv_SubC0_init(hSubC0);
    }
    else
    {
        DCACHE_Init(cacheOff, cacheThrough, cacheOff, cacheOff); // ����EFLASH����ģ��
        ICACHE_Init(cacheOff, cacheThrough, cacheOff, cacheOff);

        PCI_InitWakeup(); // PCI reinit.

        drv_SubC0_wakeup();
    }

    /* ����gint0~5�жϻ��� */
    EPORT_PullConfig(EPORT_PIN2, EPORT_PULLUP);
    EPORT_Init(EPORT_PIN2, FALLING_EDGE_INT);
    EPORT_PullConfig(EPORT_PIN3, EPORT_PULLUP);
    EPORT_Init(EPORT_PIN3, FALLING_EDGE_INT);
    EPORT_PullConfig(EPORT_PIN4, EPORT_PULLUP);
    EPORT_Init(EPORT_PIN4, FALLING_EDGE_INT);
    EPORT_PullConfig(EPORT_PIN5, EPORT_PULLUP);
    EPORT_Init(EPORT_PIN5, FALLING_EDGE_INT);

    /* ����TSI����͹��� */
    app_tsi_init(&htsi);
    for (uint32_t i = 0; i < 2000000; i++)
    {
        app_tsi_handle(&htsi);
        if (htsi.Touch.Current < TSI_MULTI_VALID) // �����ް���
        {
            printf("Touch Key is %d\r\n", htsi.Touch.Current);
        }
    }
    app_tsi_sleep(&htsi);
    //    printf_tsiReg();

    /* ����SubC0����͹��� */
    drv_SubC0_sleep();

    /* ����AsycTimer��ʱ���� */
    PCI_AsyncTC_Init(2000);
    while (1)
    {
        printf("Enter POFF1.5... %08x\r\n", CPM->CPM_PADWKINTCR);
        CPM_PowerOff_1p5();
    }
}
#endif

void Lowpower_Demo(void)
{
    DelayMS(2000); //����ʱ���������ʱ��Ԥ���ϵ����͹����޷�����

    EPORT_Init(EPORT_PIN5, LOW_LEVEL_INT);

    EPORT_ConfigGpio(EPORT_PIN1, GPIO_INPUT);
    EPORT_ConfigGpio(EPORT_PIN2, GPIO_INPUT);
    EPORT_ConfigGpio(EPORT_PIN3, GPIO_INPUT);
    EPORT_ConfigGpio(EPORT_PIN4, GPIO_INPUT);

    while (1)
    {
        //���USB�Ƿ��в���
        if (get_usb_det_sta() == TRUE)
        {
            printf("USB DET STA IS H\r\n");
        }

        //eport 1 - normal sleep
        if (EPORT_ReadGpioData(EPORT_PIN1) == 0)
        {
            DelayMS(50);
            if (EPORT_ReadGpioData(EPORT_PIN1) == 0)
            {

                printf("Enter sleep...\r\n");
                CPM_Sleep();
                printf("wake up from sleep.\r\n");
            }
        }

        //eport 2 - poweroff 1
        if (EPORT_ReadGpioData(EPORT_PIN2) == 0)
        {
            DelayMS(50);
            if (EPORT_ReadGpioData(EPORT_PIN2) == 0)
            {

                printf("enter power off 1...\r\n");
                CPM_PowerOff_1();                         //POFF1����ջ���ϵ���Ҫ��ʼ����ȫ�ֱ����ŵ�DRAM�У���0x1FFF8000~0x1FFFFFFF
                WDT_Close();                              //�ر�CLK
                TC_Close(TC);                               //�ر�TC
                UART_Debug_Init(SCI1, g_ips_clk, 115200); //��ʼ������
                printf("wake up from sleep.\r\n");
            }
        }

        //eport 3 - poweroff 1.5
        if (EPORT_ReadGpioData(EPORT_PIN3) == 0)
        {
            DelayMS(50);
            if (EPORT_ReadGpioData(EPORT_PIN3) == 0)
            {

                printf("enter power off 1.5...\r\n");
                CPM_PowerOff_1p5();
            }
        }

        //eport 4 - poweroff 2
        if (EPORT_ReadGpioData(EPORT_PIN4) == 0)
        {
            DelayMS(50);
            if (EPORT_ReadGpioData(EPORT_PIN4) == 0)
            {
                printf("\n\tenter power off 2...\n");
                CPM_PowerOff_2();
            }
        }
    }
}
