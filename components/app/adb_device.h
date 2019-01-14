#ifndef __ADB_DEVICE_H__
#define __ADB_DEVICE_H__

#include "adb_protocol.h"

int usb_send_packet(amessage *msg, uint8_t *buffer);
void adb_connect(void);
int ADB_RecvFrame(apacket *p);
int ADB_RecvData(uint8_t *buf, uint8_t len);

#endif
