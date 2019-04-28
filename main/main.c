#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_wifi.h"

#include "usb_hub.h"
#include "esp_bluetooth.h"
#include "adb_device.h"

#include "sha1withrsa.h"

//uint8_t output_buffer[4096];

void app_main()
{
    //uint8_t test_in[] = {0x82,0x8d,0x99,0xb9,0x0b,0x90,0x55,0xd5,0x11,0x8d,0xd6,0x2e,0xac,0x05,0x1f,0x33,0x0b,0x24,0x6f,0xa9};

    esp_bluetooth_init();

    //xTaskCreate(bt_send_task, "bt_send_task", 4*1024, NULL, 1, NULL);
    xTaskCreate(usb_hub_task, "usb_hub_task", 16*1024, NULL, 6, NULL);


    //SHA1withRSA(test_in,sizeof(test_in),output_buffer);

    while(1) {
        //printf("cnt: %d\n", cnt++);
        vTaskDelay(90000 / portTICK_RATE_MS);
    }
}

