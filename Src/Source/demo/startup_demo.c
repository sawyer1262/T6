// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : startup_demo.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "startup_demo.h"

void I2C_Config_Gpio_Reboot(I2C_PIN I2C_PINx, UINT8 GPIO_Dir)
{
	I2C_TypeDef *i2c = (I2C_TypeDef *)I2C1_BASE_ADDR;
	//i2c = (I2C_TypeDef *)I2C1_BASE_ADDR;
	
	*(volatile UINT32*)(0x40023074) &= ~0x00000040;		//SDA��TSI CH6���ã���ֹTSI CH6

	i2c->PCR |= 0xc0;//config gpio
	if (GPIO_Dir == GPIO_OUTPUT)
	{
		i2c->DDR |= (1<<I2C_PINx);//output
	}
	else if (GPIO_Dir == GPIO_INPUT)
	{
		i2c->DDR &= (~(1<<I2C_PINx));//input
	}
}

UINT8 I2C_Read_Gpio_Data_Reboot(I2C_PIN I2C_PINx)
{
	UINT8 bitstatus = 0x00;

	I2C_TypeDef *i2c = (I2C_TypeDef *)I2C1_BASE_ADDR;

	bitstatus = i2c->PDR;
	if (bitstatus &(Bit_SET<<I2C_PINx))
		bitstatus = Bit_SET;
	else
		bitstatus = Bit_RESET;
	
	i2c->PCR &= (~0xc0);	//���ó�i2c����

	return bitstatus;
}

void Demo_StartupConfig(StartupMode Mode, FunctionalState ResetEN)
{
    if(Mode == Startup_From_ROM)
    {
        UINT32 eflash_clk;
	
        if(0 == g_sys_clk)
        {
            eflash_clk = Get_Sys_Clock();
        }
        else
        {
            eflash_clk = g_sys_clk;
        }
        
        EFLASH_Init(eflash_clk/1000);
        
        //ִ����EFlash_Recovery_to_Boot()���´��ϵ��λ����ROM����
        EFlash_Recovery_to_Boot();
        
    }
    else if(Mode == Startup_From_EFLASH)
    {
        UINT32 eflash_clk;
	
        eflash_clk = g_sys_clk;
	
        EFM_Init(eflash_clk);
        
        //ִ����EFlash_Disboot()���´��ϵ��λ����EFlash����
        EFlash_Disboot();
    }
    else
    {
        return;
    }
    
    if(ResetEN == ENABLE)
    {
        //�ָ�boot�󣬲��ϵ�ֱ������
        Set_POR_Reset();
    }
    
    while(1);
}

void Startup_Demo(void)
{
//	Demo_StartupConfig(Startup_From_ROM, ENABLE);
    Demo_StartupConfig(Startup_From_EFLASH, DISABLE);
}

