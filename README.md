# About-Embedded
关于嵌入式


=== 
### Linux 命令
>
- cp 覆盖拷贝：


===
## RTOS
>
1. freeRTOS
2. Linux
3. HuaweiLiteOS
4. U-BOOT
>	> 1. 编译：make ARCH=arm CROSS_COMPILE=arm-hisiv600-linux- hi355x_xxx_config
>	> * make ARCH=arm CROSS_COMPILE=arm-hisiv600-linux-



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

> ### 外设器件
>	>
1. 蓝牙
2. LCD
3. 陀螺仪
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
## 网络应用
>
1. CS网络应用程序。
2. RTSP/LIVE555
3. HTTP服务器
>	>
	1. HTTP文件服务器。 192.168.1.100 可以显示当前服务器目录下的文件列表。HTTP服务器放在SD卡目录下。
#### HTTP协议

1. 超文本协议，客户端和服务端请求和应答的标准（TCP）。
* 由HTTP客户端发起一个请求，建立一个到服务器指定端口
* HTTP服务器则在那个端口监听客户端发送过来的请求
* 一旦收到请求，服务器（向客户端）发回一个状态行
* 消息的消息体可能是请求的文件、错误消息、或者其它一些信息
* HTTP使用TCP而不是UDP的原因在于（打开）一个网页必须传送很多数据，而TCP协议提供传输控制，按顺序组织数据，和错误纠正。








4. 


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






























