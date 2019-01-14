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

uint8_t is_first_recv_auth_token = 1;
ADBTxRx_S adb_rx_s, adb_tx_s;

uint32_t local_id = 1;
uint32_t remote_id;
ADB_Connect_Status adb_c_s = ADB_CONNECT_NOT_CHECK;

int usb_send_packet(amessage *msg, uint8_t *buffer)
{
    
    uint16_t i_count = 0;

    get_adb_packet(msg, buffer);
    QueryADB_Send((uint8_t *)msg, sizeof(amessage));

    for (i_count = 0; i_count < msg->data_length; i_count += 64)
    {
        if ((msg->data_length - i_count) < 64)
        {
            
            QueryADB_Send((uint8_t *)(buffer + i_count), msg->data_length - i_count);
            //printf_byte((uint8_t *)(buffer + i_count), msg->data_length - i_count);
            //printf_byte_str((uint8_t *)(buffer + i_count), msg->data_length - i_count);
        }
        else
        {
            QueryADB_Send((uint8_t *)(buffer + i_count), 64);
            //printf_byte((uint8_t *)(buffer + i_count), 64);
            //printf_byte_str((uint8_t *)(buffer + i_count), 64);
        }
    }
	return 0;
}

void adb_connect(void)
{
    is_first_recv_auth_token = 1;
    send_cnxn_connect();
}

int ADB_RecvData(uint8_t *buf, uint8_t len)
{
    static ADB_Read_Status adb_read_status = ADB_READ_IDLE;
    static apacket p;

    if (adb_read_status == ADB_READ_IDLE && len == 24)
    {
        memcpy(&p.msg, buf, len);

        if (check_header(&p) == 0)
        {
            if(p.msg.data_length == 0)
            {
                ADB_RecvFrame(&p);
                adb_read_status = ADB_READ_IDLE;
                return 0;
            }else{
                p.count = 0;
                adb_read_status = ADB_READ_GOT_HEAD;
            }

            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if (adb_read_status == ADB_READ_GOT_HEAD)
    {
        if ((p.msg.data_length - p.count) >= len)
        {
            memcpy((p.data + p.count), buf, len);
            p.count += len;
            if (p.msg.data_length == p.count)
            {
                if (check_data(&p) == 0)
                {
                    ADB_RecvFrame(&p);
                    adb_read_status = ADB_READ_IDLE;
                    return 0;
                }
                else
                {
                    goto reset;
                }
            }
            return 0;
        }
        else
        {
            goto reset;
        }
    }

reset:
    adb_read_status = ADB_READ_IDLE;
    return -1;
}

int ADB_RecvFrame(apacket *p)
{
    
    printf("====================\r\n");  
    printf("command\t0x%02X\r\n", p->msg.command);
    printf("arg0\t0x%02X\r\n", p->msg.arg0);
    printf("arg1\t0x%02X\r\n", p->msg.arg1);
    printf("length\t0x%02X\r\n", p->msg.data_length);  
    printf("====================\r\n");  

    switch (p->msg.command)
    {
    case A_SYNC:

        break;

    case A_CNXN: /* CONNECT(version, maxdata, "system-id-string") */
            adb_c_s = ADB_CONNECT_CHECK_OK;
            connect_to_remote(local_id);
        break;

    case A_AUTH:
        if (p->msg.arg0 == ADB_AUTH_TOKEN)
        {
            if(is_first_recv_auth_token == 1)
            {
                is_first_recv_auth_token = 0;
                send_auth_response(p);
            }else{
                send_auth_publickey(p);
            }
            
        }
        else if (p->msg.arg0 == ADB_AUTH_SIGNATURE)
        {

        }
        else if (p->msg.arg0 == ADB_AUTH_RSAPUBLICKEY)
        {
        }
        break;

    case A_OPEN: /* OPEN(local-id, 0, "destination") */

        break;

    case A_OKAY: /* READY(local-id, remote-id, "") */
        if(adb_c_s != ADB_CONNECT_INTO_SHELL)
        {
            adb_c_s = ADB_CONNECT_INTO_SHELL;
            remote_id = p->msg.arg1;
            
        }

        break;

    case A_CLSE: /* CLOSE(local-id, remote-id, "") */

        break;

    case A_WRTE:

        send_ready(local_id,remote_id);
        break;

    default:
        printf("handle_packet: what is %08x?!\r\n", p->msg.command);
        break;
    }

    return 0;
}

