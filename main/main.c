#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "usb_hub.h"
#include "esp_bluetooth.h"

void app_main()
{
    esp_bluetooth_init();
    xTaskCreate(usb_hub_task, "usb_hub_task", 41024, NULL, 0, NULL);

    while(1) {
        //printf("cnt: %d\n", cnt++);
        vTaskDelay(90000 / portTICK_RATE_MS);
    }
}

