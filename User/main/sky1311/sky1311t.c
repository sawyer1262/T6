/******************************************************************************/
/*               (C) ˹���������������Ƽ����޹�˾(SKYRELAY)                   */
/*                                                                            */
/* �˴�����˹���������������Ƽ����޹�˾Ϊ֧�ֿͻ���д��ʾ�������һ����       */
/* ����ʹ��˹���������оƬ���û������޳�ʹ�øô��룬���豣������������       */
/* �������Ϊ��ʾʹ�ã�����֤�������е�Ӧ��Ҫ�󣬵����ض��淶������ʱ��       */
/* ʹ����������ȷ�ϴ����Ƿ����Ҫ�󣬲���Ҫ��ʱ����������Ӧ���޸ġ�           */
/* http://www.skyrelay-ic.com                                                 */
/******************************************************************************/
/**
 ******************************************************************************
 ** �� �� ���� sky1311T.c
 **
 ** �ļ������� ����sky1311s�����к����������ͱ���
 **
 ** �汾��ʷ:
 ** 2015-10-05 V1.00  EH   ��һ����ʽ�汾
 ** 2017-3-25  V.10  Release    ���´���ͺ����ṹ�����·���
 ** 2018-06-12 V1.1.2 Release   ���������շ��������޸Ĳ��ִ�������������
 ** 2018-09-02 V1.2.0 Release   �޸��˶ദ���������������շ��������޸��˲��ֺ����Ľṹ��ʵ�ַ�ʽ
 ** 2018-09-19 V1.2.1 Release   �쿨ʱ�ر������ж�Դ��ʹ�����жϲ����ż쿨
 ** 2019-08-02 V1.3.0 Beta      �޸���һЩ���󣬾����˷��ͺͽ��պ���������˳�ʱ����
 ** 2020-04-27 V1.3.0 Release   ��ʱ�Ϳ��س������޸ģ� ��������ˮ�ߺ���
 ** 2020-11-17 V1.3.1 Release   �޸�bitRateSet �������ö౶�٣������Ż�
 ** 2021-08-24 V1.3.2 Release   �޸Ľ��պ������жϴ���״̬�Ĵ���ʱ�ж�λ��Ӧ����0xC0
 ** 2021-08-27 V1.3.2 Release   �޸�sky1311TxCrcOff��������֮ǰ ���(!TX_CRC_EN)ȥ��
          
 ******************************************************************************/
#include "sky1311_drv.h"
#include "sky1311t_reg.h"
#include "timer.h"

 

/**
 ******************************************************************************
 ** \������ ȫ�ֱ�������
 **
 ******************************************************************************/
#if CHECKCARD==1
    uint8_t maxRCADVal=MAXADVAL;         // ��⵽�����ADֵ
    uint8_t freqScanNeed=TRUE;  // ϵͳ��ҪRCƵ��ɨ��
#endif

uint8_t  err_sta;   // ����״̬����
uint8_t  irq_sta;   // �ж�����״̬����

#define     FIFO_LEVEL              16
/**
 ******************************************************************************
 ** \��  ��  ����M1��ʱ�򿪽��յ�CRCУ��(sky1311s)
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
void sky1311RxCrcOn(void)
{
    sky1311WriteReg(ADDR_RX_CTRL, RX_MIFARE_ON|RX_CRC_EN|RX_PARITY_EN|RX_PARITY_ODD);
}
/**
 ******************************************************************************
 ** \��  ��  ����M1��ʱ�رս��յ�CRCУ��(sky1311s)
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
void sky1311RxCrcOff(void)
{
    sky1311WriteReg(ADDR_RX_CTRL, RX_MIFARE_ON|RX_PARITY_EN|RX_PARITY_ODD);
}
/**
 ******************************************************************************
 ** \��  ��  ����M1��ʱ�򿪷��͵�CRCУ��(sky1311s)
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
void sky1311TxCrcOn(void)
{
    sky1311WriteReg(ADDR_TX_CTRL, TX_CRC_EN|TX_PARITY_ODD|TX_POLE_HIGH);
}
/**
 ******************************************************************************
 ** \��  ��  ����M1��ʱ�رշ��͵�CRCУ��(sky1311s)
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
void sky1311TxCrcOff(void)
{
    //sky1311WriteReg(ADDR_TX_CTRL, (!TX_CRC_EN)|TX_PARITY_ODD|TX_POLE_HIGH); //20210827
    sky1311WriteReg(ADDR_TX_CTRL, TX_PARITY_ODD|TX_POLE_HIGH);
}
/**
 ******************************************************************************
 ** \��  ��  ���üĴ�������λ
 **
 ** \��  ��  uint8_t regAddr �Ĵ�����ַ�� uint8_t mask Ҫ���õ�"����"�ֽ�
 ** \����ֵ  none
 ******************************************************************************/
void SetBitMask(uint8_t regAddr, uint8_t mask)
{
    uint8_t tmp;
    tmp = sky1311ReadReg(regAddr);
    sky1311WriteReg(regAddr, tmp|mask);
}
/**
 ******************************************************************************
 ** \��  ��  ����Ĵ�������λ
 **
 ** \��  ��  uint8_t regAddr �Ĵ�����ַ�� uint8_t mask Ҫ�����"����"�ֽ�
 ** \����ֵ  none
 ******************************************************************************/
void ClearBitMask(uint8_t regAddr, uint8_t mask)
{
    uint8_t tmp;
    tmp = sky1311ReadReg(regAddr);
    sky1311WriteReg(regAddr, tmp & ~mask);
}

/**
 ******************************************************************************
 ** \��  ��  ������Բ�����ʼ��
 **
 ** \��  ��  ѡ��ͨ��MFOUT��������źţ�
 **          bit2,1,0
 **             0 0 0 rx phase signal after demod, before sample
 **             0 0 1 tx bit stream
 **             0 1 0 rx bit stream
 **             0 1 1 rx data from analog
 **             1 0 0 DO from digital
 ** \����ֵ  none
 ******************************************************************************/
/*static void directModeInit(uint8_t mfout_sel)
{
    sky1311WriteReg(ADDR_MFOUT_SEL,  mfout_sel);
}*/

/**
 ******************************************************************************
 ** \��  ��  ��PCD����
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
void pcdAntennaOn(void)
{
    SetBitMask(ADDR_ANA_CFG1, TX_EN);
}

/**
 ******************************************************************************
 ** \��  ��  �ر�PCD����
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
void pcdAntennaOff(void)
{
    ClearBitMask(ADDR_ANA_CFG1, TX_EN);  // Disable TX
}
/**
 ******************************************************************************
 ** \��  ��  ��������жϱ��
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
void irqClearAll(void)
{
    sky1311WriteReg(ADDR_IRQ_STA, 0xFF);
}
/**
 ******************************************************************************
 ** \��  ��  ���ָ�����жϱ��
 **
 ** \��  ��  uint8_t irq
 ** \����ֵ  none
 ******************************************************************************/
void irqClear(uint8_t irq)
{
    sky1311WriteReg(ADDR_IRQ_STA,irq);
}
/**
 ******************************************************************************
 ** \��  ��  ����sky1311�뿨ͨ�ŵı���, ISO14443-4���ֹ涨���౶�ٵ�PICC
 **
 ** \��  ��  ���õı��٣�106��212��424
 ** \����ֵ  none
 ******************************************************************************/
void bitRateSet(uint16_t high_rate)
{
    sky1311WriteReg(ADDR_FSM_STATE, RATE_SEL);                           // enable high-speed mode
    if(high_rate == 212){
        sky1311WriteReg(ADDR_TX_CTRL, TX_212|TX_POLE_HIGH|TX_CRC_EN|TX_PARITY_ODD);
        sky1311WriteReg(ADDR_RATE_CTRL, RX_RATE_WID_2|RX_RATE_212|RX_RATE_PAR_ODD|RX_RATE_CRC_EN|RX_RATE_PAR_EN);
        sky1311WriteReg(ADDR_TX_PUL_WID,0x11);  //0x14                           // tx pulse width = 1.5u
        sky1311WriteReg(ADDR_RATE_FRAME_END,0x40);                       // stop counter = 0x40
        sky1311WriteReg(ADDR_RATE_THRES,0x20);                           // threshold = 0x20
    }else if(high_rate == 424){
        sky1311WriteReg(ADDR_TX_CTRL, TX_424|TX_POLE_HIGH|TX_CRC_EN|TX_PARITY_ODD);
        sky1311WriteReg(ADDR_RATE_CTRL, RX_RATE_WID_2|RX_RATE_424|RX_RATE_PAR_ODD|RX_RATE_CRC_EN|RX_RATE_PAR_EN);
        sky1311WriteReg(ADDR_TX_PUL_WID,0x07);//0x0A                           // tx pulse width = 0.8u
        sky1311WriteReg(ADDR_RATE_FRAME_END,0x20);                       // stop counter = 0x20
        sky1311WriteReg(ADDR_RATE_THRES,0x10);                           // threshold = 0x10
    }
    else if(high_rate == 848){
        sky1311WriteReg(ADDR_TX_CTRL, TX_848|TX_POLE_HIGH|TX_CRC_EN|TX_PARITY_ODD);
        sky1311WriteReg(ADDR_RATE_CTRL, RX_RATE_WID_2|RX_RATE_848|RX_RATE_PAR_ODD|RX_RATE_CRC_EN|RX_RATE_PAR_EN);
        sky1311WriteReg(ADDR_TX_PUL_WID,0x04);                           // tx pulse width = 0.4u
        sky1311WriteReg(ADDR_RATE_FRAME_END,0x10);                       // stop counter = 0x10
        sky1311WriteReg(ADDR_RATE_THRES,0x08);                           // threshold = 0x8
    }
}
/**
 ******************************************************************************
 ** \��  ��  ����sky1311��ʱ��ʱ��
 ** \ʹ�����ߵĻ�׼etu��Ϊʱ�ӣ�128/13.56MHz Լ9.44΢�룬������24λ�����ɼ���0x3F FFFF
 ** \����ʱʱ���� 9��44΢�� * 0x3F FFFF = 39.6�룬Ҳ����Լ39592����
 ** \��ʱ����ʹ��Լ1���룬����ֵ = 1ms x 106, ��ʱʱ���1���뵽39568����֮��
 ** \��  �� �� ��ʱʱ�䣬��λ������
 ** \����ֵ  none
 ******************************************************************************/
void setPCDTimeOut(uint16_t dlyMs)
{
    uint32_t dlyCount;
    uint8_t cnt;
    if(dlyMs>39568)
        dlyCount = 0x3FFFFF;
    else
     dlyCount = (uint32_t)dlyMs*106;
    cnt = (uint8_t)(dlyCount & 0xff);
    sky1311WriteReg(ADDR_TIME_OUT0, cnt);
    cnt = (uint8_t)((dlyCount>>8) & 0xff);
    sky1311WriteReg(ADDR_TIME_OUT1, cnt);
    cnt = (uint8_t)((dlyCount>>16) & 0x3f);
    sky1311WriteReg(ADDR_TIME_OUT2, 0x80 + cnt);       // time_out timer star condition = beginning of RX SOF

}

/**
 ******************************************************************************
 ** \��  ��  ѡ��type A ����Ϊ��������
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
void typeAOperate(void)
{
    sky1311WriteReg(ADDR_ANA_CFG2, ANA2_A);             // analogA select
    sky1311WriteReg(ADDR_FSM_STATE, TYPE_A_SEL);        // typeA select
    sky1311WriteReg(ADDR_CRC_CTRL, CRC_A);              // crcA enable
    sky1311WriteReg(ADDR_TX_PUL_WID,0x26);              // set to default value
    sky1311WriteReg(ADDR_M1_CTRL, 0x00);                // disable M1 operation
    sky1311WriteReg(ADDR_ANA_CFG3, 0x09);
    sky1311WriteReg(ADDR_ANA_CFG3, 0x81);
//		extern void RegReadTest(void);
//		RegReadTest();
}
/**
 ******************************************************************************
 ** \��  ��  ѡ��type B ����Ϊ��������
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
void typeBOperate(void)
{
    sky1311WriteReg(ADDR_ANA_CFG2, ANA2_B);            // analogB select
    sky1311WriteReg(ADDR_FSM_STATE, TYPE_B_SEL);       // typeB select
    sky1311WriteReg(ADDR_CRC_CTRL, CRC_B);             // crcB enable
    sky1311WriteReg(ADDR_TX_PUL_WID,0x26);              // set to default value
    sky1311WriteReg(ADDR_M1_CTRL, 0x00);                // disable M1 operation
    sky1311WriteReg(ADDR_ANA_CFG3, 0x09);
    sky1311WriteReg(ADDR_ANA_CFG3, 0x81);
}
/**
 ******************************************************************************
 ** \��  ��  ����FIFO�շ�ˮ��
 **  FIFO 64�ֽڣ��շ����ݵ�ʱ��ﵽ�趨��ˮ�߾ʹ���ˮ���жϣ����㼰ʱ�շ�����
 ** \��  ��  ˮ��ֵ�� 1--63
 ** \����ֵ  none
 ******************************************************************************/
void setFifoWaterLevel(uint8_t waterlevel)
{
    if(waterlevel<1)  
        waterlevel = 1;
    else if(waterlevel>63) 
        waterlevel = 63;
    sky1311WriteReg(ADDR_FIFO_CTRL,waterlevel);
}
/**
 ******************************************************************************
 ** \��  ��  ģ�������ʼ��������ģ������Ĵ���
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
//static void analogInit(void)
//{
//    sky1311WriteReg(ADDR_ANA_CFG0, RD_PAVOL); //  7-4: RC OSC Freq
//                                              //  3-2: PA Driver
//                                              //  1: ixtal
//                                              //  0: External LDO
//    sky1311WriteReg(ADDR_ANA_CFG2, ANA2_A);
//    sky1311WriteReg(ADDR_ANA_CFG1, 0x00);
//    delay(100);
//    sky1311WriteReg(ADDR_ANA_CFG1, 0xFC);   //sky1311WriteReg(ADDR_ANA_CFG1, 0xF4);
//                                              //  7: txen
//                                              //  6: rxen
//                                              //  5-4: clk_sel
//                                              //  00(osc off, xtal off)
//                                              //  01(osc on, xtal off)
//                                              //  10(osc off, xtal 13.56)
//                                              //  11(osc off, xtal 27.12)
//}

/**
 ******************************************************************************
 ** \��  ��  ��ʼ��SKY1311T�Ĵ���, ���þ�������������ѡ��ʱ��Դ����ʱ�ӣ�PA��ѹ
 **          ���ý��ճ�ʱʱ��100ms���жϿ�����Ĭ��ѡ��A��Э�飬��ʼ�����ֽ�������
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
 
 void RegReadTest(void)
 {
	 uint8_t i;
//	 uint8_t reg[100]={0};
	 for(i=0;i<0x40;i++)
	 {
		 	 printf("read reg:0x%02x=0x%02x\r\n",i,sky1311ReadReg(i));
			//reg[i]=sky1311ReadReg(i);
	 }
 }
void sky1311Init(void)
{
	//	RegReadTest();  
    sky1311WriteCmd(CMD_SW_RST);                // reset status
   // analogInit();
    sky1311WriteReg(ADDR_ANA_CFG0, 0x20|RD_PAVOL|IXTAL); //  7-4: RC OSC Freq(���ﲻ����)
                                                //  3-2: PA Driver
                                                //  1: ixtal (default: 0)
                                                //  0: External LDO (default: 0)

    sky1311WriteReg(ADDR_ANA_CFG2, ANA2_A);
//    sky1311WriteReg(ADDR_ANA_CFG1, 0x00);
//    delay(1000);
  //   sky1311WriteReg(ADDR_ANA_CFG1, 0xFC);   //sky1311WriteReg(ADDR_ANA_CFG1, 0xF4);
		sky1311WriteReg(ADDR_ANA_CFG1, TX_EN|RX_EN|CLK_SEL3|0x0C);
    DelayMS(3);  //Crystal Stable time
    setPCDTimeOut(100);       // default timeout time 100 mS, stop condition = beginning of RX SOF
    sky1311WriteReg(ADDR_RX_PUL_DETA, 0x34);     // ��4λʶ������˹��������Խ���ݴ�����Խǿ
    sky1311WriteReg(ADDR_RX_PRE_PROC, 0x00);
    sky1311WriteReg(ADDR_RX_START_BIT_NUM,0);
    sky1311WriteReg(ADDR_MOD_SRC,0x02);
    sky1311WriteReg(ADDR_IRQ_EN, IRQ_M1_EN|IRQ_TOUT_EN|IRQ_TX_EN|IRQ_RX_EN|IRQ_HIGH_EN|IRQ_LOW_EN);    // enable  IRQ
    sky1311WriteReg(ADDR_FIFO_CTRL,FIFO_LEVEL); //sky1311WriteReg(ADDR_FIFO_CTRL,8);            // set water-level of FIFO
}
/**
 ******************************************************************************
 ** \��  ��  SKY1311S��λ��������disable״̬
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
void sky1311Reset(void)
{
    sky1311WriteReg(ADDR_ANA_CFG0, 0);
    sky1311WriteReg(ADDR_ANA_CFG1, 0x01);   // close TX, RX, OSC off
    sky1311WriteCmd(CMD_SW_RST);            // reset state machine
	//	sky1311WriteCmd(CMD_IDLE); 
		SKY1311_ChipDis();
		DelayMS(2);
}

/**
 ******************************************************************************
 ** \��  ��  ��λPICC���رճ�5ms,�ٴ�
 **
 ** \��  ��  none
 ** \����ֵ  none
 ******************************************************************************/
void resetPicc(void)
{
    ClearBitMask(ADDR_ANA_CFG1,TX_EN);
    DelayMS(5);
    SetBitMask(ADDR_ANA_CFG1,TX_EN);
    DelayMS(10);

}
/**
 ******************************************************************************
 ** \��  ��  ���ݷ��亯����������д��FIFO�в�ͨ����Ƶ�ӿڷ��͸�PICC
 **
 ** \��  ��   
 **           uint8_t *txBuff,      // ��������
 **           uint16_t txSize       // ���ݴ�С
 ** \����ֵ  none
 ******************************************************************************/
void sky1311_fifo_tx(uint8_t *txBuff, uint16_t txSize)
{
    uint16_t tx_res = txSize;
    volatile uint32_t delayCount;
    sky1311WriteCmd(CMD_IDLE);              // reset state machine to Idle mode
    sky1311WriteCmd(CMD_CLR_FF);            // clear FIFO
    irqClearAll();                          // clear all IRQ state
		
		uint32_t startWhile=GetTimerCount();
    /* when TX length<=FIFO's depth, write all data to FIFO */
    if( txSize <= 64){
        sky1311WriteFifo(txBuff, txSize);
        sky1311WriteCmd(CMD_TX_RX);             // transceive & into receive mode
    }
    /* when TX length > FIFO's depth */
    else{               // txLen > 64
        sky1311WriteFifo(txBuff, 64);       // send 64 bytes
        sky1311WriteCmd(CMD_TX_RX);
        tx_res = txSize - 64;
        while(tx_res>0){                    // send remain bytes
            if(1==SKY1311_IRQ_READ()){
                irq_sta = sky1311ReadReg(ADDR_IRQ_STA);
                if(irq_sta & IRQ_LOW){  // FIFO low
                    if(tx_res>=(64-FIFO_LEVEL)){
                        sky1311WriteFifo(&txBuff[txSize - tx_res], 64-FIFO_LEVEL);
                        tx_res -=(64-FIFO_LEVEL);
                    }
                    else{
                        sky1311WriteFifo(&txBuff[txSize - tx_res], tx_res);
                        tx_res = 0;
                    }

                    irqClear(irq_sta);//irqClearAll();
                }
                else        // the other IRQ
                    irqClear(irq_sta);//return;
            }
					//	if(GetTimerElapse(startWhile)>500)return;
        }
    }
    /* wait TX finished */
		startWhile=GetTimerCount();
    while(1){
        delayCount = 0xFFFF;
        while( 0==SKY1311_IRQ_READ() && --delayCount);      // ���IRQ�����Ƿ����ж�
        irq_sta=sky1311ReadReg(ADDR_IRQ_STA);
        if((delayCount==0)||(irq_sta & IRQ_TOUT)){             // tiemout
            sky1311WriteCmd(CMD_IDLE);
            return;
        }
        if(irq_sta & IRQ_TX){
            irqClear(IRQ_TX);
            return;
        }
        else{
            irqClear(irq_sta);  //irqClearAll();
        }
				if(GetTimerElapse(startWhile)>500)return;
    }

}
/**
 ******************************************************************************
 ** \��  ��  ���ݽ��պ�������FIFO�����ݶ�����
 **
 ** \��  ��   uint8_t *rxBuff,      // ��������
 **           uint16_t rxSize       // ���ݴ�С
 ** \����ֵ  Ok,��ȷ���յ����� ; ��������������
 ******************************************************************************/
sta_result_t sky1311_fifo_rx( uint8_t *rxBuff, uint16_t *rxSize)
{
    uint16_t rx_buf_cnt=0;
    uint8_t  temp_len;
    volatile uint32_t delayCount;
    /* wait RX data */
    delayCount=0xFFFF;  //�����Ƭ����Ƶ�Ͽ죬���ֵҪ�Ӵ󣬿��Ըĳ�32λ����
    while( (0==SKY1311_IRQ_READ()) && (--delayCount));      // ���IRQ�����Ƿ����ж�
    irq_sta = sky1311ReadReg(ADDR_IRQ_STA);

    if((delayCount==0)||(irq_sta & IRQ_TOUT)){             // tiemout
        sky1311WriteCmd(CMD_IDLE);
        irqClearAll();
        return NoResponse;
    }
    err_sta = sky1311ReadReg(ADDR_ERR_STA);
		if(err_sta & 0xC0)                         // error exits
    {
        sky1311WriteCmd(CMD_CLR_FF);           // noise occur, restart the rx
        sky1311WriteCmd(CMD_RX);
        irqClearAll();
        return Error;
    }
		uint32_t startWhile=GetTimerCount();
		
    while(1)
    {
        irq_sta = sky1311ReadReg(ADDR_IRQ_STA);
        if(irq_sta & IRQ_HIGH){        // FIFO High
            sky1311ReadFifo(&rxBuff[rx_buf_cnt], 64-FIFO_LEVEL);   // load next 64-water-level bytes into FIFO
            rx_buf_cnt += (64-FIFO_LEVEL);                         // 
            irqClear(IRQ_HIGH);
        }
        else if(irq_sta & IRQ_RX){          // Received
            temp_len = sky1311ReadReg(ADDR_FIFO_LEN);               // get FIFO length
            sky1311ReadFifo(&rxBuff[rx_buf_cnt], temp_len);        // get data ,FIFO-->rx_buf
            rx_buf_cnt += temp_len;
            *rxSize  = rx_buf_cnt;
#if SKY_DEBUG
            //temp_len= sky1311ReadReg(ADDR_RX_NUM_L);
            //printf("\nFIFO Receive Size=%d, Rx Number = %d\n",rx_buf_cnt,temp_len);
#endif
            irqClear(IRQ_RX);
            return Ok;
        }
        else if(irq_sta & IRQ_LOW){    //���IRQ_LOW�ж�
            irqClear(IRQ_LOW);
        }
        else if(irq_sta & IRQ_TOUT)
        {
            sky1311WriteCmd(CMD_IDLE);
            irqClearAll();
					
            return ErrorTimeout;
        }  
				if(GetTimerElapse(startWhile)>500)return ErrorTimeout;
    }
}
/**
 ******************************************************************************
 ** \��  ��  �������ݺ�����ת���������
 **          Ҫ���͵����ݱ����ڱ���pSendDataBuf�У����ͻ�������
 **          ���յ����ݱ����ڱ���pRcvDataBuf�����յ������������ڱ���pRcvLen��
 ** \��  ��
 **          uint8_t *pSendDataBuf, ���ͻ�����
 **          uint8_t ucSendLen �������ݵĳ��ȣ�
 **          uint8_t *pRcvDataBuf, �������ݻ�����
 **          uint16_t *pRcvLen �� �������ݳ���
 **
 ** \����ֵ  Ok: ���յ����ص����ݣ� ������û���յ����ص�����
 ******************************************************************************/
sta_result_t ExchangeData(uint8_t *pSendDataBuf, uint16_t ucSendLen,
                          uint8_t *pRcvDataBuf,  uint16_t *pRcvLen)
{
    sta_result_t sta;
    sky1311_fifo_tx(pSendDataBuf, ucSendLen);
    sta = sky1311_fifo_rx(pRcvDataBuf, pRcvLen);
    return sta;
}

/**
 ******************************************************************************
 ** \��  ��  Type A ����ͻ���ݷ��ͺ�����������д��FIFO�в�ͨ����Ƶ�ӿڷ��͸�PICC
 **          ���͵����ݱ�����ȫ�ֱ���tx_buf�У����ͻ�����
 **
 ** \��  ��  uint8_t* txBuf           ���ݻ�����
 **          uint8_t txLen          ��������ݳ��ȣ������������Ĳ��֣�,
 **          uint8_t lastBitNum     ���һ���ֽ���Чλ��
 ** \����ֵ  Ok--�ɹ����䣬Timeout--��ʱ, Error -- ��������
 ******************************************************************************/
sta_result_t bitCollisionTrans(uint8_t* txBuf, uint8_t txLen, uint8_t lastBitNum)
{
    volatile uint16_t delayCount;
    if(txLen>7)          // The maximum length of for transmission from PCD to PICC shall be 55 data bits
        return Error;
    sky1311WriteCmd(CMD_IDLE);              // reset state machine to Idle mode
    sky1311WriteCmd(CMD_CLR_FF);            // clear FIFO
    irqClearAll();                          // clear all IRQ state
    /* write numbers */
    sky1311WriteReg(ADDR_TX_BYTE_NUM, txLen);
    sky1311WriteReg(ADDR_TX_BIT_NUM,lastBitNum);
    sky1311WriteFifo(txBuf, txLen);         // write data to FIFO
    sky1311WriteCmd(CMD_TX_RX);             // transceive & into receive mode
    /* wait TX finished */
			//	Dprint("wait TX finished:\r\n");
		 DelayMS(3); 
		uint32_t startWhile=GetTimerCount();
    while(1){
        delayCount = 0x4FFF;                // timeout count
        while( 0==SKY1311_IRQ_READ() && --delayCount);      // ���IRQ�����Ƿ����ж�
        //while((0==(irq_sta=sky1311ReadReg(ADDR_IRQ_STA))) && --delayCount);   // waiting for TX STOP IRQ
        irq_sta = sky1311ReadReg(ADDR_IRQ_STA);
        if((delayCount==0)||(irq_sta & IRQ_TOUT)){
            sky1311WriteCmd(CMD_IDLE);
            irqClearAll();
            return Timeout;
        }
        else if(irq_sta & IRQ_TX){
            irqClear(IRQ_TX);
            break;
        }
        else{
            irqClear(irq_sta);  //irqClearAll(); //20200825 
        }
				if(GetTimerElapse(startWhile)>500)return Timeout;
    }
    /* �ȴ������������ */
    delayCount = 0x4FFF;                            // timeout count
    while( 0==SKY1311_IRQ_READ() && --delayCount);      // ���IRQ�����Ƿ����ж�
    //while((0==(irq_sta=sky1311ReadReg(ADDR_IRQ_STA))) && --delayCount);     // waiting for RX STOP IRQ
    irq_sta = sky1311ReadReg(ADDR_IRQ_STA);
    if((delayCount==0)||(irq_sta & IRQ_TOUT)){
        sky1311WriteCmd(CMD_IDLE);
        irqClearAll();
        return Timeout;
    }else if(!(irq_sta & IRQ_RX)){
        irqClearAll();
        return Error;
    }
    irqClear(irq_sta);
    return Ok;
}


/**
 ******************************************************************************
 ** \��  ��  ɨ�����ŵ�RCƵ�ʣ�Ϊ�͹���ѯ���ã�
 **          ע�⣺��Ҫ�ڶ���������û���κε��������ڵ�ʱɨ��
 **
 ** \��  ��  none
 ** \����ֵ  ��8λ�ɼ��������ADֵ��
         bit8--bit11 ��ʾ��ֵ���ʱ��Ƶ��ֵ����ӦAnalog0�Ĵ�����bit7--bit4
   		 bit12 ��ʾ�Ƿ�ɨ�赽���ֵ��1--ɨ�赽��0--û�С�
   		 bit13--15 û��ʹ�ã�����Ϊ0
 ******************************************************************************/
#if 1       // Ѱ�����ADֵ
uint16_t sky1311RCFreqCali(void)
{
    
    uint8_t currAdcVal=0;       // ��ǰADֵ
    uint8_t maxAdcVal=0;        // ���ADֵ
    uint8_t currRCFreqVal=0;     // ��ǰƵ��ֵ
    uint8_t maxRCFreq = 0;    // ����Ƶ��ֵ
    SKY1311_ENABLE();
    DelayMS(5);
  //  sky1311WriteReg(ADDR_ANA_CFG6, 0x00); //stop Auto CheckCard
    sky1311WriteReg(ADDR_ANA_CFG1, 0x1C); //OSC Enable
    sky1311WriteReg(ADDR_ANA_CFG2, 0xA0);
		for(currRCFreqVal=0;currRCFreqVal<16;currRCFreqVal++)   // from 0000 to 1111 scan
    {
        sky1311WriteReg(ADDR_ANA_CFG0, (currRCFreqVal << 4) | CHK_PAVOL); // ����Ƶ��
        /*  �ó���������β���AD�ɼ���ת�� */
        sky1311WriteReg(ADDR_ANA_CFG4, 0x00);
        sky1311WriteReg(ADDR_ANA_CFG4, 0x40);   // pull up RSSI_EN
        delay(200);
        sky1311WriteReg(ADDR_ANA_CFG4, 0x60);   // pull up TX_EN
        delay(60);
        sky1311WriteReg(ADDR_ANA_CFG4, 0x70);   // pull up ADC_EN
        delay(60);
        sky1311WriteReg(ADDR_ANA_CFG4, 0x50);   // pull low TX_EN
        delay(150);
        sky1311WriteReg(ADDR_ANA_CFG4, 0x00);   // pull low ADC_EN, RSSI_EN
        delay(30);
        currAdcVal = sky1311ReadReg(ADDR_ANA_CFG5); // read ADC value from analog5 register
        
        //AdcValue[currRCFreqVal] = currAdcVal;  //for debug adc value
        if(currAdcVal > maxAdcVal){			    // ��ǰƵ�ʵ�ADֵ���������ֵ
            maxAdcVal = currAdcVal; 			// �õ�ǰֵȡ�����ֵ
            maxRCFreq = currRCFreqVal;   	// ���µ�ǰƵ��ֵ
        }
	}
    SKY1311_DISABLE();
    if(maxAdcVal>MINADVAL){
        maxRCADVal = maxAdcVal;
        #if SKY_DEBUG==1
        printf("\n--RC frequency calibate:\n");
        printf("RC Param = %2X, AD Value=%2X\n",maxRCFreq,maxRCADVal);
        #endif
        return ((0x10 | (maxRCFreq-0)) << 8 | maxAdcVal);		// ���سɹ���Ǻ�Ƶ��ֵ
    }
    else{
        maxRCADVal = MAXADVAL;
        #if SKY_DEBUG==1
        printf("\n--RC frequency calibate: Error!\n");
        #endif
        return 0;
    }
}
#else
uint16_t sky1311RCFreqCali(void)
{
    uint8_t currRCFreqVal=0;     // ��ǰƵ��ֵ
    uint8_t maxAdcVal=0;         // ���ADֵ
    uint8_t maxRCFreqVal = 0;    // ����Ƶ��ֵ
    uint8_t cryADValue;
    uint8_t rcADValue[16],decADValue[16];
    uint8_t minDecVal=0xff;

    uint8_t caliRCFreqVal;
    uint16_t RetRCFValue;             //����RCFֵ

    SKY1311_ENABLE();
    DelayMS(5);
    /* ʹ���ⲿ������13.56MHz�µ�ADֵ */
    sky1311WriteReg(ADDR_ANA_CFG0, CHK_PAVOL|0x02);
    sky1311WriteReg(ADDR_ANA_CFG1, 0x3C); //OSC Enable 27.12MHz
    sky1311WriteReg(ADDR_ANA_CFG2, 0xA0);
    DelayMS(5);                             // Wait OSC established

    sky1311WriteReg(ADDR_ANA_CFG4, 0x00);
    delay(30);
    sky1311WriteReg(ADDR_ANA_CFG4, 0x40);   // pull up RSSI_EN
    delay(30);
    sky1311WriteReg(ADDR_ANA_CFG4, 0x60);   // pull up TX_EN
    delay(10);
    sky1311WriteReg(ADDR_ANA_CFG4, 0x70);   // pull up ADC_EN
    delay(10);
    sky1311WriteReg(ADDR_ANA_CFG4, 0x50);   // pull low TX_EN
    delay(20);
    sky1311WriteReg(ADDR_ANA_CFG4, 0x00);   // pull low ADC_EN, RSSI_EN
    delay(10);
    cryADValue = sky1311ReadReg(ADDR_ANA_CFG5); // read ADC value from analog5 register

    DelayMS(10);
    /* �ҵ�16��RC��Ӧ��ADֵ, ��ȡ���ⲿ�����µ�ADֵ�Ĳ�ֵ */
    sky1311WriteReg(ADDR_ANA_CFG1, 0x1C); //OSC Enable
    sky1311WriteReg(ADDR_ANA_CFG2, 0xA0);
    for(currRCFreqVal=0;currRCFreqVal<16;currRCFreqVal++)   // from 0000 to 1111 scan
    {
        sky1311WriteReg(ADDR_ANA_CFG0, (currRCFreqVal << 4) | CHK_PAVOL);
        sky1311WriteReg(ADDR_ANA_CFG4, 0x00);
        delay(30);
        sky1311WriteReg(ADDR_ANA_CFG4, 0x40);   // pull up RSSI_EN
        delay(30);
        sky1311WriteReg(ADDR_ANA_CFG4, 0x60);   // pull up TX_EN
        delay(10);
        sky1311WriteReg(ADDR_ANA_CFG4, 0x70);   // pull up ADC_EN
        delay(10);
        sky1311WriteReg(ADDR_ANA_CFG4, 0x50);   // pull low TX_EN
        delay(20);
        sky1311WriteReg(ADDR_ANA_CFG4, 0x00);   // pull low ADC_EN, RSSI_EN
        delay(50);
        rcADValue[currRCFreqVal] = sky1311ReadReg(ADDR_ANA_CFG5); // read ADC value from analog5 register

        if(rcADValue[currRCFreqVal]>maxAdcVal)
        {
            maxAdcVal = rcADValue[currRCFreqVal];
            maxRCFreqVal = currRCFreqVal;
        }
        // ��ǰADֵ���ⲿ����ʱ��ADֵ����ֵ�ľ���ֵ
        if(rcADValue[currRCFreqVal]>cryADValue)
            decADValue[currRCFreqVal] = rcADValue[currRCFreqVal] - cryADValue;
        else
            decADValue[currRCFreqVal] = cryADValue - rcADValue[currRCFreqVal];
	}
    //=========== �ҵ�����ӽ�13.56MHz�� RCֵ,�Ȳ�ֵ��С�Ķ�Ӧ��RCֵ ===========
    //for(currRCFreqVal=2;/*(currRCFreqVal<=maxRCFreqVal)&&*/(currRCFreqVal<13);currRCFreqVal++)
    for(currRCFreqVal=0;currRCFreqVal<=maxRCFreqVal;currRCFreqVal++)
    {
       if(decADValue[currRCFreqVal]<minDecVal)
       {
         minDecVal = decADValue[currRCFreqVal];
         caliRCFreqVal = currRCFreqVal;
       }
    }
    RetRCFValue = (uint16_t)(((0x10 | (caliRCFreqVal)&0xf)<<8) | rcADValue[caliRCFreqVal]);
    return  RetRCFValue ;
}

#endif
/**
 ******************************************************************************
 ** \��  ��  ����Ƿ��п���������뿪��(�͹���ѯ��)
 **
 ** \��  ��  uint8_t RCƵ���趨ֵ
 ** \����ֵ  û�ж�����0  (���õ͹���ѯ�����)
 **          �п����룬1  (���ö������)
 **          �п��뿪��2  (��������У׼��ѯ��Ƶ�ʱ��)
 ******************************************************************************/
void checkCardInit(uint16_t rc_val)
{
    uint8_t maxRCFreq, maxADVal;

    maxRCFreq = (uint8_t)((rc_val >> 4) & 0xF0); // RCƵ��У׼ֵ
    maxADVal = (uint8_t)((rc_val & 0xFF));         // ��Ӧ�����ADֵ

    sky1311WriteReg(ADDR_ANA_CFG1, 0x00);           // disable OSC
    sky1311WriteReg(ADDR_IRQ_EN,0);                 // disable all interrupt
    SKY1311_ENABLE();
    DelayMS(2);
    sky1311WriteReg(ADDR_ANA_CFG0, maxRCFreq | CHK_PAVOL);
    sky1311WriteReg(ADDR_ANA_CFG2, 0x10);
    sky1311WriteReg(ADDR_ANA_CFG4, 0x00);
    sky1311WriteReg(ADDR_ANA_CFG7, ADC_SAMPLE_5US | TX_SETTLE_0US | RSSI_DELTA);
    sky1311WriteReg(ADDR_ANA_CFG8, maxADVal-RSSI_ABS);
    sky1311WriteReg(ADDR_ANA_CFG6, WKU_EN | WKU_OR | WKU_300MS | RSSI_4AVG);
    irqClearAll();
    sky1311WriteReg(ADDR_ANA_CFG1, 0x1C);
	//	sky1311WriteReg(ADDR_IRQ_EN,0); 
    SKY1311_DISABLE();
//    while(1)
//		{
//				if(SKY1311_IRQ_READ())
//				{
//						maxRCFreq=sky1311ReadReg(ADDR_ANA_CFG5);
//						maxRCFreq=sky1311ReadReg(ADDR_IRQ_STA);
//						irqClearAll();
//				}
//		}
}

void checkCardDeinit(void)
{
    SKY1311_ENABLE();
    DelayMS(5);
    sky1311WriteReg(ADDR_ANA_CFG6, 0x22);
    sky1311WriteReg(ADDR_ANA_CFG7, 0x3F);
    sky1311WriteReg(ADDR_ANA_CFG8, 0xFF);
    SKY1311_DISABLE();
}

/**
 ******************************************************************************
 ** \��  ��  ����Ƿ��п��뿪��
 **
 ** \��  ��  uint8_t RCƵ��ɨ��õ������ADֵ
 ** \����ֵ  true,���Ѿ��뿪�� false�� ��û���뿪
 ******************************************************************************/
UINT8 checkCardRemove(uint16_t rc_val)
{
    uint8_t maxRCFreqVal;
    uint8_t maxAdcVal;
    uint8_t currAdcVal;       // ��ǰADֵ
    maxRCFreqVal = (uint8_t)((rc_val >> 4) & 0xF0);
    maxAdcVal = (uint8_t)(rc_val & 0xff);
    SKY1311_ENABLE();
    DelayMS(5);
    sky1311WriteReg(ADDR_ANA_CFG1, 0x1C); //OSC Enable
    sky1311WriteReg(ADDR_ANA_CFG2, 0xA0);
    sky1311WriteReg(ADDR_ANA_CFG0, maxRCFreqVal | CHK_PAVOL);

    sky1311WriteReg(ADDR_ANA_CFG4, 0x00);
    sky1311WriteReg(ADDR_ANA_CFG4, 0x40);   // pull up RSSI_EN
    delay(30);
    sky1311WriteReg(ADDR_ANA_CFG4, 0x60);   // pull up TX_EN
    delay(10);
    sky1311WriteReg(ADDR_ANA_CFG4, 0x70);   // pull up ADC_EN
    delay(10);
    sky1311WriteReg(ADDR_ANA_CFG4, 0x50);   // pull low TX_EN
    delay(20);
    sky1311WriteReg(ADDR_ANA_CFG4, 0x00);   // pull low ADC_EN, RSSI_EN
    delay(10);
    currAdcVal = sky1311ReadReg(ADDR_ANA_CFG5);
    SKY1311_DISABLE();
    #if SKY_DEBUG==1
    printf("currAdcVal=%d\r",currAdcVal);
    #endif
    if(currAdcVal > (maxAdcVal - RSSI_DELTA))
    {
        return TRUE;
    }
    else
        return FALSE;
}
