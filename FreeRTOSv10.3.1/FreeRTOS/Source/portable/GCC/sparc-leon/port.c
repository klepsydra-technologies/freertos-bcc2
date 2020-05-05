/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <drv/timer.h>
#include <stdio.h>

#ifndef configKERNEL_INTERRUPT_PRIORITY
	#define configKERNEL_INTERRUPT_PRIORITY 255
#endif

/* The priority used by the kernel is assigned to a variable to make access
from inline assembler easier. */
const unsigned long ulKernelPriority = configKERNEL_INTERRUPT_PRIORITY;

int  xPortScheduleHandler(struct leonbare_pt_regs *regs);

/* Each task maintains its own interrupt status in the critical nesting variable. */
static unsigned portBASE_TYPE uxCriticalNesting = 0xaaaaaaaa;

static struct timer_priv *tdev = NULL;
static void *sub = NULL;
static int counter = 0;

/* Counts the total number of times that the high frequency timer has 'ticked'.
This value is used by the run time stats function to work out what percentage
of CPU time each task is taking. */
volatile unsigned long ulHighFrequencyTimerTicks = 0UL;

/* Exception handlers. */
void xPortSysTickHandler( void *arg, int source );
void vPortSVCHandler( void );
/*
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
	unsigned int stk; struct freertos_stack *a;

	stk = (((unsigned int) pxTopOfStack) - ( FREERTOS_STACK_SIZE + SF_REGS_SZ + (SF_REGS_SZ * 2)) & ~7);

	a = (struct freertos_stack *)((unsigned int)stk + SF_REGS_SZ);
	a->psr = freeos_getpsr() & ~PSR_PIL;
	a->pc = ((unsigned int)pxCode)-8;
	a->o0 = (unsigned int)pvParameters;
	a->regs[8+6] = stk + SF_REGS_SZ; /* fp */

	return (portSTACK_TYPE *)stk;
}

typedef void tskTCB;
extern volatile tskTCB * volatile pxCurrentTCB;
extern int nestedirq;

/*
 * See header file for description.
 */
portBASE_TYPE xPortStartScheduler( void )
{
	int intnum;

#warning "no_inirq_check equivalent for BCC2?"
	//no_inirq_check = 1;
	//nestedirq = 0;

	portDISABLE_INTERRUPTS();

	bcc_timer_tick_init();

	timer_autoinit();

	tdev = timer_open(portTIMER_NUM);
	intnum = timer_get_devreg(portTIMER_NUM)->interrupt;

	bcc_isr_register(intnum, xPortSysTickHandler, NULL);
	bcc_int_unmask(intnum);

	sub = timer_sub_open(tdev, portTIMER_SUB);
	timer_set_ctrl(sub, GPTIMER_CTRL_IP);
	timer_set_reload(sub, portTIMER_RELOAD);
	timer_set_ctrl(sub, GPTIMER_CTRL_IE | GPTIMER_CTRL_LD | GPTIMER_CTRL_RS | GPTIMER_CTRL_EN);

	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;

	portENABLE_INTERRUPTS();

	/* Start the first task. */
	FreeRtosSwitchTo(pxCurrentTCB);

	/* Should not get here! */
	return 0;
}

void vPortEndScheduler( void )
{
	/* It is unlikely that the CM3 port will require this function as there
	is nothing to return to.  */
}

void vPortEnterCritical( void )
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	uxCriticalNesting--;
	if( uxCriticalNesting == 0 )
	{
		portENABLE_INTERRUPTS();
	}
}

void xPortSysTickHandler( void *arg, int source )
{
	unsigned long ulDummy, psr;

	timer_set_ctrl(sub, timer_get_ctrl(sub) | GPTIMER_CTRL_IP);

	psr = freeos_getpsr();
	if ((psr & PSR_PIL) != PSR_PIL){
		freeos_error();
	}

	ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		ulHighFrequencyTimerTicks++;

#warning "What to do if xTaskIncrementTick returns pdFALSE?"
		if (counter == 0) {
			xTaskIncrementTick();
		}
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( ulDummy );

#if configUSE_PREEMPTION == 1
	if (counter == 0) {
		vPortYieldFromISR();
	}
#endif

	counter++;
	counter %= 10;
}

int  xPortScheduleHandler(struct leonbare_pt_regs *regs) {
	return 0;
}

__attribute__((weak)) void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed portCHAR *pcTaskName ) {
	printf("Fail vApplicationStackOverflowHook\n");
	for( ;; );
}

__attribute__((weak)) void vApplicationTickHook( void ) {

}

__attribute__((weak)) void vApplicationMallocFailedHook( void ) {
	printf("Fail vApplicationMallocFailedHook\n");
	for( ;; );
}

