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

SemaphoreHandle_t s;
void lambda_calling_thread(void *pvParameters) {
    cb callback = reinterpret_cast<cb>(pvParameters);

    printf("lambda_calling_thread\n");

    printf("pvParameters: %p\n", pvParameters);
    printf("callback: %p\n", callback);

    callback();

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
        tskIDLE_PRIORITY + 1,
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

