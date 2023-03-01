#ifndef _TRNG_DRV_H_
#define _TRNG_DRV_H_

#define TRNGIF_TLB_BAS              (TRNG_BASE_ADDR)
#define rTRNGIF_CTRL                (*(volatile unsigned int *)(TRNGIF_TLB_BAS + 0x00))
#define rTRNGIF_DATA                (*(volatile unsigned int *)(TRNGIF_TLB_BAS + 0x04))

#define TRNGIF_CTRL_EN_MASK               0x00000100
#define TRNGIF_CTRL_IE_MASK               0x00000200
#define TRNGIF_CTRL_CLR_MASK              0x00000400
#define TRNGIF_CTRL_STA_MASK              0x00000800

#define TRNG_ENABLE                       (1<<25)

/*******************************************************************************
* Function Name  : GetRandomWord
* Description    : ��������ɺ���
* Input          : None
* Output         : None
* Return         : 1��unsigned int�������
******************************************************************************/
extern unsigned int GetRandomWord(void);

/*******************************************************************************
* Function Name  : Init_Trng
* Description    : ��������ܳ�ʼ��������
*                  ���������ģ��ʱ�ӣ������������ģ��ʱ��Ƶ��Ϊϵͳʱ��Ƶ�ʵ�1/8
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void Init_Trng(void);

/*******************************************************************************
* Function Name  : Trng_Disable
* Description    : �ر������ģ��ʱ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void Trng_Disable(void);

#endif


