/* Define for CH373 & CH374          */
/* Website:  http://winchiphead.com  */
/* Email:    tech@winchiphead.com    */
/* Author:   W.ch 2005.10            */
/* V1.3 for CH373/CH374              */

#ifndef __CH374INC_H__
#define __CH374INC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ********************************************************************************************************************* */
/* 常用类型和常量定义 */

#ifndef		BOOL
typedef		unsigned char	BOOL;
#endif
#ifndef		TRUE
#define		TRUE	1
#define		FALSE	0
#endif
#ifndef		NULL
#define		NULL	0
#endif

#ifndef UINT8
typedef unsigned char                UINT8;
#endif
#ifndef UINT16
typedef unsigned short               UINT16;
#endif
#ifndef UINT32
typedef unsigned long                UINT32;
#endif
#ifndef PUINT8
typedef unsigned char               *PUINT8;
#endif
#ifndef PUINT16
typedef unsigned short              *PUINT16;
#endif
#ifndef PUINT32
typedef unsigned long               *PUINT32;
#endif
#ifndef UINT8V
typedef unsigned char volatile       UINT8V;
#endif

/* ********************************************************************************************************************* */
/* USB协议层定义 */

/* USB的包标识PID, 主机方式可能用到 */
#ifndef	DEF_USB_PID_SETUP
#define	DEF_USB_PID_NULL	0x00			/* 保留PID, 未定义 */
#define	DEF_USB_PID_SOF		0x05
#define	DEF_USB_PID_SETUP	0x0D
#define	DEF_USB_PID_IN		0x09
#define	DEF_USB_PID_OUT		0x01
#define	DEF_USB_PID_ACK		0x02
#define	DEF_USB_PID_NAK		0x0A
#define	DEF_USB_PID_STALL	0x0E
#define	DEF_USB_PID_DATA0	0x03
#define	DEF_USB_PID_DATA1	0x0B
#define	DEF_USB_PID_PRE		0x0C
#endif

/* ********************************************************************************************************************* */
/* 芯片定义 */

/* 寄存器及缓冲区 */

#define	RAM_ENDP0_SIZE		0x08		/* 端点0的缓冲区长度 */
#define	RAM_ENDP0_TRAN		0x20		/* 端点0发送缓冲区的起始地址 */
#define	RAM_ENDP0_RECV		0x28		/* 端点0接收缓冲区的起始地址 */
#define	RAM_ENDP1_SIZE		0x08		/* 端点1的缓冲区长度 */
#define	RAM_ENDP1_TRAN		0x30		/* 端点1发送缓冲区的起始地址 */
#define	RAM_ENDP1_RECV		0x38		/* 端点1接收缓冲区的起始地址 */
#define	RAM_ENDP2_SIZE		0x40		/* 端点2的缓冲区长度 */
#define	RAM_ENDP2_TRAN		0x40		/* 端点2发送缓冲区的起始地址 */
#define	RAM_ENDP2_RECV		0xC0		/* 端点2接收缓冲区的起始地址 */
#define	RAM_ENDP2_EXCH		0x80		/* 端点2备用缓冲区的起始地址 */
#define	RAM_HOST_TRAN		0x40		/* 主机发送缓冲区的起始地址 */
#define	RAM_HOST_RECV		0xC0		/* 主机接收缓冲区的起始地址 */
#define	RAM_HOST_EXCH		0x80		/* 主机备用缓冲区的起始地址 */

#define	REG_HUB_SETUP		0x02		/* 仅USB主机方式: ROOT-HUB配置 */
#define	REG_HUB_CTRL		0x03		/* 仅USB主机方式: ROOT-HUB控制 */
#define	REG_SYS_INFO		0x04		/* 系统信息, 只读 */
#define	REG_SYS_CTRL		0x05		/* 系统控制, 不受软件复位影响 */
#define	REG_USB_SETUP		0x06		/* USB配置 */
#define	REG_INTER_EN		0x07		/* 中断使能 */
#define	REG_USB_ADDR		0x08		/* USB设备地址 */
#define	REG_INTER_FLAG		0x09		/* 中断标志, 只读, 位写1清0 */
#define	REG_USB_STATUS		0x0A		/* USB状态, 只读 */
#define	REG_USB_LENGTH		0x0B		/* USB长度, 读为当前USB接收长度, 设备方式下写为USB端点2, 主机方式下写为USB主机发送长度 */
#define	REG_USB_ENDP0		0x0C		/* 仅USB设备方式: USB端点0控制 */
#define	REG_USB_ENDP1		0x0D		/* 仅USB设备方式: USB端点1控制 */
#define	REG_USB_H_PID		0x0D		/* 仅USB主机方式: USB主机令牌 */
#define	REG_USB_ENDP2		0x0E		/* 仅USB设备方式: USB端点2控制 */
#define	REG_USB_H_CTRL		0x0E		/* 仅USB主机方式: USB主机控制 */

/* 寄存器的位及常用宏定义 */

#define	BIT_HUB0_EN			0x01		/* HUB0端口的USB传输使能: 0-禁止, 1-允许 */
#define	BIT_HUB0_RESET		0x02		/* HUB0端口的USB总线复位控制: 0-不复位, 1-复位 */
#define	BIT_HUB0_POLAR		0x04		/* HUB0端口的信号极性控制: 0-正极性/全速, 1-负极性/低速 */
#define	BIT_HUB0_ATTACH		0x08		/* HUB0端口的USB设备连接状态(只读): 0-尚未连接/断开/拔出, 1-已经连接/插入 */
#define	BIT_HUB1_DX_IN		0x10		/* HUB1全速正极性时UD+引脚/低速负极性时UD-引脚的采样状态: 0-低电平,速度失配, 1-高电平,速度匹配 */
#define	BIT_HUB2_DX_IN		0x20		/* HUB2全速正极性时UD+引脚/低速负极性时UD-引脚的采样状态: 0-低电平,速度失配, 1-高电平,速度匹配 */
#define	BIT_HUB_PRE_PID		0x40		/* 低速前置包PRE PID输出控制: 0-禁止, 1-允许(外部设备是USB-HUB) */
#define	BIT_HUB_DISABLE		0x80		/* 禁止ROOT-HUB根集线器功能: 0-允许(仅USB主机方式), 1-禁止(默认) */

#define	BIT_HUB1_EN			0x01		/* HUB1端口的USB传输使能: 0-禁止, 1-允许 */
#define	BIT_HUB1_RESET		0x02		/* HUB1端口的USB总线复位控制: 0-不复位, 1-复位 */
#define	BIT_HUB1_POLAR		0x04		/* HUB1端口的信号极性控制: 0-正极性/全速, 1-负极性/低速 */
#define	BIT_HUB1_ATTACH		0x08		/* HUB1端口的USB设备连接状态(只读): 0-尚未连接/断开/拔出, 1-已经连接/插入 */
#define	BIT_HUB2_EN			0x10		/* HUB2端口的USB传输使能: 0-禁止, 1-允许 */
#define	BIT_HUB2_RESET		0x20		/* HUB2端口的USB总线复位控制: 0-不复位, 1-复位 */
#define	BIT_HUB2_POLAR		0x40		/* HUB2端口的信号极性控制: 0-正极性/全速, 1-负极性/低速 */
#define	BIT_HUB2_ATTACH		0x80		/* HUB2端口的USB设备连接状态(只读): 0-尚未连接/断开/拔出, 1-已经连接/插入 */

#define	BIT_INFO_HW_ID		0x03		/* 硬件识别位: 总是常量01, 否则说明读操作或硬件连接有误 */
#define	BIT_INFO_USB_DM		0x04		/* USB总线HUB0的UD-引脚的逻辑电平状态: 0-低电平, 1-高电平 */
#define	BIT_INFO_USB_DP		0x08		/* USB总线HUB0的UD+引脚的逻辑电平状态: 0-低电平, 1-高电平 */
#define	BIT_INFO_CLK_8KHZ	0x10		/* 硬件8KHz时钟位 */
#define	BIT_INFO_SOF_PRES	0x20		/* 硬件1mS定时周期状态,对于USB主机, 1说明将要产生SOF */
#define	BIT_INFO_WAKE_UP	0x40		/* 芯片唤醒状态: 0-正在睡眠或唤醒过程中, 1-已唤醒 */
#define	BIT_INFO_POWER_RST	0x80		/* 硬件上电复位完成状态: 0-正在复位, 1-复位完成 */

#define	BIT_CTRL_OSCIL_OFF	0x01		/* 时钟振荡器控制: 0-允许振荡, 1-停止振荡 */
#define	BIT_CTRL_CLK_12MHZ	0x02		/* 输入时钟频率选择: 0-24MHz, 1-12MHz */
#define	BIT_CTRL_USB_POWER	0x04		/* V3引脚的USB电源调节器控制: 0-开启, 1-禁用 */
#define	BIT_CTRL_RESET_NOW	0x08		/* 芯片软件复位控制: 0-不复位, 1-复位 */
#define	BIT_CTRL_WATCH_DOG	0x10		/* RST引脚和RST#引脚的看门狗复位使能: 0-禁用, 1-启用 */
#define	BIT_CTRL_INT_PULSE	0x20		/* INT#引脚的中断输出方式: 0-低电平中断, 1-低电平脉冲中断 */
#define	BIT_CTRL_OE_POLAR	0x40		/* UEN引脚的USB输出使能极性: 0-高电平使能, 1-低电平使能 */

#define	BIT_SETP_TRANS_EN	0x01		/* 仅USB设备方式: USB设备传输使能: 0-禁止, 1-允许 */
#define	BIT_SETP_PULLUP_EN	0x02		/* 仅USB设备方式: USB上拉电阻控制: 0-禁用上拉电阻, 1-启用上拉电阻 */
#define	BIT_SETP_BUS_CTRL	0x03		/* 仅USB主机方式: USB总线状态控制: 00-正常/空闲, 01-D+低D-低(总线复位), 10-禁用, 11-D+低D-高(总线恢复) */
#define	M_SET_USB_BUS_FREE( old )		( ((old) & ~ BIT_SETP_BUS_CTRL) | 0x00 )		/* 仅USB主机方式: USB总线空闲 */
#define	M_SET_USB_BUS_RESET( old )		( (old) & ~ BIT_SETP_BUS_CTRL | 0x01 )		/* 仅USB主机方式: USB总线状态控制/D+低D-低(总线复位) */
#define	M_SET_USB_BUS_RESUME( old )		( (old) & ~ BIT_SETP_BUS_CTRL | 0x03 )		/* 仅USB主机方式: USB总线状态控制/D+低D-高(总线恢复) */
#define	BIT_SETP_RAM_MODE	0x0C		/* 备用缓冲区应用方式: 00-禁用备用缓冲区, 01-连接接收缓冲区以连续接收128字节, 10-用于发送第二缓冲区, 11-用于接收第二缓冲区 */
#define	M_SET_RAM_MODE_OFF( old )		( (old) & ~ BIT_SETP_RAM_MODE | 0x00 )		/* 备用缓冲区方式/禁用备用缓冲区 */
#define	M_SET_RAM_MODE_128( old )		( (old) & ~ BIT_SETP_RAM_MODE | 0x04 )		/* 备用缓冲区方式/连接接收缓冲区以连续接收128字节 */
#define	M_SET_RAM_MODE_2TX( old )		( (old) & ~ BIT_SETP_RAM_MODE | 0x08 )		/* 备用缓冲区方式/用于发送的第二缓冲区,支持连续发送 */
#define	M_SET_RAM_MODE_2RX( old )		( (old) & ~ BIT_SETP_RAM_MODE | 0x0C )		/* 备用缓冲区方式/用于接收的第二缓冲区,支持连续接收 */
#define	BIT_SETP_LOW_SPEED	0x20		/* USB总线传输速度: 0-12Mbps, 1-1.5Mbps */
#define	BIT_SETP_USB_SPEED	0x30		/* USB总线速率: 00-全速模式/正极性12Mbps, 11-低速模式/负极性1.5Mbps */
#define	BIT_SETP_LED_ACT	0x40		/* 仅USB设备方式: ACT#引脚低电平的激活事件: 0-收发传输过程, 1-USB主机活动 */
#define	BIT_SETP_AUTO_SOF	0x40		/* 仅USB主机方式: 自动产生SOF使能: 0-禁止, 1-允许 */
#define	BIT_SETP_HOST_MODE	0x80		/* USB主从方式选择: 0-设备方式, 1-主机方式 */

#define	BIT_IE_TRANSFER		0x01		/* USB传输完成中断使能, 1有效 */
#define	BIT_IE_BUS_RESET	0x02		/* 仅USB设备方式: USB总线复位中断使能, 1有效 */
#define	BIT_IE_DEV_DETECT	0x02		/* 仅USB主机方式: USB设备检测中断使能, 1有效 */
#define	BIT_IE_USB_SUSPEND	0x04		/* USB总线挂起中断使能, 1有效 */
#define	BIT_IE_USB_RESUME	0x08		/* USB总线恢复/唤醒中断使能, 1有效, 0-使能芯片唤醒完成中断, 1-使能USB总线恢复中断 */
#define	BIT_IE_CLK_OUT_DIV	0xF0		/* 可编程时钟的分频除数: 输出频率为48MHz/(该值+1), 例如: 0000-48MHz, 0001-24MHz, 0010-16MHz, 1111-3MHz */
#define	M_SET_CLK_DIV( old, div )		( (old) & ~ BIT_IE_CLK_OUT_DIV | (div) << 4 )	/* 设置时钟输出分频除数 */

#define	BIT_ADDR_USB_DEV	0x7F		/* 在设备方式下为自身作为USB设备的地址, 在主机方式下为当前被操作的USB设备地址 */

#define	BIT_IF_INTER_FLAG	0x0F		/* 所有的USB中断标志 */
#define	BIT_IF_TRANSFER		0x01		/* USB传输完成中断标志, 1有效, 向该位写1清标志, 该位在每次USB传输完成后自动置1 */
#define	BIT_IF_BUS_RESET	0x02		/* 仅USB设备方式: USB总线复位中断标志, 1有效, 向该位写1清标志, 该位在检测到USB总线复位时自动置1 */
#define	BIT_IF_DEV_DETECT	0x02		/* 仅USB主机方式: USB设备插拔检测中断标志, 1有效, 向该位写1清标志, 该位在检测到USB设备插拔后自动置1 */
#define	BIT_IF_USB_SUSPEND	0x04		/* USB总线挂起中断标志, 1有效, 向该位写1清标志, 该位在检测到USB总线挂起时自动置1 */
#define	BIT_IF_WAKE_UP		0x08		/* 芯片唤醒完成中断标志, 1有效, 向该位写1清标志, 该位在芯片唤醒完成后自动置1 */
#define	BIT_IF_USB_RESUME	0x08		/* USB总线恢复/唤醒中断标志, 1有效, 向该位写1清标志, 该位在检测到USB总线恢复时自动置1 */
#define	BIT_IF_USB_PAUSE	0x10		/* USB传输暂停标志, 1有效, 向该位写1清标志, 该位在每次USB传输完成后自动置1 */
#define	BIT_IF_DEV_ATTACH	0x20		/* USB设备连接状态: 0-尚未连接/断开/拔出, 1-至少有一个USB设备已经连接/插入 */
#define	BIT_IF_USB_OE		0x40		/* UEN引脚的USB输出使能状态: 0-UEN引脚为低电平, 1-UEN引脚为高电平 */
#define	BIT_IF_USB_DX_IN	0x80		/* HUB0全速正极性时UD+引脚/低速负极性时UD-引脚的采样状态: 0-低电平,速度失配, 1-高电平,速度匹配 */
#define	BIT_HUB0_DX_IN		0x80		/* HUB0全速正极性时UD+引脚/低速负极性时UD-引脚的采样状态: 0-低电平,速度失配, 1-高电平,速度匹配 */

#define	BIT_STAT_THIS_ENDP	0x03		/* 仅USB设备方式: USB传输的目的端点号: 00-端点0, 01-端点1, 10-端点2, 11-保留 */
#define	BIT_STAT_THIS_PID	0x0C		/* 仅USB设备方式: USB传输的事务/令牌PID: 00-OUT事务, 01-保留, 10-IN事务, 11-SETUP事务 */
#define	BIT_STAT_PID_ENDP	0x0F		/* 仅USB设备方式: USB传输的事务和端点号,参考后面的USB_INT_EP*定义 */
#define	BIT_STAT_DEV_RESP	0x0F		/* 仅USB主机方式: USB设备的应答PID: XX00=错误或超时,其它值-同PID定义,参考后面的USB_INT_RET_*定义 */
#define	M_IS_HOST_TIMEOUT( status )		( ( (status) & 0x03 ) == 0 )		/* 检查USB主机状态是否为应答超时/出错 */
#define	M_IS_HOST_IN_DATA( status )		( ( (status) & BIT_STAT_DEV_RESP & ~ ( DEF_USB_PID_DATA0 ^ DEF_USB_PID_DATA1 ) ) == ( DEF_USB_PID_DATA0 & DEF_USB_PID_DATA1 ) )	/* 检查是否返回DATA0或者DATA1 */
#define	BIT_STAT_TOG_MATCH	0x10		/* 指示当前的传输是否成功/当前接收的数据包是否同步: 0-不同步, 1-同步 */
#define	BIT_STAT_BUS_RESET	0x20		/* 当前USB总线复位状态: 0-没有复位, 1-正在复位 */
#define	BIT_STAT_SUSPEND	0x40		/* 当前USB总线挂起状态: 0-总线有活动, 1-总线挂起 */
#define	BIT_STAT_SIE_FREE	0x80		/* 当前USB接口引擎SIE的状态: 0=忙/正在传输, 1=空闲/等待 */

#define	BIT_EP0_TRAN_RESP	0x0F		/* 仅USB设备方式: 端点0发送响应: 0000~1000-应答数据长度0~8, 1110-应答NAK, 1111-应答STALL,其它值-禁用 */
#define	M_SET_EP0_TRAN_ACK( old, len )	( (old) & ~ BIT_EP0_TRAN_RESP | (len) & 0x0F )	/* 仅USB设备方式: 端点0发送响应/应答ACK */
#define	M_SET_EP0_TRAN_NAK( old )		( (old) & ~ BIT_EP0_TRAN_RESP | 0x0E )		/* 仅USB设备方式: 端点0发送响应/应答NAK */
#define	M_SET_EP0_TRAN_STA( old )		( (old) & ~ BIT_EP0_TRAN_RESP | 0x0F )		/* 仅USB设备方式: 端点0发送响应/应答STALL */
#define	BIT_EP0_RECV_RESP	0x30		/* 仅USB设备方式: 端点0接收响应: 00-应答ACK, 01-禁用, 10-应答NAK, 11-应答STALL */
#define	M_SET_EP0_RECV_ACK( old )		( (old) & ~ BIT_EP0_RECV_RESP | 0x00 )		/* 仅USB设备方式: 端点0接收响应/应答ACK */
#define	M_SET_EP0_RECV_NAK( old )		( (old) & ~ BIT_EP0_RECV_RESP | 0x20 )		/* 仅USB设备方式: 端点0接收响应/应答NAK */
#define	M_SET_EP0_RECV_STA( old )		( (old) & ~ BIT_EP0_RECV_RESP | 0x30 )		/* 仅USB设备方式: 端点0接收响应/应答STALL */
#define	BIT_EP0_TRAN_TOG	0x40		/* 仅USB设备方式: 端点0发送同步标志: 0-DATA0, 1-DATA1 */
#define	BIT_EP0_RECV_TOG	0x80		/* 仅USB设备方式: 端点0接收同步标志: 0-DATA0, 1-DATA1 */

#define	BIT_EP1_TRAN_RESP	0x0F		/* 仅USB设备方式: 端点1发送响应: 0000~1000-应答数据长度0~8, 1110-应答NAK, 1111-应答STALL,其它值-禁用 */
#define	M_SET_EP1_TRAN_ACK( old, len )	( (old) & ~ BIT_EP1_TRAN_RESP | (len) & 0x0F )	/* 仅USB设备方式: 端点1发送响应/应答ACK */
#define	M_SET_EP1_TRAN_NAK( old )		( (old) & ~ BIT_EP1_TRAN_RESP | 0x0E )		/* 仅USB设备方式: 端点1发送响应/应答NAK */
#define	M_SET_EP1_TRAN_STA( old )		( (old) & ~ BIT_EP1_TRAN_RESP | 0x0F )		/* 仅USB设备方式: 端点1发送响应/应答STALL */
#define	BIT_EP1_RECV_RESP	0x30		/* 仅USB设备方式: 端点1接收响应: 00-应答ACK, 01-禁用, 10-应答NAK, 11-应答STALL */
#define	M_SET_EP1_RECV_ACK( old )		( (old) & ~ BIT_EP1_RECV_RESP | 0x00 )		/* 仅USB设备方式: 端点1接收响应/应答ACK */
#define	M_SET_EP1_RECV_NAK( old )		( (old) & ~ BIT_EP1_RECV_RESP | 0x20 )		/* 仅USB设备方式: 端点1接收响应/应答NAK */
#define	M_SET_EP1_RECV_STA( old )		( (old) & ~ BIT_EP1_RECV_RESP | 0x30 )		/* 仅USB设备方式: 端点1接收响应/应答STALL */
#define	BIT_EP1_TRAN_TOG	0x40		/* 仅USB设备方式: 端点1发送同步标志: 0-DATA0, 1-DATA1 */
#define	BIT_EP1_RECV_TOG	0x80		/* 仅USB设备方式: 端点1接收同步标志: 0-DATA0, 1-DATA1 */

#define	BIT_HOST_PID_ENDP	0x0F		/* 仅USB主机方式: 目的端点号: 0000~1111-端点号0~15 */
#define	BIT_HOST_PID_TOKEN	0xF0		/* 仅USB主机方式: 指定事务/令牌PID: 1101-SETUP事务, 0001-OUT事务, 1001-IN事务, 0101-SOF包,其它值-禁用 */
#define	M_MK_HOST_PID_ENDP( pid, endp )	( (pid) << 4 | ((endp) & BIT_HOST_PID_ENDP ))		/* 用事务/令牌PID和目的端点号生成USB主机令牌数据 */

#define	BIT_EP2_TRAN_RESP	0x03		/* 仅USB设备方式: 端点2发送响应: 00-应答数据, 01-同步/等时传输, 10-应答NAK, 11-应答STALL */
#define	M_SET_EP2_TRAN_ACK( old )		( (old) & ~ BIT_EP2_TRAN_RESP | 0x00 )		/* 仅USB设备方式: 端点2发送响应/应答ACK */
#define	M_SET_EP2_TRAN_ISO( old )		( (old) & ~ BIT_EP2_TRAN_RESP | 0x01 )		/* 仅USB设备方式: 端点2发送响应/同步/等时传输/无需应答 */
#define	M_SET_EP2_TRAN_NAK( old )		( (old) & ~ BIT_EP2_TRAN_RESP | 0x02 )		/* 仅USB设备方式: 端点2发送响应/应答NAK */
#define	M_SET_EP2_TRAN_STA( old )		( (old) & ~ BIT_EP2_TRAN_RESP | 0x03 )		/* 仅USB设备方式: 端点2发送响应/应答STALL */
#define	BIT_EP2_RECV_RESP	0x30		/* 仅USB设备方式: 端点2接收响应: 00-应答ACK, 01-同步/等时传输, 10-应答NAK, 11-应答STALL */
#define	M_SET_EP2_RECV_ACK( old )		( (old) & ~ BIT_EP2_RECV_RESP | 0x00 )		/* 仅USB设备方式: 端点2接收响应/应答ACK */
#define	M_SET_EP2_RECV_ISO( old )		( (old) & ~ BIT_EP2_RECV_RESP | 0x10 )		/* 仅USB设备方式: 端点2接收响应/同步/等时传输/不作应答 */
#define	M_SET_EP2_RECV_NAK( old )		( (old) & ~ BIT_EP2_RECV_RESP | 0x20 )		/* 仅USB设备方式: 端点2接收响应/应答NAK */
#define	M_SET_EP2_RECV_STA( old )		( (old) & ~ BIT_EP2_RECV_RESP | 0x30 )		/* 仅USB设备方式: 端点2接收响应/应答STALL */
#define	BIT_EP2_TRAN_TOG	0x40		/* 仅USB设备方式: 端点2发送同步标志: 0-DATA0, 1-DATA1 */
#define	BIT_EP2_RECV_TOG	0x80		/* 仅USB设备方式: 端点2接收同步标志: 0-DATA0, 1-DATA1 */

#define	BIT_HOST_TRAN_ISO	0x01		/* 仅USB主机方式: 主机发送的传输类型: 0-控制/批量/中断传输, 1-同步/等时传输 */
#define	BIT_HOST_START		0x08		/* 仅USB主机方式: 主机传输启动控制: 0-暂停, 1-启动传输,完成后自动清0 */
#define	BIT_HOST_RECV_ISO	0x10		/* 仅USB主机方式: 主机接收的传输类型: 0-控制/批量/中断传输, 1-同步/等时传输 */
#define	BIT_HOST_TRAN_TOG	0x40		/* 仅USB主机方式: 主机发送同步标志: 0-DATA0, 1-DATA1 */
#define	BIT_HOST_RECV_TOG	0x80		/* 仅USB主机方式: 主机接收同步标志: 0-DATA0, 1-DATA1 */

/* 状态及命令等常量 */

#define	CMD_SPI_374READ		0xC0		/* SPI读操作命令 */
#define	CMD_SPI_374WRITE	0x80		/* SPI写操作命令 */

/* ********************************************************************************************************************* */
/* USB状态 */

/* 以下状态代码用于USB设备方式 */
/*   位4指示当前接收的数据包是否同步, 0=不同步, 1-同步 */
/*   位3-位2指示当前事务, 00=OUT, 10=IN, 11=SETUP */
/*   位1-位0指示当前端点, 00=端点0, 01=端点1, 10=端点2 */
#ifndef	USB_INT_EP0_SETUP
#define	USB_INT_EP0_SETUP	0x0C			/* USB端点0的SETUP */
#define	USB_INT_EP0_OUT		0x00			/* USB端点0的OUT */
#define	USB_INT_EP0_IN		0x08			/* USB端点0的IN */
#define	USB_INT_EP1_OUT		0x01			/* USB端点1的OUT */
#define	USB_INT_EP1_IN		0x09			/* USB端点1的IN */
#define	USB_INT_EP2_OUT		0x02			/* USB端点2的OUT */
#define	USB_INT_EP2_IN		0x0A			/* USB端点2的IN */
#endif

/* 以下状态代码用于USB主机方式 */
/*   位4指示当前接收的数据包是否同步, 0=不同步, 1-同步 */
/*   位3-位0指示USB设备的应答: 0010=ACK, 1010=NAK, 1110=STALL, 0011=DATA0, 1011=DATA1, XX00=应答错误或者超时无应答 */
#ifndef	USB_INT_RET_ACK
#define	USB_INT_RET_ACK		DEF_USB_PID_ACK		/* 错误:对于OUT/SETUP事务返回ACK */
#define	USB_INT_RET_NAK		DEF_USB_PID_NAK		/* 错误:返回NAK */
#define	USB_INT_RET_STALL	DEF_USB_PID_STALL	/* 错误:返回STALL */
#define	USB_INT_RET_DATA0	DEF_USB_PID_DATA0	/* 错误:对于IN事务返回DATA0 */
#define	USB_INT_RET_DATA1	DEF_USB_PID_DATA1	/* 错误:对于IN事务返回DATA1 */
#define	USB_INT_RET_TOUT	0x00			/* 错误:应答错误或者超时无应答 */
#define	USB_INT_RET_TOUT1	0x04			/* 错误:应答错误或者超时无应答 */
#define	USB_INT_RET_TOUT2	0x08			/* 错误:应答错误或者超时无应答 */
#define	USB_INT_RET_TOUT3	0x0C			/* 错误:应答错误或者超时无应答 */
#endif

/* 以下状态代码1XH用于USB主机方式的操作状态代码, 可以自行定义任意值, 下面的定义仅为了兼容CH375 */
#ifndef	USB_INT_SUCCESS
#define	USB_INT_SUCCESS		0x14			/* USB事务或者传输操作成功 */
#define	USB_INT_CONNECT		0x15			/* 检测到USB设备连接事件 */
#define	USB_INT_DISCONNECT	0x16			/* 检测到USB设备断开事件 */
#define	USB_INT_BUF_OVER	0x17			/* USB控制传输的数据太多, 缓冲区溢出 */
#define	USB_INT_DISK_ERR	0x1F			/* USB存储器操作失败 */
#endif
#ifndef	USB_INT_CONNECT_LS
#define	USB_INT_CONNECT_LS	0x13			/* 检测到低速USB设备连接事件 */
#endif

/* ********************************************************************************************************************* */
/* 常用USB结构和相关常量 */

typedef struct _USB_SETUP_REQ {
	UINT8	bType;
	UINT8	bReq;
	UINT8	wValueL;
	UINT8	wValueH;
	UINT8	wIndexL;
	UINT8	wIndexH;
	UINT8	wLengthL;
	UINT8	wLengthH;
} USB_SETUP_REQ, *PUSB_SETUP_REQ;

typedef struct _USB_DEVICE_DESCRIPTOR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bcdUSBL;
    UINT8 bcdUSBH;
    UINT8 bDeviceClass;
    UINT8 bDeviceSubClass;
    UINT8 bDeviceProtocol;
    UINT8 bMaxPacketSize0;
    UINT8 idVendorL;
    UINT8 idVendorH;
    UINT8 idProductL;
    UINT8 idProductH;
    UINT8 bcdDeviceL;
    UINT8 bcdDeviceH;
    UINT8 iManufacturer;
    UINT8 iProduct;
    UINT8 iSerialNumber;
    UINT8 bNumConfigurations;
} USB_DEV_DESCR, *PUSB_DEV_DESCR;

typedef struct _USB_CONFIG_DESCRIPTOR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 wTotalLengthL;
    UINT8 wTotalLengthH;
    UINT8 bNumInterfaces;
    UINT8 bConfigurationValue;
    UINT8 iConfiguration;
    UINT8 bmAttributes;
    UINT8 MaxPower;
} USB_CFG_DESCR, *PUSB_CFG_DESCR;

typedef struct _USB_INTERF_DESCRIPTOR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bInterfaceNumber;
    UINT8 bAlternateSetting;
    UINT8 bNumEndpoints;
    UINT8 bInterfaceClass;
    UINT8 bInterfaceSubClass;
    UINT8 bInterfaceProtocol;
    UINT8 iInterface;
} USB_ITF_DESCR, *PUSB_ITF_DESCR;

typedef struct _USB_HID_DESCRIPTOR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bcdHIDL;
    UINT8 bcdHIDH;
    UINT8 bCountryCode;
    UINT8 bNumDescriptors;
    UINT8 bDescriptorType2;
    UINT8 bDescriptorLengthL;
    UINT8 bDescriptorLengthH;
} USB_HID_DESCR, *PUSB_HID_DESCR;

typedef struct _USB_ENDPOINT_DESCRIPTOR {
    UINT8 bLength;
    UINT8 bDescriptorType;
    UINT8 bEndpointAddress;
    UINT8 bmAttributes;
    UINT8 wMaxPacketSize;
    UINT8 wMaxPacketSize1;
    UINT8 bInterval;
} USB_ENDP_DESCR, *PUSB_ENDP_DESCR;

typedef struct _USB_CONFIG_DESCRIPTOR_LONG {
	USB_CFG_DESCR	cfg_descr;
	USB_ITF_DESCR	itf_descr;
	USB_ENDP_DESCR	endp_descr[4];
} USB_CFG_DESCR_LONG, *PUSB_CFG_DESCR_LONG;

/* USB请求类型 */
#ifndef	DEF_USB_REQ_TYPE
#define	DEF_USB_REQ_READ		0x80			/* 控制读操作 */
#define	DEF_USB_REQ_WRITE		0x00			/* 控制写操作 */
#define	DEF_USB_REQ_TYPE		0x60			/* 控制请求类型 */
#define	DEF_USB_REQ_STAND		0x00			/* 标准请求 */
#define	DEF_USB_REQ_CLASS		0x20			/* 设备类请求 */
#define	DEF_USB_REQ_VENDOR		0x40			/* 厂商请求 */
#define	DEF_USB_REQ_RESERVE		0x60			/* 保留请求 */
#endif

/* USB标准设备请求, RequestType的位6位5=00(Standard) */
#ifndef	DEF_USB_GET_DESCR
#define	DEF_USB_CLR_FEATURE		0x01
#define	DEF_USB_SET_FEATURE		0x03
#define	DEF_USB_GET_STATUS		0x00
#define	DEF_USB_SET_ADDRESS		0x05
#define	DEF_USB_GET_DESCR		0x06
#define	DEF_USB_SET_DESCR		0x07
#define	DEF_USB_GET_CONFIG		0x08
#define	DEF_USB_SET_CONFIG		0x09
#define	DEF_USB_GET_INTERF		0x0A
#define	DEF_USB_SET_INTERF		0x0B
#define	DEF_USB_SYNC_FRAME		0x0C
#endif

/* 描述符类型 */
#ifndef	USB_DEVICE_DESCR_TYPE
#define	USB_DEVICE_DESCR_TYPE		0x01
#define	USB_CONFIG_DESCR_TYPE		0x02
#define	USB_STRING_DESCR_TYPE		0x03
#define	USB_INTERF_DESCR_TYPE		0x04
#define	USB_ENDP_DESCR_TYPE			0x05
#define	USB_POWER_DESCR_TYPE		0x06
#define	USB_HID_DESCR_TYPE			0x21
#define	USB_REPORT_DESCR_TYPE		0x22
#define	USB_PHYSIC_DESCR_TYPE		0x23
#endif

/* 端点类型和属性 */
#ifndef	USB_ENDP_TYPE_MASK
#define	USB_ENDP_DIR_MASK			0x80
#define	USB_ENDP_TYPE_MASK			0x03
#define	USB_ENDP_TYPE_CTRL			0x00
#define	USB_ENDP_TYPE_ISOCH			0x01
#define	USB_ENDP_TYPE_BULK			0x02
#define	USB_ENDP_TYPE_INTER			0x03
#endif

/* 设备类别 */
#ifndef	USB_DEV_CLASS_HUB
#define	USB_DEV_CLASS_RESERVED		0x00
#define	USB_DEV_CLASS_AUDIO			0x01
#define	USB_DEV_CLASS_COMMUNIC		0x02
#define	USB_DEV_CLASS_HUMAN_IF		0x03
#define	USB_DEV_CLASS_MONITOR		0x04
#define	USB_DEV_CLASS_PHYSIC_IF		0x05
#define	USB_DEV_CLASS_POWER			0x06
#define	USB_DEV_CLASS_PRINTER		0x07
#define	USB_DEV_CLASS_STORAGE		0x08
#define	USB_DEV_CLASS_HUB			0x09
#define	USB_DEV_CLASS_VEN_SPEC		0xFF
#endif

#define	DEFAULT_ENDP0_SIZE	8				/* 默认的端点0最大包长度 */

#ifdef __cplusplus
}
#endif

#endif
