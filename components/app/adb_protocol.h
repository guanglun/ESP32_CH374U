#ifndef __ADB_PROTOCOL_H__
#define __ADB_PROTOCOL_H__

#define MAX_PAYLOAD 4096

#define A_SYNC 0x434e5953
#define A_CNXN 0x4e584e43
#define A_OPEN 0x4e45504f
#define A_OKAY 0x59414b4f
#define A_CLSE 0x45534c43
#define A_WRTE 0x45545257
#define A_AUTH 0x48545541

#define A_VERSION 0x01000000        // ADB protocol version

#define ADB_AUTH_TOKEN         1
#define ADB_AUTH_SIGNATURE     2
#define ADB_AUTH_RSAPUBLICKEY  3

typedef enum
{
  ADB_IDLE= 0,
  ADB_READ_DATA,
  ADB_SEND_DATA,
}
ADB_Status;

// #define PACKAGE_STR                   "com.guanglun.uiatuomatordemo"
// #define CHECK_PACKAGE_STR             "pm list packages com.guanglun.uiatuomatordemo"
// #define CHECK_PACKAGE_ISRUNING_STR    "ps | grep com.guanglun.uiatuomatordemo"
// #define START_PACKAGE_STR             "am instrument -w -r -e package com.guanglun.uiatuomatordemo -e debug false com.guanglun.uiatuomatordemo.test/android.support.test.runner.AndroidJUnitRunner &"
// #define CHECK_PACKAGE_START_STR       "INSTRUMENTATION_STATUS_CODE"

#define PACKAGE_STR                   "ATouchService"
#define PACKAGE_WITH_PATH_STR         "/data/local/tmp/ATouchService"
#define CHECK_PACKAGE_STR             "ls /data/local/tmp/ATouchService"
#define CHECK_PACKAGE_ISRUNING_STR    "ps | grep ATouchService"
#define CHECK_PACKAGE_ISRUNING_STR2    "ps -A | grep ATouchService"
#define START_PACKAGE_STR             "/data/local/tmp/ATouchService &"
#define CHECK_PACKAGE_START_STR       "ATouchService is runing"

#define CP_PACKAGE_STR                "cp /mnt/sdcard/ATouch/ATouchService /data/local/tmp"
#define CHMOD_PACKAGE_STR             "chmod 777 /data/local/tmp/ATouchService"

typedef enum
{
  ADB_DISCONNECT = 0,
  ADB_CONNECT,

  ADB_GOTO_SHELL_WAIT,
  ADB_GOTO_SHELL_SUCCESS,
  ADB_GOTO_SHELL_FAIL,

  ADB_CP_PACKAGE_WAIT,
  ADB_CP_PACKAGE_SUCCESS,
  ADB_CP_PACKAGE_FAIL,

  ADB_CHMOD_PACKAGE_WAIT,
  ADB_CHMOD_PACKAGE_SUCCESS,
  ADB_CHMOD_PACKAGE_FAIL,

  ADB_EXIT_SHELL_WAIT,
  ADB_EXIT_SHELL_SUCCESS_WAIT_END,
  ADB_EXIT_SHELL_SUCCESS,
  ADB_EXIT_SHELL_FAIL,

  ADB_CHECK_PACKAGE_WAIT,
  ADB_CHECK_PACKAGE_SUCCESS_WAIT_END,
  ADB_CHECK_PACKAGE_SUCCESS,
  ADB_CHECK_PACKAGE_FAIL,

  ADB_CHECK_PACKAGE_ISRUNING_WAIT,
  ADB_CHECK_PACKAGE_ISRUNING_TRUE,
  ADB_CHECK_PACKAGE_ISRUNING_FALSE,

  ADB_CHECK_PACKAGE_ISRUNING_WAIT2,
  ADB_CHECK_PACKAGE_ISRUNING_TRUE2,
  ADB_CHECK_PACKAGE_ISRUNING_FALSE2,

  ADB_START_PACKAGE_WAIT,
  ADB_START_PACKAGE_SUCCESS_WAIT_END,
  ADB_START_PACKAGE_SUCCESS,
  ADB_START_PACKAGE_FAIL,

  ADB_CONNECT_TCPSERVER_WAIT,
  ADB_CONNECT_TCPSERVER_SUCCESS,
  ADB_CONNECT_TCPSERVER_FAIL,

  ADB_SEND_TCPSERVER_WAIT,
  ADB_SEND_TCPSERVER_SUCCESS,
  ADB_SEND_TCPSERVER_FAIL,  

  ADB_NULL_STATUS
}
ADB_Connect_Status;

typedef struct _ADBTxRx_S
{
  volatile ADB_Status status;
  uint8_t  buffer[0x200];
  uint8_t  buffer_tmp[0x200];
  uint32_t length;
} ADBTxRx_S;

typedef struct amessage amessage;
typedef struct apacket apacket;

struct amessage {
    unsigned command;       /* command identifier constant      */
    unsigned arg0;          /* first argument                   */
    unsigned arg1;          /* second argument                  */
    unsigned data_length;   /* length of payload (0 is allowed) */
    unsigned data_check;    /* checksum of data payload         */
    unsigned magic;         /* command ^ 0xffffffff             */
};

struct apacket
{
    amessage msg;
    unsigned char data[MAX_PAYLOAD];
    unsigned count;
};

typedef enum
{
  ADB_READ_IDLE = 0,
  ADB_READ_GOT_HEAD,
}
ADB_Read_Status;

void send_cnxn_connect(void);
void get_adb_packet(amessage *msg, uint8_t *buf);
void send_ready(uint32_t local,uint32_t remote);
void send_just_open_shell(uint32_t local,uint32_t remote);
void send_shell(uint32_t local,uint32_t remote,uint8_t *buf);
void connect_to_remote(uint32_t local);
void send_auth_publickey(apacket *p);
void send_auth_response(apacket *p);
int check_data(apacket *p);
int check_header(apacket *p);
void send_open_shell(uint32_t local,uint32_t remote,uint8_t *buf);
void send_connect_tcpserver(uint32_t local,uint32_t remote,uint8_t *port_buf);
void send_tcpserver(uint32_t local,uint32_t remote,uint8_t *buf,uint16_t len);
void send_recv_tcpserver_okay(uint32_t local,uint32_t remote);
void send_okay(uint32_t local,uint32_t remote);

#endif
