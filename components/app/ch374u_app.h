#ifndef __CH374U_APP_H__
#define __CH374U_APP_H__

#include "CH374INC.H"

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

typedef struct _INF
{
	unsigned char bAddr;
	unsigned char bDevType; 
	unsigned char bUpPort; 
	unsigned char bEndpSize;
	
	union _KUNO
	{
		struct _HUB
		{
			unsigned char bNumPort;
			unsigned char bHUBendp;
			unsigned char bInterval;
			unsigned char bSlavePort[7];
		}
		HUB; // 集线器属�?

		struct _DEV
		{
			unsigned char bSpeed;

		}
		DEV; 
	}
	KUNO; 
}
INF, *PINF; 

typedef struct _NUM
{
	INF Num[50]; 
}
NUM, *PNUM;

typedef struct _RootHubDev
{
	uint8_t DeviceStatus;  // 设备状态,0-无设备,1-有设备但尚未初始化,2-有设备但初始化枚举失败,3-有设备且初始化枚举成功
	uint8_t DeviceAddress; // 设备被分配的USB地址
	uint8_t DeviceSpeed;   // 0为低速,非0为全速
	uint8_t DeviceType;	// 设备类型
						   //	union {
						   //		struct MOUSE {
						   //			uint8_t	MouseInterruptEndp;		// 鼠标中断端点号
						   //			uint8_t	MouseIntEndpTog;		// 鼠标中断端点的同步标志
						   //			uint8_t	MouseIntEndpSize;		// 鼠标中断端点的长度
						   //		}
						   //		struct PRINT {
						   //		}
						   //	}
						   //.....    struct  _Endp_Attr   Endp_Attr[4];	//端点的属性,最多支持4个端点
	uint8_t GpVar;		   // 通用变量
	uint8_t Endp_In;
	uint8_t Endp_Out;
	USB_DEV_DESCR dev_descr;
	USB_CFG_DESCR cfg_descr;
	bool send_tog_flag;
	bool recv_tog_flag;
}S_RootHubDev;

typedef struct _DevOnHubPort
{
	uint8_t DeviceStatus;  // 设备状态,0-无设备,1-有设备但尚未初始化,2-有设备但初始化枚举失败,3-有设备且初始化枚举成功
	uint8_t DeviceAddress; // 设备被分配的USB地址
	uint8_t DeviceSpeed;   // 0为低速,非0为全速
	uint8_t DeviceType;	// 设备类型
						   //.....    struct  _Endp_Attr   Endp_Attr[4];	//端点的属性,最多支持4个端点
	uint8_t GpVar;		   // 通用变量
}S_DevOnHubPort;

#define	HUB_TYPE		0x66
#define FUNCTION_DEV	0x77
#define	FULL_SPEED		0x88
#define LOW_SPEED		0x99
#define FIND_ATTACH		0xaa
#define FIND_REMOVE		0xbb

//USB设备相关信息表，CH374U最多支持3个设备
#define ROOT_DEV_DISCONNECT 0
#define ROOT_DEV_CONNECTED 1
#define ROOT_DEV_FAILED 2
#define ROOT_DEV_SUCCESS 3

void Init374Host(void); // 初始化USB主机
bool Query374Interrupt(uint8_t *inter_flag_reg);
void HostDetectInterrupt(uint8_t inter_flag_reg); // 处理USB设备插拔事件中断
void NewDeviceEnum(void);
void DeviceLoop(void);
void QueryADB_Send(uint8_t *buf,uint8_t len);

// CH374传输事务，输入目的端点地址/PID令牌/同步标志，返回同CH375，NAK不重试，超时/出错重试
uint8_t HostTransact374(uint8_t endp_addr, uint8_t pid, bool tog);

// CH374传输事务，输入目的端点地址/PID令牌/同步标志/以mS为单位的NAK重试总时间(0xFFFF无限重试)，返回同CH375，NAK重试，超时出错重试
uint8_t WaitHostTransact374(uint8_t endp_addr, uint8_t pid, bool tog, uint16_t timeout);

uint8_t HostCtrlTransfer374(uint8_t *ReqBuf, uint8_t *DatBuf, uint8_t *RetLen); // 执行控制传输,ReqBuf指向8字节请求码,DatBuf为收发缓冲区
// 如果需要接收和发送数据，那么DatBuf需指向有效缓冲区用于存放后续数据，实际成功收发的总长度保存在ReqLen指向的字节变量中

void SetHostUsbAddr(uint8_t addr); // 设置USB主机当前操作的USB设备地址

void HostEnableRootHub(void); // 启用内置的Root-HUB


uint8_t GetDeviceDescr(uint8_t *buf); // 获取设备描述符

uint8_t GetConfigDescr(uint8_t *buf); // 获取配置描述符

uint8_t SetUsbAddress(uint8_t addr); // 设置USB设备地址

uint8_t SetUsbConfig(uint8_t cfg); // 设置USB设备配置

uint8_t GetHubDescriptor(void); // 获取HUB描述符

uint8_t GetPortStatus(uint8_t port); // 查询HUB端口状态

uint8_t SetPortFeature(uint8_t port, uint8_t select);

uint8_t ClearPortFeature(uint8_t port, uint8_t select);

void DisableRootHubPort(uint8_t index); // 关闭指定的ROOT-HUB端口,实际上硬件已经自动关闭,此处只是清除一些结构状态

void ResetRootHubPort(uint8_t index); // 检测到设备后,复位相应端口的总线,为枚举设备准备,设置为默认为全速

bool EnableRootHubPort(uint8_t index); // 使能ROOT-HUB端口,相应的BIT_HUB?_EN置1开启端口,返回FALSE设置失败(可能是设备断开了)

void SetUsbSpeed(bool FullSpeed); // 设置当前USB速度

void SelectHubPort(uint8_t HubIndex, uint8_t PortIndex); // PortIndex=0选择操作指定的ROOT-HUB端口,否则选择操作指定的ROOT-HUB端口的外部HUB的指定端口

void AnalyzeRootHub(void); // 分析ROOT-HUB状态,处理ROOT-HUB端口的设备插拔事件
//处理HUB端口的插拔事件，如果设备拔出，函数中调用DisableHubPort()函数，将端口关闭，插入事件，置相应端口的状态位

uint8_t AnalyzeHidIntEndp(void); // 从描述符中分析出HID中断端点的地址

uint8_t InitDevice(uint8_t index); // 初始化/枚举指定ROOT-HUB端口的USB设备

uint8_t SearchRootHubPort(uint8_t type); // 搜索指定类型的设备所在的端口号,输出端口号为0xFF则未搜索到

uint16_t SearchAllHubPort(uint8_t type); // 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号,输出端口号为0xFFFF则未搜索到
// 输出高8位为ROOT-HUB端口号,低8位为外部HUB的端口号,低8位为0则设备直接在ROOT-HUB端口上

#endif
