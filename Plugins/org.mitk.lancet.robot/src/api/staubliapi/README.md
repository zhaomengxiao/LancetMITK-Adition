## RobotApi

#### 简单说明：

- 本简易demo作为RobotApi使用示范，注释还不够完善，有待后续改善；
- robotapid.lib文件为debug版本,robotapi.lib为release版本；
- Qt_5_12_2_MSVC2017_64bit



#### 基础使用方法

Connect - PowerOn

其实挺尴尬的，机械臂底层控制器也需要开启程序，需要远程有人配合。总之，**需要测试的时候联系我**。

#### 2020-6-5

- 修复movel底层bug；
- 增加机械臂标定跑点功能，设置边长，设置初始点，后续点一次NextPoint就会运动到下一点；
  - 使用方法：基础使用方法 --> 设置立方体边长（默认为5，单位cm）--> SetInitialPoint --> NextPoint  
- Polling data 改良为一旦机械臂连接成功就自动开始查询实时数据；



#### 2020-5-16更新

- 调整机械臂实际数据使用方式，原来是需要在外部设立定时器，现在改到api内部启动定时查询数据，尽管外部仍然有可能需要定时读取数据，但可以做到两边不同帧率，其实改完想了下，没什么太大卵用。。。

- 增加了tcp设置与获取接口；

  - ```c++
    // TCP infomation
    struct TcpInfo
    {
        TcpType tcptype;						// Tcp 类型
        ToolCoordinates tc;						// Tcp Coordinates
        CenterOfGravity cog;					// Tcp 重心
    };
    ```

  - 只是简单加了接口，底层支持还不够完善，但暂时不影响使用，有待后续完善；