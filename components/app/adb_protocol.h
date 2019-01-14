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

typedef enum
{
  ADB_CONNECT_NOT_CHECK = 0,
  ADB_CONNECT_CHECK_OK,
  ADB_CONNECT_INTO_SHELL,
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
void send_shell(uint32_t local,uint32_t remote,uint8_t *buf,uint16_t len);
void connect_to_remote(uint32_t local);
void send_auth_publickey(apacket *p);
void send_auth_response(apacket *p);
int check_data(apacket *p);
int check_header(apacket *p);

#endif
