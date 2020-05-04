#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define rx_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE )
#define tx_TASK_STACK_SIZE		( configMINIMAL_STACK_SIZE )
#define mainCREATOR_TASK_PRIORITY       ( tskIDLE_PRIORITY + 3 )

void vRxTask( void *pvParameters ) {
	while (1) {
		printf("rx 0x%x\n", freeos_getsp());
		vTaskDelay( 10 );
	}
}

void vTxTask( void *pvParameters ) {
	while (1) {
		printf("tx 0x%x\n",freeos_getsp());
		vTaskDelay( 10 );
	}
}

int main() {

	xTaskCreate( vRxTask, ( signed portCHAR * ) "rx", rx_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );
	xTaskCreate( vTxTask, ( signed portCHAR * ) "tx", tx_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, NULL );

	/* The suicide tasks must be created last as they need to know how many
	   tasks were running prior to their creation in order to ascertain whether
	   or not the correct/expected number of tasks are running at any given time. */
	vCreateSuicidalTasks( mainCREATOR_TASK_PRIORITY );

	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1) {

	}
}

/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide an
 * implementation of vApplicationGetIdleTaskMemory() to provide the memory that
 * is used by the Idle task. */
void vApplicationGetIdleTaskMemory(	StaticTask_t ** ppxIdleTaskTCBBuffer,
									StackType_t ** ppxIdleTaskStackBuffer,
									uint32_t * pulIdleTaskStackSize )
{
	/* If the buffers to be provided to the Idle task are declared inside this
	 * function then they must be declared static - otherwise they will be
	 * allocated on the stack and so not exists after this function exits. */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( 32 ) ) );

	/* Pass out a pointer to the StaticTask_t structure in which the Idle
	 * task's state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
/*-----------------------------------------------------------*/

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
 * application must provide an implementation of vApplicationGetTimerTaskMemory()
 * to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
									 StackType_t ** ppxTimerTaskStackBuffer,
									 uint32_t * pulTimerTaskStackSize )
{
	/* If the buffers to be provided to the Timer task are declared inside this
	 * function then they must be declared static - otherwise they will be
	 * allocated on the stack and so not exists after this function exits. */
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ] __attribute__( ( aligned( 32 ) ) );

	/* Pass out a pointer to the StaticTask_t structure in which the Timer
	 * task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

