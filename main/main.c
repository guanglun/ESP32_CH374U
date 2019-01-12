#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "ch374u_app.h"


static void gpio_task_example(void* arg)
{
    ch374u_loop();
}

void app_main()
{

    ch374u_init();

    xTaskCreate(gpio_task_example, "gpio_task_example", 8*1024, NULL, 10, NULL);

    while(1) {
        //printf("cnt: %d\n", cnt++);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

