# VirtualTouchPadForLinux

#### 项目介绍
将手机屏幕作为电脑的触摸板,支持五个手指。

#### 软件架构
1. VirtualTouchPadDriver：虚拟触摸板的设备驱动。
2. VirtualTouchPadServer：daemon进程，用于接受手机、平板设备发送过来的数据，并发送给Driver。
3. VirtualTouchPadAndroidClient：Android端的触摸板模拟程序，手机屏幕上的手指信息并发送给Server。


#### 安装教程
//TODO 
1. Android端添加方向（旋转）
2. 优化触摸压力
3. 优化手指按下时坐标的颤抖

#### 使用说明


1. 打开Client程序，设置目标地址目标端口，连接后进入TouchPad界面


### TODO
1. VirtualTouchPadDriver中设备的信息应通过Server获取后进行设置
#### 参与贡献

1. Fork 本项目
2. 新建 Feat_xxx 分支
3. 提交代码
4. 新建 Pull Request
