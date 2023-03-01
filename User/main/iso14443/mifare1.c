/******************************************************************************/
/*               (C) ˹���������������Ƽ����޹�˾(SKYRELAY)                   */
/*                                                                            */
/* �˴�����˹���������������Ƽ����޹�˾Ϊ֧�ֿͻ���д��ʾ�������һ����       */
/* ����ʹ��˹���������оƬ���û������޳�ʹ�øô��룬���豣������������       */
/* �������Ϊ��ʾʹ�ã�����֤�������е�Ӧ��Ҫ�󣬵����ض��淶������ʱ��       */
/* ʹ����������ȷ�ϴ����Ƿ����Ҫ�󣬲���Ҫ��ʱ����������Ӧ���޸ġ�           */
/* (V1.10)                                                                    */
/******************************************************************************/
/**
 ******************************************************************************
 ** �� �� ���� mifare1.c
 **
 ** �ļ������� ����Mifare�����������к����������ͱ���
 **
 ** �汾��ʷ:
 ** 2015-10-05 V1.00  EH   ��һ����ʽ�汾
 ** 2017-03-25 V1.10  Release    ���´���ͺ����ṹ�����·���
 ** 2018-06-12 V1.1.2 Release   �޸Ĳ��ִ�������������
 ** 2018-09-02 V1.2.0 Release  �޸���M1��������ĺ������޸�M1���Ķ�д������������
 ** 2019-08-02 V1,3.0 Beta      ������M1������Inc, Dec, Restore 3��ָ����߼�����
 ******************************************************************************/
#include "iso14443.h"

void sky1311RxCrcOn(void);
void sky1311RxCrcOff(void);
void sky1311TxCrcOn(void);
void sky1311TxCrcOff(void);
#if 1
/**
 ******************************************************************************
 ** \��  ��  M1���ļ�����֤,
 **
 ** \��  ��  uint8_t auth_mode ��֤����Կ��Key_A ���� Key B)
 **          uint8_t* m1Key ��Կ
             uint8_t* UID ����UID��
             uint8_t blockAddr Ҫ������������ַ��0--63��
 ** \����ֵ  ��֤״̬
 ******************************************************************************/
sta_result_t M1_Authentication(uint8_t auth_mode, uint8_t *m1Key,uint8_t *uid, uint8_t blockAddr)
{
    //uint8_t irq_sta;
    volatile uint16_t delayCount;
    sky1311TxCrcOn();
    sky1311RxCrcOff();
    // load M1_KEY ( ���ֽ���ǰ����д��)
    sky1311WriteReg(ADDR_M1_KEY, m1Key[5]);
    sky1311WriteReg(ADDR_M1_KEY, m1Key[4]);
    sky1311WriteReg(ADDR_M1_KEY, m1Key[3]);
    sky1311WriteReg(ADDR_M1_KEY, m1Key[2]);
    sky1311WriteReg(ADDR_M1_KEY, m1Key[1]);
    sky1311WriteReg(ADDR_M1_KEY, m1Key[0]);

    // load UID �� ���ֽ���ǰ����д�룩
    //sky1311WriteReg(ADDR_M1_ID, uid[4]);
    sky1311WriteReg(ADDR_M1_ID, uid[3]);
    sky1311WriteReg(ADDR_M1_ID, uid[2]);
    sky1311WriteReg(ADDR_M1_ID, uid[1]);
    sky1311WriteReg(ADDR_M1_ID, uid[0]);

    // set auth mode & begin
    sky1311WriteReg(ADDR_TX_BYTE_NUM, 2);
    sky1311WriteReg(ADDR_TX_BIT_NUM, 8);
    sky1311WriteReg(ADDR_FIFO, auth_mode);
    sky1311WriteReg(ADDR_FIFO, blockAddr);
    irqClearAll();
    sky1311WriteCmd(CMD_AUTO_M1);

    // wait PICC return result
    delayCount = 0x4FFF;
    while((0==(irq_sta=sky1311ReadReg(ADDR_IRQ_STA))) && --delayCount);
    //while( 0==SKY1311_IRQ_READ() && --delayCount);      // ���IRQ�����Ƿ����ж�
    irq_sta = sky1311ReadReg(ADDR_IRQ_STA);
    if(irq_sta & IRQ_M1)
    {
        irqClearAll();//irqClear(IRQ_M1);
        if(sky1311ReadReg(ADDR_M1_SUC_STATE) & 0x04)
            return M1Error;
        if(sky1311ReadReg(ADDR_M1_SUC_STATE) & 0x08)
            return Ok;
    }
    else if((delayCount==0) || (irq_sta & IRQ_TOUT))
    {
        irqClear(IRQ_TOUT);
        sky1311WriteCmd(CMD_IDLE);
        return NoResponse;
    }
    else
    {
        irqClearAll();
        return Error;
    }
    return Ok;
}
#else
/**
 ******************************************************************************
 ** \��  ��  M1���ļ�����֤,
 **
 ** \��  ��  uint8_t auth_mode ��֤����Կ��Key_A ���� Key B)
 **          uint8_t* m1Key ��Կ
             uint8_t* UID ����UID��
             uint8_t blockAddr Ҫ������������ַ��0--63��
 ** \����ֵ  ��֤״̬
 ******************************************************************************/
sta_result_t M1_Authentication(uint8_t auth_mode,uint8_t *m1Key,uint8_t *uid, uint8_t blockAddr)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t tmpBuf[16];

    volatile uint16_t delayCount;

    uint32_t RA = 0xAACBD156;       // ��������������������ֵ����

    sky1311WriteReg(ADDR_M1_CTRL, 0x00);    // ��λM1���ƼĴ���
    sky1311TxCrcOn();
    sky1311RxCrcOff();
    // load M1_KEY ( ���ֽ���ǰ����д��)
    sky1311WriteReg(ADDR_M1_KEY, m1Key[5]);
    sky1311WriteReg(ADDR_M1_KEY, m1Key[4]);
    sky1311WriteReg(ADDR_M1_KEY, m1Key[3]);
    sky1311WriteReg(ADDR_M1_KEY, m1Key[2]);
    sky1311WriteReg(ADDR_M1_KEY, m1Key[1]);
    sky1311WriteReg(ADDR_M1_KEY, m1Key[0]);

   // load UID �� ���ֽ���ǰ����д�룩
    sky1311WriteReg(ADDR_M1_ID, uid[3]);
    sky1311WriteReg(ADDR_M1_ID, uid[2]);
    sky1311WriteReg(ADDR_M1_ID, uid[1]);
    sky1311WriteReg(ADDR_M1_ID, uid[0]);

    sky1311WriteReg(ADDR_M1_CTRL, 0x53);      // pass1: ���������UID����M1��ͨ�ſ�ʼ

    tmpBuf[0] = auth_mode;//M1_AUTH;          // ��֤ Key A or Key B
    tmpBuf[1] = blockAddr;

    //sky1311_fifo_tx(TYPE_A, tmpBuf, 2);
    //sta = sky1311_fifo_rx(TYPE_A, RATE_OFF, tmpBuf, &tmpSize);  //��ȡ Token RB
    sta = ExchangeData(tmpBuf,2,tmpBuf,&tmpSize);
    if(sta == NoResponse)
      return NoResponse;

    sky1311WriteReg(ADDR_M1_CTRL, 0x05);       // pass2:
    sky1311TxCrcOff();
    sky1311RxCrcOff();
    tmpBuf[0] = (uint8_t)(RA >> 24);
    tmpBuf[1] = (uint8_t)(RA >> 16);
    tmpBuf[2] = (uint8_t)(RA >> 8);
    tmpBuf[3] = (uint8_t)(RA >> 0);

    delayCount = 0x7FFF;
    while(((sky1311ReadReg(ADDR_M1_SUC_STATE)&0x01)==0) && delayCount--);    // waiting for SUC64 ready

    tmpBuf[4] = sky1311ReadReg(ADDR_M1_SUC64_3);
    tmpBuf[5] = sky1311ReadReg(ADDR_M1_SUC64_2);
    tmpBuf[6] = sky1311ReadReg(ADDR_M1_SUC64_1);
    tmpBuf[7] = sky1311ReadReg(ADDR_M1_SUC64_0);

    sky1311_fifo_tx(tmpBuf, 8);             // ���� Token AB
    sky1311WriteReg(ADDR_M1_CTRL, 0x09);            // pass3���˺�����ͨ�ż���

    sta = sky1311_fifo_rx(tmpBuf, &tmpSize);
    if(sta == NoResponse)
        return NoResponse;
    //  ��֤ Token BA
    if(sky1311ReadReg(ADDR_M1_SUC96_3) == tmpBuf[0] &&
       sky1311ReadReg(ADDR_M1_SUC96_2) == tmpBuf[1] &&
       sky1311ReadReg(ADDR_M1_SUC96_1) == tmpBuf[2] &&
       sky1311ReadReg(ADDR_M1_SUC96_0) == tmpBuf[3]
    )
        return Ok;
    else
        return NoResponse;

}
#endif

/**
 ******************************************************************************
 ** \��  ��  M1��д�������ݡ�һ��д��16���ֽ�����
 **
 ** \��  ��  uint8_t blockAddr        Ҫ�������������ַ��0--63��
             uint8_t* data           Ҫд�������
 ** \����ֵ  ����״̬ Ok��ʾ�ɹ��� ������ʾ�д���
 ******************************************************************************/
sta_result_t M1_Write(uint8_t blockAddr, uint8_t* data)
{
    sta_result_t sta;
    uint8_t tmpBuf[16];
    uint16_t tmpSize;

    sky1311TxCrcOn();
    sky1311RxCrcOff();

    tmpBuf[0] = M1_WRITE;       // CMD = 0xA0
    tmpBuf[1] = blockAddr;      //
    sta = ExchangeData(tmpBuf,2,tmpBuf,&tmpSize);
    if(sta == NoResponse)
        return NoResponse;
    if((tmpBuf[0] & 0x0f) != M1_ACK)
        return M1Error;

    sta = ExchangeData(data,16,tmpBuf,&tmpSize);
    if(sta == NoResponse)
      return NoResponse;
    if((tmpBuf[0] &0x0f) != M1_ACK)
      return M1Error;

    return sta;
}
/**
 ******************************************************************************
 ** \��  ��  M1�����������ݣ�
 **
 ** \��  ��  uint8_t blockAddr    Ҫ�������������ַ��0--63��
 **          uint8_t *blockBuff  ��ȡ�����ݴ洢��ַ
 ** \����ֵ  ����״̬
 ******************************************************************************/
sta_result_t M1_Read(uint8_t blockAddr, uint8_t *blockBuff)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t tmpBuf[16];

    sky1311TxCrcOn();
    sky1311RxCrcOn();

    tmpBuf[0] = M1_READ;    // 0x30
    tmpBuf[1] = blockAddr;
    //sky1311_fifo_tx(TYPE_A, tmpBuf, 2);
    //sta = sky1311_fifo_rx(TYPE_A, RATE_OFF, blockBuff, &tmpSize);
    sta = ExchangeData(tmpBuf,2,blockBuff,&tmpSize);
    return sta;
}
/**
 ******************************************************************************
 ** \��  ��  M1���������ݼ�ֵ
 **
 ** \��  ��  uint8_t blockAddr Ҫ�������������ַ��0--63��
             uint32_t Ҫ���ӵ���
 ** \����ֵ  ����״̬
 ******************************************************************************/
sta_result_t M1_Increment(uint8_t blockAddr, uint32_t value)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t tmpBuf[16];

    sky1311TxCrcOn();
    sky1311RxCrcOff();

    tmpBuf[0] = M1_INCREMENT;
    tmpBuf[1] = blockAddr;
    sta = ExchangeData(tmpBuf,2,tmpBuf,&tmpSize);
    if(sta == NoResponse)
        return NoResponse;

    if((tmpBuf[0] & 0x0f) != M1_ACK)
        return M1Error;

    tmpBuf[0] = (uint8_t)value;
    tmpBuf[1] = (uint8_t)(value>>8);
    tmpBuf[2] = (uint8_t)(value>>16);
    tmpBuf[3] = (uint8_t)(value>>24);
    setPCDTimeOut(10);
    sky1311_fifo_tx(tmpBuf, 4);     // send data 4 bytes
    DelayMS(5);
    sta = M1_Transfer(blockAddr);
    setPCDTimeOut(1000);
    return sta;
}
/**
 ******************************************************************************
 ** \��  ��  M1���������ݼ�ֵ
 **
 ** \��  ��  uint8_t blockAddr Ҫ�������������ַ��0--63��
             uint32_t Ҫ���ٵ���
 ** \����ֵ  ����״̬
 ******************************************************************************/
sta_result_t M1_Decrement(uint8_t blockAddr, uint32_t value)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t tmpBuf[16];

    sky1311TxCrcOn();
    sky1311RxCrcOff();

    tmpBuf[0] = M1_DECREMENT;
    tmpBuf[1] = blockAddr;
    sta = ExchangeData(tmpBuf,2,tmpBuf,&tmpSize);
    if(sta == NoResponse)
        return NoResponse;
    if((tmpBuf[0] & 0x0f) != M1_ACK)
        return M1Error;
    tmpBuf[0] = (uint8_t)value;
    tmpBuf[1] = (uint8_t)(value>>8);
    tmpBuf[2] = (uint8_t)(value>>16);
    tmpBuf[3] = (uint8_t)(value>>24);
    setPCDTimeOut(10);
    sky1311_fifo_tx(tmpBuf, 4);     // send data 4 bytes
    DelayMS(5);
    sta = M1_Transfer(blockAddr);
    setPCDTimeOut(1000);
    return sta;
}

/**
 ******************************************************************************
 ** \��  ��  M1�����ݻָ�����block�е������Ƶ���ʧ�ԼĴ���
 **
 ** \��  ��  uint8_t blockAddr Ҫ�������������ַ��0--63��
 ** \����ֵ  ����״̬
 ******************************************************************************/
sta_result_t M1_Restore(uint8_t blockAddr)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t tmpBuf[16];

    sky1311TxCrcOn();
    sky1311RxCrcOff();
    setPCDTimeOut(10);
    tmpBuf[0] = M1_RESTORE;
    tmpBuf[1] = blockAddr;
    sta = ExchangeData(tmpBuf,2,tmpBuf,&tmpSize);
    sta = ExchangeData(tmpBuf,2,tmpBuf,&tmpSize);
    if(sta == NoResponse)
        return NoResponse;
    if((tmpBuf[0] & 0x0f) != M1_ACK)
        return M1Error;

    setPCDTimeOut(10);
    sky1311_fifo_tx(tmpBuf, 4);     // send data 4 bytes
    DelayMS(5);
    sta = M1_Transfer(blockAddr);
    setPCDTimeOut(1000);
    return sta;
}

/**
 ******************************************************************************
 ** \��  ��  ����������ʧ�ԼĴ����е�����ŵ�ָ����Block��EEPROM����
 **
 ** \��  ��  uint8_t blockAddr Ҫ�������������ַ��0--63��
 ** \����ֵ  ����״̬
 ******************************************************************************/
sta_result_t M1_Transfer(uint8_t blockAddr)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t tmpBuf[16];

    sky1311TxCrcOn();
    sky1311RxCrcOff();

    tmpBuf[0] = M1_TRANSFER;
    tmpBuf[1] = blockAddr;
    sta = ExchangeData(tmpBuf,2,tmpBuf,&tmpSize);
    if(sta == NoResponse)
        return NoResponse;
    if((tmpBuf[0] & 0x0f)!= M1_ACK)
        return M1Error;
    return sta;
}


sta_result_t M1_PwdChange(uint8_t blockAddr, uint8_t *key_a, uint8_t *key_b)
{
    sta_result_t sta;
    uint8_t tmpBuf[16];

    tmpBuf[0]    =   key_a[0];
    tmpBuf[1]    =   key_a[1];
    tmpBuf[2]    =   key_a[2];
    tmpBuf[3]    =   key_a[3];
    tmpBuf[4]    =   key_a[4];
    tmpBuf[5]    =   key_a[5];
    tmpBuf[6]    =   0xff;
    tmpBuf[7]    =   0x07;
    tmpBuf[8]    =   0x80;
    tmpBuf[9]    =   0x69;          // FFH 07H 80H 69H ��Ĭ�ϵĿ�����
    tmpBuf[10]   =   key_b[0];      //keyB ��ȷ���ܿ�������λ 
    tmpBuf[11]   =   key_b[1];
    tmpBuf[12]   =   key_b[2];
    tmpBuf[13]   =   key_b[3];
    tmpBuf[14]   =   key_b[4];
    tmpBuf[15]   =   key_b[5];
    sta = M1_Write(blockAddr,tmpBuf);
    return sta;
}
 
//���ݿ�ID����keyA,keyB
uint8_t CardGetKey(uint8_t *keyA,uint8_t *keyB,uint8_t *id)
{
		keyA[0]=~id[0]+3;
		keyA[1]=id[0]&id[1];
		keyA[2]=~id[0]+0x75;
		keyA[3]=id[3]&id[2];
		keyA[4]=id[0]^id[1]^id[2]^id[3];
		keyA[5]=id[0]+id[1]+id[2]+id[3];
		
		keyB[0]=0xFF-keyA[0];
		keyB[1]=0xFF-keyA[1];
		keyB[2]=0xFF-keyA[2];
		keyB[3]=0xFF-keyA[3];
		keyB[4]=0xFF-keyA[4];
		keyB[5]=0xFF-keyA[5];
	
		return 1;
}


uint8_t mifare1_ReadTest(uint8_t *UID,uint8_t *roll) 
{
    sta_result_t sta;
	
    uint8_t tmpBuf[20];
    uint8_t default_keyA[6] = {0};
		uint8_t default_keyB[6] = {0};
		
		CardGetKey(default_keyA,default_keyB,UID);                        //����KEYA,KEYB
		
		sta = M1_Authentication(M1_AUTH_KEYB, default_keyB , UID, 63);    //ʹ��KEYA��KEYBУ�鶼û����
		if(sta==Ok)
		{
				sta = M1_Read(62, tmpBuf);
				if(sta==Ok)
				{
						if(tmpBuf[0]=='G' && tmpBuf[1]=='M' && tmpBuf[2]=='T')
						{		
								if(roll!=NULL)    //��Ҫ��ȡ����
								{
										sta = M1_Read(60, tmpBuf);
										if(sta==Ok)
										{
												roll[0]=tmpBuf[0];
												roll[1]=tmpBuf[1];
												roll[2]=tmpBuf[2];
												roll[3]=tmpBuf[3];
												return 0;
										}
								}
								else return 0;  
						}
				}
		}
    return 1;
}


sta_result_t mifare1_WriteTest(uint8_t *UID,uint8_t sectorN)         //blockN:0-15,������
{
    sta_result_t sta;
    uint8_t default_key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};     //Ĭ��KEY��keya,keybһ��
    uint8_t i;
    uint8_t writeData[] = "ABCDEF1234567890";                         //д������������
    uint8_t keyb[]={1,2,3,4,5,6};                                     //�޸�keyb
    uint8_t keya[]={0xFF ,0xFF ,0xFF ,0xFF ,0xFF ,0xFF };             //keya���ֲ��� 
		uint8_t secCtrl=sectorN*4+3;
		uint8_t secBlockStart=sectorN*4;
		
    sta = M1_Authentication(M1_AUTH_KEYA, default_key, UID, secCtrl);
    if(Ok==sta)
    {
        sta = M1_PwdChange(secCtrl,keya,keyb);           //2����д��3---(2-1)*4+3=7
        if(Ok == sta)
        {
            sky1311WriteReg(ADDR_M1_CTRL, 0x81);
        }
    }
    else
    {
        piccHaltA();
        sky1311WriteReg(ADDR_M1_CTRL, 0x00);
        return Error;

    }
    for(i=0;i<3;i++)
    {
        irq_sta = sky1311ReadReg(ADDR_M1_CTRL);
				
        sta = M1_Write(secBlockStart++, writeData);
        if(Ok == sta)
        {
						printf("block %02d write success\n",secBlockStart);
        }
        else
				{
						printf("block %02d write fail\n",secBlockStart);
        }

    }
    return Ok;
}



