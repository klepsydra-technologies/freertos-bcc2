/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <iostream>
#include <functional>

#include "posix_demo.h"

static const int num_threads = 2;

extern "C" void lambda_calling_thread(void *pvParameters);

typedef void (*cb)();

double get_cpu_usage() {
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    unsigned long ulTotalRunTime, ulStatsAsPercentage;
    TaskHandle_t idle_task_handle;
    double total_run_time;
    double cpu_usage = 0.0;

    idle_task_handle = xTaskGetIdleTaskHandle();

    uxArraySize = uxTaskGetNumberOfTasks();
    pxTaskStatusArray = (TaskStatus_t*)pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    if (pxTaskStatusArray != NULL) {
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);
        total_run_time = ulTotalRunTime / 100.0;

        if (total_run_time > 0.0) {
            for(x = 0; x < uxArraySize; x++) {
                if (pxTaskStatusArray[x].xHandle == idle_task_handle) {
                    cpu_usage = 100.0 - (pxTaskStatusArray[x].ulRunTimeCounter / total_run_time);
                    break;
                }
            }
        } else {
            cpu_usage = 100.0;
        }

        vPortFree(pxTaskStatusArray);
    }

    return cpu_usage;
}

SemaphoreHandle_t s;
void lambda_calling_thread(void *pvParameters) {
    double cpu_usage;
    cb callback = reinterpret_cast<cb>(pvParameters);

    printf("lambda_calling_thread\n");

    printf("pvParameters: %p\n", pvParameters);
    printf("callback: %p\n", callback);

    callback();

    vTaskDelay(1);

    cpu_usage = get_cpu_usage();
    printf("CPU Usage: %0.1lf%%\n", cpu_usage);

    xSemaphoreGive(s);
}

static void launch_thread() {
    //std::function<void()> helloWorldLambda = [](){printf("Hello world! from thread\n");};
    auto helloWorldLambda = [](){printf("Hello world! from thread\n");};
    cb x = helloWorldLambda;

    printf("helloWorldLambda: %p\n", &helloWorldLambda);
    printf("x: %p\n", x);

    xTaskCreate(
        lambda_calling_thread,
        "task_name",
        configMINIMAL_STACK_SIZE,
        reinterpret_cast<void*>(x),
        tskIDLE_PRIORITY + 3,
        NULL
    );
}

void vStartPOSIXDemo(void *pvParameters) {
    s = xSemaphoreCreateBinary();
    xSemaphoreGive(s);

    xSemaphoreTake(s, portMAX_DELAY);

    // Create the lambda in a separate function, so that
    // we are sure that when we're waiting for the other
    // thread to finish, the lambda object has been destroyed
    launch_thread();

    xSemaphoreTake(s, portMAX_DELAY);

    exit(0);
}

