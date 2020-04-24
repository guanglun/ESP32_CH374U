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
#include "esp_log.h"
#include "uart.h"

uint8_t shell_end_str[20];
uint8_t shell_tmp_str[4096];

char pid[8];
char pid_kill = 0;

uint8_t is_first_recv_auth_token = 1;
ADBTxRx_S adb_rx_s, adb_tx_s;

uint32_t local_id = 1, remote_id = 0;

ADB_Connect_Status adb_c_s = ADB_DISCONNECT;

bool is_close = true, is_tcp_send_done = true;

int find_pid_str(char *str,char *pid)
{
    int i = 0,i_cnt = 0,flg = 0;
    

    str = strstr(str, "shell");
    ESP_LOGI("ATouch", "start found \"%s\"",str);
    while(str[i++] != '\0')
    {
        if((flg == 0) && (str[i] == ' '))
        {
            flg = 1;
        }else if((flg == 1) && (str[i] == ' ')){

        }else if((flg == 1) && (str[i] != ' ')){
            flg = 2;
            pid[i_cnt++] = str[i];
        }else if((flg == 2) && (str[i] != ' ')){
            pid[i_cnt++] = str[i];
        }else if((flg == 2) && (str[i] == ' ')){
            pid[i_cnt++] = '\0';
            return 0;
        }

        if(str[i] == '\r')
        {
            flg = 0;
            i_cnt = 0;
        }
    }
    pid[0] = '\0';
    return -1;
}

int printf_adb_frame(amessage *msg, uint8_t *buffer, bool is_recv)
{
    ESP_LOGI("ATouch", ">>>");
    if (is_recv == true)
    {
        ESP_LOGI("ATouch", "ADB RECV: ");
    }
    else
    {
        ESP_LOGI("ATouch", "ADB SEND: ");
    }
    switch (msg->command)
    {
    case A_SYNC:
        ESP_LOGI("ATouch", "SYNC ");
        break;

    case A_CNXN: /* CONNECT(version, maxdata, "system-id-string") */
        ESP_LOGI("ATouch", "CNXN ");
        break;

    case A_AUTH:
        ESP_LOGI("ATouch", "AUTH ");

        return 0;

        break;

    case A_OPEN: /* OPEN(local-id, 0, "destination") */
        ESP_LOGI("ATouch", "OPEN ");
        break;

    case A_OKAY: /* READY(local-id, remote-id, "") */
        ESP_LOGI("ATouch", "OKAY ");

        break;

    case A_CLSE: /* CLOSE(local-id, remote-id, "") */
        ESP_LOGI("ATouch", "CLOSE ");
        break;

    case A_WRTE:
        ESP_LOGI("ATouch", "WRTE ");

        break;

    default:
        ESP_LOGI("ATouch", "handle_packet: what is %08x?!", msg->command);
        break;
    }

    ESP_LOGI("ATouch", "");
    //printf_byte((uint8_t *)msg, sizeof(amessage));
    printf_byte_str(buffer, msg->data_length);
    //printf_byte(buffer, msg->data_length);
    return 0;
}

#define MAX_DATA_LEN 64
int usb_send_packet(amessage *msg, uint8_t *buffer, uint8_t flag)
{

    uint16_t i_count = 0;

    get_adb_packet(msg, buffer);

#ifdef ADB_LOG
    printf_adb_frame(msg, buffer, false);
#endif

    QueryADB_Send((uint8_t *)msg, sizeof(amessage), 0);

    for (i_count = 0; i_count < msg->data_length; i_count += MAX_DATA_LEN)
    {
        if ((msg->data_length - i_count) <= MAX_DATA_LEN)
        {

            QueryADB_Send((uint8_t *)(buffer + i_count), msg->data_length - i_count, flag);
            //小米青春版数据最后是64字节的话无法成功发送，通过下面语句解决
            if ((msg->data_length) - i_count == 64)
            {
                send_okay(0, 0);
            }
            //printf_byte((uint8_t *)(buffer + i_count), msg->data_length - i_count);
            //printf_byte_str((uint8_t *)(buffer + i_count), msg->data_length - i_count);
        }
        else
        {
            QueryADB_Send((uint8_t *)(buffer + i_count), MAX_DATA_LEN, 0);
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

void adb_shell_recv_reset(void)
{
    shell_tmp_str[0] = '\0';
}

uint8_t * adb_shell_recv(uint8_t * recv_data)
{
    strcat((char *)shell_tmp_str, (const char *)recv_data);

    if (strstr((const char *)shell_tmp_str, (const char *)shell_end_str) != NULL)
    {
        ESP_LOGI("ATouch", "==============>>>");
        ESP_LOGI("ATouch", "%s", shell_tmp_str);
        ESP_LOGI("ATouch", "==============<<<");
        return recv_data;
    }else{
        return NULL;
    }
    return NULL;
}

int get_str_count(char * tar_str,char *found_str)
{
    int count = 0;
    char *str_tmp = tar_str;
    
    str_tmp = strstr(str_tmp,found_str);
    while(str_tmp != NULL)
    {
        str_tmp += strlen(found_str);
        count++;
        str_tmp = strstr(str_tmp,found_str);
    }
    //ESP_LOGI("ATouch", "get_str_count:%d",count);
    return count;
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
            if (p.msg.data_length == 0)
            {
                ADB_RecvFrame(&p);
                adb_read_status = ADB_READ_IDLE;
                return 0;
            }
            else
            {
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

    ESP_LOGD("ATouch", "====================");
    ESP_LOGD("ATouch", "command\t0x%02X", p->msg.command);
    ESP_LOGD("ATouch", "arg0\t0x%02X", p->msg.arg0);
    ESP_LOGD("ATouch", "arg1\t0x%02X", p->msg.arg1);
    ESP_LOGD("ATouch", "length\t0x%02X", p->msg.data_length);
    ESP_LOGD("ATouch", "====================");

    *(p->data + p->msg.data_length) = '\0';

#ifdef ADB_LOG
    printf_adb_frame(&(p->msg), p->data, true);
#endif

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
            if (is_first_recv_auth_token == 1)
            {
                is_first_recv_auth_token = 0;
                send_auth_response(p);
            }
            else
            {
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

        if (adb_c_s == ADB_CONNECT_TCPSERVER_WAIT)
        {
            ESP_LOGI("ATouch", "tcpserver connect success");
            adb_c_s = ADB_CONNECT_TCPSERVER_SUCCESS;
            remote_id = p->msg.arg0;
            local_id = p->msg.arg1;
            is_tcp_send_done = true;
        }
        else if (adb_c_s == ADB_SEND_TCPSERVER_WAIT)
        {
            ESP_LOGI("ATouch", "tcpserver send success");
            adb_c_s = ADB_SEND_TCPSERVER_SUCCESS;
        }
        else if (adb_c_s == ADB_CONNECT_TCPSERVER_SUCCESS)
        {
            is_tcp_send_done = true;
        }
        break;

    case A_CLSE: /* CLOSE(local-id, remote-id, "") */
        if (adb_c_s == ADB_CONNECT_TCPSERVER_WAIT)
        {
            ESP_LOGI("ATouch", "tcpserver connect fail");
            adb_c_s = ADB_CONNECT_TCPSERVER_FAIL;
        }
        else if (adb_c_s == ADB_EXIT_SHELL_SUCCESS_WAIT_END)
        {
            ESP_LOGI("ATouch", "exit shell success");
            adb_c_s = ADB_EXIT_SHELL_SUCCESS;
        }

        is_close = true;
        break;

    case A_WRTE:
        if (adb_c_s == ADB_CHECK_PACKAGE_WAIT)
        {
            if(adb_shell_recv(p->data) != NULL)
            {
                if(strstr((const char *) shell_tmp_str,"No such file or directory") == NULL)
                {
                    ESP_LOGI("ATouch", "package found");
                    adb_c_s = ADB_CHECK_PACKAGE_SUCCESS;
                }else{
                    ESP_LOGI("ATouch", "package not found");
                    adb_c_s = ADB_CHECK_PACKAGE_FAIL;
                }
            }
        }     
        else if (adb_c_s == ADB_CHECK_PACKAGE_ISRUNING_WAIT)
        {

            if(adb_shell_recv(p->data) != NULL)
            {
                if(get_str_count((char *) shell_tmp_str,(char *)PACKAGE_STR) >= 2)
                {
                    ESP_LOGI("ATouch", "package is running");
                    if(find_pid_str((char *)shell_tmp_str,pid) == 0)
                    {
                        ESP_LOGI("ATouch", "found pid %s",pid);
                    }
                    adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_TRUE;
                }else{
                    ESP_LOGI("ATouch", "package is not running");
                    adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_FALSE;
                }
            }
        }
        else if (adb_c_s == ADB_CHECK_PACKAGE_ISRUNING_WAIT2)
        {

            if(adb_shell_recv(p->data) != NULL)
            {
                if(get_str_count((char *) shell_tmp_str,(char *)PACKAGE_STR) >= 2)
                {
                    ESP_LOGI("ATouch", "package is running2");

                    if(find_pid_str((char *)shell_tmp_str,pid) == 0)
                    {
                        ESP_LOGI("ATouch", "found pid %s",pid);
                    }
                    adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_TRUE2;
                }else{
                    ESP_LOGI("ATouch", "package is not running2");
                    adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_FALSE2;
                }
            }
        }        
        else if (adb_c_s == ADB_START_PACKAGE_WAIT)
        {
            if(adb_shell_recv(p->data) != NULL)
            {
                // if(get_str_count((char *) shell_tmp_str,(char *)PACKAGE_WITH_PATH_STR) == 1)
                // {
                    ESP_LOGI("ATouch", "package start success");
                    adb_c_s = ADB_START_PACKAGE_SUCCESS;
                // }else{
                //     ESP_LOGI("ATouch", "package start fail");
                //     adb_c_s = ADB_START_PACKAGE_FAIL;
                // }
            }
        }
        else if (adb_c_s == ADB_CONNECT_TCPSERVER_SUCCESS)
        {
            ESP_LOGI("ATouch", "recv tcpserver data %s",p->data);       
            set_wifi_info((char *)p->data);  
        }
        else if (adb_c_s == ADB_GOTO_SHELL_WAIT)
        {
            char *ret;
            if(adb_shell_recv(p->data) != NULL)
            {
                if ((ret = strchr((const char *)shell_tmp_str, ':')) != NULL)
                {
                    *ret = '\0';
                    memcpy(shell_end_str, shell_tmp_str, ((uint8_t *)ret - shell_tmp_str + 1));
                    remote_id = p->msg.arg0;
                    local_id = p->msg.arg1;

                    adb_c_s = ADB_GOTO_SHELL_SUCCESS;
                    ESP_LOGI("ATouch", "goto shell success %s", shell_end_str);
                }
                else
                {
                    ESP_LOGI("ATouch", "goto shell fail");
                    adb_c_s = ADB_GOTO_SHELL_FAIL;
                }
            }
        }
        else if (adb_c_s == ADB_CHECK_PACKAGE_KILL_PID_WAIT)
        {
            if(adb_shell_recv(p->data) != NULL)
            {
                if(get_str_count((char *) shell_tmp_str,(char *)PACKAGE_STR) >= 2)
                {
                    ESP_LOGI("ATouch", "kill pid %s success",pid);
                    adb_c_s = ADB_CHECK_PACKAGE_KILL_PID_TRUE;
                }else{
                    ESP_LOGI("ATouch", "kill pid %s success",pid);
                    adb_c_s = ADB_CHECK_PACKAGE_KILL_PID_TRUE;
                }
            }
        }        
        else if (adb_c_s == ADB_EXIT_SHELL_WAIT)
        {
            if (strstr((const char *)p->data, (const char *)"exit") != NULL)
            {
                adb_c_s = ADB_EXIT_SHELL_SUCCESS_WAIT_END;
            }
            else
            {
                adb_c_s = ADB_EXIT_SHELL_FAIL;
            }
        }
        else if (adb_c_s == ADB_EXIT_SHELL_SUCCESS_WAIT_END)
        {
            adb_c_s = ADB_EXIT_SHELL_FAIL;
        }
        else if (adb_c_s == ADB_CP_PACKAGE_WAIT)
        {
            if(adb_shell_recv(p->data) != NULL)
            {
                if(get_str_count((char *) shell_tmp_str,(char *)PACKAGE_STR) == 1)
                {
                    ESP_LOGI("ATouch", "cp package success");
                    adb_c_s = ADB_CP_PACKAGE_SUCCESS;
                }else{
                    ESP_LOGI("ATouch", "cp package fail");
                    //此处如果拷贝失败可能是Text file busy文件已经正在被使用。
                    adb_c_s = ADB_CP_PACKAGE_FAIL;
                    //adb_c_s = ADB_CP_PACKAGE_SUCCESS;
                }
            }
        }
        else if (adb_c_s == ADB_CHMOD_PACKAGE_WAIT)
        {
            if(adb_shell_recv(p->data) != NULL)
            {
                if(get_str_count((char *) shell_tmp_str,(char *)PACKAGE_STR) == 1)
                {
                    ESP_LOGI("ATouch", "chmod package success");
                    adb_c_s = ADB_CHMOD_PACKAGE_SUCCESS;
                }else{
                    ESP_LOGI("ATouch", "chmod package fail");
                    adb_c_s = ADB_CHMOD_PACKAGE_FAIL;
                }
            }
        }
        send_recv_tcpserver_okay(local_id, remote_id);
        break;

    default:
        ESP_LOGI("ATouch", "handle_packet: what is %08x?!", p->msg.command);
        break;
    }

    ADB_Process();

    return 0;
}

void ADB_Process(void)
{
    switch (adb_c_s)
    {
    case ADB_CONNECT: //ADB AUTH完成之后第一个状态
    adb_shell_recv_reset();
        send_just_open_shell(local_id, remote_id);
        adb_c_s = ADB_GOTO_SHELL_WAIT;
        pid_kill = 0;
        break;

    //goto shell result
    case ADB_GOTO_SHELL_SUCCESS: //检测ATouchService
        // adb_shell_recv_reset();
        // send_shell(local_id, remote_id, (uint8_t *)CHECK_PACKAGE_STR);
        // adb_c_s = ADB_CHECK_PACKAGE_WAIT;
        adb_shell_recv_reset();
        send_shell(local_id, remote_id, (uint8_t *)CP_PACKAGE_STR);//不管有没有存在都会复制一次来保证最新
        adb_c_s = ADB_CP_PACKAGE_WAIT;        
        break;
    case ADB_GOTO_SHELL_FAIL: //进入shell失败后重试
        adb_shell_recv_reset();
        send_just_open_shell(local_id, remote_id);
        adb_c_s = ADB_GOTO_SHELL_WAIT;    
        break;

    //cp package result
    case ADB_CP_PACKAGE_SUCCESS:
        adb_shell_recv_reset();
        // send_shell(local_id, remote_id, (uint8_t *)CHMOD_PACKAGE_STR);
        // adb_c_s = ADB_CHMOD_PACKAGE_WAIT;
        send_shell(local_id, remote_id, (uint8_t *)CHECK_PACKAGE_STR);
        adb_c_s = ADB_CHECK_PACKAGE_WAIT;
        break;
    case ADB_CP_PACKAGE_FAIL:
        if(pid_kill == 0)
            pid_kill = 1;

        adb_shell_recv_reset();
        send_shell(local_id, remote_id, (uint8_t *)CHECK_PACKAGE_ISRUNING_STR);
        adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_WAIT;
        break;

    //chmod package result
    case ADB_CHMOD_PACKAGE_SUCCESS:
    case ADB_CHMOD_PACKAGE_FAIL:
        adb_shell_recv_reset();
        send_shell(local_id, remote_id, (uint8_t *)CHECK_PACKAGE_ISRUNING_STR);
        adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_WAIT;
        break;

    //check package is exist or not result
    case ADB_CHECK_PACKAGE_SUCCESS: //检测到ATouchService
        adb_shell_recv_reset();
        // send_shell(local_id, remote_id, (uint8_t *)CHECK_PACKAGE_ISRUNING_STR);
        // adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_WAIT;

        send_shell(local_id, remote_id, (uint8_t *)CHMOD_PACKAGE_STR);
        adb_c_s = ADB_CHMOD_PACKAGE_WAIT;
        break;
    case ADB_CHECK_PACKAGE_FAIL: //未检测到ATouchService
        adb_shell_recv_reset();
        send_shell(local_id, remote_id, (uint8_t *)CP_PACKAGE_STR);
        adb_c_s = ADB_CP_PACKAGE_WAIT;
        break;

    //check package is running result (1,2)
    case ADB_CHECK_PACKAGE_ISRUNING_TRUE:
    case ADB_CHECK_PACKAGE_ISRUNING_TRUE2:

        if(pid_kill == 1 && pid[0] != '\0')
        {
            char str[80];
            strcpy (str,KILL_PID_STR);
            strcat (str,pid);
            adb_shell_recv_reset();
            send_shell(local_id, remote_id, (uint8_t *)str);
            pid_kill = 2;
            adb_c_s = ADB_CHECK_PACKAGE_KILL_PID_WAIT;
        }else{
            
            adb_shell_recv_reset();
            local_id++;
            remote_id = 0;
            send_connect_tcpserver(local_id, remote_id, (uint8_t *)"1989");
            adb_c_s = ADB_CONNECT_TCPSERVER_WAIT;
        }
        break;
    case ADB_CHECK_PACKAGE_ISRUNING_FALSE:
        adb_shell_recv_reset();
        send_shell(local_id, remote_id, (uint8_t *)CHECK_PACKAGE_ISRUNING_STR2);
        adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_WAIT2;
        break;
    case ADB_CHECK_PACKAGE_ISRUNING_FALSE2:
        adb_shell_recv_reset();
        send_shell(local_id, remote_id, (uint8_t *)START_PACKAGE_STR);
        adb_c_s = ADB_START_PACKAGE_WAIT;

        //vTaskDelay(2000 / portTICK_RATE_MS);
        break;

    //run package result (run ATouch)
    case ADB_START_PACKAGE_SUCCESS:
    case ADB_START_PACKAGE_FAIL:
        adb_shell_recv_reset();
        send_shell(local_id, remote_id, (uint8_t *)CHECK_PACKAGE_ISRUNING_STR);
        adb_c_s = ADB_CHECK_PACKAGE_ISRUNING_WAIT;
        break;

    //kill pid result
    case ADB_CHECK_PACKAGE_KILL_PID_TRUE:
    case ADB_CHECK_PACKAGE_KILL_PID_FALSE:
        adb_shell_recv_reset();
        send_shell(local_id, remote_id, (uint8_t *)CP_PACKAGE_STR);//不管有没有存在都会复制一次来保证最新
        adb_c_s = ADB_CP_PACKAGE_WAIT;        
        break;

    //tcp connect result
    case ADB_CONNECT_TCPSERVER_SUCCESS:
        break;
    case ADB_CONNECT_TCPSERVER_FAIL:
        adb_shell_recv_reset();
        send_connect_tcpserver(local_id, remote_id, (uint8_t *)"1989");
        adb_c_s = ADB_CONNECT_TCPSERVER_WAIT;
        break;

    default:
        break;
    }
}

