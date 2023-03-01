/**************************************************************************//**
 * @file     system_ARMCM4.c
 * @brief    CMSIS Device System Source File for
 *           ARMCM4 Device Series
 * @version  V2.00
 * @date     18. August 2015
 ******************************************************************************/
/* Copyright (c) 2011 - 2015 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

#include <stdio.h>
#include "libRegOpt.h"

#if defined (ARMCM4)
  #include "ARMCM4.h"
#elif defined (ARMCM4_FP)
  #include "ARMCM4_FP.h"
#else
  #error device not specified!
#endif

///*----------------------------------------------------------------------------
//  Define clocks
// *----------------------------------------------------------------------------*/
//#define  XTAL            ( 1000000U)      /* Oscillator frequency             */

//#define  SYSTEM_CLOCK    (25 * XTAL)


///*----------------------------------------------------------------------------
//  System Core Clock Variable
// *----------------------------------------------------------------------------*/
//uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency      */


//void SystemCoreClockUpdate (void)
//{
//  SystemCoreClock = SYSTEM_CLOCK;
//}

extern void __Vectors(void);

void SystemInit (void)
{
#if (__FPU_USED == 1)
	SCB->CPACR |= ((3UL << 10*2) |                 /* set CP10 Full Access */
								 (3UL << 11*2)  );               /* set CP11 Full Access */
#endif

#ifdef UNALIGNED_SUPPORT_DISABLE
	SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
#endif
	
	#define  BOOT_SIZE                          (0x8000)
	SCB->VTOR = ((unsigned int)__Vectors|BOOT_SIZE) & 0xFFFFFE00;
	//SCB->VTOR = ((unsigned int)__Vectors) & 0xFFFFFE00;
	
//  SystemCoreClock = SYSTEM_CLOCK;
}

void LVD_EN(void)
{
//	CPM_PWRCR |= 0x0c;
	CPM_PWRCR_OptBits(0x0c,1);
}

void IO_Latch_Clr(void)
{
//	CPM_PWRCR = (CPM_PWRCR &~ 0x40000000) | 0x80000000;
//	CPM_PWRCR &=~ 0x80000000;
	CPM_PWRCR_OptBits(0x40000000,0);
	CPM_PWRCR_OptBits(0x80000000,1);
	CPM_PWRCR_OptBits(0x80000000,0);

}

void NMI_Handler(void)
{
	printf("warning:NMI Handler\r\n");
}

void HardFault_Handler(void)
{
		printf("warning:HardFault Handler\r\n");
		extern void Set_POR_Reset(void);
		Set_POR_Reset();
		while(1);   
}

void MemManage_Handler(void)
{
	printf("warning:MemManage Handler\r\n");
}

void BusFault_Handler(void)
{
	printf("warning:BusFault Handler\r\n");
}

void UsageFault_Handler(void)
{
	printf("warning:UsageFault Handler\r\n");
}

void SVC_Handler(void)
{
	printf("warning:SVC Handler\r\n");
}

void DebugMon_Handler(void)
{
	printf("warning:DebugMon Handler\r\n");
}

void PendSV_Handler(void)
{
	printf("warning:PendSV Handler\r\n");
}




