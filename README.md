# ATouch

## ATouch项目中的硬件源码部分，ESP32+CH374U  

## **ATouch线上文档请点击下面链接** 

[ATouch线上文档](http://guanglundz.com/atouch)  

## 说明

* 串口指令（波特率921600 不加回车换行符）


| 命令 | 效果 | 说明 |
|:-----:|:-----:|:-----:|
| open | 打开串口传输模式 | 将会只传输状态和键鼠信息（关闭log） |
| close | 关闭串口传输模式 | 停止传输状态和键鼠信息 |
| slogn | 设置LOG级别 NONE | 停止输出任何LOG |
| sloge | 设置LOG级别 ERROR | 只输出 ERROR LOG |
| slogw | 设置LOG级别 WARN | 输出 WARN 及以上 LOG |
| slogi | 设置LOG级别 INFO | 输出 INFO 及以上 LOG |
| slogd | 设置LOG级别 DEBUG | 输出 DEBUG 及以上 LOG |
| slogv | 设置LOG级别 VERBOSE |  输出所有LOG |

* 上电默认LOG模式：INFO （slogi）  

<br/>

## 下载资源
[手机APP](https://gitee.com/guanglunking/ATouch/releases)  
[WINDOWS客户端](https://gitee.com/guanglunking/ATouchClient/releases)  
[LINUX客户端【请自行编译】](https://gitee.com/guanglunking/ATouchClient)  
[ESP32客户端固件](https://gitee.com/guanglunking/ESP32_CH374U/releases)  
[Android后台固件](https://gitee.com/guanglunking/ATouchService/releases)  

## 源码和资料
[ATouch安卓APP源码](https://gitee.com/guanglunking/ATouch)【开发环境：AndroidStudio】  
[ATouch WIN&LINUX客户端源码（支持嵌入式LINUX）](https://gitee.com/guanglunking/ATouchClient)【开发环境：gcc or MinGW】  
[ATouch板子源码](https://gitee.com/guanglunking/ESP32_CH374U) 【开发环境：Linux SDK:ESP-DIF3.2】  
[ATouch安卓后台程序源码](https://gitee.com/guanglunking/ATouchService)【开发环境：android-ndk-r13b】  
[淘宝店铺](https://item.taobao.com/item.htm?id=595635571591)  
[演示视频](https://www.bilibili.com/video/av53687214)  

## 按键映射 操作说明

| 按键 | 效果 |
|:-----:|:-----:|
| 鼠标左键 | 射击（攻击）或触摸指针位置（唤醒鼠标指针的模式下） |
| 鼠标中键 | 唤醒鼠标指针 和 隐藏鼠标指针切换 |
| 鼠标右键 | 打开瞄准镜 |
| W | 前进（W+Shift为加速向前跑） |
| S | 后退 |
| A | 左走 |
| D | 右走 |
| Ctrl(左) | 趴下 |
| Alt(左) | 蹲下 |
| 空格 | 跳跃 |
| Z | 开车 |
| X | 上副驾驶 |
| C | 下车 |
| Q | 左武器切换 |
| E | 右武器切换 |
| R | 换弹药 |
| M | 地图显示、关闭 |
| B | 背包显示、关闭 |
| F | 环视（身体及行动的方向不变看四周情况） |
| G | 用药 |
| H | 救援 |

<br/>

![效果图](https://images.gitee.com/uploads/images/2020/0408/110002_b982beff_683968.png "atouch2.png")

![设置界面](https://images.gitee.com/uploads/images/2020/0408/110030_b23d7f55_683968.png "atouch3.png")

![gif](https://images.gitee.com/uploads/images/2020/0423/150126_eb0fbd7b_683968.gif "bili_v_d_1587624907338.gif")

![gif](https://images.gitee.com/uploads/images/2020/0423/150325_ff6f7a4a_683968.gif "bili_v_d_1587625362445.gif")

### 基本命令

## 配置项目

`make menuconfig`

## 编译

`make -j4 all`

## 编译 烧写

`make -j4 flash`

## 编译 烧写 调试
`make -j4 flash monitor`

(To exit the serial monitor, type ``Ctrl-]``.)

* 更新日志  

| 时间 | 内容 |
|:-----:|:-----:|
| 2020/5/5 | 1.做了一下HID Report的简单解析 2.兼容更多键盘鼠标（暂时不兼容一个接收器的无线键鼠）  3.串口调试和数据连接更改为波特率921600  4.推出V1.0.9版本   |
| 2020/4/25 | 1.解决一些手机打开ADB后/etc/mkshrc文件调用busybox的resize获取终端串口大小的问题 |

## 欢迎加入 光流电子交流群  558343678  

