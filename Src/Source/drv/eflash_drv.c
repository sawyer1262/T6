// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : eflash_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "common.h"
#include "libEflash.h"
#include "eflash_drv.h"
#include <string.h>

UINT32  eflash_buffer[EFLASH_PAGE_SIZE_BYTE/4];

/*******************************************************************************
* Function Name  : EFLASH_Set_RWSC
* Description    : ����EFLASH RWSCֵ
* Input          : -rwsc: rwscֵ��Ĭ��Ϊ0x07
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_Set_RWSC(UINT8 rwsc)
{
	UINT32 efcr_reg = readl(EFCR);
	
	efcr_reg &= (~0x0000000F);
	efcr_reg |= (rwsc & 0x0F);
	
	writel(EFCR,efcr_reg);
}

/*******************************************************************************
* Function Name  : EFLASH_Init
* Description    : EFLASH��ʼ��
* Input          : - f_sys_khz: ϵͳʱ�ӣ���λkhz��
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_Init(UINT32 sys_clk_khz)
{
	EFM_Init(sys_clk_khz);
}

/*******************************************************************************
* Function Name  : EFLASH_SetWritePermission
* Description    : ����EFLASH�׿����
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_SetWritePermission(void)
{
	EFlash_Set_Main_Permission(EFLASH_WRITEABLE, EFLASH_READABLE);
}

/*******************************************************************************
* Function Name  : EFLASH_ClrWritePermission
* Description    : �ر�EFLASH�׿����
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void EFLASH_ClrWritePermission(void)
{
	EFlash_Set_Main_Permission(EFLASH_DISWRITEABLE, EFLASH_READABLE);
}

/*******************************************************************************
* Function Name  : EFLASH_ByteRead
* Description    : ��EFLASHĿ���ַ��ȡһ��byte����
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : ��ȡ��byte����
******************************************************************************/
UINT8 EFLASH_ByteRead(UINT32 addr)
{
	return (*(volatile UINT8 *)(addr));
}

/*******************************************************************************
* Function Name  : EFLASH_HalfWordRead
* Description    : ��EFLASHĿ���ַ��ȡһ��half word����
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : ��ȡ��half word����
******************************************************************************/
UINT16 EFLASH_HalfWordRead(UINT32 addr)
{
	return (*(volatile UINT16 *)(addr));
}

/*******************************************************************************
* Function Name  : EFLASH_WordRead
* Description    : ��EFLASHĿ���ַ��ȡһ��word����
* Input          : - addr: EFLASH destination address
*
* Output         : None
* Return         : ��ȡ��word����
******************************************************************************/
UINT32 EFLASH_WordRead(UINT32 addr)
{
	return (*(volatile UINT32 *)(addr));
}

/*******************************************************************************
* Function Name  : EFLASH_WordsRead
* Description    :  ��EFLASHĿ���ַ��ȡ����Ϊlen��word����
* Input          : - addr: EFLASH destination address
*                  - len:  ��Ҫ��ȡword���ݵĳ���
*
* Output         : - des:  ��ȡ��word���ݻ���
* Return         : None
******************************************************************************/
void EFLASH_WordsRead(UINT32 addr, UINT32 *des, UINT32 len)
{
	UINT32 i;

	for(i = 0; i < len; i ++)
	{
		des[i] = (*(volatile UINT32 *)(addr+i*4));
	}
}

/*******************************************************************************
* Function Name  : EFLASH_PageErase
* Description    : EFLASHҳ����
* Input          : - addr: Ҫ������EFLASHҳ��ַ
*
* Output         : None
* Return         :  TRUE�������ɹ�   FALSE������ʧ��
******************************************************************************/
UINT8 EFLASH_PageErase(UINT32 addr)
{
	if(EFlash_Page_Erase(addr))
	{
		return FALSE;
	}
	return TRUE;
}

/*******************************************************************************
* Function Name  : EFLASH_WordProg
* Description    : EFLASH�ĵ�word���
* Input          : - addr: Ҫ��̵�EFLASH��ַ
*                  - dat:  ��̵�word����
*
* Output         : None
* Return         :  TRUE����̳ɹ�   FALSE�����ʧ��
******************************************************************************/
UINT8 EFLASH_WordProg(UINT32 addr, UINT32 dat)
{
	if(EFlash_Bulk_Program((addr), 1, &dat))
	{
		return FALSE;
	}
	return TRUE;
}

/*******************************************************************************
* Function Name  : EFLASH_WordsProg
* Description    :  EFLASH�Ķ�word���
* Input          : - addr: Ҫ��̵�EFLASH��ַ
*                  - des:  ��̵�word����ָ��
*                  - len:  Ҫ��̵�word���ݳ���
*
* Output         : None
* Return         :  TRUE����̳ɹ�   FALSE�����ʧ��
******************************************************************************/
UINT8 EFLASH_WordsProg(UINT32 addr, UINT32 *des, UINT32 len)
{
	if(EFlash_Bulk_Program((addr), len, des))
	{
		return FALSE;
	}

	return TRUE;
}

/*******************************************************************************
* Function Name  : FLASH_checkAddr
* Description    :  EFLASH��ַ���
* Input          : - flash_addr: Ҫ��̵�EFLASH��ַ
*                  - len:  Ҫ��̵�byte���ݳ���
*
* Output         : None
* Return         :  TRUE����̳ɹ�   FALSE�����ʧ��
******************************************************************************/
static UINT8 FLASH_checkAddr(UINT32 flash_addr,UINT32 len)
{
	if((flash_addr%4) != 0)
	{
		return FALSE;
	}
	if((flash_addr < EFLASH_BASE) || (flash_addr+len) > (EFLASH_BASE + EFLASH_BYTE_SIZE - 0x1000))
	{
		return FALSE;
	}

	return TRUE;
}

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
UINT8 EFLASH_Write(UINT32 flash_addr, const UINT8* ram_addr, UINT32 len)
{
	UINT16 offset;
	UINT16 pageleft;
	UINT16 pageright;
	UINT32 dest = flash_addr;
	UINT8* src  = (UINT8*)ram_addr;
	UINT8 result;
	
	pageright = pageright;

	if (FLASH_checkAddr(flash_addr,len) == FALSE)
	{
		return FALSE;
	}

	offset  = ((UINT32)dest & PAGE_MASK);
	pageleft  = EFLASH_PAGE_SIZE_BYTE - offset;
	pageright = ((UINT16)(dest + len) & PAGE_MASK);

	/*���ҳû�ж��룬���ȶԵ�һҳ����*/
	if (offset != 0)
	{
		dest -= offset;/*destָ����ת����ǰҳͷ*/
		memcpy((UINT8*)eflash_buffer, (UINT8*)dest, EFLASH_PAGE_SIZE_BYTE);
		if (len>pageleft)
		{
			memcpy((UINT8*)eflash_buffer + offset, src, pageleft);
			len  -= pageleft;
			src  += pageleft;
		}
		else
		{
			memcpy((UINT8*)eflash_buffer + offset, src, len);
			len=0;
		}
		result = EFLASH_PageErase(dest);
		result = EFLASH_WordsProg(dest, eflash_buffer, EFLASH_PAGE_SIZE_BYTE/4);
		dest += EFLASH_PAGE_SIZE_BYTE;
		//offset = 0;
	}
	while (len > 0)
	{
		/*��������м�ҳ*/
		if (len >= EFLASH_PAGE_SIZE_BYTE)
		{
			memcpy((UINT8*)eflash_buffer, src, EFLASH_PAGE_SIZE_BYTE);
			result = EFLASH_PageErase(dest);
			result = EFLASH_WordsProg(dest, eflash_buffer, EFLASH_PAGE_SIZE_BYTE/4);

			dest += EFLASH_PAGE_SIZE_BYTE;
			src  += EFLASH_PAGE_SIZE_BYTE;
			len  -= EFLASH_PAGE_SIZE_BYTE;
		}
		else
		{   /*�����ҳ����*/
			memcpy((UINT8*)eflash_buffer, (UINT8*)dest, EFLASH_PAGE_SIZE_BYTE);
			memcpy((UINT8*)eflash_buffer, src, len);
			result = EFLASH_PageErase(dest);
			result = EFLASH_WordsProg(dest, eflash_buffer, EFLASH_PAGE_SIZE_BYTE/4);
			len = 0;
		}
	}
    
    return result;
}
