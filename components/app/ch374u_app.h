#ifndef __CH374U_APP_H__
#define __CH374U_APP_H__

#define CLEAR_HUB_FEATURE	0x20
#define CLEAR_PORT_FEATURE	0x23
#define GET_BUS_STATE		0xa3
#define GET_HUB_DESCRIPTOR	0xa0
#define GET_HUB_STATUS		0xa0
#define GET_PORT_STATUS		0xa3
#define SET_HUB_DESCRIPTOR	0x20
#define SET_HUB_FEATURE		0x20
#define SET_PORT_FEATURE	0x23

//////Hub Class Feature Selectors
#define	C_HUB_LOCAL_POWER	0
#define C_HUB_OVER_CURRENT	1
#define PORT_CONNECTION		0
#define PORT_ENABLE			1
#define PORT_SUSPEND		2
#define PORT_OVER_CURRENT	3
#define PORT_RESET			4
#define PORT_POWER			8
#define	PORT_LOW_SPEED		9
#define C_PORT_CONNECTION	16
#define C_PORT_ENABLE		17
#define C_PORT_SUSPEND		18
#define C_PORT_OVER_CURRENT	19
#define C_PORT_RESET		20

////////Hub Class Request Codes
#define GET_STATUS			0
#define CLEAR_FEATURE		1
#define GET_STATE			2
#define SET_FEATURE			3
#define GET_DESCRIPTOR		6
#define SET_DESCRIPTOR		7

// 输入: 内置HUB端口号0/1/2
// 输出: 0-操作失败, 0x31-成功枚举到USB键盘, 0x32-成功枚举到鼠标, 0x70-成功枚举到打印机, 0x80-成功枚举到U盘, 0xFF-成功枚举未知设备, 其它值暂未定义
#define DEV_ERROR 0x00
#define DEV_KEYBOARD 0x31
#define DEV_MOUSE 0x32
#define DEV_PRINT 0x70
#define DEV_DISK 0x80
#define DEV_HUB 0x90
#define DEV_ADB 0xA0
#define DEV_UNKNOWN 0xFF

typedef struct _HUB_DESCRIPTOR
{
	unsigned char bDescLength;
	unsigned char bDescriptorType;
	unsigned char bNbrPorts;
	unsigned char wHubCharacteristics[2];
	unsigned char bPwrOn2PwrGood;
	unsigned char bHubContrCurrent;
	unsigned char DeviceRemovable;
	
}
HUBDescr,*PHUBDescr;

/////////// 动态集线器信息分配表
typedef struct _INF
{
	unsigned char bAddr; // 本设备的地址
	unsigned char bDevType; // 设备是HUB还是功能设备
	unsigned char bUpPort; // 上一级端口地址,0xff代表主端口
	unsigned char bEndpSize; // 端点0包大小
	
	union _KUNO
	{
		struct _HUB
		{
			unsigned char bNumPort; // 从属端口数量
			unsigned char bHUBendp; // 中断端点地址
			unsigned char bInterval; // 最大中断间隔时间
			unsigned char bSlavePort[7]; // 从属端口的地址,默认集线器最多7个端口
		}
		HUB; // 集线器属性

		struct _DEV
		{
			unsigned char bSpeed; // 功能设备的速度，全速还是低速
			// 剩余保留，可放置功能设备的其他属性

		}
		DEV; // 功能设备属性
	}
	KUNO; // 集线器属性,功能设备属性共用
}
INF, *PINF; // 设备信息

typedef struct _NUM
{
	//INF Num[127]; // 最多127个设备连接
	INF Num[50]; // 最多127个设备连接
}
NUM, *PNUM;

// 自己定义的便于区分
#define	HUB_TYPE		0x66
#define FUNCTION_DEV	0x77
#define	FULL_SPEED		0x88
#define LOW_SPEED		0x99
#define FIND_ATTACH		0xaa
#define FIND_REMOVE		0xbb


void ch374u_init(void);
void ch374u_loop(void);

#endif
