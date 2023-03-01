// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : cache_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __CACHE_DRV_C__
#define __CACHE_DRV_C__

#include "type.h"
#include "memmap.h"
#include "cache_reg.h"


#define ICACHE		    ((CACHE_TypeDef *)(CACHE_BASE_ADDR))
#define DCACHE		    ((CACHE_TypeDef *)(CACHE2_BASE_ADDR))

typedef enum{
	cacheOff = 0,
	cacheThrough,
	cacheBack,
}cache_com;

/*******************************************************************************
* Function Name  : ICACHE_Init
* Description    : CACHE��ʼ��
* Input          :   -CACHEx :CACHE�Ļ���ַ
*											- rom: The attributes of rom memory regions��
*                          ȡֵcacheThrough, cacheBack or cacheOff
*                   - boot��The attributes of boot memory regions
*                          ȡֵcacheThrough, cacheBack or cacheOff
*                   - spim1�� The attributes of spim1 memory regions
*                          ȡֵcacheThrough, cacheBack or cacheOff
*                   - eflash�� The attributes of eflash memory regions
*                          ȡֵcacheThrough, cacheBack or cacheOff
* Output         : None
*
* Return         : None
* Note			  ���ϵ��ִ��һ�Σ������ظ�ִ�С�
******************************************************************************/
extern void ICACHE_Init(cache_com spim1,  cache_com eflash, cache_com rom, cache_com boot);

/*******************************************************************************
* Function Name  : DCACHE_Init
* Description    : CACHE��ʼ��
* Input          :   -CACHEx :CACHE�Ļ���ַ
*											- rom: The attributes of rom memory regions��
*                          ȡֵcacheThrough, cacheBack or cacheOff
*                   - boot��The attributes of boot memory regions
*                          ȡֵcacheThrough, cacheBack or cacheOff
*                   - spim1�� The attributes of spim1 memory regions
*                          ȡֵcacheThrough, cacheBack or cacheOff
*                   - eflash�� The attributes of eflash memory regions
*                          ȡֵcacheThrough, cacheBack or cacheOff
* Output         : None
*
* Return         : None
* Note			  ���ϵ��ִ��һ�Σ������ظ�ִ�С�
******************************************************************************/
extern void DCACHE_Init(cache_com spim1,  cache_com eflash, cache_com rom, cache_com boot);

#endif /* __CACHE_DRV_C__ */
