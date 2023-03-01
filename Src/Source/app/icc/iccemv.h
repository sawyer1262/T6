/*
 * iccemv.h
 *
 *  Created on: 2017��5��26��
 *      Author: YangWenfeng
 */

#ifndef ICCEMV_H_
#define ICCEMV_H_

#include "usi_drv.h"
#include "icc_conf.h"
#include "main.h"

#ifdef BCTC_DEMO
#define USI_PIT
#endif


/*******************************************
             IC������״̬
 *******************************************/
#define ICC_SUCCESS                   0
#define ICC_VCCMODEERR            (-2500)       /* ��ѹģʽ����*/
#define ICC_INPUTSLOTERR          (-2501)       /* ѡ��ͨ���ڴ���*/
#define ICC_VCCOPENERR            (-2502)       /* */
#define ICC_ICCMESERR             (-2503)       /* ��ͨѶʧ��*/

#define ICC_T0_TIMEOUT            (-2200)       /* �ȴ���Ƭ��Ӧ��ʱ  */
#define ICC_T0_MORESENDERR        (-2201)       /* �ط�����*/
#define ICC_T0_MORERECEERR        (-2202)       /* ���մ���*/
#define ICC_T0_PARERR             (-2203)       /* �ַ���ż����*/
#define ICC_T0_INVALIDSW          (-2204)       /* ״̬�ֽ���Ч*/

#define ICC_DATA_LENTHERR         (-2400)       /* ���ݳ��ȴ���  */
#define ICC_PARERR                (-2401)       /* ��ż����      */
#define ICC_PARAMETERERR          (-2402)       /* ����ֵΪ��*/
#define ICC_SLOTERR               (-2403)       /* ��ͨ������   */
#define ICC_PROTOCALERR           (-2404)       /* Э�����     */
#define ICC_CARD_OUT              (-2405)       /* ������       */
#define ICC_NO_INITERR            (-2406)       /* û�г�ʼ��   */
#define ICC_ICCMESSOVERTIME       (-2407)       /* ��ͨѶ��ʱ*/

#define ICC_ATR_TSERR             (-2100)       /* ������Լ������,TS����*/
#define ICC_ATR_TCKERR            (-2101)       /* ��λУ��(T=1,TCK����)����    */
#define ICC_ATR_TIMEOUT           (-2102)       /* ��λ�ȴ���ʱ    */
#define ICC_TS_TIMEOUT            (-2115)       /* �ַ������ʱ    */
#define ICC_ATR_TA1ERR            (-2103)       /* TA1����         */
#define ICC_ATR_TA2ERR            (-2104)       /* TA2����         */
#define ICC_ATR_TA3ERR            (-2105)       /* TA3����         */
#define ICC_ATR_TB1ERR            (-2106)       /* TB1����         */
#define ICC_ATR_TB2ERR            (-2107)       /* TB2����         */
#define ICC_ATR_TB3ERR            (-2108)       /* TB3����         */
#define ICC_ATR_TC1ERR            (-2109)       /* TC1����         */
#define ICC_ATR_TC2ERR            (-2110)       /* TC2����         */
#define ICC_ATR_TC3ERR            (-2111)       /* TC3����         */
#define ICC_ATR_TD1ERR            (-2112)       /* TD1����         */
#define ICC_ATR_TD2ERR            (-2113)       /* TD2����         */
#define ICC_ATR_LENGTHERR         (-2114)       /* ATR���ݳ��ȴ���  */

#define ICC_T1_BWTERR             (-2300)       /* T=1����ȴ�ʱ�����  */
#define ICC_T1_CWTERR             (-2301)       /* T=1�ַ��ȴ�ʱ�����  */
#define ICC_T1_ABORTERR           (-2302)       /* �쳣(ABORT)ͨ�Ŵ��� */
#define ICC_T1_EDCERR             (-2303)       /* ����У����(EDC)���� */
#define ICC_T1_SYNCHERR           (-2304)       /* ͬ��ͨ�Ŵ���*/
#define ICC_T1_EGTERR             (-2305)       /* �ַ�����ʱ�����    */
#define ICC_T1_BGTERR             (-2306)       /* ���鱣��ʱ�����    */
#define ICC_T1_NADERR             (-2307)       /* ������NAD����       */
#define ICC_T1_PCBERR             (-2308)       /* ����PCB����         */
#define ICC_T1_LENGTHERR          (-2309)       /* ����LEN����         */
#define ICC_T1_IFSCERR            (-2310)       /* IFSC����            */
#define ICC_T1_IFSDERR            (-2311)       /* IFSD����            */
#define ICC_T1_MOREERR            (-2312)       /* ��δ��ʹ��󲢷���  */
#define ICC_T1_PARITYERR          (-2313)       /* �ַ���ż����*/
#define ICC_T1_INVALIDBLOCK       (-2314)       /* ��Ч������*/



#define   IC_MAXSLOT       1   //define the 7816_x
#define   USER_CARD        0

//#define debug_print
#define ISO7816_AUTO_RETRANSMIT
#define ISO7816_PBOC_CARD	// 7816 PBOC��,Level1������Ҫ��
//#define JTAG_MODE

#define   ICMAXBUF         512

typedef struct
{
	UINT8   	    Command[ 4 ]; // CLA INS  P1 P2
	UINT16  	    Lc;
	UINT8   	    DataIn[512];
	UINT16  	    Le;
} APDU_SEND;

typedef struct
{
	UINT16       LenOut;    // length of dataout
	UINT8 	    DataOut[512];
	UINT8        SWA;
	UINT8        SWB;
} APDU_RESP;


typedef struct
{
	UINT8 open;         /* 0 �ر� 1 ��  */
	UINT8 existent;     /* 0 �޿� 1 �п�  */
	UINT8 resetstatus;  /* 0-û�и�λ��1-���临λ��2-���ȸ�λ */
	UINT8 autoresp;     /* �Զ�Ӧ�����ã�0=�Զ�Ӧ��  1=���Զ�Ӧ��  */
#ifndef ISO7816_PBOC_CARD
	UINT16 F;
#endif	
	UINT8 D;            /* �޸��첽ͨ�Ų����ʱ�������   */
	UINT8 T;            /* ��ƬЭ������  ֵΪ0��1 */
	UINT8 TS;           /* ����������Լ��  3B=���� 3F=���� */
	UINT8 ATR_len;      /*��Ƭ��λӦ�𳤶� */
	UINT8 ATR[40];      /*��Ƭ��λӦ������ */
	UINT8 TA;       /* ��4λ��Ч����Ӧ��TA1��TA2��TA3,TA4�Ĵ���   */
	UINT8 TA1;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TA2;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TA3;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TA4;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TB;       /* ��4λ��Ч����Ӧ��TB1��TB2��TB3,TB4�Ĵ���   */
	UINT8 TB1;      /* ��Ƭ��λ�ӿ��ַ�    */
	UINT8 TB2;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TB3;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TB4;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TC;       /* ��4λ��Ч����Ӧ��TC1��TC2��TC3,TC4�Ĵ���   */
	UINT8 TC1;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TC2;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TC3;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TC4;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TD;       /* ��4λ��Ч����Ӧ��TD1��TD2��TD3,TD4�Ĵ���   */
	UINT8 TD1;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TD2;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 TD3;      /* ��Ƭ��λ�ӿ��ַ�   */
	UINT8 IFSC;     /* ��Ƭ�������������Ϣ����   */
	UINT8 IFSD;     /* �ն��������������Ϣ����   */
	UINT8 term_pcb; /* T=1Э�����ն˷����¸��ֶθ�ʽ�����к�  */
	UINT8 card_pcb; /* T=1Э���¿������¸��ֶθ�ʽ�����к�    */
	UINT8 sw1;      /* ״̬�ֽ�1  */
	UINT8 sw2;      /* ״̬�ֽ�2  */
}ASYNCARD_INFO;


extern volatile UINT32          k_Ic_TotalTimeCount;         //�ַ����ܳ�ʱ�����м����
extern volatile UINT32          k_Ic_TimeCount;              //�ַ������ʱ�����м����
extern volatile UINT32          k_IcMax_TotalTimeCount;      //�����ܳ�ʱ����ֵ
extern volatile UINT32          k_IcMax_TimeCount;           //�ַ������ʱ����ֵ
extern volatile int             k_timeover,k_total_timeover; //�ַ���ʱ���ܳ�ʱ��־
extern volatile UINT32          k_IcMax_EtuCount;           //�ַ������ʱ����ֵ����λETU

//static   UINT8                  card_Outputdata[ICMAXBUF];
//static   UINT8                  rstbuf[40];
volatile static UINT8           WTX;
volatile static UINT32          WWT,CWT,BWT;
extern volatile UINT8           current_slot;
extern volatile  UINT8          Parity_Check_Enable; /* żУ����󵥸��ַ��ط������־  */
extern volatile  UINT8          k_IccErrPar;       /* ��żУ��    */
extern ASYNCARD_INFO            asyncard_info[IC_MAXSLOT];
extern SC_ATR                   SC_A2R;
extern SC_ADPU_Commands         SC_ADPU;
extern SC_ADPU_Responce         SC_Responce;
extern SC_State                 *locState;
extern SC_READER_SET            SC_Slot_Default;
extern SC_READER_SET            *SC_Slot_Active;
extern volatile UINT8           SC_ATR_Table[40];
extern volatile UINT8           SC_ATR_Len;
extern volatile UINT8           guc_hotrstFlag;
extern volatile UINT8           guc_Leflag;
extern volatile UINT8           SC_T0_T1_Status;
extern volatile UINT8           changeTa1;          //20110729
extern volatile UINT8           guc_AdjustETU;		//�����ַ����ETU��Ҫ��1��1������
extern volatile UINT8           guc_GuardTime;		//20131220������Ϊint����(��ԭֵ����ƥ��)������������GT�Ĵ���ΪĬ��12ETU
extern volatile UINT16          respLength;
extern volatile UINT8           guc_Class;
extern volatile UINT8           ICCardResetOK;      //��ʾIC����λ�ɹ�
extern volatile UINT8           ICCardInsert;       //��ʾ���Ѿ�����
extern volatile UINT8           ICCardInfoSent;      //ic�������Ϣ���ͣ�00 ��ʼ״̬   01 ������   02 ���Ƴ�
extern volatile UINT8           g_usbCommOK;

//icc_hardware.c

/*******************************************************************************
* Function Name  : delayETU
* Description    : Delay a ETU
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void delay1ETU(void);

/*******************************************************************************
* Function Name  : DelayNETU
* Description    : Delay N ETUs
* Input          : count: Delay Parame
* Output         : None
* Return         : None
*******************************************************************************/
void DelayNETU(UINT16 count);

/*******************************************************************************
* Function Name  : SC_VoltageConfig
* Description    : Configures the card power voltage.
* Input          : - SC_Voltage: specifies the card power voltage.
*                    This parameter can be one of the following values:
*                        - MB525 SC_Voltage_5V: 5V cards.
*                        - MB525 SC_Voltage_3V: 3V cards.
*                        - NCN6001 0x01 1.8V
*                        - NCN6001 0x02 3V
*                        - NCN6001 0x03 5V
* Output         : None
* Return         : None
*******************************************************************************/
void SC_VoltageConfig(UINT32 SC_Voltage);

/*******************************************************************************
* Function Name  : Open_ICCard_Vcc_3V
* Description    : open ic card vcc = 5V
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Open_ICCard_Vcc_5V(void);

/*******************************************************************************
* Function Name  : Open_ICCard_Vcc_3V
* Description    : open ic card vcc = 3V
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Open_ICCard_Vcc_3V(void);

/*******************************************************************************
* Function Name  : Open_ICCard_Vcc_1V8
* Description    : open ic card vcc = 1V8
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Open_ICCard_Vcc_1V8(void);

/*******************************************************************************
* Function Name  : Close_ICCard_Vcc
* Description    : close ic card vcc
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Close_ICCard_Vcc(void);

/*******************************************************************************
* Function Name  : Open_ICCard_VCC
* Description    : open ic card vcc
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Open_ICCard_VCC(void);

/*******************************************************************************
* Function Name  : SC_Init
* Description    : Initializes all peripheral used for Smartcard interface.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SC_Init(void);

/*******************************************************************************
* Function Name  : SC_HotInit
* Description    : Smart card �ȸ�λ��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SC_HotInit(void);

/*******************************************************************************
* Function Name  : SC_DeInit
* Description    : Deinitializes all ressources used by the Smartcard interface.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SC_DeInit(void);

/*******************************************************************************
* Function Name  : SC_Reset
* Description    : SIM Card ��λ
* Input          : mode  ��λ��ʽ���ȸ�λ�����临λ
*
* Output         : None
* Return         : ���ظ�λ״̬  0����λ�ɹ�   -1��λʧ��
******************************************************************************/
char SC_Reset(UINT8 mode);

/*******************************************************************************
* Function Name  : SC_RSTSET
* Description    : Sets or clears the Smartcard reset pin.
* Input          : - ResetState: this parameter specifies the state of the Smartcard
*                    reset pin.
*                    BitVal must be one of the BitAction enum values:
*                       - Bit_RESET: to clear the port pin.
*                       - Bit_SET: to set the port pin.
* Output         : None
* Return         : None
*******************************************************************************/
void SC_RSTSET(UINT8 ResetState);

/*******************************************************************************
* Function Name  : SC_Init
* Description    : Initializes all peripheral used for Smartcard interface.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SC_RegInit(USI_TypeDef* USIx);


//iccemv.c
/*******************************************************************************
* Function Name  : USART_ByteReceive
* Description    : Receives a new data while the time out not elapsed.
* Input          : None
* Output         : None
* Return         : An ErrorStatus enumuration value:
*                         - SUCCESS: New data has been received
*                         - ERROR: time out was elapsed and no further data is
*                                  received
*******************************************************************************/
ErrorStatus USART_ByteReceive(UINT8 *Data, UINT32 TimeOut);

/*******************************************************************************
* Function Name  : USART_TsReceive
* Description    : Receives a new data while the time out not elapsed.
* Input          : None
* Output         : None
* Return         : An ErrorStatus enumuration value:
*                         - SUCCESS: New data has been received
*                         - ERROR: time out was elapsed and no further data is
*                                  received
*******************************************************************************/
ErrorStatus USART_TsReceive(UINT8 *Data, UINT32 TimeOut);

/*******************************************************************************
* Function Name  : SC_GetByte
* Description    : Get byte from IO
* Input          : Data pointer
* Output         : None
* Return         : None
*******************************************************************************/

ErrorStatus SC_GetByte(UINT8 *Data);

/*******************************************************************************
* Function Name  : SC_PutByte
* Description    : Put byte
* Input          : Data to send
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 SC_PutByte(UINT8 Data);

/*******************************************************************************
* Function Name  : SC_Command
* Description    : Handles all Smartcard states and serves to send and receive all
*                  communication data between Smartcard and reader.
* Input          : - locadpu: pointer to an adpu buffer.
*                : adpulen : adpu'length
* Output         : - locresp: pointer to a Responce buffer.
*                : resplen : response's length which includes SW1&SW2
* Return         : SW1 & SW2
*******************************************************************************/
UINT16 SC_Command(UINT16 adpulen, UINT8 * adpubuf, UINT16 * resplen, UINT8 * respbuf);

/*******************************************************************************
* Function Name  : SC_Handler
* Description    : Handles all Smartcard states and serves to send and receive all
*                  communication data between Smartcard and reader.
* Input          : - SCState: pointer to an SC_State enumeration that will contain
*                    the Smartcard state.
*                  - SC_ADPU: pointer to an SC_ADPU_Commands structure that will be
*                    initialized.
*                  - SC_Response: pointer to a SC_ADPU_Responce structure which will
*                    be initialized.
* Output         : None
* Return         : None
*******************************************************************************/
void SC_Handler(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_Response);
/*******************************************************************************
* Function Name  : Icc_Reset
* Description    :��ʼ��SLOTͨ����IC���������Կ��ϵ磬��λ���жϿ���Э�����ͣ�
*                 ��T=1��T=0��������ATR����λӦ�𣩶���*ATRָ���⡣
* Input          : slot ��Ҫ��ʼ����ͨ����
*                  ATR  ATR����BUF
*
* Output         : None
* Return         : None
******************************************************************************/
int Icc_Reset(UINT8 slot,UINT8 *ATR);
/*******************************************************************************
* Function Name  : exp2_n
* Description    :����2��N�η�����
* Input          :en : ����
*
* Output         : None
* Return         : ���ز������
******************************************************************************/
ushort exp2_n(UINT8 en);
/*******************************************************************************
* Function Name  : GetAtr
* Description    :��ȡIC���ĸ�λӦ���ź�ATR
* Input          :Buff : ATR�Ĵ��BUFF
*
* Output         : None
* Return         : ���ز��������0x00���ɹ�
******************************************************************************/
int GetAtr(UINT8 *Buff);
/*******************************************************************************
* Function Name  : ClearAtr
* Description    :���ATR BUF
* Input          :Channel : ��Ҫ�����ATRbuf���߼���
*
* Output         : None
* Return         : None
******************************************************************************/
void ClearAtr(UINT8 Channel);
/*******************************************************************************
* Function Name  : Icc_Command
* Description    :��������
*                 ֧��T0Э����T1Э��
*                 ��֧��CASE2���͵������
* Input          : slot     �������˿�
*                  ApduSend APDU����
*                  ApduResp Ӧ������
*
* Output         : None
* Return         : ���ز������
******************************************************************************/
int Icc_Command(UINT8 slot,SC_ADPU_Commands * ApduSend,SC_ADPU_Responce * ApduResp);

/*******************************************************************************
* Function Name  : IccSendCommand
* Description    :�ֱ����T=1��T=0Э�齫CLA+INS+P1+P2+LC��LE��+INDATA���������ݰ���ʽ
*                 ���� ת����������Ӧ�ĸ�ʽ��IC�����н��������������״̬������OUTDATA��
*                 T=1ʱ�� OUTDATA�� LEN��2��+DATA+SW��1��2����
*                 T=0ʱ�� OUTDATA�� LEN��2��+DATA�� SW���浽ȫ�ֱ�����
* Input          :cla : CLA
*                 ins : INS
*                 p1  : P1
*                 p2  : P2
*                 p3  : P3
*                 indata:data
*                 le   :Le
*                 outdata:���BUF
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��
******************************************************************************/
int IccSendCommand(UINT8 cla, 
									 UINT8 ins,
									 UINT8 p1,
									 UINT8 p2,
									 UINT16 p3,
									 UINT8 *indata,
									 UINT16 le,
									 UINT8 *outdata);

/*******************************************************************************
* Function Name  : T0_send_command
* Description    :T=0Э���·�������ͷ��CLA+INS+P1+P2+P3�����ݰ���DATABUF��������ȡIC����Ӧ�����ݣ�OUTBUF����״̬�ֽڣ�RSLT����
* Input          :Class : CLA
*                 Ins : INS
*                 p1  : P1
*                 p2  : P2
*                 p3  : P3
*                 databuf�����ն˷��������ݰ������䳤��ΪP3����
*                 outbuf:��IC���������������ݣ�[0]Ϊ���ݳ��ȣ�
*                 relt:��CASE����ţ�1��2��3��4��
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��       ����ʱ����SW1��SW2״̬�ֽڡ�
******************************************************************************/
int T0_send_command(UINT8 cla,
	                  UINT8 ins,
                    UINT8 p1,
                    UINT8 p2,
                    UINT8 p3,
                    UINT8 *databuf,
                    UINT8 *outbuf,
                    UINT8 *rslt);

/*******************************************************************************
* Function Name  : T1_send_command
* Description    :T=1Э���µ��������ݷ����������Ӧ���ݡ�
*                   ע��: T1��֧����չָ����Ҫ֧������Ҫ�޸�
* Input          :NAD : ����ַ������Ϊ0X00����
*                 INBUF : ���͵���Ϣ�ֶ�LEN+INF��INBUF[0]Ϊ���ȣ�
*                 OUTBUF: ����Ӧ�����ݵ�Ԫ����ʽNAD+LEN��2�ֽڣ�+DATA+SW1+SW2
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��
******************************************************************************/
int T1_send_command(UINT8 nad,UINT8 *inbuf,UINT8 *outbuf);
/*******************************************************************************
* Function Name  : T1_Send_Block
* Description    :T=1Э���·���һ����
* Input          :Inbuf : �����������buf
*
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��
******************************************************************************/
int  T1_Send_Block(UINT8 *Inbuf);
/*******************************************************************************
* Function Name  : T1_Rece_Block
* Description    :T=1Э���½���һ����
* Input          :Outbuf : �������ݻ���buf
*
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��
******************************************************************************/
int  T1_Rece_Block(UINT8 *Outbuf);
/*******************************************************************************
* Function Name  : T1_IFSD_command
* Description    :���յ���ȷ�ĸ�λӦ��󣬶���T=1��Ƭ�뷢��һ��IFSD������ָʾ�ն�����ȡ������鳤�ȡ�
* Input          :slot : �����߼���
*
* Output         : None
* Return         : ����ִ�н����0x00���ɹ�  ������ʧ��
******************************************************************************/
int  T1_IFSD_command(UINT8 slot);
/*******************************************************************************
* Function Name  : SC_PollCard_Init
* Description    : SIM card insertion init detection
* Input          : None
*
* Output         : None
* Return         : None
*******************************************************************************/
void SC_PollCard(void);

#endif /* ICCEMV_H_ */
