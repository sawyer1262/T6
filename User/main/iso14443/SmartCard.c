/******************************************************************************/
/*               (C) ˹���������������Ƽ����޹�˾(SKYRELAY)                   */
/*                                                                            */
/* �˴�����˹���������������Ƽ����޹�˾Ϊ֧�ֿͻ���д��ʾ�������һ����       */
/* ����ʹ��˹���������оƬ���û������޳�ʹ�øô��룬���豣������������       */
/* �������Ϊ��ʾʹ�ã�����֤�������е�Ӧ��Ҫ�󣬵����ض��淶������ʱ��       */
/* ʹ����������ȷ�ϴ����Ƿ����Ҫ�󣬲���Ҫ��ʱ����������Ӧ���޸ġ�           */
/* (V1.00)                                                                    */
/******************************************************************************/
/**
 ******************************************************************************
 ** �� �� ���� SmartCard.c
 **
 ** �ļ������� ����CPU���������к����������ͱ���
 **
 ** �汾��ʷ:
 ** 2019-07-09 V1.00  EH   ��һ����ʽ�汾
 **
 ******************************************************************************/

#include "iso14443.h"
#include "smartcard.h"
/**
 ******************************************************************************
 ** \��  ��  ��CPU������ I-Block ���ݣ������շ������ݣ�����ʧ���ط�һ��
 **
 ** \��  ��
 **         sendLen:���͵����ݳ���
 **		    sendBuff:���͵�ָ������ָ��
 **         recLen: �������ݳ���ָ��
 **         recBuff: ��������ָ��
 ** \����ֵ  ����״̬
 ******************************************************************************/
sta_result_t CPU_I_Block(uint8_t sendLen,uint8_t *sendBuff,uint16_t *recLen,uint8_t *recBuff)
{
    sta_result_t sta;
    uint8_t length;
    uint8_t i;
    setPCDTimeOut(1000);  // timeout 1000ms
    for(i=0;i<2;i++)
    {
        length=1;
        sky1311WriteFifo(&g_PCB,1);
        if (g_PCB & 0x08)//�ж��Ƿ���CID�ֽ�
        {
            sky1311WriteFifo(&g_bCID,1);
            length=2;
        }
        sky1311_fifo_tx(sendBuff, sendLen+length);
        sta = sky1311_fifo_rx(recBuff, recLen);
        if(sta==Ok)
        {
            PCD_PCB();
            break;
        }
    }
    return sta;
}

/**
 ******************************************************************************
 ** \��  ��  ��CPU������ R-Block ���ݣ������շ������ݣ�����ʧ���ط�һ��
 **
 ** \��  ��
 **         recLen: �������ݳ���ָ��
 **         recBuff: ��������ָ��
 ** \����ֵ  ����״̬
 ******************************************************************************/
sta_result_t CPU_R_Block(uint8_t *recBuff,uint16_t *recLen)
{
    sta_result_t sta;
 	uint8_t sendBuff[2];
    uint8_t i;
	setPCDTimeOut(1000);            // timeout 1000ms
    for(i=0;i<2;i++)
    {
        sky1311WriteCmd(CMD_CLR_FF);    // clear FIFO
        if (g_PCB & 0x08)              //�ж��Ƿ���CID
        {
            sendBuff[0] = (g_PCB & 0x0F) | 0xA0;
            sendBuff[1] = g_bCID;
            sta = ExchangeData(sendBuff,2,recBuff, recLen);
        }
        else
        {
            sendBuff[0] = (g_PCB & 0x0F) | 0xA0;
            sta = ExchangeData(sendBuff,1,recBuff, recLen);
        }
        if (sta==Ok)
        {
            PCD_PCB();
            break;
        }
    }
	return sta;
}
/**
 ******************************************************************************
 ** \��  ��  ��CPU������ S-Block ���ݣ������շ�������
 **
 ** \��  ��
 **         PCB_byte:����PCB�ֽ�
 **         recLen: �������ݳ���ָ��
 **         recBuff: ��������ָ��
 ** \����ֵ  ����״̬
 ******************************************************************************/
uint8_t CPU_S_Block(uint8_t PCB_byte,uint16_t *recLen,uint8_t *recBuff)
{
    sta_result_t sta;
 	uint8_t sendBuff[3];
	setPCDTimeOut(1000);            // timeout 1000ms
    sky1311WriteCmd(CMD_CLR_FF);    // clear FIFO
    sendBuff[0] =0xC0|PCB_byte;
    if ((g_PCB&0x08)==0x08)     //�ж��Ƿ���CID�ֽ�
    {
        if((PCB_byte&0xF0)==0xF0) //WTX
        {
		  	sendBuff[1] = g_bCID;
			sendBuff[2] = 0x01;
			sta=ExchangeData(sendBuff,3,recBuff,recLen);
		}
		else                    //DESELECT
		{
			sendBuff[1] = g_bCID;
			sta=ExchangeData(sendBuff,2,recBuff,recLen);
		}
	}
	else
	{
		if ((PCB_byte&0xF0)==0xF0) //WTX
        {
		  	sendBuff[1] = 0x01;
			sta=ExchangeData(sendBuff,2,recBuff,recLen);
		}
		else                    //DESELECT
		{
			sendBuff[1] = 0x00;
			sta=ExchangeData(sendBuff,2,recBuff,recLen);
		}
	}
	if(sta==Ok)
	{
			PCD_PCB();
	}
	return sta;
 }



