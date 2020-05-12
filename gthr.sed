s@#include <bits/gthr-default.h>@#ifdef _GCC_MULTITHREAD_FREERTOS_ENABLE \
#include "gthr-FreeRTOS.h" \
#else \
#include <bits/gthr-default.h> \
#endif@g

