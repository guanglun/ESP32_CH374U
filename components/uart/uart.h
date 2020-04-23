#ifndef __UART_H__
#define __UART_H__

extern bool is_uart_connect;

void uart_init(void);
void uart_send(char *buff,int len);

#endif
