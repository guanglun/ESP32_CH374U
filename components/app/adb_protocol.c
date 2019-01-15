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
#include "sha1withrsa.h"

#define PUBLIC_KEY "QAAAAC/VcToxbiTUhT4Bm3tvGDXxT3+XC7G6ntpPb07RqtVhffOJATCB1nWbOenUpr5DOeOLDEttPjFCSzaXgatPXT6JkCfZ5RomXhk4D40Gfdo9X0DvRgmiu6sO7YxiOhsHppnoABcXtBnFX9cfvOv8ShI78+/j6Q9F+eNBG2bbhhLYsdq1ynSvLlT/1Smp7fN/X/IA3A8zDtN/JhmzdsM/vTZr4hahTlljFo4AWDEMSqmSF9Mh/zXxlHTq402/LWTGZirkLKF30/3U9zxIvkXJnwDKilaFOBcLTS44F7jZByQ4J+GFpmtbaIFwlcxCim5SBoHwSu+6P0RObbFDg8/BY47KiwLjW00BLhiBgPr5xU3N/oqXDS13Rv4duf25mF5ZPuqimC9vjMXj6R/HLwPByOidDkPaNnZKlp6q4aclTw6UAK8kWuGiuFBgJBwGlXsjRT0luYxd1AGzK9ryxYsSk/o1jqhJQyVEIqo0wb9xmChlzReTFB/rwzZCfPZT/rVWPdR9oKZDvnZUf49yRT3PXmSelgzN5Kw2Ca7Nzo8Evh/tBeOl35X7CCYwyie6iFifARrMgfVe/3br33ohGjl8WUWerDn3TKSU9ujKv3F6yvsR4AxjMEElcpRLQUnn5VxBYMoDABjLtItLeDeeC7v8VTHkdNjhUUtpksuiS/epLy+Kza9sbQEAAQA= @unknown"
#define CNXN_CMD_STR "host::features=stat_v2,shell_v2,cmd"



void get_adb_packet(amessage *msg, uint8_t *buf)
{
    unsigned char *x;
    unsigned int sum;
    unsigned int count;

    msg->magic = msg->command ^ 0xffffffff;

    count = msg->data_length;
    x = (unsigned char *)buf;
    sum = 0;
    while (count-- > 0)
    {
        sum += *x++;
    }
    msg->data_check = sum;
}

int check_header(apacket *p)
{
    if(p->msg.magic != (p->msg.command ^ 0xffffffff)) {
        printf("check_header(): invalid magic\r\n");
        return -1;
    }

    if(p->msg.data_length > MAX_PAYLOAD) {
        printf("check_header(): %d > MAX_PAYLOAD\r\n", p->msg.data_length);
        return -1;
    }

    return 0;
}

int check_data(apacket *p)
{
    unsigned count, sum;
    unsigned char *x;

    count = p->msg.data_length;
    x = p->data;
    sum = 0;
    while(count-- > 0) {
        sum += *x++;
    }

    if(sum != p->msg.data_check) {
        return -1;
    } else {
        return 0;
    }
}


void send_cnxn_connect(void)
{
    amessage msg;
    msg.command = A_CNXN;
    msg.arg0 = A_VERSION;
    msg.arg1 = MAX_PAYLOAD;
    msg.data_length = strlen(CNXN_CMD_STR);
    
    usb_send_packet(&msg, (uint8_t *)CNXN_CMD_STR);
}

void send_auth_response(apacket *p)
{
    amessage msg;
    uint8_t RSABuffer[256];

    msg.command = A_AUTH;
    msg.arg0 = ADB_AUTH_SIGNATURE;
    msg.arg1 = 0;
    msg.data_length = 256;

    SHA1withRSA(p->data, p->msg.data_length, RSABuffer);
    usb_send_packet(&msg, RSABuffer);
}

void send_auth_publickey(apacket *p)
{
    uint8_t public_key[2048] = {0};
    amessage msg;

    msg.command = A_AUTH;
    msg.arg0 = ADB_AUTH_RSAPUBLICKEY;
    msg.arg1 = 0;
    msg.data_length = strlen(PUBLIC_KEY) + 1;

    memcpy(public_key,PUBLIC_KEY,strlen(PUBLIC_KEY));

    usb_send_packet(&msg, public_key);
}

void connect_to_remote(uint32_t local)
{
    uint8_t shell_cmd[] = {0x73,0x68,0x65,0x6c,0x6c,0x2c,0x76,0x32,0x2c,0x70,0x74,0x79,0x3a,0x00};
    amessage msg;

    msg.command = A_OPEN;
    msg.arg0 = local;
    msg.arg1 = 0;
    msg.data_length = sizeof(shell_cmd);

    usb_send_packet(&msg, shell_cmd);    
}

void send_ready(uint32_t local,uint32_t remote)
{
    amessage msg;

    msg.command = A_OPEN;
    msg.arg0 = remote;
    msg.arg1 = local;
    msg.data_length = 0;

    usb_send_packet(&msg, NULL);   
}

void send_shell(uint32_t local,uint32_t remote,uint8_t *buf,uint16_t len)
{
    amessage msg;

    msg.command = A_WRTE;
    msg.arg0 = remote;
    msg.arg1 = local;
    msg.data_length = len;

    usb_send_packet(&msg, buf);   
}






