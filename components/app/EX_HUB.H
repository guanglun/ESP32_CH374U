#ifndef __EX_HUB_H__
#define __EX_HUB_H__

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



#endif
/////////// 动态集线器信息分配表
