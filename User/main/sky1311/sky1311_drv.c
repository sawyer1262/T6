#include "sky1311_drv.h"
#include "sky1311t_reg.h"  
#include "i2c_drv.h"
#include "card.h"



void RFID_PortDeInit(void)
{   
		//ʹ�ܽ�
		RFID_CHIPEN_L();
		//SPI��
		SPI_ConfigGpio(RFID_SPI, SPI_MISO,GPIO_OUTPUT);
		SPI_ConfigGpio(RFID_SPI, SPI_MOSI,GPIO_OUTPUT);
		SPI_ConfigGpio(RFID_SPI, SPI_SCK,GPIO_OUTPUT);
		SPI_ConfigGpio(RFID_SPI, SPI_SS,GPIO_OUTPUT);
		SPI_WriteGpioData(RFID_SPI, SPI_MISO, Bit_SET); 
		SPI_WriteGpioData(RFID_SPI, SPI_MOSI, Bit_SET);
		SPI_WriteGpioData(RFID_SPI, SPI_SCK, Bit_SET);
		SPI_WriteGpioData(RFID_SPI, SPI_SS, Bit_SET);
		//IRQ�жϽ�
		EPORT_ConfigGpio(RFID_IRQ_PORT,GPIO_INPUT);
		EPORT_PullConfig(RFID_IRQ_PORT,EPORT_PULLDOWN);
}


void RFID_PortInit(void)
{      
		RFID_CHIPEN_INIT();		 
		RFID_CHIPEN_H();	
		DelayMS(5);
		RFID_CS_INIT();  			
		RFID_CS_H();      
		RFID_IRQ_INIT(); 
		SPI_InitTypeDef SPI_InitStruct;
		SPI_StructInit(SPI_Mode_Master,&SPI_InitStruct);
		SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; //75M/10=7.5M
		SPI_Init(RFID_SPI,&SPI_InitStruct);                              //spi2
}


void SKY1311_ChipEn(void)
{
		RFID_CHIPEN_H();
		delay(200);
}
void SKY1311_ChipDis(void)
{
		RFID_CHIPEN_L();
		delay(200);
}
void SKY1311CsEn(void)
{
		RFID_CS_L();
		delay(200);
}
void SKY1311CsDis(void)
{
		RFID_CS_H();
		delay(200);
}
static inline uint8_t SPIByteCom(uint8_t ch)
{
	//	uint8_t res;
		SPI_MasterTransceive(RFID_SPI,&ch,1);
		return ch;
}

 /******************************************************************************
 ** \��  ��  ͨ��SPI�ӿ���1311дһ���ֽڵ�����
 **
 ** \��  ��  ������
 ** \����ֵ  none
 ******************************************************************************/
void sky1311WriteCmd(uint8_t cmd)
{
    SKY1311CsEn();
    cmd = (cmd & 0x1F) | 0x80;	        // bit7,6,5 = 100b, mean command
    SPIByteCom(cmd);
    SKY1311CsDis();
}
/**
 ******************************************************************************
 ** \��  ��  ͨ��SPI�ӿ���1311�ļĴ���дһ���ֽ�����
 **
 ** \��  ��  uint8_t regAdd: �Ĵ�����ַ�� uint8_t data: Ҫд�������
 ** \����ֵ  none
 ******************************************************************************/
void sky1311WriteReg(uint8_t regAdd, uint8_t data)
{
    SKY1311CsEn();
    regAdd      =   (regAdd & 0x3F);        // bit7,6=00, config as addr/write mode
    SPIByteCom(regAdd);
    SPIByteCom(data);
    SKY1311CsDis();
	//Dprint("regAdd=0x%02x:0x%02x\r\n",regAdd,data);
}
/**
 ******************************************************************************
 ** \��  ��  ͨ��SPI�ӿڶ�ȡ1311�ļĴ���
 **
 ** \��  ��  uint8_t regAdd: �Ĵ�����ַ
 ** \����ֵ  uint8_t �Ĵ�������
 ******************************************************************************/
uint8_t sky1311ReadReg(uint8_t regAdd)
{
    uint8_t value;
    SKY1311CsEn();
    regAdd      =   (regAdd & 0x3F) | 0x40;            // bit7,6=01, config as addr/read mode
    SPIByteCom(regAdd);
    value = SPIByteCom(0xFF);
    SKY1311CsDis();
//	Dprint("Reg0x%02x:0x%02x\r\n",regAdd,value);
    return value;
}
/**
 ******************************************************************************
 ** \��  ��  ͨ��SPI�ӿ���1311��FIFOдָ����Ŀ������
 **
 ** \��  ��  uint8_t* ��������ͷ��ַ�� uint8_t count: Ҫд�����������
 ** \����ֵ  none
 ******************************************************************************/
void sky1311WriteFifo(uint8_t *data, uint8_t count)
{
    uint8_t add;
    if(count==0) return;
    SKY1311CsEn();
    add      =   (ADDR_FIFO & 0x3F);               // bit7,6=00, config as addr/write mode
    SPIByteCom(add);
    while(count--)
    {
        SPIByteCom(*data++);
    }
    SKY1311CsDis();

}
/**
 ******************************************************************************
 ** \��  ��  ͨ��SPI�ӿ���1311��FIFO��ȡָ����Ŀ������
 **
 ** \��  ��  uint8_t* data �����ȡ���ݵĻ������׵�ַ�� uint8_t count ��ȡ���ֽ���
 ** \����ֵ  none
 ******************************************************************************/
void sky1311ReadFifo(uint8_t *data, uint8_t count)
{
    uint8_t add;
    if(count==0) return;

    SKY1311CsEn();
    add   =   (ADDR_FIFO & 0x3F) | 0x40;            // bit7,6=01, config as addr/read mode
    SPIByteCom(add);
    while(count--)
    {
        *data++ = SPIByteCom(0xFF);
    }
    SKY1311CsDis();
}

