#ifndef __SCMD_H__
#define __SCMD_H__


void send_buffer(unsigned char *buf,unsigned long len);
unsigned char cmd_creat(unsigned char cmd,unsigned char *in_buffer,unsigned char in_len,unsigned char *out_buffer);
void mouse_cmd_send(unsigned char *in_buffer,unsigned char in_len);
void keyboard_cmd_send(unsigned char *in_buffer,unsigned char in_len);

#endif

