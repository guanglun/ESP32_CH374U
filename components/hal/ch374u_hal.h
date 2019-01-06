#ifndef __CH374U_HAL_H__
#define __CH374U_HAL_H__

void ch374u_hal_init(void);
uint8_t	Read374Byte(uint8_t mAddr);
void Write374Byte(uint8_t mAddr, uint8_t mData);
void Modify374Byte( uint8_t mAddr, uint8_t mAndData, uint8_t mOrData );
void Read374Block( uint8_t mAddr, uint8_t mLen, uint8_t *mBuf );  /* 从指定起始地址读出数据块 */
void Write374Block( uint8_t mAddr, uint8_t mLen, uint8_t *mBuf );  /* 向指定起始地址写入数据块 */
void CH374_PORT_INIT();

#endif
