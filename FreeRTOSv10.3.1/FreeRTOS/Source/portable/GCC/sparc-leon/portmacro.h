/*
    FreeRTOS V6.0.5 - Copyright (C) 2010 Real Time Engineers Ltd.

    ***************************************************************************
    *                                                                         *
    * If you are:                                                             *
    *                                                                         *
    *    + New to FreeRTOS,                                                   *
    *    + Wanting to learn FreeRTOS or multitasking in general quickly       *
    *    + Looking for basic training,                                        *
    *    + Wanting to improve your FreeRTOS skills and productivity           *
    *                                                                         *
    * then take a look at the FreeRTOS eBook                                  *
    *                                                                         *
    *        "Using the FreeRTOS Real Time Kernel - a Practical Guide"        *
    *                  http://www.FreeRTOS.org/Documentation                  *
    *                                                                         *
    * A pdf reference manual is also available.  Both are usually delivered   *
    * to your inbox within 20 minutes to two hours when purchased between 8am *
    * and 8pm GMT (although please allow up to 24 hours in case of            *
    * exceptional circumstances).  Thank you for your support!                *
    *                                                                         *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/


#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "leonstack.h"

#ifndef __ASSEMBLER__
#include <bcc/bcc.h>

struct freertos_stack {
	unsigned long regs[8*2];
	unsigned long psr;
	unsigned long wim;
	unsigned long pc;
	unsigned long o0;
};
#endif
#define FREERTOS_STACK_SIZE (4*(8*2+4))
#define FREERTOS_STACK_PSR_OFF (4*(8*2))
#define FREERTOS_STACK_WIM_OFF (4*(8*2+1))
#define FREERTOS_STACK_PC_OFF  (4*(8*2+2))
#define FREERTOS_STACK_O0_OFF  (4*(8*2+3))


/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	unsigned portLONG
#define portBASE_TYPE	long

#ifndef __ASSEMBLER__
typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
#endif

#if( configUSE_16_BIT_TICKS == 1 )
#ifndef __ASSEMBLER__
	typedef uint16_t TickType_t;
#endif
	#define portMAX_DELAY ( TickType_t ) 0xffff
#else
#ifndef __ASSEMBLER__
	typedef uint32_t TickType_t;
#endif
	#define portMAX_DELAY ( TickType_t ) 0xffffffff
#endif
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT			8
/*-----------------------------------------------------------*/

#define portTIMER_NUM				0
#define portTIMER_SUB				5
#define portTIMER_RELOAD			500

/* Scheduler utilities. */
#ifndef __ASSEMBLER__
extern void vPortYieldFromISR( void );
#endif

#define portYIELD()					vPortYieldFromISR()

#define portEND_SWITCHING_ISR( xSwitchRequired ) if( xSwitchRequired ) vPortYieldFromISR()
/*-----------------------------------------------------------*/


/* Critical section management. */

#define portSET_INTERRUPT_MASK_FROM_ISR()	bcc_int_disable()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)	bcc_int_enable(x);(void)x


#ifndef __ASSEMBLER__
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
#endif

#define portDISABLE_INTERRUPTS()	bcc_int_disable()
#define portENABLE_INTERRUPTS()		bcc_int_enable(0)

#ifndef __ASSEMBLER__
void vTaskEnterCritical( void );
void vTaskExitCritical( void );
#endif
#define portENTER_CRITICAL()			vTaskEnterCritical()
#define portEXIT_CRITICAL()				vTaskExitCritical()

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

#define portNOP()

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

