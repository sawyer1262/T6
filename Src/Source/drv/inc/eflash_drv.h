// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : eflash_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef __EFLASH_DRV_H__
#define __EFLASH_DRV_H__

#include "type.h"
#include "memmap.h"

#define EFCR	EFM_BASE_ADDR

#define EFLASH_BASE				          (0x8000000)
#define EFLASH_PAGE_SIZE_BYTE       (0x200)		//512
#define EFLASH_PAGE_SIZE_HFWORD     (0x100)		//256
#define EFLASH_PAGE_SIZE_WORD   	  (0x80)		//128
#define EFLASH_BYTE_SIZE	           (0x80000)	//512kbytes, 128k words
#define EFLASH_ALL_PAGE_LEN         (1024)      //1024page
#define PAGE_MASK                   (EFLASH_PAGE_SIZE_BYTE-1)

#define EFLASH_WRITEABLE	  TRUE
#define EFLASH_READABLE	    TRUE
#define EFLASH_DISWRITEABLE	FALSE
#define EFLASH_DISREADABLE	FALSE

#define EFM0_MAIN_BASEADDR	(0x08000000)
#define EFM0_MAIN_LEN				(0x00080000)

#define EFLASH_PAGE_SIZE_BYTE			(0x200)
#define EFLASH_PAGE_SIZE_HFWORD   (0x100)		//256
#define EFLASH_PAGE_SIZE_WORD   	(0x80)		//128

#define EFLASH_OP_PASS		0
#define EFLASH_OP_FAIL		1
#define EFLASH_PARA_ERR		2

/*******************************************************************************
* Function Name  : EFlash_Disboot
* Description    : disboot
* Input          : - None
*
* Output         : None
* Return         : TRUE or FALSE
******************************************************************************/
extern UINT8 EFlash_Disboot(void);

/*******************************************************************************
* Function Name  : EFlash_Recovery_to_Boot
* Description    : �ص�ROM BOOT״̬
* Input          : - None
*
* Output         : None
* Return         : TRUE or FALSE
******************************************************************************/
extern UINT8 EFlash_Recovery_to_Boot(void);

/*******************************************************************************
* Function Name  : EFLASH_Set_RWSC
* Description    : ����EFLASH RWSCֵ
* Input          : -rwsc: rwscֵ��Ĭ��Ϊ0x07
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_Set_RWSC(UINT8 rwsc);

/*******************************************************************************
* Function Name  : EFLASH_Init
* Description    : EFLASH��ʼ��
* Input          : - f_sys_khz: ϵͳʱ�ӣ���λkhz��
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_Init(UINT32 sys_clk_khz);

/*******************************************************************************
* Function Name  : EFLASH_SetWritePermission
* Description    : ����EFLASH�׿����
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_SetWritePermission(void);

/*******************************************************************************
* Function Name  : EFLASH_ClrWritePermission
* Description    : �ر�EFLASH�׿����
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_ClrWritePermission(void);

/*******************************************************************************
* Function Name  : EFLASH_ByteRead
* Description    : ��EFLASHĿ���ַ��ȡһ��byte����
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : ��ȡ��byte����
******************************************************************************/
extern UINT8   EFLASH_ByteRead(UINT32 addr);

/*******************************************************************************
* Function Name  : EFLASH_HalfWordRead
* Description    : ��EFLASHĿ���ַ��ȡһ��half word����
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : ��ȡ��half word����
******************************************************************************/
extern UINT16  EFLASH_HalfWordRead(UINT32 addr);

/*******************************************************************************
* Function Name  : EFLASH_WordRead
* Description    : ��EFLASHĿ���ַ��ȡһ��word����
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : ��ȡ��word����
******************************************************************************/
extern UINT32  EFLASH_WordRead(UINT32 addr);

/*******************************************************************************
* Function Name  : EFLASH_WordsRead
* Description    :  ��EFLASHĿ���ַ��ȡ����Ϊlen��word����
* Input          : - addr: EFLASH destination address
*                  - len:  ��Ҫ��ȡword���ݵĳ���
*
* Output         : - des:  ��ȡ��word���ݻ���
* Return         : None
******************************************************************************/
extern void    EFLASH_WordsRead(UINT32 addr, UINT32 *des, UINT32 len);

/*******************************************************************************
* Function Name  : EFLASH_PageErase
* Description    : EFLASHҳ����
* Input          : - addr: Ҫ������EFLASHҳ��ַ
*
* Output         : None
* Return         :  0�������ɹ�   1������ʧ��
******************************************************************************/
extern UINT8   EFLASH_PageErase(UINT32 addr);

/*******************************************************************************
* Function Name  : EFLASH_WordProg
* Description    : EFLASH�ĵ�word���
* Input          : - addr: Ҫ��̵�EFLASH��ַ
*                  - dat:  ��̵�word����
*
* Output         : None
* Return         :  0����̳ɹ�   1�����ʧ��
******************************************************************************/
extern UINT8 EFLASH_WordProg(UINT32 addr, UINT32 dat);

/*******************************************************************************
* Function Name  : EFLASH_WordsProg
* Description    :  EFLASH�Ķ�word���
* Input          : - addr: Ҫ��̵�EFLASH��ַ
*                  - des:  ��̵�word����ָ��
*                  - len:  Ҫ��̵�word���ݳ���
*
* Output         : None
* Return         :  0����̳ɹ�   1�����ʧ��
******************************************************************************/
extern UINT8   EFLASH_WordsProg(UINT32 addr, UINT32 *des, UINT32 len);

/*******************************************************************************
* Function Name  : EFLASH_Write
* Description    :  EFLASH�����ַд����
* Input          : - flash_addr: Ŀ�ĵ�ַ
*                  - ram_addr:  Դ��ַָ��
*                  - len:  ���ݳ���
*
* Output         : None
* Return         :  TRUE����̳ɹ�   FALSE�����ʧ��
******************************************************************************/
UINT8 EFLASH_Write(UINT32 flash_addr, const UINT8* ram_addr, UINT32 len);

#endif // end of __EFLASH_DRV_H__
