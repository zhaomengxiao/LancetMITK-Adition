/*
V2.3.3 更新时间：2022-10-10
API
1.新增绕点旋转求尖端信息的API函数：
	Aim_InitToolTipPivotWithToolId()
	Aim_ProceedToolTipPivot()
	Aim_SaveToolTipPivot()
2.新增对轻量版光学定位系统的兼容。
3.更改Aim_API_Initial()和Aim_API_Close()两个函数的返回值类型，同时，新增两个函数返回值：
	当AimHandle不为NULL时，调用初始化函数，会返回AIMOOE_INITIAL_FAIL
	当AimHandle为NULL时，调用其他API函数，会返回AIMOOE_HANDLE_IS_NULL
4.对所有API函数加锁，以防止多个API函数同时调用时出错。
5.解决的工具制作、校准、尖端注册等进度显示的一些bug。
6.解决Aim_CheckToolFile()函数在某些情况下引起的bug。
7.解决连接多台定位系统时可能出现未连接的bug。
8.新增mac地址获取函数Aim_GetAimPositonMacAddress()。
9.新增二维点集到三维点集计算函数Aim_Calculate3DPoints()，方便用户进行深度开发。
10.解决注册板注册时的bug。

V2.3.2 更新时间：2022-4-8
1.把Aim_SetMarkerRoundnessThreshold()函数改成Aim_SetMarkerParameters()函数，可以设置标记点识别的各个参数的阈值，包括圆度，面积，亮度等。
2.增加Aim_SetToolFindRTDirection()和Aim_GetToolFindRTDirection()函数。
3.增加Aim_FindSingleToolInfo()函数和T_AimToolDataResultSingle结构体。
4.增加Aim_SetToolFindPointMatchOptimizeEnable(）和Aim_GetToolFindPointMatchOptimizeEnable(）函数。
5.解决多次调用Aim_API_Close()时崩溃的bug。
6.解决三种连接方式的返回值不一致的bug。
7.增加Aim_CheckToolFile()函数。
8.增加Aim_SetFlashDelay()函数，该函数在V1.2.3及以上的固件版本可用。
9.解决旋转向量在某些情况输出不正常的bug。

V2.3.1 更新时间：2022-1-22
1.增加采集模式设置（默认连续采集模式，两种单次采集模式仅在V1.2.0及以上的固件版本的仪器中有效）。
2.设立了两套数据采集的方式：
	2.1采用API内部线程从仪器获取数据放入内部缓存，API从内部缓存获取数据。
	2.2API函数直接从仪器获取数据。
3.增加设置圆度判断阈值的API函数。
4.优化wifi通讯的重试机制。
5.增加幻影点指示。
6.增加获取当前定位仪IP的函数。

V2.3.0 更新时间：2021-11-4
1.更改了部分接口函数名称，详见函数升级名称对照表。
2.删除了部分接口函数（所删除函数可以用其他函数替代）
3.对2个工具查找函数和空间配准函数进行升级，增加最少匹配点数这一参数，允许点被遮挡时仍可实现工具识别和空间配准。

V2.2.4 更新时间：2021-9-6
1.优化多点工具识别的判断
2.优化校准功能

V2.2.3 更新时间：2021-8-17
1.优化针尖的注册方式
2.优化工具识别的判断标准

V2.2.2 更新时间：2021-7-30
1.优化网络通讯的连接方式
2.优化多点共极线的处理方式

V2.2.1 更新时间：2021-7-6
1.优化部分注释。
2.调整工具识别函数的阈值设置的上下限。
3.优化网络通讯机制，加入重试机制。
4.支持多于4点的工具识别和制作。

V2.2.0 更新时间：2021-6-15
1.支持V1.1.0固件。
2.增加根据使用距离设置双目相机曝光值的函数。
3.支持多个同种类型工具识别。
4.优化wifi通讯机制。

V2.1.6 更新时间：2021-5-26
1.修正工具查找时返回的点集序号异常的问题；
2.优化AAK精度工具验证的功能；
3.优化添加工具识别文件的功能；
4.添加通过ID识别工具的校准初始化函数；
5.优化边界示警的机制；
6.优化空间匹配函数；
7.修改头文件中的函数顺序，使其更具可读性；

V2.1.5 更新时间：2021-3-19
1.工具误差定义为点间距的平均误差和显示更新；
2.工具识别算法优化；
3.USB和有线网数据校验优化，定位仪固件需要更新
4.增加工具识别全数据接口
5.增加固件版本信息

V2.1.4
更新时间：2020-12-2
1.增加工具信息获取功能
2.优化有线网获取图像数据
3.优化工具跟踪算法，匹配机器人控制
4.增加机器人基座坐标下目标位置计算
5.增加机器人标定结果输入接口

V2.1.3
更新时间：2020-9-30
1.增加空间注册功能

V2.1.2
更新时间：2020-9-24
1.增加工具识别文件功能
2.增加精度测试功能

V2.1.1
更新时间：2020-8-26
1.曝光设置支持多型号定位系统AP-100/AP-200
2.AimToolBox：优化图像显示，优化资源占用和内存管理

V2.1.0
更新时间：2020-8-21
1.支持多型号定位系统AP-100/AP-20..

V1.4.5
更新时间：2020-8-4
1.增加有线网络设置接口
2.dll增加版本尾缀

V1.4.004
更新时间：2020-8-3
1.增加标记点移动到边缘的提示，在获取标记点的结果中体现；
2.自我标定过程采用四点svd算法，标定尖端采用三点平面算法；
3.尖端标定误差通过计算尖端到某一点的距离的RMS替代

V1.4.003
更新时间：2020-7-27
1.修正四元素的赋值和界面显示
2.优化计算RT算法，支持三点平面算法
3.优化工具识别计算过程和准确性；

V1.4.002
更新时间：2020-6-1
1.优化工具注册算法
2.优化工具注册界面进度条显示效果
3.优化内存管理；

V1.4.001
更新时间：2020-3-27
1.增加工具自我标定功能，工具尖端注册功能；
2.增加库的版本读取功能；
3.优化工具识别算法，优化工具数据结构；
4.优化工具基本信息获取结构

V1.3.001
更新时间：2019-12-5
1.增加工具识别功能，返回工具识别的相关参数结果

V1.2.3:
更新时间：2019-10-30
更新内容：
1.增加图像采集的同步帧，以保证长时间运行的图像稳定性。

V1.2.2：
更新时间：2019-10-20
更新内容：
1.完善上一版本的修改，补充了以太网连接时的判断；
2.解决多次重连时出现的读写错误。

V1.2.1：
更新时间：2019-10-10
更新内容：
1.修改同时获取信息数据，双目图像数据，彩色图像数据时的机制，以解决可能出现的串帧现象。

V1.2.0：
更新时间：2019-9-27
更新内容：
1.修改API函数架构，整合不同通讯方式的API函数，极大缩减了API函数数量，具体体现为：USB、以太网、wifi三类API函数合为一类；
2.修改API调用方式，如：在获取数据前需要先通过Aim_SetAcquireData函数配置所要获取的数据 等；
3.在同时获取图像数据和信息数据时，信息数据也能达到60帧的速度；
4.降低信息数据获取的延时时间（WIFI通讯除外），保证信息数据延时小于1帧（1/60 秒）。

V1.1.1：
更新时间：2019-9-9
更新内容：
1.优化USB通讯，在异常退出时可以正确重连；
2.优化设备未更新时的输出机制；
3.新增背景亮度指示功能。

V1.1.0：
更新时间：2019-9-2
更新内容：
1.优化立体匹配的共极线判断条件，并保留非共极线点的输出。

V1.0.1：
更新时间：2019-7-11
更新内容：
1.增加了三个定位系统指令：
（1）SC_AF_FIX_INFINITY，用于控制中间彩色相机固定焦距到无穷远；
（2）SC_AF_EXP_AUTO_ON，用于控制中间彩色相机开启自动曝光；
（3）SC_AF_EXP_AUTO_OFF，用于控制中间彩色相机关闭自动曝光；
2.修改了原来设置双目相机曝光值的函数名，并增加了设置中间彩色相机曝光值的函数。

*/
#pragma once
#include"AimPositionDef.h"


/*------------------------------API FUNCTION---------------------------*/

/***********************************************************************/
/**
*	描述：
*		在调用其他函数前，请先调用此函数对API进行初始化。
*	参数：
*		aimHandle:初始化时，初值要赋NULL，每台系统使用一个AimHandle。
*	返回值：
*		无
*************************************************************************/
DLLExport E_ReturnValue Aim_API_Initial(AimHandle &aimHandle);

/************************************************************************/
/**
*	描述：
*		在程序结束时，请调用此函数关闭API。
*	参数：
*		aimHandle:当前要关闭的API的句柄。
*	返回值：
*		无
*************************************************************************/
DLLExport E_ReturnValue Aim_API_Close(AimHandle &aimHandle);

/************************************************************************/
/**
*	描述：
*		使用所选接口连接设备，各接口可同时使用。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
         o_pospara:  定位仪的型号类别,图像参数
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_CONNECT_ERROR : 设备连接失败，请确认设备正常工作。
*		AIMOOE_READ_FAULT : 读设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_WRITE_FAULT : 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_ConnectDevice(AimHandle aimHandle, E_Interface interfaceType, T_AIMPOS_DATAPARA &o_pospara);

/***********************************************************************/
/**
*	描述：
*		获取软件版本号。
*	参数：
*		无
*	返回值：
*		无
*************************************************************************/
DLLExport char* Aim_API_GetLibVersion();

/************************************************************************/
/**
*	描述：
*		设置定位仪采集模式：连续，单次主动，单次从动。
*		本函数仅在V1.2.0及以上的固件版本的仪器中有效。
*		本函数在调用前要确保Aim_SetAcquireData()所设置的获取的数据为DT_NONE，系统连接后默认为DT_NONE
*		本函数仅在USB和以太网通讯下使用。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		mode: 采集模式选择。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_READ_FAULT: 读设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*  注意：
*      1.开机默认为连续采集模式。此函数不属于记忆类参数设置函数，其设置的值不会保存在定位仪中。
*		因此，重启仪器后会自动恢复出厂设置。
*      2.选择单次采集模式时，需要根据不同的机器选择对应的主从模式。
*		3.本函数在调用前要确保Aim_SetAcquireData()所设置的获取的数据为DT_NONE，系统连接后默认为DT_NONE。
*		4.本函数无法在wifi下使用。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetAcquireMode(AimHandle aimHandle, E_Interface interfaceType, E_AcquireMode mode);

/************************************************************************/
/**
*	描述：
*		从硬件系统获取标记点坐标和系统状态信息。
*		本函数在调用前要确保Aim_SetAcquireData()所设置的获取的数据为DT_NONE，系统连接后默认为DT_NONE。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		markerSt: 存放回传的三维坐标信息数据。
*		statusSt: 存放回传的系统状态信息数据。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_READ_FAULT: 读设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*  注意：
*      1.本函数在调用前要确保Aim_SetAcquireData()所设置的获取的数据为DT_NONE，系统连接后默认为DT_NONE。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetMarkerAndStatusFromHardware(AimHandle aimHandle, E_Interface interfaceType, T_MarkerInfo & markerSt, T_AimPosStatusInfo& statusSt);

/************************************************************************/
/**
*	描述：
*		从硬件系统获取标记点坐标、系统状态和双目图像信息。
*		本函数在调用前要确保Aim_SetAcquireData()所设置的获取的数据为DT_NONE，系统连接后默认为DT_NONE。
*		本函数仅在USB和以太网通讯下使用。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		markerSt: 存放回传的三维坐标信息数据。
*		statusSt: 存放回传的系统状态信息数据。
*		imageL: 指向存放左图像数据的地址，需用户自行定义，图像大小
*		AP-100:1280*720*1，AP-200:1280*1240*1 每个像素1个byte，像素刷新方向从左到右，从上到下。
*		imageR: 指向存放右图像数据的地址，需用户自行定义，图像大小
*		AP-100:1280*720*1，AP-200:1280*1240*1 每个像素1个byte，像素刷新方向从左到右，从上到下。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_READ_FAULT: 读设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*  注意：
*      1.本函数在调用前要确保Aim_SetAcquireData()所设置的获取的数据为DT_NONE，系统连接后默认为DT_NONE。
*		2.本函数仅在USB和以太网通讯下有效。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetMarkerStatusAndGreyImageFromHardware(AimHandle aimHandle, E_Interface interfaceType, T_MarkerInfo & markerSt, T_AimPosStatusInfo& statusSt, char * imageL, char * imageR);

/************************************************************************/
/**
*	描述：
*		从硬件系统获取中间相机彩色图像。
*		本函数在调用前要确保Aim_SetAcquireData()所设置的获取的数据为DT_NONE，系统连接后默认为DT_NONE。
*		本函数仅在USB和以太网通讯下使用。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		imageC: 指向存放彩色图像数据的地址，需用户自行定义，图像大小
*		1280*720*2，每个像素2个byte（RGB565，低位在前，高位在后，从高到低
*		为R->G->B），像素刷新方向从左到右，从上到下。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_READ_FAULT: 读设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_NOT_REFLASH: 若已通过Aim_SetAcquireData函数正确设置定位仪所要获取的数据，则图像数据未更新。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*  注意：
*      1.本函数在调用前要确保Aim_SetAcquireData()所设置的获取的数据为DT_NONE，系统连接后默认为DT_NONE。
*		2.本函数仅在USB和以太网通讯下有效。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetColorImageFromHardware(AimHandle aimHandle, E_Interface interfaceType, char* imageC);

/************************************************************************/
/**
*	描述：
*		设置API的内部线程从定位仪获取的数据，在调用以下get函数前，需先调用本函数：
*		Aim_GetMarkerInfo()
*		Aim_GetStatusInfo()
*		Aim_GetGreyImageDual()
*		Aim_GetColorImageMiddle()
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		dataType: 确定API从所指定的通讯接口读取的数据。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*  注意：
*      1.本函数调用一次后长期有效，当重连系统或出现读写错误时，需要重新调用本函数。
*      2.要确保从定位仪获取的数据包含了所要读取的数据，否则Get函数会返回AIMOOE_NOT_REFLASH的结果。
*      3.此函数不属于记忆类参数设置函数，其设置的值不会保存在定位仪中，重启仪器后会自动恢复默认值DT_NONE。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetAcquireData(AimHandle aimHandle, E_Interface interfaceType, E_DataType dataType);

/************************************************************************/
/**
*	描述：
*		从API内部存储空间中读取最新的左右两个相机灰度图像。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		imageL: 指向存放左图像数据的地址，需用户自行定义，图像大小
*		AP-100:1280*720*1，AP-200:1280*1240*1 每个像素1个byte，像素刷新方向从左到右，从上到下。
*		imageR: 指向存放右图像数据的地址，需用户自行定义，图像大小
*		AP-100:1280*720*1，AP-200:1280*1240*1 每个像素1个byte，像素刷新方向从左到右，从上到下。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_READ_FAULT: 读设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_NOT_REFLASH: 若已通过Aim_SetAcquireData函数正确设置定位仪所要获取的数据，则图像数据未更新。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetGreyImageDual(AimHandle aimHandle, E_Interface interfaceType, char* imageL, char* imageR);

/************************************************************************/
/**
*	描述：
*		从API内部存储空间中读取最新的中间相机彩色图像。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		imageC: 指向存放彩色图像数据的地址，需用户自行定义，图像大小
*		1280*720*2，每个像素2个byte（RGB565，低位在前，高位在后，从高到低
*		为R->G->B），像素刷新方向从左到右，从上到下。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_READ_FAULT: 读设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_NOT_REFLASH: 若已通过Aim_SetAcquireData函数正确设置定位仪所要获取的数据，则图像数据未更新。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetColorImageMiddle(AimHandle aimHandle, E_Interface interfaceType, char* imageC);

/************************************************************************/
/**
*	描述：
*		从API内部存储空间中读取最新的标记点三维坐标信息。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		markerSt: 存放回传的三维坐标信息数据。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_READ_FAULT: 读设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_NOT_REFLASH: 坐标信息未更新，此时markerSt中的三维点数量为0，请确认已通过Aim_SetAcquireData()函数正确设置定位仪所要获取的数据
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*  注意：
*      在连接系统后，请先调用Aim_SetAcquireData()函数，再调用本函数。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetMarkerInfo(AimHandle aimHandle, E_Interface interfaceType, T_MarkerInfo & markerSt);

/************************************************************************/
/**
*	描述：
*		读取系统状态信息。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		statusSt: 存放回传的系统状态信息数据。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_READ_FAULT: 读设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_NOT_REFLASH: 若已通过Aim_SetAcquireData函数正确设置定位仪
*		所要获取的数据，则系统状态信息未更新。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetStatusInfo(AimHandle aimHandle, E_Interface interfaceType, T_AimPosStatusInfo & statusSt);

/************************************************************************/
/**
*	描述：
*		读取定位仪出厂信息。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		manufactureInfo: 存放回传的出厂信息数据。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetManufactureInfo(AimHandle aimHandle, E_Interface interfaceType, T_ManufactureInfo & manufactureInfo);

/************************************************************************/
/**
*	描述：
*		下发定位仪系统指令。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		com: 要下发的命令值。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_WRITE_FAULT : 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetSystemCommand(AimHandle aimHandle, E_Interface interfaceType, E_SystemCommand com);

/************************************************************************/
/**
*	描述：
*		设置左右相机的曝光值（开机默认使用上一次设置的值），范围：
*		16-16384 (即：0x10-0x4000)，单位us，最小分辨精度是16us。
*		此曝光值的设置会影响采集精度，请勿随意修改。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		expTime: 要设置的双目左右相机曝光值，范围：16-16384 (即：0x10-0x4000)，
*		单位为us,最小分辨精度是16us。

*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*		1.此函数属于记忆类参数设置函数，其设置的值将保存在定位仪中，下一次开启
*		定位仪时会被沿用。
*		2.由于记忆类参数设置函数需要等待定位仪把数据保存，此类函数被调用后，需
*		要延时0.5s以上，才能再调用下一个记忆类参数设置函数。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetDualExpTime(AimHandle aimHandle, E_Interface interfaceType, int expTime);

/************************************************************************/
/**
*	描述：
*		根据使用距离设置双目相机曝光值。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		distanceInMM: 使用距离，单位为毫米
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*		1.此函数属于记忆类参数设置函数，其设置的值将保存在定位仪中，下一次开启
*		定位仪时会被沿用。
*		2.由于记忆类参数设置函数需要等待定位仪把数据保存，此类函数被调用后，需
*		要延时0.5s以上，才能再调用下一个记忆类参数设置函数。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetDualExpTimeByDistance(AimHandle aimHandle, E_Interface interfaceType, int distanceInMM);

/************************************************************************/
/**
*	描述：
*		设置闪光灯与曝光时间之间的延时。包括启动延时和关闭延时。
*		正常使用API时，无需对延时参数进行修改。
*		本函数仅在USB和以太网通讯下使用。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		flashOnDelay: 从闪光灯启动到相机曝光启动的延时时间=flashOnDelay*125us，flashOnDelay的取值范围0-255。
*		flashOffDelay: 从闪光灯关闭到相机曝光关闭的延时时间=flashOffDelay*125us，flashOffDelay的取值范围0-255，
*								另外，从闪光灯关闭到相机曝光关闭的延时时间必须小于曝光时间。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*		1.此函数不属于记忆类参数设置函数，其设置的值不会保存在定位仪中，重启仪器后会自动恢复默认值。
*		2.本函数无法在wifi下使用。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetFlashDelay(AimHandle aimHandle, E_Interface interfaceType, int flashOnDelay, int flashOffDelay);

/************************************************************************/
/**
*	描述：
*		设置中间彩色相机的曝光值，范围：48-8192 (即：0x30-0x2000)。
*		若彩色相机自动曝光已打开（开机默认打开），则此设置将无效。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		expTimeAF: 要设置的中间彩色相机曝光值，范围：48-8192 (即：0x30-
*		0x2000)，单位为：1行图像的传输时间/16。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*		1.由于开机默认打开中间彩色相机的自动曝光，因此本函数所设置的曝光值不会保存在定位仪中。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetColorExpTime(AimHandle aimHandle, E_Interface interfaceType, int expTimeAF);

/************************************************************************/
/**
*	描述：
*		设置碰撞检测灵敏度，范围：1-10，1灵敏度最高，10灵敏度最低。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		level: 灵敏度参数，值越低，越灵敏。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*		1.此函数属于记忆类参数设置函数，其设置的值将保存在定位仪中，下一次开启
*		定位仪时会被沿用。
*		2.由于记忆类参数设置函数需要等待定位仪把数据保存，此类函数被调用后，需
*		要延时0.5s以上，才能再调用下一个记忆类参数设置函数。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetCollisinoDetectLevel(AimHandle aimHandle, E_Interface interfaceType, UCHAR level);


/************************************************************************/
/**
*	描述：
*		设置定位仪IP（出厂默认为192.168.31.10），设置后重启定位仪有效。
*		目前仅支持USB和网口连接下，进行定位仪IP设置。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		IP_A、IP_B、IP_C、IP_D: 要设置的IP地址的四个字段A.B.C.D，每个字段
*		占一个字节。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*		1.此函数属于记忆类参数设置函数，其设置的值将保存在定位仪中，下一次开启
*		定位仪时会被沿用。
*		2.由于记忆类参数设置函数需要等待定位仪把数据保存，此类函数被调用后，需
*		要延时0.5s以上，才能再调用下一个记忆类参数设置函数。
*		3.本函数无法在wifi下使用。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetAimPositionIP(AimHandle aimHandle, E_Interface interfaceType, UCHAR IP_A, UCHAR IP_B, UCHAR IP_C, UCHAR IP_D);
/************************************************************************/
/**
*	描述：
*		获取当前定位仪所使用的IP。
*		任意方式连接成功后可用。
*	参数：
*		interfaceType: 指定所选用的通讯接口。
*		IP_A、IP_B、IP_C、IP_D: 获取到的IP地址的四个字段A.B.C.D，每个字段
*		占一个字节。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetAimPositionIP(AimHandle aimHandle, E_Interface interfaceType, UCHAR &IP_A, UCHAR &IP_B, UCHAR &IP_C, UCHAR &IP_D);

/************************************************************************/
/**
*	描述：
*		以太网连接时，设置所要连接的定位仪的IP，需与所设置的定位仪IP一致。
*	参数：
*		IP_A、IP_B、IP_C、IP_D: 要连接的IP地址的四个字段A.B.C.D，每个字段
*		占一个字节。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*		1.此函数需要在Aim_ConnectDevice()函数调用前使用。
*		2.若定位仪IP使用出厂默认设置（192.168.31.10）则无需调用此函数。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetEthernetConnectIP(AimHandle aimHandle, UCHAR IP_A, UCHAR IP_B, UCHAR IP_C, UCHAR IP_D);
/************************************************************************/
/**
*	描述：
*		获取当前设置的所要连接的定位仪的IP，该IP不等同于当前定位仪所使用的IP。
*	参数：
*		IP_A、IP_B、IP_C、IP_D: 获取到的IP地址的四个字段A.B.C.D，每个字段
*		占一个字节。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*		1.该IP不等同于当前定位仪所使用的IP。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetEthernetConnectIP(AimHandle aimHandle, UCHAR &IP_A, UCHAR &IP_B, UCHAR &IP_C, UCHAR &IP_D);

/************************************************************************/
/**
*	描述：
*		设置定位仪LCD屏幕是否显示未经处理过的点。
*		出厂默认不显示未经处理过的点，即只有左右相机的点是匹配的才显示。
*		本函数仅在USB和以太网通讯下使用。
*	参数：
*		isRawPointShow: true则显示未经处理过的点；false则只显示处理过的点；
*   返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*		1.此函数属于记忆类参数设置函数，其设置的值将保存在定位仪中，下一次开启
*		定位仪时会被沿用。
*		2.由于记忆类参数设置函数需要等待定位仪把数据保存，此类函数被调用后，需
*		要延时0.5s以上，才能再调用下一个记忆类参数设置函数。
*		3.本函数无法在wifi下使用。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetLCDShowRawPoint(AimHandle aimHandle, E_Interface interfaceType, bool  isRawPointShow = false);

/************************************************************************/
/**
*	描述：
*		设置标记点识别的参数阈值，一般不建议修改。
*		本函数仅在USB和以太网通讯下使用。
*	参数：
*		minRoundness: 圆度阈值，范围：0-100；
*		maxArea: 面积阈值，范围：5-4000；
*		minBrightness: 亮度阈值，范围：1-255；
*   返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_WRITE_FAULT: 写设备错误，请重新连接设备或重启设备再试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*		1.此函数不属于记忆类参数设置函数，其设置的值不会保存在定位仪中。
*		因此，重启仪器后会自动恢复出厂设置。
*		2.本函数无法在wifi下使用。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetMarkerParameters(AimHandle aimHandle, E_Interface interfaceType, int minRoundness=75, int maxArea=1000, int minBrightness=80);

/************************************************************************/
/**
*	描述：
*		设置工具识别时点误差的最大允许偏差，单位mm
*	参数：
*		offset: 识别工具时的点误差的最大允许偏差，范围：0.3-3.0。
*   返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*      1.此函数不属于记忆类参数设置函数，其设置的值不会保存在定位仪中，重启仪器后会自动恢复默认值1.5。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetToolFindOffset(AimHandle aimHandle, float offset = 1.5f);

/************************************************************************/
/**
*	描述：
*		获取当前工具识别时点误差的最大允许偏差，单位mm
*	参数：
*		offset: 返回当前的最大允许偏差，范围：0.3-3.0。
*   返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetToolFindOffset(AimHandle aimHandle, float &offset);

/************************************************************************/
/**
*	描述：
*		设置工具识别时，是否启动点匹配误差优化，默认启动。
*		启动时，会对匹配的点进行误差判断，当某点误差比其他点误差大时，在计算RT矩阵时会自动把该点滤掉。
*	参数：
*		en: true为启动，false为不启动。
*   返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：
*      1.此函数不属于记忆类参数设置函数，其设置的值不会保存在定位仪中，重启仪器后会自动恢复默认值。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetToolFindPointMatchOptimizeEnable(AimHandle aimHandle,  bool en= true);

/************************************************************************/
/**
*	描述：
*		获取当前工具识别时，是否启动点匹配误差优化。
*		启动时，会对匹配的点进行误差判断，当某点误差比其他点误差大时，在计算RT矩阵时会自动把该点滤掉。
*	参数：
*		en: 返回值，true为启动，false为不启动。
*   返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetToolFindPointMatchOptimizeEnable(AimHandle aimHandle, bool &en);

/************************************************************************/
/**
*	描述：
*		读取并检查工具文件数据是否正确，读取到的数据存放在toolData中。
*	参数：
*		filePathFullName: 工具文件路径+名字+后缀。
*		toolData：返回读到的工具文件数据。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 工具文件有误。
*************************************************************************/
DLLExport E_ReturnValue Aim_CheckToolFile(AimHandle aimHandle, char * filePathFullName, T_ToolFileData &toolData);

/************************************************************************/
/**
*	描述：
*		设置工具文件的路径，系统会遍历此路径下所有工具文件的信息。
*		调用此函数时，原有的工具文件信息会先自动清除。
*		本函数无需多次调用，建议在调用Aim_ConnectDevice()连接函数后调用一次本函数，即可。
*	参数：
*		path: 路径参数。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 路径为空，或者路径下没有工具文件。
*  注意：
*		在调用本函数时，请勿在其他线程调用与工具相关的函数（如工具查找函数），否则可能会导致代码崩溃！
*************************************************************************/
DLLExport E_ReturnValue Aim_SetToolInfoFilePath(AimHandle aimHandle, char * path);
/************************************************************************/
/**
*	描述：
*		获取当前设置的工具文件路径。
*	返回值：
*		const char * 路径参数
*************************************************************************/
DLLExport const char * Aim_GetToolInfoFilePath(AimHandle aimHandle);
/**
*	描述：
*		根据ID识别所选工具文件中的参数：标记点数及工具文件中的坐标
*	参数：
*		ptoolid: 工具识别号。
*		marksize: 工具文件中的标记点数量。
*		toolsysinfo:工具文件中的标记点数据集
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetSpecificToolFileInfoList(AimHandle aimHandle, const char *ptoolid, int &marksize, std::list<float>*toolsysinfo);
/************************************************************************/
/**
*	描述：
*		根据ID识别所选工具文件中的参数：标记点数及工具文件中的坐标
*	参数：
*		ptoolid: 工具识别号。
*		marksize: 工具文件中的标记点数量。
*		toolsysinfo:工具文件中的标记点数据集
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetSpecificToolFileInfoArray(AimHandle aimHandle, const char* ptoolid, int& markersize, float* toolsysinfo);
/************************************************************************/
/**
*	描述：
*		获取所选路径下的工具文件数量。
*	参数：
*		size: 数量。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetCountOfToolInfo(AimHandle aimHandle, int &size);
/************************************************************************/
/**
*	描述：
*		获取所选路径下的所有工具文件的基本信息，可先调用Aim_GetCountOfToolInfo()函数获取工具文件数量，对参数数组进行初始化
*	参数：
*		ptools: 返回工具基本信息的结构体数组，结构体中包括名称，类型 和标记点数量
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetAllToolFilesBaseInfo(AimHandle aimHandle, t_ToolBaseInfo *ptools);

/************************************************************************/
/**
*	描述：
*		通过输入三维数据，对所选路径下的所有工具进行识别。
*		每次调用此函数前，可通过Aim_GetMarkerInfo()或
*		Aim_GetMarkerAndStatusFromHardware()函数来获取最新的三维数据信息。
*		建议使用Aim_FindSpecificToolInfo()代替本函数，该函数返回的数据更加全面
*	参数：
*		marker: 三维数据。
*		pResultList: 返回存放找到的工具信息数据。
*		minimumMatchPts:多点工具进行识别时，可允许的最少匹配点数(≥3)。缺省值0表示全匹配。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未能识别工具。
*************************************************************************/
DLLExport E_ReturnValue Aim_FindToolInfo(AimHandle aimHandle, T_MarkerInfo & marker, T_AimToolDataResult*pResultList, int minimumMatchPts =0);
/************************************************************************/
/**
*	描述：
*		通过输入三维数据和工具ID信息（可多个），对对应工具（可多个）进行识别。
*		每次调用此函数前，可通过Aim_GetMarkerInfo()或Aim_GetMarkerAndStatusFromHardware()
*		函数来获取最新的三维数据信息。
*	参数：
*		marker: 三维数据列表
*		toolids:工具识别的ID列表
*		pResultList: 返回存放找到的工具信息数据集。
*		minimumMatchPts:多点工具进行识别时，可允许的最少匹配点数(≥3)。缺省值0表示全匹配。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未能识别工具。
*************************************************************************/
DLLExport E_ReturnValue Aim_FindSpecificToolInfo(AimHandle aimHandle, T_MarkerInfo & marker,
	std::vector<std::string>&toolids, T_AimToolDataResult*pResultList, int minimumMatchPts=0);

/************************************************************************/
/**
*	描述：
*		通过输入三维数据和单个工具的ID信息，对该工具进行识别。
*		每次调用此函数前，可通过Aim_GetMarkerInfo()或Aim_GetMarkerAndStatusFromHardware()
*		函数来获取最新的三维数据信息。
*	参数：
*		marker: 三维数据列表
*		toolids:要识别的工具的ID
*		dataResult: 返回存放找到的工具信息数据集。
*		minimumMatchPts:多点工具进行识别时，可允许的最少匹配点数(≥3)。缺省值0表示全匹配。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未能识别工具。
*************************************************************************/
DLLExport E_ReturnValue Aim_FindSingleToolInfo(AimHandle aimHandle, T_MarkerInfo & marker,
	const char * toolids, T_AimToolDataResultSingle &dataResult, int minimumMatchPts = 0);

/************************************************************************/
/**
*	描述：
*		设置工具查找时，得到的结果中，所有坐标转换关系（RT矩阵、旋转向量和四元数）的方向，默认方向是从工具坐标系转换到系统坐标系。
*		受本函数设置影响的API函数有：
*		Aim_FindToolInfo()
*		Aim_FindSpecificToolInfo()
*		Aim_FindSingleToolInfo()
*		Aim_MappingPointSetsForMarkerSpaceReg()
*	参数：
*		direction: 方向有两个，从工具到系统和从系统到工具。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未能识别工具。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetToolFindRTDirection(AimHandle aimHandle, E_RTDirection direction);

/************************************************************************/
/**
*	描述：
*		获取工具查找时，得到的结果中，所有坐标转换关系（RT矩阵、旋转向量和四元数）的方向。
*	参数：
*		direction: 方向有两个，从工具到系统 和 从系统到工具。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未能识别工具。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetToolFindRTDirection(AimHandle aimHandle, E_RTDirection &direction);

/************************************************************************/
/**
*	描述：
*		根据工具ID和标记点数量，初始化要制作的工具文件信息。
*		文件保存在Aim_SetToolInfoFilePath()函数所设置的工具路径下。
*		因此在调用本函数前，需先调用Aim_SetToolInfoFilePath()函数。
*	参数：
*		markcnt：工具上标记点数量 
*		id：工具ID号
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_InitToolMadeInfo(AimHandle aimHandle, const int markcnt, const char * id);
/************************************************************************/
/**
*	描述：
*		制作工具文件，使用前需要调用Aim_InitToolMadeInfo()初始化工具信息。
*		工具制作过程需循环调用获取标记点坐标的和本函数，直到Proinfo.isMadeProFinished为true。
*	参数：
*		marker：标记点集合；
*		ProInfo:  制作过程信息,当Proinfo.isMadeProFinished为true时，制作完成；
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_ProceedToolMade(AimHandle aimHandle, T_MarkerInfo &marker, t_ToolMadeProInfo&ProInfo);
/************************************************************************/
/**
*	描述：
*		保存或者取消工具制作的文件信息，在制作工具完成后调用。
*	参数：
*		saved: true 默认保存工具文件信息；false 删除当前Aim_DoneToolMade制作后的文件信息；
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_SaveToolMadeRlt(AimHandle aimHandle, bool saved = true);

/************************************************************************/
/**
*	描述：
*		选择需要注册（标定）尖端的工具和使用的注册板（标定板）。(建议不使用此函数)
*		建议用Aim_InitToolTipCalibrationWithToolId()代替本函数
*	参数：
*		CalToolIndex 当前选择的注册板（标定板）（获取到的工具文件的索引）
		PosToolIndex 当前选择的工具（获取到的工具文件的索引）
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_InitToolTipCalibration(AimHandle aimHandle, int CalToolIndex, int PosToolIndex);
/************************************************************************/
/**
*	描述：
*		通过工具ID选择需要注册（标定）尖端的工具和使用的注册板（标定板）。
*	参数：
*		CalTool 当前使用的注册板（标定板）的ID号
*		PosTool 当前选择的工具的ID号
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 函数执行失败。
*************************************************************************/
DLLExport E_ReturnValue Aim_InitToolTipCalibrationWithToolId(AimHandle aimHandle, const char* CalTool, const char* PosTool);
/************************************************************************/
/**
*	描述：
*		对所选择的工具和注册板（标定板）进行操作，得到工具的尖端信息，在调用前，需要
*	    调用Aim_InitToolTipCalibrationWithToolId函数设置所选择的工具和注册板（标定板）。
*		本函数（以及获取标记点坐标的函数）需多次调用，直到info.isCalibrateFinished为true
*	参数：
*		marker ：当前采集的标记点
*		ProInfo：返回针尖的参数，当标志位ProInfo.isCalibrateFinished为true时，结果有效
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 函数执行失败。
*************************************************************************/
DLLExport E_ReturnValue Aim_ProceedToolTipCalibration(AimHandle aimHandle, T_MarkerInfo & marker, t_ToolTipCalProInfo &ProInfo);
/************************************************************************/
/**
*	描述：
*		保存注册（标定）后的工具尖端信息到原工具文件中。
*	参数：
*		无
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 保存失败
*************************************************************************/
DLLExport E_ReturnValue Aim_SaveToolTipCalibration(AimHandle aimHandle);

/************************************************************************/
/**
*	描述：
*		通过工具ID选择需要注册（标定）尖端的工具，通过绕点旋转的方式完成尖端注册。
*	参数：
*		toolID: 当前选择的工具的ID号
*		clearTipMid: true时，针尖上一点的数据清0；false时，针尖上一点数据维持不变。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 函数执行失败。
*************************************************************************/
DLLExport E_ReturnValue Aim_InitToolTipPivotWithToolId(AimHandle aimHandle, const char* toolID, bool clearTipMid=false);
/************************************************************************/
/**
*	描述：
*		对所选择的工具进行数据采集，得到工具的尖端信息，在调用前，需要
*	    调用Aim_InitToolTipPivotWithToolId函数设置所选择的工具。
*		本函数（以及获取标记点坐标的函数）需多次调用，直到info.isCalibrateFinished为true
*		请提前开始对要注册的针尖进行绕点旋转操作，再条用本函数。
*	参数：
*		marker ：当前采集的标记点
*		pivotInfo：返回针尖的参数，当标志位ProInfo.isCalibrateFinished为true时，结果有效
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 函数执行失败。
*************************************************************************/
DLLExport E_ReturnValue Aim_ProceedToolTipPivot(AimHandle aimHandle, T_MarkerInfo & marker, T_ToolTipPivotInfo &pivotInfo);
/************************************************************************/
/**
*	描述：
*		保存注册（标定）后的工具尖端信息到原工具文件中。
*	参数：
*		无
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 保存失败
*************************************************************************/
DLLExport E_ReturnValue Aim_SaveToolTipPivot(AimHandle aimHandle);
/************************************************************************/
/**
*	描述：
*		选择需要校准的工具，在调用Aim_ProceedToolSelfCalibration前调用
*		不建议调用此函数；
*		建议使用Aim_InitToolSelfCalibrationWithToolId()代替本函数；
*	参数：
*		ToolIndex 当前选择的工具（获取到的工具文件的索引）
*		markcnt  返回所选择工具的标记点数量
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 函数执行失败。
*************************************************************************/
DLLExport E_ReturnValue Aim_InitToolSelfCalibration(AimHandle aimHandle, int ToolIndex, int &markcnt);
/**
*	描述：
*		选择需要校准的工具，在调用Aim_ProceedToolSelfCalibration前调用
*	参数：
*		tool 当前选择的工具的ID号
*		markcnt  返回所选择工具的标记点数量
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 函数执行失败。
*************************************************************************/
DLLExport E_ReturnValue Aim_InitToolSelfCalibrationWithToolId(AimHandle aimHandle, const char * tool, int &markcnt);
/************************************************************************/
/**
*	描述：
*		对所选择的工具进行自我标定操作，在调用前需要调用Aim_InitToolSelfCalibrationWithToolId
*	设置所选择的工具。此函数需要多次调用，直到ProInfo中的isCalibrateFinished
*	参数为true。根据ProInfo中的其他参数确认精度是否满足要求，是否进行结果的保存。
*	参数：
*		marker: 当前采集的标记点
*		ProInfo: 返回校准结果，其中的ProInfo.isCalibrateFinished参数为true时，表示校准操作完成。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 函数执行失败，当次校准无效。
*************************************************************************/
DLLExport E_ReturnValue Aim_ProceedToolSelfCalibration(AimHandle aimHandle, T_MarkerInfo &marker, t_ToolFixProInfo&ProInfo);
/************************************************************************/
/**
*	描述：
*		对所选择的工具的校准结果进行操作处理

*	参数：
*		fixrltcmd 取消，重做和保存
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_SaveToolSelfCalibration(AimHandle aimHandle, E_ToolFixRlt fixrltcmd);

/************************************************************************/
/**
*	描述：
*		初始化使用的精度测试工具ID号，仅支持左右四点的aimooe精度工具
*	参数：
*		toolids:工具所有点的ID号，toolidl左侧4点工具ID，toolid2右侧4点工具 ID
*	返回值：
*		AIMOOE_OK: 函数执行成功，说明可以进行精度测试。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。	
*************************************************************************/
DLLExport E_ReturnValue Aim_InitAccuracyCheckTool(AimHandle aimHandle, const char *toolids, const char*toolid1, const char*toolid2);
/************************************************************************/
/**
*	描述：
*		根据需要，循环调用本函数（以及获取标记点坐标的函数）。
*	参数：
*		markarr:标记点集[markcnt][3]
*		markcnt:标记点数量
*		T_AccuracyToolResult:当次采集的左右工具的距离与旋转角度
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*  注意：
*	此函数调用后，数据会存储在内部，供Aim_AccuracyCheckToolCalculateError函数进行计算
*************************************************************************/
DLLExport E_ReturnValue Aim_AccuracyCheckTool(AimHandle aimHandle, double** markarr,const int markcnt,T_AccuracyToolResult &Rlt);
/************************************************************************/
/**
*	描述：
*		获取定位系统精度误差数据，包含均值，标准差，角度误差
*	参数：
*		meanerro 平均误差，stdev 距离标准差，angle[3]角度平均误差。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*	注意：此函数调用后，内部存储的数据会清零。
*************************************************************************/
DLLExport E_ReturnValue Aim_AccuracyCheckToolCalculateError(AimHandle aimHandle, float &meanerro,float &stdev,float *angle);

/************************************************************************/
/**
*	描述：
*		空间配准的点集初始化，把空间点集信息当做一个工具传给光学定位系统
*	参数：
*		ImgPtArr 图像空间坐标点集
*		ImgPtSize 点集的数量
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_InitMappingPointSetsForMarkerSpaceReg(AimHandle aimHandle, const float(*ImgPtArr)[3], const int ImgPtSize);
/************************************************************************/
/**
*	描述：
*		空间点集配准，获取配准后的相关信息
*	参数：
*		marker：光学定位系统获取到的三维点集
*		pResultList：配准后的结果
*		minimumMatchPts:多点工具进行识别时，可允许的最少匹配点数(≥3)。缺省值0表示全匹配。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_MappingPointSetsForMarkerSpaceReg(AimHandle aimHandle, T_MarkerInfo & marker, T_AimToolDataResult * pResultList, int minimumMatchPts=0);

/************************************************************************/
/**
*	描述：
*		设置机器人控制的标定结果和被跟踪的结果
*	参数：
*		Sys2RobotBaseRTArray 光学定位系统到机器人基座的RT数组（3*4 的数组，第0-2列为R，第3列维T）
* 	    Tool2RobotEndRTArray 工具到机器人末端的RT数组（3*4 的数组，第0-2列为R，第3列维T）
		toolid 工具ID号
		
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_SetRobotCalculateRlt(AimHandle aimHandle, const double(*Sys2RobotBaseRTArray)[4], const double(*Tool2RobotEndRTArray)[4],const char*toolid);
/************************************************************************/
/**
*	描述：
*		计算机器人路径的目标位置
*	参数：
*		TargetPathArr 光学定位系统下的路径：格式2*3的数组，0*3为安全点，1*3为靶点（XYZ）
* 	    targetPoseArr 目标位置数组[X,Y,Z,Rx,Ry,Rz]

*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_CalculateRobotTargetPose(AimHandle aimHandle, const double(*TargetPathArr)[3], float* targetPoseArr);

/************************************************************************/
/**
*	描述：
*		获取设备MAC地址（格式：16进制 xx:xx:xx:xx:xx:xx ）
*	参数：
*		addr：当前设备的Mac地址
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_GetAimPositonMacAddress(AimHandle aimHandle, char addr[18]);

/************************************************************************/
/**
*	描述：
*		根据二维点坐标计算三维点坐标。
*	参数：
*		leftPoint: 左图像二维点坐标数组（x，y），最多200个。
*		leftNum：左图像二维点数，最多200个。
*		rightPoint: 右图像二维点坐标数组（x，y），最多200个。
*		rightNum：右图像二维点数，最多200个。
*		markerSt: 存放回传的三维坐标信息数据。
*	返回值：
*		AIMOOE_OK: 函数执行成功。
*		AIMOOE_NOT_CONNECT: 设备未连接，请先连接设备。
*		AIMOOE_ERROR: 未知错误，请确认函数调用正确。
*************************************************************************/
DLLExport E_ReturnValue Aim_Calculate3DPoints(AimHandle aimHandle, float leftPoint[400], int leftNum, float rightPoint[400], int rightNum, T_MarkerInfo & markerSt);


/*---------------------------------API END------------------------------*/