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

uint8_t is_first_recv_auth_token = 1;
ADBTxRx_S adb_rx_s, adb_tx_s;

uint32_t local_id = 1,remote_id = 0;

ADB_Connect_Status adb_c_s = ADB_DISCONNECT;

bool is_close = true,is_tcp_send_done = true;

int printf_adb_frame(amessage *msg, uint8_t *buffer,bool is_recv)
{
    printf(">>>\r\n");
    if(is_recv == true)
    {
        printf("ADB RECV: ");
    }else{
        printf("ADB SEND: ");
    }
    switch (msg->command)
    {
    case A_SYNC:
        printf("SYNC ");
        break;

    case A_CNXN: /* CONNECT(version, maxdata, "system-id-string") */
        printf("CNXN ");
        break;

    case A_AUTH:
        printf("AUTH \r\n");

        return 0;

        break;

    case A_OPEN: /* OPEN(local-id, 0, "destination") */
        printf("OPEN ");
        break;

    case A_OKAY: /* READY(local-id, remote-id, "") */
        printf("OKAY ");

        break;

    case A_CLSE: /* CLOSE(local-id, remote-id, "") */
        printf("CLOSE ");
        break;

    case A_WRTE:
        printf("WRTE ");

        break;

    default:
        printf("handle_packet: what is %08x?!", msg->command);
        break;
    }

    printf("\r\n");
    printf_byte((uint8_t *)msg, sizeof(amessage));
    printf_byte_str(buffer, msg->data_length);
    printf_byte(buffer, msg->data_length);
    return 0;
}

int usb_send_packet(amessage *msg, uint8_t *buffer)
{
    
    uint16_t i_count = 0;

    get_adb_packet(msg, buffer);

    printf_adb_frame(msg,buffer,false);

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
    is_close = false;
    
	return 0;
}

void adb_connect(void)
{
    is_tcp_send_done = true;
    is_close = true;
    local_id = 1;
    remote_id = 0;
    adb_c_s = ADB_DISCONNECT;
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
    
    // printf("====================\r\n");  
    // printf("command\t0x%02X\r\n", p->msg.command);
    // printf("arg0\t0x%02X\r\n", p->msg.arg0);
    // printf("arg1\t0x%02X\r\n", p->msg.arg1);
    // printf("length\t0x%02X\r\n", p->msg.data_length);  
    // printf("====================\r\n");  

    *(p->data + p->msg.data_length) = '\0';
    printf_adb_frame(&(p->msg),p->data,true);
    
    switch (p->msg.command)
    {
        
    case A_SYNC:

        break;

    case A_CNXN: /* CONNECT(version, maxdata, "system-id-string") */
            adb_c_s = ADB_CONNECT;
            //connect_to_remote(local_id);
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
        // if(adb_c_s != ADB_CONNECT_INTO_SHELL)
        // {
        //     adb_c_s = ADB_CONNECT_INTO_SHELL;
        // }
        //remote_id = p->msg.arg1;

        if(adb_c_s == ADB_CONNECT_TCPSERVER_WAIT)
        {
            printf("tcpserver connect success\r\n");
            adb_c_s = ADB_CONNECT_TCPSERVER_SUCCESS;
            remote_id = p->msg.arg0;
            is_tcp_send_done = true;
        }else if(adb_c_s == ADB_SEND_TCPSERVER_WAIT)  
        {
            printf("tcpserver send success\r\n");
            adb_c_s = ADB_SEND_TCPSERVER_SUCCESS;
        }else if(adb_c_s == ADB_CONNECT_TCPSERVER_SUCCESS)
        {
            is_tcp_send_done = true; 
        }   
        break;

    case A_CLSE: /* CLOSE(local-id, remote-id, "") */
        if(adb_c_s == ADB_START_PACKAGE_WAIT)
        {
            adb_c_s = ADB_CHECK_PACKAGE_SUCCESS;
        }else if(adb_c_s == ADB_CONNECT_TCPSERVER_WAIT)
        {
            printf("tcpserver connect fail\r\n");
            adb_c_s = ADB_CONNECT_TCPSERVER_FAIL;
        }else if(adb_c_s == ADB_CONNECT_TCPSERVER_WAIT)
        {
            printf("tcpserver connect fail\r\n");
            adb_c_s = ADB_CONNECT_TCPSERVER_FAIL;
        }else if(adb_c_s == ADB_CHECK_PACKAGE_ISRUNING_WAIT)
        {
            printf("package is not running\r\n");
            adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_FALSE;
        }

        is_close = true;
        break;

    case A_WRTE:
        if(adb_c_s == ADB_CHECK_PACKAGE_WAIT)
        {
            if(strstr((const char *)p->data,"com.guanglun.uiatuomatordemo") != NULL)
            {
                printf("package found\r\n");
                adb_c_s = ADB_CHECK_PACKAGE_SUCCESS;
                
            }else{
                printf("package not found\r\n");
                adb_c_s = ADB_CHECK_PACKAGE_FAIL;
            }
        }else if(adb_c_s == ADB_CHECK_PACKAGE_ISRUNING_WAIT)
        {
            if(strstr((const char *)p->data,"com.guanglun.uiatuomatordemo") != NULL)
            {
                printf("package is running\r\n");
                adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_TRUE;
                
            }else{
                printf("package is not running\r\n");
                adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_FALSE;
            }
        }else if(adb_c_s == ADB_START_PACKAGE_WAIT)
        {
            is_close = true;
            adb_c_s = ADB_CHECK_PACKAGE_SUCCESS;
        }else if(adb_c_s == ADB_CONNECT_TCPSERVER_SUCCESS)
        {
            printf("recv tcpserver data\r\n");
            send_recv_tcpserver_okay(local_id,remote_id);
        }

        break;

    default:
        printf("handle_packet: what is %08x?!\r\n", p->msg.command);
        break;
    }

    ADB_Process();

    return 0;
}

void ADB_Process(void)
{
    switch(adb_c_s)
    {
        case ADB_CONNECT:
        send_open_shell(local_id,remote_id,(uint8_t *)"pm list packages com.guanglun.uiatuomatordemo");
        adb_c_s = ADB_CHECK_PACKAGE_WAIT;

        break;
        case ADB_CHECK_PACKAGE_SUCCESS:
        if(is_close != true)
        {
            break;
        }
        send_open_shell(local_id,remote_id,(uint8_t *)"ps | grep com.guanglun.uiatuomatordemo");
        adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_WAIT;
        break;     

        case ADB_CHECK_PACKAGE_ISRUNING_FALSE:
        if(is_close != true)
        {
            break;
        }
        send_open_shell(local_id,remote_id,(uint8_t *)"am instrument -w -r -e package com.guanglun.uiatuomatordemo -e debug false com.guanglun.uiatuomatordemo.test/android.support.test.runner.AndroidJUnitRunner");
        adb_c_s = ADB_START_PACKAGE_WAIT;

        break;       

        case ADB_CHECK_PACKAGE_ISRUNING_TRUE:
        if(is_close != true)
        {
            break;
        }
        send_connect_tcpserver(local_id,remote_id,(uint8_t *)"1989");
        adb_c_s = ADB_CONNECT_TCPSERVER_WAIT;
        break;     

        case ADB_CONNECT_TCPSERVER_FAIL:
        if(is_close != true)
        {
            break;
        }
        send_connect_tcpserver(local_id,remote_id,(uint8_t *)"1989");
        adb_c_s = ADB_CONNECT_TCPSERVER_WAIT;             
        //send_tcpserver(local_id,remote_id,(uint8_t *)"hello",strlen("hello"));
        //adb_c_s = ADB_SEND_TCPSERVER_WAIT;        

        break;  

        default:
        break;
    }
}

uint8_t ADB_TCP_Send(uint8_t *buf,uint16_t len)
{
    unsigned char buf_tmp[100];
    unsigned char send_len = 0,s = 0;

    if(adb_c_s == ADB_CONNECT_TCPSERVER_SUCCESS && is_tcp_send_done == true)
    {
        is_tcp_send_done = false;

        send_len = cmd_creat(0x02,buf,len,buf_tmp);
        send_tcpserver(local_id,remote_id,buf_tmp,send_len);

        printf("TCP Mouse data: ");
		for (s = 0; s < send_len; s++)
			printf("0x%02X ", *(buf_tmp + s));
		printf("\r\n");
        return 0;
    }else{
        return 1;
    }
}

