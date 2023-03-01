/*
 * alg_drv.h
 *
 *  Created on: 2017��6��9��
 *      Author: ccore
 */

#ifndef ALG_DRV_H_
#define ALG_DRV_H_
#include "type.h"
#include "memmap.h"

#define SRAM_BEGIN    (UINT32)(0x20000000)
#define SRAM_END      (UINT32)(0x20038000)

#define EFLASH_BEGIN  (UINT32)(0x8000000)
#define EFLASH_END    (UINT32)(0x807F000)

typedef enum
{
	TYPE_ERROR = 0,
	TYPE_SRAM  = 1,
	TYPE_EFLASH= 2,
}TYPE_ADDR;

/*******Hash�㷨���ͱ��********/
#define	HASH_SM3				0
#define	HASH_SHA0				1
#define	HASH_SHA1				2
#define	HASH_SHA224				3
#define HASH_SHA256				4
#define HASH_SHA384				5
#define HASH_SHA512				6

/*******Hash�㷨���ݽṹ********/
typedef	struct Hash_tx
{
	unsigned int  hash_len[4];	   //���δ������ݳ��ȣ�
	unsigned int  DataBuf[16];     //����������
	unsigned char HASHMode;	       //Hash�㷨���ͱ��
	unsigned char fb_flag[1];      //��һ�����ݱ�־,1 - ��һ�����ݴ�����;0 - δ���

}Hash_tx;

#define SHA0_R_Len				20
#define SHA1_R_Len  			20
#define SHA224_R_Len 			28
#define SHA256_R_Len 			32
#define SM3_R_Len				32
#define SHA384_R_Len 			48
#define SHA512_R_Len 			64

//sm2
#define	SM2_TEST_BITS       	256
//����������ص����ݽṹ���������㺯��
#define ECC_BIGINT32_MAXLEN 	18

/*******������32λ����ṹ********/
typedef struct
{
 	UINT16   uLen;                         //32λ�����ʵ�ʳ���
	UINT32   auValue[ECC_BIGINT32_MAXLEN]; //32λ�����ֵ������ĵ�32λ��ַ��Ŵ����ĵ�32λ��Ч������32λ��ַ��Ŵ����ĸ�32λ��Ч��
}ECC_STU_BIGINT32;

/*******SM2�����ݽṹ********/
typedef struct
{
    UINT16 uBits;                               // ģ��P��λ��
	ECC_STU_BIGINT32  stuPrimeP;				// ģ��P
	ECC_STU_BIGINT32  stuCoefficientA;			// ��Բϵ��A
	ECC_STU_BIGINT32  stuCoefficientB;			// ��Բϵ��B
	ECC_STU_BIGINT32  stuGx;				    // ��Բ��������Gx
	ECC_STU_BIGINT32  stuGy;				    // ��Բ��������Gy
	ECC_STU_BIGINT32  stuPrimeN;				// ��Բ��������G�Ľ�
} SM2_STU_PRAMS;

/******SM2��Կ�ṹ********/
typedef struct
{
	ECC_STU_BIGINT32  stuQx;			        // ��Կ����Qx
	ECC_STU_BIGINT32  stuQy;				    // ��Կ����Qy
} SM2_STU_PUBKEY;


extern SM2_STU_PRAMS stuEccPrams;
extern SM2_STU_PUBKEY stuPubKey;
extern ECC_STU_BIGINT32 stuPrivKey;
extern ECC_STU_BIGINT32 stuSignatureR;
extern ECC_STU_BIGINT32 stuSignatureS;


//rsa
//#define		RSA_TEST_BITS	512
//#define		RSA_TEST_BITS	1024
#define		RSA_TEST_BITS	2048

//#define MAX_RSA_MODULUS_BITS 512
//#define MAX_RSA_MODULUS_BITS 1024
#define MAX_RSA_MODULUS_BITS 2048

#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 31) / 32)
#define MAX_RSA_PRIME_BITS	((MAX_RSA_MODULUS_BITS +  1) / 2 )
#define MAX_RSA_PRIME_LEN	((MAX_RSA_PRIME_BITS   + 31) / 32)

/******RSA��Կ�ṹ********/
typedef struct
{
	unsigned int bits;                                /* length in bits of modulus */
	unsigned int modulus[MAX_RSA_MODULUS_LEN];        /* modulus */
	unsigned int exponent[MAX_RSA_MODULUS_LEN];       /* public exponent */
} STU_RSA_PUBKEY;

/******RSA˽Կ�ṹ********/
typedef struct
{
	unsigned int bits;                                  /* length in bits of modulus */
	unsigned int modulus[MAX_RSA_MODULUS_LEN];          /* modulus */
    unsigned int n_inv[MAX_RSA_MODULUS_LEN];
	unsigned int publicExponent[MAX_RSA_MODULUS_LEN];   /* public exponent */
	unsigned int exponent[MAX_RSA_MODULUS_LEN];         /* private exponent */
	unsigned int prime[2][MAX_RSA_PRIME_LEN];           /* prime factors */
	unsigned int primeExponent[2][MAX_RSA_PRIME_LEN];   /* exponents for CRT */
	unsigned int primeEnu[2][MAX_RSA_PRIME_LEN];
	unsigned int primeInv[2][MAX_RSA_PRIME_LEN];
	unsigned int coefficient[MAX_RSA_PRIME_LEN];        /* CRT coefficient */
} STU_RSA_PRIVKEY;

typedef struct
{
	unsigned int  bits;               /*length in bits of modulus*/
	unsigned char *modulus;           /* modulus */
	unsigned char *exponent;          /* public exponent */
	unsigned int  explen;             /* exponent bytes  */
}rsa_pub_key_t;

extern STU_RSA_PUBKEY  publicKeyRSA;
extern STU_RSA_PRIVKEY privateKeyRSA;

/*******************************************************************************
* Function Name  : AES_Enable
* Description    : ʹ��AESģ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void AES_Enable(void);

/*******************************************************************************
* Function Name  : AES_Enable
* Description    : ����AESģ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void AES_Disable(void);

/*******************************************************************************
* Function Name  : AES_EnDecrypt
* Description    : ͨ��AES (128bits��192bits��256bits)���мӽ�������
*
*                 ��ע��
*                 1��ԭ���Ŀ�˺����ڲ�����֮ǰ����EDMACʱ�ӣ��˳�֮ǰ�ر���EDMACʱ��;�����걾����֮������������ʹ��EDMACģ�飬��Ҫ��������EDMACģ��ʱ�ӣ�
*                 2��ԭ�����¿������˳��������ͬ��
*                 3���������ڲ���EDMACʱ��δ�����κβ�����
*
* Input          : EncDe_Mode - �ӽ���ѡ��
*                      0 - ��ʾ����;
*                      1 - ��ʾ����.
*                  CTR_ECB_MODE - ģʽѡ��
*                      0 - ECBģʽ;
*                      1 - CTRģʽ.
*                  Key  -  �ӽ�����Կ
*                  cntIV -  CTRģʽ��ʼ����ֵ
*                  key_mode - AES�㷨��Կ����
*                      0 - 128bit;
*                      1 - 192bit;
*                      2 - 256bit.
*                  srcbuffer - �ӽ������ݵ�ַ
*                  Output - �ӽ��ܽ����ַ
*                  datalen - �ӽ������ݳ��ȣ�16�ֽڵ�������
* Output         : None
* Return         : None
******************************************************************************/
extern void AES_EnDecrypt(U8 EncDe_Mode, U8 CTR_ECB_MODE,U32 *Key, U32 *cntIV,U32 key_mode, U32 *srcbuffer, U32 *Output, U32 datalen);

/*******************************************************************************
* Function Name  : DES_Enable
* Description    : ʹ��DESģ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void DES_Enable(void);

/*******************************************************************************
* Function Name  : DES_Disable
* Description    : ����DESģ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void DES_Disable(void);

/*******************************************************************************
* Function Name  : AES_EnDecrypt
* Description    : ͨ��des/3des (64bits��128bits��192bits) ���мӽ�������
*
*                 ��ע��
*                 1��ԭ���Ŀ�˺����ڲ�����֮ǰ����EDMACʱ�ӣ��˳�֮ǰ�ر���EDMACʱ��;�����걾����֮������������ʹ��EDMACģ�飬��Ҫ��������EDMACģ��ʱ�ӣ�
*                 2���������ڲ���EDMACʱ��δ�����κβ�����
*
* Input          : EncDe_Mode - �ӽ���ѡ��
*                      0 - ��ʾ����;
*                      1 - ��ʾ����.
*                  ECB_CBC_Mode - ģʽѡ��
*                      0 - ECBģʽ;
*                      1 - CBCģʽ
*                  Key  -  �ӽ�����Կ
*                  KeyLen - �ӽ�����Կ���ȣ���λΪ�ֽ�
*                      8  - 64bit��des�ӽ���;
*                      16 - 128bit��3des�ӽ���;
*                      24 - 192bit��3des�ӽ���.
*                  ivbuffer - ��ʼ����ֵ���̶�Ϊ8�ֽڳ��ȵ�ֵ
*                  srcbuffer - �ӽ������ݵ�ַ
*                  Output - �ӽ��ܽ����ַ
*                  datalen - �ӽ������ݳ��ȣ�8�ֽڵ�������
*                  check_parity�������key�Ƿ���Ҫ����У�飬���У������򷵻�keyУ������ټ����ӽ���
*                      1 - �򿪴˹���;
*                      0 - ���ô˹���.
* Output         : None
* Return         : None
******************************************************************************/
extern void DES_EnDecrypt(U8 EncDe_Mode, U8 ECB_CBC_Mode, U32 *Key, U32 KeyLen, U32 *ivbuffer, U32 *srcbuffer, U32 *Output,U32 datalen, U8 check_parity);

/*******************************************************************************
* Function Name  : SMS4_Enable
* Description    : ʹ��SMS4ģ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void SMS4_Enable(void);

/*******************************************************************************
* Function Name  : SMS4_Disable
* Description    : ����SMS4ģ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void SMS4_Disable(void);

/*******************************************************************************
* Function Name  : SMS4_EnDecrypt
* Description    : ͨ��SMS4ģ������ݼӽ���
*
*                 ��ע��
*                 1��ԭ���Ŀ�˺����ڲ�����֮ǰ����EDMACʱ�ӣ��˳�֮ǰ�ر���EDMACʱ��;�����걾����֮������������ʹ��EDMACģ�飬��Ҫ��������EDMACģ��ʱ�ӣ�
*                 2���������ڲ���EDMACʱ��δ�����κβ�����
*
* Input          : EncDe_Mode - �ӽ���ѡ��
*                      0 - ��ʾ����;
*                      1 - ��ʾ����.
*                  ECB_CBC_Mode - ģʽѡ��
*                      0 - ECBģʽ;
*                      1 - CBCģʽ
*                  KEY - 128λ�ӽ�����Կ
*                  IV - 128λ��ʼ����
*                  ivbuffer - ��ʼ����ֵ���̶�Ϊ8�ֽڳ��ȵ�ֵ
*                  srcbuffer - �ӽ������ݵ�ַ
*                  Output - �ӽ��ܽ����ַ
*                  datalen - �ӽ������ݳ��ȣ�16�ֽڵ�������
* Output         : None
* Return         : None
******************************************************************************/
extern void SMS4_EnDecrypt (unsigned char EncDe_Mode, unsigned char ECB_CBC_Mode, unsigned int *KEY, unsigned int*IV, unsigned int *srcbuffer, unsigned int *Output, unsigned int datalen);

/*******************************************************************************
* Function Name  : SHA_Enable
* Description    : ʹ��SHAģ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void SHA_Enable(void);

/*******************************************************************************
* Function Name  : SHA_Disable
* Description    : ����SHAģ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void SHA_Disable(void);

/*******************************************************************************
* Function Name  : SHA_Init
* Description    : SHA�㷨ģ���ʼ��
*
*                 ��ע��
*                 1��ʹ��SHAģ�麯��֮ǰ������������SHA��EDAMCģ��ʱ��;
*                 2����ԭ���汾��Ƚ�����Hash_tx *phash_tx������
*
* Input          : phash_tx - ��ϣ�㷨���ݽṹ
*                  HashModeNum - ��ϣ�㷨��ţ�ȡֵ��Χ0~4
* Output         : None
* Return         : None
******************************************************************************/
extern void SHA_Init(Hash_tx *phash_tx, unsigned char HashModeNum);

/*******************************************************************************
* Function Name  : SHA_Update
* Description    : ����Ҫ����SHA���������
*
*                 ��ע��
*                 1��ʹ��SHAģ�麯��֮ǰ������������SHA��EDAMCģ��ʱ�ӣ�
*                 2����ԭ���汾��Ƚ�����Hash_tx *phash_tx������
*
* Input          : phash_tx - ��ϣ�㷨���ݽṹ
*                  pMessageBlock �C ָ�򱾴δ���������ݣ���ַ���4�ֽڶ���
*                  DataLen �C ���δ��������ݳ��ȣ��������Ϊ4��������
* Output         : None
* Return         : None
******************************************************************************/
extern void SHA_Update(Hash_tx *phash_tx, unsigned char *pMessageBlock, unsigned int DataLen);

/*******************************************************************************
* Function Name  : SHA_Final
* Description    : ����SHA��������ȡ���
*
*                 ��ע��
*                 1��ʹ��SHAģ�麯��֮ǰ������������SHA��EDAMCģ��ʱ�ӣ�
*                 2����ԭ���汾��Ƚ�����Hash_tx *phash_tx������
*
* Input          : phash_tx - ��ϣ�㷨���ݽṹ
*                  pRes �C ��ϣ������
* Output         : None
* Return         : ����SHA�������ֽڳ���
******************************************************************************/
extern unsigned int SHA_Final(Hash_tx *phash_tx, unsigned int *pRes);

/*******************************************************************************
* Function Name  : SM2_Enable
* Description    : SM2ģ��ʹ��
*
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2_Enable(void);

/*******************************************************************************
* Function Name  : SM2_Disable
* Description    : SM2ģ�����
*
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2_Disable(void);

/*******************************************************************************
* Function Name  : SM2Init
* Description    : SM2ģ���ʼ��
*
*                 ��ע��
*                 ԭ�⺯������ΪECCInit(unsigned short bits);
*
* Input          : bits - SM2λ��
* Output         : None
* Return         : None
******************************************************************************/
//extern void SM2Init(unsigned short bits);

/*******************************************************************************
* Function Name  : SM2GenerateKeyPair
* Description    : SM2���ɵ���Կ��
*
*                 ��ע��
*                 1���˺����ڲ���ʹ�������ģ�飬��������ģ��ʱ�Ӳ�����InitTrng������
*                 2�������ڲ�ʹ��EDMAC����Ҫ������EDMACģ��ʱ�ӣ�
*                 3���ڲ�ʹ��CRYPTOģ�飬��Ҫ������CRYPTOģ��ʱ�ӣ�
*
* Input          : pstuSM2Prams->uBits			           ģ��P��λ��
*                  pstuSM2Prams->stuCoefficientA	��Բϵ��A
*                  pstuSM2Prams->stuCoefficientA	��Բϵ��B
*                  pstuSM2Prams->stuGx			           ��Բ��������Gx
*                  pstuSM2Prams->stuGy			           ��Բ��������Gy
*                  pstuSM2Prams->stuPrimeN		           ��Բ��������G�Ľ�
*                  pstuPrivKey		                                           ������ɵ�˽Կ
*                  pstuPbuKey					           ������ɵĹ�Կ
* Output         : None
* Return         : 1 - ��Կ�Բ�����ȷ
*                  0 - ��Կ�Բ���ʧ��
******************************************************************************/
extern char SM2GenerateKeyPair(SM2_STU_PRAMS *pstuSM2Prams, ECC_STU_BIGINT32 *pstuPrivKey,SM2_STU_PUBKEY *pstuPbuKey);

/*******************************************************************************
* Function Name  : SM2Encrypt_V2
* Description    : ʹ�ù�Կ�����ļ��ܣ����������ĳ��� = ���ĳ��� + 96�ֽ�
*                  SM2Encrypt_V2��ʹ�ù�Կ�����ļ��ܣ����������Ĵ洢��ʽΪ��C1|C3|C2��
*
*                 ��ע��
*                 1.�����ڲ�ʹ��EDMAC����Ҫ��EDMACģ��ʱ�ӣ�
*                 2.�����ڲ�ʹ��SHAģ�飬��Ҫ��SHAģ��ʱ�ӣ�
*                 3.�ڲ�ʹ��CRYPTOģ�飬��Ҫ��CRYPTOģ��ʱ�ӣ�
*
* Input          : mes	���ģ����ֽ��ں󣬵��ֽ���ǰ����˽ṹ����mes[0]Ϊ���ֽڣ������һ���ֽڲ���32λ�����ڸ��ֽڵ�ĩβ��0ֱ����32λ;
*                       ����mes��Ϊ48bit����123456789abc����mes[0]Ϊ0x12345678��mes[1]Ϊ0x9abc0000��
*                  klen       		�����ֽڳ���
*                  pstuPubKey		��Կ
*                  pstuEccPrams   	��Բ���߲���
*                  cipher		          �����������ģ��洢��ʽ��������ͬ
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2Encrypt_V2(unsigned char *mes, unsigned short klen, SM2_STU_PUBKEY *pstuPubKey,SM2_STU_PRAMS *pstuEccPrams, unsigned char *cipher);

/*******************************************************************************
* Function Name  : SM2Decrypt_V2
* Description    : ʹ��˽Կ�����Ľ���
*                  SM2Decrypt_V2��ʹ��˽Կ�Դ洢��ʽΪC1|C3|C2�����Ľ��н��ܣ�
*
*                 ��ע��
*                 1.�����ڲ�ʹ��EDMAC����Ҫ��EDMACģ��ʱ�ӣ�
*                 2.�����ڲ�ʹ��SHAģ�飬��Ҫ��SHAģ��ʱ�ӣ�
*                 3.�ڲ�ʹ��CRYPTOģ�飬��Ҫ��CRYPTOģ��ʱ�ӣ�
*                 4.ԭ�⣺ʹ��˽Կ�Դ洢��ʽΪC1|C3|C2�����Ľ��н���
*
* Input          : cipher		  	 ���ģ���˽ṹ
*                  klen         	 �����ֽڳ���
*                  stuPrivKey		 ˽Կ
*                  pstuEccPrams     ��Բ���߲���
*                  plain��		           ������ģ���˽ṹ
* Output         : None
* Return         : 1 - �ɹ�
*                  0 - ʧ��
******************************************************************************/
extern UINT8 SM2Decrypt_V2(unsigned char *cipher, unsigned short klen, ECC_STU_BIGINT32 *stuPrivKey, SM2_STU_PRAMS *pstuEccPrams, unsigned char *plain);

/*******************************************************************************
* Function Name  : SM2Signature
* Description    : ʹ��˽Կ�����Ľ���ǩ����
*
*                 ��ע��
*                 1.�˺����ڲ�ʹ�������ģ�飬��Ҫ��ģ��ʱ��;
*                 2.�˺����ڲ�ʹ��EDAMCģ�飬��Ҫ��ģ��ʱ��;
*                 3.�˺����ڲ�ʹ��CRYPTOģ�飬��Ҫ���ģ��ʱ��;
*
* Input          : mes				  ����
*                  klen			             �����ֽڳ���
*                  pstuEccPrams	             ��Բ���߲���
*                  pstuPubKey		  ��Կ
*                  pSignatureR	             ������ĵ�ǩ����ϢR
*                  pSignatureS	             ������ĵ�ǩ����ϢS
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2Signature(unsigned char *mes, unsigned short klen, SM2_STU_PRAMS *pstuEccPrams, ECC_STU_BIGINT32 *pstuPrivKey, ECC_STU_BIGINT32 *pSignatureR, ECC_STU_BIGINT32 *pSignatureS);

/*******************************************************************************
* Function Name  : SM2SignatureWithIDA
* Description    : ʹ��˽Կ�������Ϣ�����Ľ���ǩ����
*
*                 ��ע��
*                 1.�˺����ڲ�ʹ�������ģ�飬��Ҫ��ģ��ʱ��;
*                 2.�˺����ڲ�ʹ��SHAģ�飬��Ҫ���ģ��ʱ��;
*                 3.�˺����ڲ�ʹ��EDMACģ�飬��Ҫ���ģ��ʱ��;
*                 4.�˺����ڲ�ʹ��CRYPTOģ�飬��Ҫ���ģ��ʱ��;
*
* Input          : mes				  ����
*                  klen			             �����ֽڳ���
*                  IDA				  ǩ�����������Ϣ
*                  entla		             ǩ���������Ϣ�ֽڳ���
*                  pstuEccPrams	             ��Բ���߲���
*                  pstuPubKey		  ��Կ
*                  pstuPrivKey	             ˽Կ
*                  pSignatureR	             ������ĵ�ǩ����ϢR
*                  pSignatureS	             ������ĵ�ǩ����ϢS
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2SignatureWithIDA(unsigned char *mes, unsigned short klen, unsigned char *IDA, unsigned short entla, SM2_STU_PRAMS *pstuEccPrams, SM2_STU_PUBKEY *pstuPubKey, ECC_STU_BIGINT32 *pstuPrivKey, ECC_STU_BIGINT32 *pSignatureR,ECC_STU_BIGINT32 *pSignatureS);

/*******************************************************************************
* Function Name  : SM2Verification
* Description    : ʹ�ù�Կ�����ĺ�ǩ��������֤��
*
*                 ��ע��
*                 1.�˺����ڲ�ʹ��EDAMCģ�飬��Ҫ��ģ��ʱ��;
*                 2.�˺����ڲ�ʹ��CRYPTOģ�飬��Ҫ��ģ��ʱ��;
*
* Input          : mes				    ����
*                  klen				    �����ֽڳ���
*                  IDA			              ǩ�����������Ϣ
*                  entla			   ǩ���������Ϣ���ֽڳ��ȣ���ע��ԭ����Ϊǩ���������Ϣ��bit���ȣ�
*                  pstuEccPrams		   ��Բ���߲���
*                  pstuPubKey		   ��Կ
*                  pSignatureR		   ���ĵ�ǩ����ϢR
*                  pSignatureS		   ���ĵ�ǩ����ϢS
* Output         : None
* Return         : 1 - ��֤ͨ��
*                  0 - ��֤ʧ��
******************************************************************************/
extern UINT8 SM2Verification(unsigned char *mes, unsigned short klen, SM2_STU_PRAMS *pstuEccPrams, SM2_STU_PUBKEY *pstuPubKey, ECC_STU_BIGINT32 *pSignatureR, ECC_STU_BIGINT32 *pSignatureS);

/*******************************************************************************
* Function Name  : SM2VerificationWithIDA
* Description    : ʹ�ù�Կ�������Ϣ�����ĺ�ǩ��������֤��
*
*                 ��ע��
*                 1.�˺����ڲ�ʹ��CRYPTOģ�飬��Ҫ��ģ��ʱ��;
*                 2.�˺����ڲ�ʹ��SHAģ�飬��Ҫ��ģ��ʱ��;
*                 3.�˺����ڲ�ʹ��EDMACģ�飬��Ҫ��ģ��ʱ��;
*                 4.�˺����ڲ�ʹ��CRYPTOģ�飬��Ҫ��ģ��ʱ��;
*
* Input          : mes				    ����
*                  klen				    �����ֽڳ���
*                  IDA			              ǩ�����������Ϣ
*                  entla			   ǩ���������Ϣ���ֽڳ��ȣ���ע��ԭ����Ϊǩ���������Ϣ��bit���ȣ�
*                  pstuEccPrams		   ��Բ���߲���
*                  pstuPubKey		   ��Կ
*                  pSignatureR		   ���ĵ�ǩ����ϢR
*                  pSignatureS		   ���ĵ�ǩ����ϢS
* Output         : None
* Return         : 1 - ��֤ͨ��
*                  0 - ��֤ʧ��
******************************************************************************/
extern UINT8 SM2VerificationWithIDA(unsigned char *mes, unsigned short klen, unsigned char *IDA, unsigned short entla, SM2_STU_PRAMS *pstuEccPrams, SM2_STU_PUBKEY *pstuPubKey, ECC_STU_BIGINT32 *pSignatureR, ECC_STU_BIGINT32 *pSignatureS);

/*******************************************************************************
* Function Name  : sign
* Description    : ����û��������Ϣ
*
*                 ��ע��
*                 1.�˺����ڲ�ʹ��SHA��SM3��ģ�飬��Ҫ��ģ��ʱ��;
*                 2.�˺����ڲ�ʹ��EDMACģ�飬��Ҫ��ģ��ʱ��;
*
* Input          : IDA				           ǩ�����������Ϣ
*                  Entla                ǩ���������Ϣ���ֽڳ��ȣ���ע��ԭ��Ϊǩ���������Ϣ��bit���ȣ�
*                  pstuPubKey			��Կ
*                  pstuEccPrams		          ��Բ���߲���
*                  za				          ����û��������Ϣ
* Output         : None
* Return         : None
******************************************************************************/
extern void sign(unsigned char *IDA, unsigned short entla, SM2_STU_PUBKEY *pstuPubKey, SM2_STU_PRAMS *pstuEccPrams, unsigned int *za);

/*******************************************************************************
* Function Name  : SM2KeyExchange
* Description    : �����û�A���û�B����Կ
*
*                 ��ע��
*                 1.�˺����ڲ�ʹ��SHA��SM3��ģ�飬��Ҫ��ģ��ʱ��;
*                 2.�˺����ڲ�ʹ��EDMACģ�飬��Ҫ��ģ��ʱ��;
*                 3.�˺����ڲ�ʹ��CRYPTOģ�飬��Ҫ��ģ��ʱ��;
*
* Input          : pstuEccPrams			��Բ���߲���
*                  pSelfPrikey			�ѷ�˽Կ
*                  pSelfTempPubkey		�ѷ���ʱ��Կ
*                  pSelfTempPrikey		�ѷ���ʱ˽Կ
*                  pOtherPubkey			�Է���Կ
*                  pOtherTempPubkey		�Է���ʱ��Կ
*                  za				          �ѷ�Zֵ
*                  zb					�Է�Zֵ
*                  pAgreedKey			Э����Կ
*                  AgreedKeyLen			Э����Կ����
*                  sA					SA��S2
*                  sB					SB��S1
*                  Mode					0��ʾ�ѷ�ΪA����1��ʾ�ѷ�ΪB��
*
* Output         : None
* Return         : 1 - ��֤ͨ��
*                  0 - ��֤ʧ��
******************************************************************************/
extern UINT8 SM2KeyExchange(SM2_STU_PRAMS *pstuEccPrams, ECC_STU_BIGINT32 *pSelfPrikey, SM2_STU_PUBKEY *pSelfTempPubkey, ECC_STU_BIGINT32 *pSelfTempPrikey, SM2_STU_PUBKEY*pOtherPubkey,
		SM2_STU_PUBKEY *pOtherTempPubkey, UINT32 *za, UINT32 *zb, UINT32 *pAgreedKey, UINT32 AgreedKeyLen, UINT32 *sA, UINT32 *sB, UINT32 Mode);

/*******************************************************************************
* Function Name  : EccBig32Initial
* Description    : �����ĳ�ʼ��������ֵȫ��ֵΪ0�����鳤�ȼ�Ϊ1
*
*
* Input          : None
*
* Output         : pstuA            ָ��ṹ��ECC_STU_BIGINT��ָ�룬��ʼ�����pstuA
* Return         : None
******************************************************************************/
extern void EccBig32Initial(ECC_STU_BIGINT32 *pstuA);

/*******************************************************************************
* Function Name  : EccCharToHex
* Description    : �ַ�������ת����ΪHex��ʽ����
*
*
* Input          : pChar            ָ����Ҫת�����ַ������ݴ洢��ַ
*                  uLen             ת�����ݵĳ��ȣ���pChar���ݵ��ֽ���
* Output         : pHex             ָ��ת����Hex��ʽ���ݵĴ洢��ַ
* Return         : None
******************************************************************************/
extern void EccCharToHex(UINT8 *pChar, UINT8 *pHex, UINT16 uLen);

/*******************************************************************************
* Function Name  : EccByBufToBigInt32
* Description    : �����޷���8λ��������ת��Ϊ�ṹ��ECC_STU_BIGINT32�Ĵ�����ʽ
*                  ���磺����Ϊ8�����ݣ�
*                             pbyOutBuf[0]=0x55;
*							  pbyOutBuf[1]=0x66;
*							  pbyOutBuf[2]=0x77;
*							  pbyOutBuf[3]=0x88;
*                             pbyOutBuf[4]=0x11;
*							  pbyOutBuf[5]=0x22;
*							  pbyOutBuf[6]=0x33;
*							  pbyOutBuf[7]=0x44;
*                        ת��Ϊ ����Ϊ2�Ĵ��� ��
*                             pstuInBigInt->auValue[0]=0x11223344;
*  				              pstuInBigInt->auValue[1]=0x55667788;
*
* Input          : pbyInBuf         ָ���޷���8λ�ַ��������׵�ַ
*                  uInLen           �޷���8λ�ַ�������ĳ���
* Output         : pstuOutBigInt    ָ��ṹ��ECC_STU_BIGINT32�ĵ�ַ�����ת����Ķ�Ӧ����
* Return         : None
******************************************************************************/
void EccByBufToBigInt32(UINT8 *pbyInBuf,UINT16 uInLen, ECC_STU_BIGINT32 *pstuOutBigInt);

/*******************************************************************************
* Function Name  : TestKeyChange
* Description    : SM2������Կ���Ժ���
*
* Input          : pstuPrivKey_A      ����˽Կ
*                  pstuPubKey_A       ������Կ
*                  pstuPrivKey_B      �Է�˽Կ
*                  pstuPubKey_B       �Է���Կ
*                  pstuEccPrams       ECC����
*                  za                 ����zֵ
*                  zb                 �Է�zֵ
*
* Output         : None
* Return         : 1 - �ɹ�
*                  0 - ʧ��
******************************************************************************/
UINT8 TestKeyChange(ECC_STU_BIGINT32 *pstuPrivKey_A,SM2_STU_PUBKEY *pstuPubKey_A,ECC_STU_BIGINT32 *pstuPrivKey_B,SM2_STU_PUBKEY *pstuPubKey_B,SM2_STU_PRAMS *pstuEccPrams,unsigned int *za,unsigned int *zb);

/*******************************************************************************
* Function Name  : AB_Check
* Description    : �����û�֮��������
*
* Input          : s1
*                  sB
*                  sA
*                  s2
*                  pstuEccPrams     Ecc����
*
* Output         : None
* Return         : 1 - �ɹ�
*                  0 - ʧ��
******************************************************************************/
extern UINT8 AB_Check(UINT32 *s1, UINT32 *sB, UINT32 *sA, UINT32 *s2, SM2_STU_PRAMS *pstuEccPrams);

/*******************************************************************************
* Function Name  : SM2VerifySA
* Description    : USER_B verification USER_A
*
* Input          : s2
*                  sA
*                  pstuEccPrams     Ecc����
*
* Output         : None
* Return         : 1 - �ɹ�
*                  0 - ʧ��
******************************************************************************/
extern UINT8 SM2VerifySA(UINT32 *s2, UINT32 *sA, SM2_STU_PRAMS *pstuEccPrams);

/*******************************************************************************
* Function Name  : SM2GenerateKA
* Description    : USER_A verification USER_B , then Send choice(SA) to USER_B
*
* Input          : pstuEccPrams
*                  pstuPubKey_B
*                  pstuPrivKey_A
*                  za
*                  zb
*                  rA
*                  sB
*                  sA
*                  RA_x1
*                  RA_y1
*                  RB_x2
*                  RB_y2
*                  kA
*
* Output         : None
* Return         : 1 - �ɹ�
*                  0 - ʧ��
******************************************************************************/
extern UINT8 SM2GenerateKA(SM2_STU_PRAMS *pstuEccPrams, SM2_STU_PUBKEY *pstuPubKey_B, ECC_STU_BIGINT32 *pstuPrivKey_A, UINT32 *za, UINT32 *zb,
		ECC_STU_BIGINT32 *rA, UINT32 *sB, UINT32 *sA, ECC_STU_BIGINT32 *RA_x1, ECC_STU_BIGINT32 *RA_y1, ECC_STU_BIGINT32 *RB_x2, ECC_STU_BIGINT32 *RB_y2, UINT32 *kA);

/*******************************************************************************
* Function Name  : SM2GenerateRBKB
* Description    : USER_B response USER_A, then Send RB and choice(SB) to USER_A
*
* Input          : pstuEccPrams
*                  pstuPubKey_A
*                  pstuPrivKey_B
*                  sB
*                  s2
*                  za
*                  zb
*                  RA_x1
*                  RA_y1
*                  RB_x2
*                  RB_y2
*                  kB
*
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2GenerateRBKB(SM2_STU_PRAMS *pstuEccPrams, SM2_STU_PUBKEY *pstuPubKey_A, ECC_STU_BIGINT32 *pstuPrivKey_B, UINT32 *sB, UINT32 *s2, UINT32 *za, UINT32 *zb,
		ECC_STU_BIGINT32 *RA_x1, ECC_STU_BIGINT32 *RA_y1, ECC_STU_BIGINT32 *RB_x2, ECC_STU_BIGINT32 *RB_y2, UINT32 *kB);

/*******************************************************************************
* Function Name  : SM2GenerateRBKB
* Description    : USER_B response USER_A, then Send RB and choice(SB) to USER_A
*
* Input          : pstuEccPrams
*                  RA_x1
*                  RA_y1
*                  rA
*
* Output         : None
* Return         : None
******************************************************************************/
extern void SM2GenerateRA(SM2_STU_PRAMS *pstuEccPrams, ECC_STU_BIGINT32 *RA_x1, ECC_STU_BIGINT32 *RA_y1, ECC_STU_BIGINT32 *rA);

/*******************************************************************************
* Function Name  : RsaModuleEnable
* Description    : ʹ��RSAģ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void RsaModuleEnable(void);

/*******************************************************************************
* Function Name  : RsaModuleDisable
* Description    : ����RSAģ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void RsaModuleDisable(void);

/*******************************************************************************
* Function Name  : RsaGenerateKeyPair
* Description    : RSA ��Կ������
* Input          : mode       RSA�㷨λ����Ӧ��дֵΪ��1024��2048�ȡ�
*                  fixkey     ��Կ��ʽ
*                      0 - ��Կ���̶����������
*                      1 - ��Կ�̶�Ϊ0x10001
*                      2 - ��Կ�ⲿ����
*                  crt        RSA��Կ��ʽ
*                      0 - ��Կ��ʽΪ��ͨ��ʽ
*                      1 - ��Կ��ʽΪCRT��ʽ
* Output         : pub_key    �����Կ��ַ
*                  prv_key    ���˽Կ��ַ
* Return         : 0 - �ɹ�
*                  1 - ʧ��
******************************************************************************/
extern unsigned char RsaGenerateKeyPair( unsigned int mode, STU_RSA_PUBKEY * pub_key, STU_RSA_PRIVKEY * prv_key,unsigned char fixkey, unsigned char crt);

/*******************************************************************************
* Function Name  : RsaPubKey
* Description    : RSA ��Կ����
* Input          : input        �������ݵ�ַ
*                  inputLen     �������ݳ���
*                  publicKey    RSA��Կ��ַ
*
* Output         : output       ������ݵ�ַ
*                  outputLen    ������ݳ���

* Return         : 0   - �ɹ�
*                  ��0 - ʧ��(0x0401)
******************************************************************************/
extern int RsaPubKey(unsigned int *output, unsigned int *outputLen, unsigned int *input, unsigned int  inputLen, STU_RSA_PUBKEY *publicKey);

/*******************************************************************************
* Function Name  : RsaPubKey
* Description    : RSA ��Կ����
* Input          : input        �������ݵ�ַ
*                  inputLen     �������ݳ���
*                  privateKey   RSA˽Կ��ַ
*
* Output         : output       ������ݵ�ַ
*                  outputLen    ������ݳ���

* Return         : 0   - �ɹ�
*                  ��0 - ʧ��(0x0401)
******************************************************************************/
extern int RsaPrivKey(unsigned int *output, unsigned int *outputLen, unsigned int *input, unsigned int inputLen, STU_RSA_PRIVKEY * privateKey, ...);

/*******************************************************************************
* Function Name  : RsaPubKey
* Description    : RSA ��Կ����
* Input          : input         �������ݵ�ַ
*                  inputLen      �������ݳ���
*                  pprivateKey   RSA˽Կ��ַ����Կ��ʽΪCRT��ʽ��
*
* Output         : output        ������ݵ�ַ
*                  outputLen     ������ݳ���

* Return         : 0   - �ɹ�
*                  ��0 - ʧ��(0x0401)
******************************************************************************/
extern int RsaPrivKeyCRT(unsigned int *output, unsigned int *outputLen, unsigned int *input, unsigned int inputLen, STU_RSA_PRIVKEY * pprivateKey);

/****************************************************************************** 
***                            ����ΪӦ�õ��ýӿ�,�û����Ե���             ****
*******************************************************************************/

//�㷨�����ķ����ȼ������ȼ�
typedef enum {
    dpa_middle,	//�еȷ����ȼ�
    dpa_high,	//��߷����ȼ�
    dpa_low,	//��ͷ����ȼ�
    dpa_disable,//û�з���
}dpa_level_t;


#define SUCCESS_ALG			 1
#define FAIL_ALG			 0
#define ERROR_DATA_LENGTH	-1
#define ERROR_KEY_LENGTH	-2
#define ERROR_KEY_MODE		-3
#define ERROR_ENDE_MODE		-4
#define ERROR_ECBCBC_MODE	-5
#define ERROR_DATA_BUF		-6
#define ERROR_OTHER_PARA	-7

/*******************************************************************************
* Function Name  : Get_AlgLib_Version
* Description    : �㷨�汾��
* Input          : -NONE
* Output         : -version:�㷨�汾��
* Return         : return 1: success, 0: fail
******************************************************************************/
extern int Get_AlgLib_Version(UINT8*version);


/*******************************************************************************
* Function Name  : U8toU32
* Description    : BYTE����ˣ�ת��Ϊ�����ͣ�С�ˣ�����
* Input          : -src��Դ���ݵ�ַ
*                  -len���ֳ���
*                  -dst: Ŀ�ĵ�ַ
* Output         : None
* Return         : UINT16 �ֵĳ���
******************************************************************************/
extern U16 U8toU32(u8 *src_p,U16 len,U32 *dst_p);

/*******************************************************************************
* Function Name  : U32toU8
* Description    : �����ͣ�С�ˣ�ת��ΪBYTE����ˣ�����
* Input          : -src��Դ���ݵ�ַ
*                  -len���ֳ���
*                  -dst: Ŀ�ĵ�ַ
* Output         : None
* Return         : UINT16 byte���ֽڳ���
******************************************************************************/
extern U16 U32toU8(U32 *src_p,U16 len,U8 *dst_p,U8 append);

/*******************************************************************************
* Function Name  : des_dpa_ctrl
* Description    : DES����
* Input          : -level�������ȼ�middle high low disable
* Output         : None
* Return         : return 1: success, 0: fail
******************************************************************************/
extern int des_dpa_ctrl(dpa_level_t level);

/*******************************************************************************
* Function Name  : des_dpa_ctrl
* Description    : DES����
* Input          : -level�������ȼ�middle high low disable
* Output         : None
* Return         : return 1: success, 0: fail
******************************************************************************/
extern int aes_dpa_ctrl(dpa_level_t level);

/*******************************************************************************
* Function Name  : des_dpa_ctrl
* Description    : DES����
* Input          : -level�������ȼ�middle high low disable
* Output         : None
* Return         : return 1: success, 0: fail
******************************************************************************/
extern int sm4_dpa_ctrl(dpa_level_t level);

/*******************************************************************************
* Function Name  : AES_EnDecrypt_2
* Description    : AES�ӽ�������
* 				         ע�⣺
* 				    None
* Input          : -EncDe_Mode      ���ӽ���ģʽ
* 				   -CBC_ECB_MODE    ��ECB �� CBCģʽ
* 				   -key             :��Կ
* 				   -cbc_iv          :CBC��ʼ����
* 				   -key_mode        ����Կģʽ
* 				   -src_buf         :Դ����buf
* 				   -data_len        :Դ���ݳ���
* Output         : -out_buf         :�������buf
*
* Return         : ������
******************************************************************************/
extern int AES_EnDecrypt_2(UINT8 EncDe_Mode, 
													 UINT8 CTR_ECB_MODE,UINT8 *key, 
													UINT8 *ctr_iv,
													UINT32 key_mode, 
													UINT8 *src_buf, 
													UINT8 *out_buf, 
													UINT32 data_len);

/*******************************************************************************
* Function Name  : DES_EnDecrypt_2
* Description    : DES�ӽ�������
* 				         ע�⣺
* 				    None
* Input          : -EncDe_Mode      ���ӽ���ģʽ
* 				   -CBC_ECB_MODE    ��ECB �� CBCģʽ
* 				   -key             :��Կ
* 				   -key_len         :��Կ����
* 				   -iv              ����ʼ����
* 				   -src_buf         :Դ����buf
* 				   -data_len        :Դ���ݳ���
* 				   -check_parity    :�����key�Ƿ���Ҫ����У�飬���У������򷵻�keyУ������ټ����ӽ���
* Output         : -out_buf         :�������buf
*
* Return         : ������
******************************************************************************/
extern int DES_EnDecrypt_2(UINT8 EncDe_Mode, 
													 UINT8 ECB_CBC_Mode, 
													 UINT8 *key, 
													 UINT32 key_len, 
													 UINT8 *iv, 
													 UINT8 *src_buf, 
													 UINT8 *out_buf,
													 UINT32 data_len, 
													 UINT8 check_parity);

/*******************************************************************************
* Function Name  : SMS4_EnDecrypt_2
* Description    : SMS4�ӽ�������
* 				         ע�⣺
* 				    None
* Input          : -EncDe_Mode      ���ӽ���ģʽ
* 				   -CBC_ECB_MODE    ��ECB �� CBCģʽ
* 				   -key             :��Կ
* 				   -iv              ����ʼ����
* 				   -src_buf         :Դ����buf
* 				   -data_len        :������ݳ���
* Output         : -out_buf         :�������buf
*
* Return         : ������
******************************************************************************/
extern int SMS4_EnDecrypt_2(UINT8 EncDe_Mode,
														UINT8 ECB_CBC_Mode,
														UINT8 *key,
														UINT8 *iv,
														UINT8 *src_buf,
														UINT8 *out_buf,
														UINT32 data_len);

/*******************************************************************************
* Function Name  : SHA_Init_2
* Description    : HASH�����ʼ��
* 				         ע�⣺
* 				    None
* Input          : -phash_tx        ����ϣ�㷨���ݽṹģʽ
* 				   -HashModeNum    ����ϣ�㷨��ţ�ȡֵ��Χ0~4
* Output         : None
* Return         : ���
******************************************************************************/
extern int SHA_Init_2(Hash_tx *phash_tx, unsigned char HashModeNum);

/*******************************************************************************
* Function Name  : SHA_Update_2
* Description    : ����Ҫ����HASH���������
* 				         ע�⣺
* 				    None
* Input          : -phash_tx         ����ϣ�㷨���ݽṹģʽ
* 				   -pMessageBlock    ����Ϣ��
* 				   -DataLen          ����Ϣ�鳤��
* Output         : None
* Return         : ִ�н��
******************************************************************************/
extern int SHA_Update_2(Hash_tx *phash_tx, unsigned char *pMessageBlock, unsigned int DataLen);

/*******************************************************************************
* Function Name  : SHA_Final_2
* Description    :  ����HASH��������ȡ���
* 				         ע�⣺
* 				    None
* Input          : -phash_tx         ����ϣ�㷨���ݽṹģʽ
* Output         : -pRes             ��HASH������
* Return         : ִ�н��
******************************************************************************/
extern int SHA_Final_2(Hash_tx *phash_tx, unsigned char *pRes);

/*******************************************************************************
* Function Name  : SM2Init
* Description    : SM2ģ���ʼ��
* Input          : testbits �� SM2λ��
* Output         : None
* Return         : None
******************************************************************************/
extern int SM2Init(UINT16 testbits);

/*******************************************************************************
* Function Name  : SM2GenerateKeyPair_2
* Description    : SM2��Կ����
* Input          : None
* Output         : -pub_key         ����Կ
*                  -pri_key         ��˽Կ
* Return         : ������
******************************************************************************/
extern int SM2GenerateKeyPair_2(UINT8 *pub_key,UINT8 *pri_key);

/*******************************************************************************
* Function Name  : SM2Encrypt_2
* Description    : SM2����
* Input          : -mes             ������
*                  -len             :���ĳ���
*                  -pub_key         :��Կ
* Output         : -cipher          ������
* Return         : ������
******************************************************************************/
extern int SM2Encrypt_2(UINT8 *mes,UINT16 len,UINT8 *pub_key,UINT8 *cipher);

/*******************************************************************************
* Function Name  : SM2Decrypt_2
* Description    : SM2����
* Input          : -cipher          ������
*                  -len             :���ĳ���
*                  -pri_key         :˽Կ
* Output         : -mes             ������
* Return         : ������
******************************************************************************/
extern int SM2Decrypt_2(UINT8 *cipher,UINT16 len,UINT8 *pri_key,UINT8 *mes);

/*******************************************************************************
* Function Name  : SM2Signature_2
* Description    : SM2ǩ��
* Input          : -mes             ������
*                  -len             :���ĳ���
*                  -pri_key         :˽Կ
* Output         : -r               ��ǩ��ֵR��
*                  -s               :ǩ��ֵS��
* Return         : ������
******************************************************************************/
extern int SM2Signature_2(UINT8 *mes,UINT16 len,UINT8 *pri_key,UINT8 *r,UINT8 *s);

/*******************************************************************************
* Function Name  : SM2Verification_2
* Description    : SM2��ǩ
* Input          : -mes             ������
*                  -len             :���ĳ���
*                  -pri_key         :˽Կ
*                  -r               ��ǩ��ֵR��
*                  -s               :ǩ��ֵS��
* Output         : None
* Return         : ��ǩ���
******************************************************************************/
extern int SM2Verification_2(UINT8 *mes,UINT16 len,UINT8 *pub_key,UINT8 *r,UINT8 *s);

/*******************************************************************************
* Function Name  : SM2GenerateE_2
* Description    : SM2����Eֵ
* Input          : -IDA             ���û�ID
*                  -ida_len         :�û�ID����
*                  -pub_key         :��Կ
*                  -pMessage        ����Ϣ
*                  -mesLen          :��Ϣ����
* Output         : -res_E           :Eֵ
* Return         : ִ�н��
******************************************************************************/
extern int SM2GenerateE_2(UINT8 *IDA,UINT16 ida_len,UINT8 *pub_key,UINT8 *pMessage,UINT32 mesLen,UINT8 *res_E);

/*******************************************************************************
* Function Name  : SM2KeyExchange_2
* Description    : SM2��Կ����
* Input          : -self_pri        ������˽Կ
*                  -self_temp_pub   :������ʱ��Կ
*                  -self_temp_pri   :������ʱ˽Կ
*                  -other_pub       ���Է���Կ
*                  -other_temp_pub  :�Է���ʱ��Կ
*                  -za              ������Zֵ
*                  -zb              :�Է�Zֵ
*                  -AgreedKeyLen    ��Э����Կ�ĳ���
*                  -Mode			����Կ�������𷽡�0��ʾ�ѷ�ΪA����1��ʾ�ѷ�ΪB��
* Output         : -agree_key       :Э����Կ
*                  -sA              ������Sֵ
*                  -sB              :�Է�Sֵ
* Return         : �������
******************************************************************************/
extern int SM2KeyExchange_2(UINT8*self_pri,
														UINT8*self_temp_pub,
														UINT8 *self_temp_pri,
														UINT8*other_pub,
														UINT8*other_temp_pub,
														UINT8*za,
														UINT8*zb,
														UINT8*agree_key,
														UINT32 AgreedKeyLen,
														UINT8*sA,
														UINT8*sB,
														UINT32 Mode);

/*******************************************************************************
* Function Name  : sign_2
* Description    : SM2����Zֵ
* Input          : -IDA             ���û�ID
*                  -entla           :�û�ID����
*                  -pub_key         :��Կ
* Output         : -za              ��Zֵ
* Return         : ִ�н��
******************************************************************************/
extern int sign_2(UINT8 *IDA, UINT16 entla, UINT8 *pub_key,UINT8*za);


/*******************************************************************************
* Function Name  : ECCInit
* Description    : ECCģ���ʼ��
* Input          : testbits �� ECCλ��(Ŀǰ֧��256��521)
* Output         : None
* Return         : None
******************************************************************************/
extern int ECCInit(UINT16 testbits);

/*******************************************************************************
* Function Name  : ECDSA_Sign
* Description    : ʹ��˽Կ�����Ľ���ǩ����
*
*                 ��ע��
*                 1.�˺����ڲ�ʹ�������ģ�飬��Ҫ��ģ��ʱ��;
*                 2.�˺����ڲ�ʹ��EDAMCģ�飬��Ҫ��ģ��ʱ��;
*                 3.�˺����ڲ�ʹ��CRYPTOģ�飬��Ҫ���ģ��ʱ��;
*
* Input          : mes				         ����
*                  klen			             �����ֽڳ���
				   hashModeNum				 �Ӵ�����
*                  pstuPrivKey		  		 ˽Կ
				   pstuEccPrams	             ��Բ���߲���
* Output         : pSignatureR	             ������ĵ�ǩ����ϢR
*                  pSignatureS	             ������ĵ�ǩ����ϢS
* Return         : success   1
				 : fail      0
******************************************************************************/
extern UINT8 ECDSA_Sign(unsigned char *mes,unsigned short klen,unsigned char hashModeNum,ECC_STU_BIGINT32 *pstuPrivKey,ECC_STU_BIGINT32 *pSignatureR,ECC_STU_BIGINT32 *pSignatureS,SM2_STU_PRAMS *pstuEccPrams);

/*******************************************************************************
* Function Name  : ECDSA_Verify
* Description    : ʹ�ù�Կ�����ĺ�ǩ��������֤��
*
*                 ��ע��
*                 1.�˺����ڲ�ʹ��EDAMCģ�飬��Ҫ��ģ��ʱ��;
*                 2.�˺����ڲ�ʹ��CRYPTOģ�飬��Ҫ��ģ��ʱ��;
*
* Input          : mes				   ����
*                  klen				   �����ֽڳ���
				   hashModeNum		   �Ӵ�����
*                  pstuPubKey		   ��Կ
*                  pSignatureR		   ���ĵ�ǩ����ϢR
*                  pSignatureS		   ���ĵ�ǩ����ϢS
*                  pstuEccPrams		   ��Բ���߲���
* Output         : None
* Return         : 1 - ��֤ͨ��
*                  0 - ��֤ʧ��
******************************************************************************/
extern UINT8 ECDSA_Verify(unsigned char *mes, unsigned short klen, unsigned char hashModeNum,SM2_STU_PUBKEY *pstuPubKey, ECC_STU_BIGINT32 *pSignatureR, ECC_STU_BIGINT32 *pSignatureS,SM2_STU_PRAMS *pstuEccPrams);


/*******************************************************************************
* Function Name  : ECDSA_Sign_Z
* Description    : ʹ��˽Կ�����ĵ�hash����ǩ����
*
*                 ��ע��
*                 1.�˺����ڲ�ʹ�������ģ�飬��Ҫ��ģ��ʱ��;
*                 2.�˺����ڲ�ʹ��EDAMCģ�飬��Ҫ��ģ��ʱ��;
*                 3.�˺����ڲ�ʹ��CRYPTOģ�飬��Ҫ���ģ��ʱ��;
*
* Input          : hash				         ���ĵ�hash
*                  klen			           ����hash�ֽڳ���
				           hashModeNum				 �Ӵ�����
*                  pstuPrivKey		  	 ˽Կ
				           pstuEccPrams	       ��Բ���߲���
* Output         : pSignatureR	             ������ĵ�ǩ����ϢR
*                  pSignatureS	             ������ĵ�ǩ����ϢS
* Return         : success   1
				 : fail      0
******************************************************************************/
extern void ECDSA_Sign_Z(unsigned int *hash, unsigned short klen, ECC_STU_BIGINT32 *pstuPrivKey, ECC_STU_BIGINT32 *pSignatureR, ECC_STU_BIGINT32 *pSignatureS, SM2_STU_PRAMS *pstuEccPrams);


/*******************************************************************************
* Function Name  : ECDSA_Verify_Z
* Description    : ʹ�ù�Կ������hash��ǩ��������֤��
*
*                 ��ע��
*                 1.�˺����ڲ�ʹ��EDAMCģ�飬��Ҫ��ģ��ʱ��;
*                 2.�˺����ڲ�ʹ��CRYPTOģ�飬��Ҫ��ģ��ʱ��;
*
* Input          : hash				   ����hash
*                  klen				   ����hash�ֽڳ���
				           hashModeNum		   �Ӵ�����
*                  pstuPubKey		   ��Կ
*                  pSignatureR		   ���ĵ�ǩ����ϢR
*                  pSignatureS		   ���ĵ�ǩ����ϢS
*                  pstuEccPrams		   ��Բ���߲���
* Output         : None
* Return         : 1 - ��֤ͨ��
*                  0 - ��֤ʧ��
******************************************************************************/
extern UINT8 ECDSA_Verify_Z(unsigned int *hash, unsigned short klen, SM2_STU_PUBKEY *pstuPubKey, ECC_STU_BIGINT32 *pSignatureR, ECC_STU_BIGINT32 *pSignatureS, SM2_STU_PRAMS *pstuEccPrams);

#if 0
/*******************************************************************************
* Function Name  : ECC_Encrypt
* Description    : ECC����
* Input          : -mes					     ����������
*                  -klen        		 �����ݳ���
									 -pstuPubKey       ����Կ
									 -pstuEccPrams     ����Բ���߲���
* Output         : -cipher      		 ����������
* Return         : ִ�н��
******************************************************************************/
extern UINT8 ECC_Encrypt(unsigned char *mes, unsigned short klen, SM2_STU_PUBKEY *pstuPubKey, SM2_STU_PRAMS *pstuEccPrams, unsigned char *cipher);
/*******************************************************************************
* Function Name  : ECC_Encrypt
* Description    : ECC����
* Input          : -cipher      		 ����������
*                  -klen        		 �����ݳ���
									 -stuPrivKey       ��˽Կ
									 -pstuEccPrams     ����Բ���߲���
* Output         : -plain      		 	 ����������
* Return         : ִ�н��
******************************************************************************/
extern UINT8 ECC_Decrypt(unsigned char *cipher, unsigned short klen, ECC_STU_BIGINT32 *stuPrivKey, SM2_STU_PRAMS *pstuEccPrams, unsigned char *plain);
#endif
/*******************************************************************************
* Function Name  : rsa_keypair_gen
* Description    : RSA��Կ����
* Input          : -rsa_bit_len     ��RSA�㷨λ��
*                  -fixkey          :Eֵ�Ƿ�̶�
* Output         : -n_addr          ��N
*                  -e_addr          :E
*                  -d_addr          :D
*                  -p_addr          :P
*                  -q_addr          :Q
*                  -dp_addr         :dP
*                  -dq_addr         :dQ
*                  -qinv_addr       :PQ
* Return         : ִ�н��
******************************************************************************/
extern int rsa_keypair_gen(UINT32 rsa_bit_len, 
													 UINT8 fixkey, 
													 UINT8* n_addr, 
													 UINT8* e_addr, 
													 UINT8 *d_addr, 
													 UINT8* p_addr, 
													 UINT8* q_addr, 
													 UINT8* dp_addr, 
													 UINT8* dq_addr, 
													 UINT8* qinv_addr);

/*******************************************************************************
* Function Name  : RsaPubKey_2
* Description    : RSA��Կ����
* Input          : -fixkey          :Eֵ�Ƿ�̶�
*                  -n_addr          ��N
*                  -e_addr          :E
*                  -in              :��������
*                  -in_len          :�������ݳ���
* Output         : -out             ���������
*                  -out_len         ��������ݳ���
* Return         : ִ�н��
******************************************************************************/
extern int RsaPubKey_2(UINT8 fixkey, 
											 UINT8 *n_addr,
											 UINT8* e_addr, 
											 UINT8 *in,UINT32 in_len,
											 UINT8 *out,
											 UINT32 *out_len);

/*******************************************************************************
* Function Name  : RsaPrivKey_2
* Description    : RSA˽Կ����
* Input          :-n_addr          ��N
*                 -d_addr          :D
*                  -in              :��������
*                  -in_len          :�������ݳ���
* Output         : -out             ���������
*                  -out_len         ��������ݳ���
* Return         : ִ�н��
******************************************************************************/
extern int RsaPrivKey_2(UINT8*n_addr,UINT8 *d_addr,UINT8 *in,UINT32 in_len,UINT8 *out,UINT32 *out_len);

/*******************************************************************************
* Function Name  : RsaPrivKey_3
* Description    : RSA˽Կ����(DPA = High)
* Input          :-n_addr          ��N
*                 -d_addr          :D
*                  -in              :��������
*                  -in_len          :�������ݳ���
* Output         : -out             ���������
*                  -out_len         ��������ݳ���
* Return         : ִ�н��
******************************************************************************/
extern int RsaPrivKey_3(UINT8 *n_addr,
												UINT8 *d_addr,
												UINT8 *e_addr,
												UINT8 *in,
												UINT32 in_len,
												UINT8 *out,
												UINT32 *out_len);

/*******************************************************************************
* Function Name  : RsaPrivKeyCRT_2
* Description    : RSA CRTģʽ����
* Input          : -p_addr          :P
*                  -q_addr          :Q
*                  -dp_addr         :dP
*                  -dq_addr         :dQ
*                  -qinv_addr       :PQ
*                  -in              :��������
*                  -in_len          :�������ݳ���
* Output         : -out             ���������
*                  -out_len         :������ݳ���
* Return         : ִ�н��
******************************************************************************/
extern int RsaPrivKeyCRT_2(UINT8*p_addr,
                                                 UINT8*q_addr,
                                                 UINT8*dp_addr,
                                                 UINT8*dq_addr,
                                                 UINT8*qinv_addr,
                                                 UINT8 *in,
                                                 UINT32 in_len,
                                                 UINT8 *out,
                                                 UINT32 *out_len);

/*******************************************************************************
* Function Name  : GetRandomWord
* Description    : ��������ɺ���
* Input          : None
* Output         : None
* Return         : 1��UINT32�������
******************************************************************************/
UINT32 GetRandomWord(void);

/*******************************************************************************
* Function Name  : Get_Random_Word
* Description    : ��������ɺ���
* Input          : None
* Output         : None
* Return         : 1��UINT32�������
******************************************************************************/
UINT32 Get_Random_Word(void);

/*******************************************************************************
* Function Name  : Init_Trng
* Description    : ��������ܳ�ʼ��������
*                  ���������ģ��ʱ�ӣ������������ģ��ʱ��Ƶ��Ϊϵͳʱ��Ƶ�ʵ�1/8
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Init_Trng(void);

/*******************************************************************************
* Function Name  : Trng_Disable
* Description    : �ر������ģ��ʱ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Trng_Disable(void);
#endif /* ALG_DRV_H_ */
