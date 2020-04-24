#ifndef __ADB_DEVICE_H__
#define __ADB_DEVICE_H__

#include "adb_protocol.h"

//#define ADB_LOG

extern ADB_Connect_Status adb_c_s;
extern bool is_tcp_send_done;
extern uint32_t local_id, remote_id;

int usb_send_packet(amessage *msg, uint8_t *buffer,uint8_t flag);
void adb_connect(void);
int ADB_RecvFrame(apacket *p);
int ADB_RecvData(uint8_t *buf, uint8_t len);
void ADB_Process(void);
void bt_send_task(void *arg);

#endif
