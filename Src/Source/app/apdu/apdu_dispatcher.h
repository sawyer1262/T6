/*
 * apdu_dispatcher.h
 *
 *  Created on: 2018��8��13��
 *      Author: YangWenfeng
 */

#ifndef APDU_DISPATCHER_H_
#define APDU_DISPATCHER_H_

#define INS_READ_VERSION			0x5C//��ȡ�����Ӳ���汾��
#define INS_TEST_SLEEP              0x5D //����sleep
#define INS_MEM_READ				0xA0
#define INS_READ_SN					0xE1
#define INS_GET_RESPONSE			0xC0


extern void usi_main(void);
#endif /* APDU_DISPATCHER_H_ */
