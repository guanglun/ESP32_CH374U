#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "log.h"
#include "ch374u_app.h"
#include "ch374u_hal.h"
#include "adb_device.h"
#include "adb_protocol.h"
#include "CH374INC.H"
#include "scmd.h"

#include "esp_bluetooth.h"
#include "esp_wifi_station.h"
#include "msg_send.h"


uint8_t test_count = 0;
uint8_t send_count = 0;
uint8_t send_temp[256];
uint8_t send_temp_len = 0;
uint8_t send_lock = 0;
signed int x = 0, y = 0;

uint8_t msg_send(uint8_t *buf, uint16_t len, uint8_t dev_class)
{
    //static uint8_t is_send_flag = 0;
    unsigned char buf_tmp[100];
    unsigned char send_len = 0;

    if (adb_c_s == ADB_CONNECT_TCPSERVER_SUCCESS && is_tcp_send_done == true)
    {
        is_tcp_send_done = false;

        if (dev_class == 0x00)
        {

            send_len = cmd_creat(0x00, buf, len, buf_tmp);
            send_tcpserver(local_id, remote_id, buf_tmp, send_len);

            printf("ADB TCP Status: ");
            printf_byte(buf_tmp, send_len);
        }
        else if (dev_class == DEV_MOUSE)
        {
            send_len = cmd_creat(0x02, buf, len, buf_tmp);
            send_tcpserver(local_id, remote_id, buf_tmp, send_len);

            printf("ADB TCP Mouse: ");
            printf_byte(buf_tmp, send_len);
        }
        else if (dev_class == DEV_KEYBOARD)
        {
            send_len = cmd_creat(0x03, buf, len, buf_tmp);
            send_tcpserver(local_id, remote_id, buf_tmp, send_len);

            printf("ADB TCP KeyBoard: ");
            printf_byte(buf_tmp, send_len);
        }
        return 0;
    }else if(is_wifi_socket_connect == true)
    {
        if (dev_class == 0x00)
        {

            send_len = cmd_creat(0x00, buf, len, buf_tmp);
            wifi_socket_send((char *)buf_tmp, send_len);

            printf("WIFI TCP Status: ");
            printf_byte(buf_tmp, send_len);
        }
        else if (dev_class == DEV_MOUSE)
        {
            send_len = cmd_creat(0x02, buf, len, buf_tmp);
            wifi_socket_send((char *)buf_tmp, send_len);

            printf("WIFI TCP Mouse: ");
            printf_byte(buf_tmp, send_len);
        }
        else if (dev_class == DEV_KEYBOARD)
        {
            send_len = cmd_creat(0x03, buf, len, buf_tmp);
            wifi_socket_send((char *)buf_tmp, send_len);

            printf("WIFI TCP KeyBoard: ");
            printf_byte(buf_tmp, send_len);
        }
        return 0;
    }
    else if (get_bluetooth_status() == 1)
    {
        if (dev_class == 0x00)
        {
            send_len = cmd_creat(0x00, buf, len, buf_tmp);
            esp_bluetooth_send(buf_tmp, send_len);
#ifdef BLE_LOG
            printf("BLUE Status: ");
            printf_byte(buf_tmp, send_len);
#endif
        }
        else if (dev_class == DEV_MOUSE)
        {
            // while(send_lock == 1);
            // send_lock = 1;

            // send_count++;

            // test_count++;
            // buf[len - 1] = test_count;

            // send_len = cmd_creat(0x02, buf, len, buf_tmp);

            // memcpy(send_temp + ((send_count - 1) * (send_len)), buf_tmp, send_len);

            // send_temp_len += send_len;

            // send_lock = 0;

            // if(send_count >= 4)
            // {
            //     send_count = 0;
            //     esp_bluetooth_send(send_temp, send_len*4);
            //         printf("BLUE Mouse: ");
            //         //printf("%d %d ", (signed char)buf[1], (signed char)buf[2]);
            //         printf_byte(send_temp, send_len*4);
            // }

            //     x += (signed char)buf[1];
            //     y += (signed char)buf[2];

            //     if (x < -128 || x > 127 || y < -128 || y > 127)
            //     {
            //         is_send_flag = 1;

            //         if(x < -128)
            //         {
            //             buf[1] = -128;
            //             //x+=128;
            //             x=0;
            //         }else if(x > 127)
            //         {
            //             buf[1] = 127;
            //             //x-=127;
            //             x=0;
            //         }else{
            //             buf[1] = x;
            //             x=0;
            //         }

            //         if(y < -128)
            //         {
            //             buf[2] = -128;
            //             //y+=128;
            //             y = 0;
            //         }else if(y > 127)
            //         {
            //             buf[2] = 127;
            //             //y-=127;
            //             y = 0;
            //         }else{
            //             buf[2] = y;
            //             y=0;
            //         }

            //     }

            //     if (send_count >= 4 && is_send_flag == 0)
            //     {
            //         is_send_flag = 1;

            //         buf[1] = (uint8_t)((signed char)x);
            //         buf[2] = (uint8_t)((signed char)y);

            //         x = 0;
            //         y = 0;
            //     }

            // if(is_send_flag == 1)
            // {
            //     is_send_flag = 0;

            test_count++;
            buf[len - 1] = test_count;

            send_len = cmd_creat(0x02, buf, len, buf_tmp);
            esp_bluetooth_send(buf_tmp, send_len);

            printf("BLUE Mouse: ");
            //printf("%d %d ", (signed char)buf[1], (signed char)buf[2]);
            printf_byte(buf_tmp, send_len);

            //         send_count = 0;
            // }
        }
        else if (dev_class == DEV_KEYBOARD)
        {
            send_len = cmd_creat(0x03, buf, len, buf_tmp);
            esp_bluetooth_send(buf_tmp, send_len);

            printf("BLUE KeyBoard: ");
            printf_byte(buf_tmp, send_len);
        }
        return 0;
    }
    else
    {
        return 1;
    }
}

// void bt_send_task(void *arg)
// {
//     while (1)
//     {
//         if (send_temp_len != 0)
//         {
//             while(send_lock == 1);
//             send_lock = 1;

//             esp_bluetooth_send(send_temp, send_temp_len);
//             printf("BLUE Mouse: ");
//             printf_byte(send_temp, send_temp_len);
//             send_count = 0;
//             send_temp_len = 0;
//             send_lock = 0;
//         }
//         vTaskDelay(20 / portTICK_RATE_MS);
//     }

//     vTaskDelete(NULL);
// }