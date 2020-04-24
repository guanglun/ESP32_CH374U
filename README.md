# ATouch

## ATouch项目中的硬件源码部分，ESP32+CH374U  

* 串口指令（波特率115200 不加回车换行符）


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

![效果图](https://images.gitee.com/uploads/images/2020/0408/110002_b982beff_683968.png "atouch2.png")


ATouch安卓APP源码：  
[https://gitee.com/guanglunking/ATouch](https://gitee.com/guanglunking/ATouch)  
【开发环境：AndroidStudio】

ATouch板子源码：  
[https://gitee.com/guanglunking/ESP32_CH374U](https://gitee.com/guanglunking/ESP32_CH374U)  
【开发环境：Linux SDK:ESP-DIF3.2】

ATouch安卓后台程序源码：   
[https://gitee.com/guanglunking/ATouchService](https://gitee.com/guanglunking/ATouchService)   
【开发环境：android-ndk-r13b】

APP下载地址：   
[https://gitee.com/guanglunking/ATouch/blob/master/app/release/ATouchV1.2.apk](https://gitee.com/guanglunking/ATouch/blob/master/app/release/ATouchV1.2.apk)


淘宝店铺：  
[https://item.taobao.com/item.htm?id=595635571591](https://item.taobao.com/item.htm?id=595635571591)  

演示视频：  
[https://www.bilibili.com/video/av53687214](https://www.bilibili.com/video/av53687214)  

![设置界面](https://images.gitee.com/uploads/images/2020/0408/110030_b23d7f55_683968.png "atouch3.png")

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