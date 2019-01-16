#include "scmd.h"
#include <stdio.h>
#include <string.h>

unsigned char cmd_creat(unsigned char cmd,unsigned char *in_buffer,unsigned char in_len,unsigned char *out_buffer)
{
    unsigned char i = 0;
    unsigned char buffer[100];
    unsigned char check = 0;

    out_buffer[0] = 0xAA;
    out_buffer[1] = 0xBB;
    out_buffer[2] = in_len+1;
    out_buffer[3] = cmd;

    memcpy(out_buffer+4, in_buffer, in_len);

    for (i = 2; i < 4+in_len; i++)
    {
        check += out_buffer[i];
    }

    out_buffer[4+in_len] = check;

    return 5+in_len;
}

//0x02
void mouse_cmd_send(unsigned char *in_buffer,unsigned char in_len)
{
    unsigned char buf_tmp[100];
    unsigned char len = 0;

    len = cmd_creat(0x02,in_buffer,in_len,buf_tmp);

    send_buffer(buf_tmp,len);
}

//0x03
void keyboard_cmd_send(unsigned char *in_buffer,unsigned char in_len)
{
    unsigned char buf_tmp[100];
    unsigned char len = 0;

    len = cmd_creat(0x03,in_buffer,in_len,buf_tmp);

    send_buffer(buf_tmp,len);
}