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
 ** �� �� ���� ultralight.h
 **
 ** �ļ������� ����Mifare Ultralight�� Ultralight C�����������к����������ͱ���
 **
 ** �汾��ʷ:
 ** 2019-05-30 V1.00  EH   ��һ����ʽ�汾
 **
 ******************************************************************************/
#define ULC_ACK			0x0A	//Ah positive acknowledge (ACK)
#define ULC_NAK			0x02	//2h NAK for EEPROM write error
#define ULC_NAKCRC	0x01	//1h NAK for parity or CRC error
#define ULC_NAKOTH	0x00	//0h NAK for any other error

#define ULC_READ		0x30
#define ULC_WRITE		0xA2
#define ULC_COMWRITE	0xA0
#define ULC_AUTH		0x1A


sta_result_t ULC_Authentication(uint8_t *Key);
sta_result_t ULC_Read(uint8_t address, uint8_t *receBuff);
sta_result_t ULC_Write(uint8_t address, uint8_t* data);
sta_result_t ULC_CompWrite(uint8_t pageAddress, uint8_t* data);
