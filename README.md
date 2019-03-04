# About-Embedded
关于嵌入式

===
### ubuntu 镜像下载
>
- http://www.ubuntu.org.cn/download/ubuntu-kylin


##### windows下安装gcc/g++
- 下载 mingw；www.mingw.org
- 运行，选择mingw32-gcc-g++，左上角安装
- 添加环境变量C:\MinGW\bin
- 验证g++ -v



### cmake
>
- 

=== 
### Linux 命令
>
- cp 覆盖拷贝：
- 查看指定软件有多少个版本：apt-cache madison package
- 搜索可安装的包： apt-cache search package
- 更新源：/etc/apt/sources.list, apt-get update
- 添加DNS：
1. 配置：sudo vim /etc/resolvconf/resolv.conf.d/base；添加nameserver 8.8.8.8
2. 执行：resolvconf -u
3. 验证：cat /etc/resolv.conf

- 添加动态库路径：export LD_LIBRARY_PATH+=:/usr/local/lib/
- 添加环境变量：export PATH+=:/usr/local/lib/
- 查看文件格式： file -b filename
- 查看默认网关：route
- 添加网关：route add default gw 192.168.1.1
- 文件传输工具，http命令行工具：curl
- 关闭防火墙： sudo ufw disable
- 查看ubuntu版本：sudo lsb_release -a

===
## RTOS
>
1. freeRTOS
2. Linux
3. HuaweiLiteOS
4. U-BOOT
>	> 1. 编译：make ARCH=arm CROSS_COMPILE=arm-hisiv600-linux- hi355x_xxx_config
>	> * make ARCH=arm CROSS_COMPILE=arm-hisiv600-linux-

* ubuntu:
网络：



=========================================
## 常用工具的使用
>
### git
* git ignore
>	> 1.  touch ./.git/.gitignore
>	> *  git config --global core.excludesfile ./.git/.gitignore
>	> *  vi ./.git/.gitignore [app.IAB etc...]
>	> *  windows在顶层目录下创建.gitignore文件，编辑好即可。

- 忽略filemode： git config --add core.filemode false; 
- 添加所有改变文件：git add -A .
- 添加所有修改和删除的文件（不包括新文件）：git add -u
 

### 搭建samba服务器
> 1. apt-get install samba
> * /etc/init.d/samba start
> 2. 配置/etc/samba/smb.conf

```
[work]
     path = /work/
     available = yes
     browsealbe = yes
     public = yes
     writable = yes
```

> * service smb restart




=========================================
## 脚本
>	
1. shell
2. 


## UI
> 
1. QT
2. HIGV
3. littleGL
>	>
github:https://github.com/littlevgl/lvgl
>	>  
4.
5.




=========================================
## 设备驱动
------
> ### 总线
>	>  
1. USB驱动
2. I2C驱动
3. CAN总线
4. SPI驱动
5. mipi总线
6. SDIO总线
7. HDMI
8. VGA
9. DVI
10. 

#### 外设器件
>
1. 蓝牙
2. LCD
#### 陀螺仪
测试角速度传感器
>
MPU9250  
1. 角速度全格感测范围 ±250,±500,±1000, ±2000°/sec (dergee per second）   
2. 16-bit ADC  
3. 

数据校准  
1. 大多数校准去掉零点偏移量，采集一定的数据求平均  
https://blog.csdn.net/csshuke/article/details/80389858  



4. GPS 
5. 无线网卡模块
6. 网卡
7. 电机
8. SD卡
9. MMC
10. DDR
11. NANDFLASH
12. NORFLASH
13. IO
14. SPERKER
15. MIC
16. 舵机
17. 交叉芯片
18. ADV7619
19. 
20. 





=========================================
### 网络

#### C/S
>
1. CS网络应用程序。
2. RTSP/LIVE555
3. HTTP服务器
>	>
	1. HTTP文件服务器。 192.168.1.100 可以显示当前服务器目录下的文件列表。HTTP服务器放在SD卡目录下。

#### HTTP协议
>
1. 超文本协议，客户端和服务端请求和应答的标准（TCP）。
* 由HTTP客户端发起一个请求，建立一个到服务器指定端口
* HTTP服务器则在那个端口监听客户端发送过来的请求
* 一旦收到请求，服务器（向客户端）发回一个状态行
* 消息的消息体可能是请求的文件、错误消息、或者其它一些信息
* HTTP使用TCP而不是UDP的原因在于（打开）一个网页必须传送很多数据，而TCP协议提供传输控制，按顺序组织数据，和错误纠正。


#### 翻墙
>
- 翻墙定义：绕过IP封锁，内容过滤，域名劫持，流量限制等，实现对网络内容的访问。
- VPN：








=========================================


## 视频

### HI3559A海思平台
>
#### 常规用法
1.VI->VPSS->VENC->VO
2.

#### 智能视频模块
> 
1.IVS

#### 智能加速模块IVE
>
1.
2.
3.

#### FPGA模块
>
1.

### FFMPEG
>
1.


### OPENGL
>
1.


=========================================
## 音频
>
1. 
2. 
3. 
4. 




=========================================
##### 封装
FC-CSP


### **Program Skill**









##### PID
参考文章：https://blog.csdn.net/asszz/article/details/79271023
比例、积分、微分
- 比例控制
- 积分控制

#### 系统移植常用命令
setenv ipaddr 192.168.1.105
setenv netmask 255.255.254.0
setenv serverip 192.168.1.101


mw.b 0x44000000 0xff 0x100000
tftp 0x44000000 u-boot-hi3559av100.bin
nand erase 0x0 0x100000
nand write 0x44000000 0x0 0x1000000


mw.b 0x44000000 0xff 0x900000
tftp 0x44000000 uImage_hi3559av100_multi-core
nand erase 0x100000 0xA00000
nand write 0x44000000 0x100000 0x900000

mw.b 0x44000000 0xff 0x1000000
tftp 0x44000000 rootfs_hi3559av100_128k.jffs2
nand erase 0xA00000 0x1000000
nand write.jffs2 0x44000000 0xA00000 0x4f6b98

setenv bootargs 'mem=512M console=ttyAMA0,115200 root=/dev/mtdblock2 rw rootfstype=yaffs2 init=/linuxrc mtdparts=hinand:1M(boot),9M(kernel),16M(rootfs)'



setenv bootcmd 'nand read 0x44000000 0x100000 0x900000;bootm 0x44000000'

saveenv


mw.b 0x44000000 0xff 0x1000000
tftp 0x44000000 rootfs_hi3559av100_2k_24bit.yaffs2
nand erase 0xA00000 0x1000000
nand write.yaffs 0x44000000 0xA00000 0xBEFC00


osdrv/pub/bin/pc/mkfs.jffs2 -d osdrv/pub/rootfs_glibc_xxx -l -e 0x20000 -o osdrv/pub/rootfs_hi3559av100_128k.jffs2

##### kernel编译错误汇总
1.
```
arch/arm64/kernel/entry.S:401: Error: operand 1 should be a floating-point register -- `stp lr,x21,[sp,#240]'
arch/arm64/kernel/entry.S:405: Error: operand 1 should be a floating-point register -- `stp lr,x21,[sp,#240]'
```
解决：编译器版本不对。



---
1. 
```
NAND write: device 0 offset 0xa00000, size 0x4f4f84
!!! The yaffs2 filesystem image has no tag information.
 please update your mkyaffs2image tool, and remake yaffs2 filesystem image.
hisilicon # 
```

##### 相机标定

单目标定：
实时的从摄像机获取拍摄到的棋盘进行标定
是已经获得了一些棋盘的图片来标定

使用opencv进行单目标定：

通过添加输入参数的代码，然后执行
source/sample/cpp/imagelist_creator.cpp
Project3.exe imagelist.yaml  left01.jpg left02.jpg left03.jpg left04.jpg left05.jpg left06.jpg left07.jpg left08.jpg left09.jpg left10.jpg left11.jpg left12.jpg left13.jpg left14.jpg right01.jpg right02.jpg right03.jpg right04.jpg right05.jpg right06.jpg right07.jpg right08.jpg right09.jpg right10.jpg right11.jpg right12.jpg right13.jpg right14.jpg 
生成imagelist.yaml

source/sample/cpp/calibration.cpp
Project3.exe -w=6 -h=9 imagelist.yaml

双目标定：
https://blog.csdn.net/t247555529/article/details/48046859

张正友标定：
单平面棋盘格的摄像机标定方法
资料文章：
https://blog.csdn.net/weixinhum/article/details/78161567



##### 机器视觉的光源选择
一个完整的机器视觉系统一般由光源，镜头，相机，采集卡以及处理软件构成。
根据目标的大小以及变化，拍摄距离及变化等决定光学成像系统的设计和控制。

稳定、清晰、高对比度成像
680nm 和780nm


相机分 
- 面阵相机 
面阵相机一次扫描拍摄的是一个二位矩阵
尺寸规格一般为1/3 1/2 3/4 1 4/3
- 线阵相机
线阵相机一次扫描是一个一维矩阵，
尺寸规格有1K,2K,4K,8K,12K,16K

- 图像处理技术
模式识别技术
图像i安索技术
像元细胞技术
三维重建技术
图像的回复与还原技术
立体视觉技术
图像系类分析技术
运算的软硬件优化技术

- 机器视觉系统的应用
工业视觉方面：
定位
PCB板检测，Wafer检测，包装检测，LCD检测，瓶盖检测，货币检测，IC卡检测，
识别应用
OCR,一维码二维码，指纹，颜色识别，车牌，人眼
天文，航天，纤维呈现，高速记录，军工行业，医疗影像。



OSe光源

##### 镜头分类
远心镜头
镜像镜头
变焦便被镜头
多CCD镜头
高放大倍率镜头
大靶面镜头


http://www.opptec.com/pd.jsp?id=93#_jcp=2
东莞市奥普泰克光源科技有限公司

滤光片：680 780








