#ifndef __ADB_DEVICE_H__
#define __ADB_DEVICE_H__

#include "adb_protocol.h"

#define ADB_LOG

int usb_send_packet(amessage *msg, uint8_t *buffer,uint8_t flag);
void adb_connect(void);
int ADB_RecvFrame(apacket *p);
int ADB_RecvData(uint8_t *buf, uint8_t len);
void ADB_Process(void);
uint8_t ADB_TCP_Send(uint8_t *buf,uint16_t len,uint8_t dev_class);
void bt_send_task(void *arg);

#endif
