#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "log.h"
#include "ch374u_hal.h"
#include "esp_log.h"

#define PORT8_0    12
#define PORT8_1    13
#define PORT8_2    14
#define PORT8_3    15
#define PORT8_4    16
#define PORT8_5    17
#define PORT8_6    18
#define PORT8_7    19

#define PORT8_SEL  (0xFF << PORT8_0)
#define CH374_CTRL_SEL  ((1ULL<<CH374_WR) | (1ULL<<CH374_RD) | (1ULL<<CH374_A0))

#define	CH374_WR 21
#define	CH374_RD 22
#define	CH374_A0 23

#define CH374_WR_HIGH()     GPIO.out_w1ts = (1 << CH374_WR);
#define CH374_WR_LOW()      GPIO.out_w1tc = (1 << CH374_WR);
#define CH374_RD_HIGH()     GPIO.out_w1ts = (1 << CH374_RD);
#define CH374_RD_LOW()      GPIO.out_w1tc = (1 << CH374_RD);
#define CH374_A0_HIGH()     GPIO.out_w1ts = (1 << CH374_A0);
#define CH374_A0_LOW()      GPIO.out_w1tc = (1 << CH374_A0);

#define	CH374_DATA_DAT_OUT(d)	{GPIO.out_w1tc = ((0xFF) << PORT8_0); GPIO.out_w1ts = (d << PORT8_0); }	
#define	CH374_DATA_DAT_IN()	    ( (GPIO.in >> PORT8_0) )		
#define	CH374_DATA_DIR_OUT()	{GPIO.enable_w1ts = (0xFF << PORT8_0);}
#define	CH374_DATA_DIR_IN()	    {GPIO.enable_w1tc = (0xFF << PORT8_0);}



void CH374_PORT_INIT()  
{
    gpio_config_t io_conf;

    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = PORT8_SEL;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);


    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = CH374_CTRL_SEL;
	io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

	CH374_WR_HIGH();
	CH374_RD_HIGH();
	CH374_A0_LOW();
	CH374_DATA_DIR_IN( ); 
}

void Write374Index( uint8_t mIndex ) 
{
    CH374_DATA_DIR_OUT();  
	CH374_DATA_DAT_OUT(mIndex); 
	CH374_A0_HIGH();
	CH374_WR_LOW(); 
	CH374_WR_HIGH(); 
	CH374_A0_LOW();
}

void Write374Data( uint8_t mData )
{
	
    CH374_DATA_DIR_OUT();  
	CH374_DATA_DAT_OUT(mData); 
	CH374_A0_LOW();
	CH374_WR_LOW(); 
    ets_delay_us(1);
	CH374_WR_HIGH(); 
}

void delay(uint16_t d)
{
	while(d--);
}

uint8_t Read374Data(void) 
{
	uint8_t	mData = 0;
	CH374_DATA_DIR_IN(); 
	CH374_A0_LOW();
	CH374_RD_LOW(); 
	ets_delay_us(1);
	mData = CH374_DATA_DAT_IN(); 
	CH374_RD_HIGH();
	return(mData);
}

uint8_t Read374Data0(void) 
{
	uint8_t	mData;
	CH374_DATA_DIR_IN(); 
	CH374_A0_HIGH();
	CH374_RD_LOW();  
    ets_delay_us(1);
	mData = CH374_DATA_DAT_IN(); 
	CH374_RD_HIGH();  
	CH374_A0_LOW();
	return(mData);
}

uint8_t	Read374Byte(uint8_t mAddr) 
{
	Write374Index(mAddr);
	return(Read374Data());
}

void Write374Byte(uint8_t mAddr, uint8_t mData) 
{
	Write374Index(mAddr);
	Write374Data(mData);
}

void Modify374Byte( uint8_t mAddr, uint8_t mAndData, uint8_t mOrData ) 
{
	Write374Index(mAddr);
	Write374Data((Read374Data0() & mAndData) | mOrData);
}

void Read374Block( uint8_t mAddr, uint8_t mLen, uint8_t *mBuf )  /* 从指定起始地址读出数据块 */
{
	uint8_t count = 0;
	Write374Index(mAddr);
	for (count = 0;count < mLen; count++) 
	{
	  	*(mBuf + count) = Read374Data();
	}
	ESP_LOGD("ATouch", "\r\nrecv ");
	printf_byte(mBuf,mLen);

	printf_byte_no_esp_log(mBuf,mLen);
	printf("\r\n");
}

void Write374Block( uint8_t mAddr, uint8_t mLen, uint8_t *mBuf )  /* 向指定起始地址写入数据块 */
{
	uint8_t count = 0;
	Write374Index( mAddr );
	for (count = 0;count < mLen; count++) 
	{
		Write374Data(*(mBuf + count));
	}
}

void ch374u_hal_init(void)
{
    uint8_t id;

    CH374_PORT_INIT();
    
	id = Read374Byte(0x04);
    if((id & 0x03) == 0x01)
    {
        ESP_LOGI("ATouch", "CH374U Connect Success %02X",id);
    }else{
        ESP_LOGI("ATouch", "CH374U Connect Fail %02X",id);
    }

}

