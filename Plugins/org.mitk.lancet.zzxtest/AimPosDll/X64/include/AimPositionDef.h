
#pragma once

#ifdef __cplusplus
#define DLLExport  extern "C" __declspec(dllexport)
#else
#define DLLExport  extern "C"
#endif

#include <list>
#include<vector>
#include<string>

#ifdef __cplusplus
extern "C" {
#endif

	typedef unsigned int  UINT;
	typedef unsigned char UCHAR;
	typedef unsigned short USHORT;
	typedef void* AimHandle;
	#define PtMaxNUM 200			/**< 标记点数量最大值，不能修改 */
	#define TOOLIDMAX 50
	enum class AIMPOS_TYPE
	{
		eAP_Basic = 0,
		eAP_Standard =1,
		eAP_Industry = 2,
		eAP_Lite=3,
		eAP_Ultimate = 4,
		eAP_NONE=100
	};

	enum E_AimToolType
	{
		ePosTool = 0,
		ePosCalBoard = 1

	};
	
	/**
	*	通讯接口类型
	*/
	enum E_Interface
	{
		I_USB=0,			/**< USB通讯接口 */
		I_ETHERNET,	/**< 以太网通讯接口 */
		I_WIFI,		/**< WIFI通讯接口 */
	};

	/**
	*	定位仪系统指令
	*/
	enum E_SystemCommand
	{
		SC_COLLISION_DISABLE,	/**< 关闭碰撞检测（默认打开），已有的碰撞发生警告会自动清除 */
		SC_COLLISION_ENABLE,	/**< 打开碰撞检测（默认打开），检测到碰撞时，系统状态信息中
								*	能读取到碰撞发生的警告，此时碰撞检测暂停，只有碰撞发生
								*	的警告被清除后才能继续进碰撞检测。另外，可通过
								*	Aim_SetCollisinoDetectLevel()函数设置碰撞检测的灵敏度。 */
		SC_COLLISION_INFO_CLEAR,/**< 清除碰撞发生的警告 */
		SC_IRLED_ON,			/**< 打开红外照明（默认打开）, 跟踪定位标记点时需要打开红外
								*	照明 */
		SC_IRLED_OFF,			/**< 关闭红外照明（默认打开），在不需要红外照明时，关掉照明
								*	环可减少功耗，延长寿命 */
		SC_LASER_ON,			/**< 打开定位激光（默认关闭） */
		SC_LASER_OFF,			/**< 关闭定位激光（默认关闭） */
		SC_LCD_PAGE_SUBPIXEL,	/**< 触控显示屏切换到标记点坐标跟踪界面（开机默认界面） */
		SC_LCD_PAGE_COLOR,		/**< 触控显示屏切换到彩色图像显示界面 */
		SC_LCD_ON,				/**< 打开触控显示屏背光灯(默认打开) */
		SC_LCD_OFF,				/**< 关闭触控显示屏背光灯(默认打开)，在不需要屏幕显示时，关
								*	掉屏幕背光灯可减少功耗，延长寿命 */
		SC_AF_CONTINUOUSLY_ON,	/**< 中间彩色相机持续自动对焦开启（默认关闭），开启后，每当
								*	彩色相机失去焦点就会进行自动对焦 */
		SC_AF_CONTINUOUSLY_OFF,	/**< 中间彩色相机持续自动对焦关闭（默认关闭），关闭后彩色相
								*	机将不再进行持续自动对焦 */
		SC_AF_SINGLE,			/**< 中间彩色相机单次自动对焦，
								*	下发指令后，中间彩色相机将进行单次自动对焦（提示：在触
								*	控显示屏彩色图像显示界面，点击彩色图像也可以触发彩色相
								*	机进行单次自动对焦） */
		SC_AF_FIX_INFINITY,		/**< 中间彩色相机固定焦距到无穷远（开机默认对焦无穷远） */
		SC_AF_EXP_AUTO_ON,		/**< 打开中间彩色相机自动曝光（默认打开）：启用后，系统根据
								*	当前彩色图像亮度调节曝光值和亮度增益 */
		SC_AF_EXP_AUTO_OFF,		/**< 关闭中间彩色相机自动曝光（默认打开）：关闭后，保持关闭
								*	时刻的曝光值和亮度增益 */
		SC_AF_RESTART,/**重启彩色相机*/
		SC_DUALCAM_AUTO_EXP_ON,/**启动双目相机自动曝光*/
		SC_DUALCAM_AUTO_EXP_OFF,/**关闭双目相机自动曝光*/
		SC_DUALCAM_POWER_ON, /**启动双目相机，开机默认启动*/
		SC_DUALCAM_POWER_OFF, /**关闭双目相机，开机默认启动*/
		SC_DUALCAM_SYNC_TRIG,/**双目相机采集同步*/

		SC_NONE
	};

	/**
	*	指示当前碰撞状态，可通过 Aim_SetCollisinoDetectLevel()函数设置碰撞检测的灵敏度。
	*/
	enum E_CollisionStatus
	{
		COLLISION_NOT_OCCURRED,	/**< 碰撞未发生 */
		COLLISION_OCCURRED,		/**< 碰撞发生，此时可通过 Aim_SetSystemCommand()函数下发
								*	SC_COLLISION_INFO_CLEAR指令清除碰撞警告。在发生碰撞时碰
								*	撞检测将暂停，只有清除当前的碰撞警告才能开启下一次碰撞
								*	检测。 */
		COLLISION_NOT_START,	/**< 碰撞检测未开启 */
	};

	/**
	*	指示当前双目相机的背景亮度是否正常，在亮度不正常的情况下获取到的标记点坐标可能不准确。
	*/
	enum E_BackgroundLightStatus
	{
		BG_LIGHT_OK,		/**< 背景光正常 */
		BG_LIGHT_ABNORMAL,	/**< 背景光异常 */
	};

	/**
	*	指示当前硬件状态，当发生异常时，请先尝试重启仪器，若不能解决，请尽快联系厂家维修。
	*/
	enum E_HardwareStatus
	{
		HW_OK,							/**< 硬件状态正常 */
		HW_LCD_VOLTAGE_TOO_LOW,			/**< 触控显示屏电压过低 */
		HW_LCD_VOLTAGE_TOO_HIGH,		/**< 触控显示屏电压过高 */
		HW_IR_LEFT_VOLTAGE_TOO_LOW,		/**< 左相机照明系统电压过低 */
		HW_IR_LEFT_VOLTAGE_TOO_HIGH,	/**< 左相机照明系统电压过高 */
		HW_IR_RIGHT_VOLTAGE_TOO_LOW,	/**< 右相机照明系统电压过低 */
		HW_IR_RIGHT_VOLTAGE_TOO_HIGH,	/**< 右相机照明系统电压过高 */
		HW_GET_INITIAL_DATA_ERROR,		/**< 获取初始化数据错误 */
	};

	/**
	*	API从定位仪读取的数据。
	*/
	enum E_DataType
	{
		DT_NONE,			/**< 不读取数据（开机默认） */			
		DT_INFO,			/**< 仅读取信息数据（包括系统状态和三维坐标）*/
		DT_MARKER_INFO_WITH_WIFI,		/**< 仅读取三维坐标数据，仅使用wifi通讯时有效 */
		DT_STATUS_INFO,		/**< 仅读取系统状态数据，仅使用wifi通讯时有效 */
		DT_IMGDUAL,			/**< 仅读取双目图像数据，使用WiFi通讯时无效 */
		DT_IMGCOLOR,		/**< 仅读取彩色图像数据，使用WiFi通讯时无效 */
		DT_INFO_IMGDUAL,	/**< 同时读取信息数据（包括系统状态和三维坐标）和双目图像数据 */
		DT_INFO_IMGCOLOR,	/**< 同时读取信息数据（包括系统状态和三维坐标）和彩色图像数据 */
		DT_INFO_IMGDUAL_IMGCOLOR,	
						/**< 同时读取信息数据（包括系统状态和三维坐标），双目图像数据和彩色图像数据*/
	};

	/**
	*	API函数的返回值
	*/
	enum E_ReturnValue
	{
		AIMOOE_ERROR = -1,		/**< 未知错误，请确认函数调用是否正确 */
		AIMOOE_OK=0,				/**< 函数执行成功 */
		AIMOOE_CONNECT_ERROR,	/**< 设备打开错误，请确认设备连接是否正确 */
		AIMOOE_NOT_CONNECT,		/**< 设备未连接，请先连接设备 */
		AIMOOE_READ_FAULT,		/**< 读设备错误，请确认设备连接是否正常 */
		AIMOOE_WRITE_FAULT,		/**< 写设备错误，请确认设备连接是否正常  */
		AIMOOE_NOT_REFLASH,		/**< 数据信息未更新，若所要获取的数据已正确设置，请重试 */
		AIMOOE_INITIAL_FAIL,		/**<初始化失败*/
		AIMOOE_HANDLE_IS_NULL,	/**<未调用初始化函数获取句柄*/
	};

	enum E_MarkWarnType
	{
		eWarn_None = 0,//未靠近视场边缘
		eWarn_Common = 1,//靠近视场边缘
		eWarn_Critical = 2,//非常靠近视场边缘
	};

	enum E_AcquireMode
	{
		ContinuousMode = 0,//连续采集
		SingleMasterMode = 1,//单次主动采集
		SingleSlaveMode = 2,//单次从动采集
	};

	enum E_ToolFixRlt
	{
		eToolFixCancle = 0,
		eToolFixRedo = 1,
		eToolFixSave = 2

	};

	enum E_RTDirection
	{
		FromToolToOpticalTrackingSystem= 0,
		FromOpticalTrackingSystemToTool = 1
	};

	struct T_Img_Info
	{
		unsigned int width;//图像宽度
		unsigned int height;//图像高度
		UCHAR channel;//灰度图像为1通道8bit，彩色图像为2通道16bit（RGB565）

	};

	struct T_AIMPOS_DATAPARA
	{

		AIMPOS_TYPE devtype;
		T_Img_Info dualimg;
		T_Img_Info colorimg;
		char hardwareinfo[20] = { 0 };//型号信息

	};

	/**
	*	包含定位仪出厂信息
	*/
	struct T_ManufactureInfo
	{
		USHORT Year;			/**< 出厂日期：年 */
		UCHAR Month;			/**< 出厂日期：月 */
		UCHAR Day;			/**< 出厂日期：日 */
		char Version[20];	/**< 版本号 */
		UCHAR VersionLength;	/**< 版本号的字符长度 */
	};

	/**
	*	包含系统状态信息
	*/
	struct T_AimPosStatusInfo
	{
		float Tcpu;			/**< CPU温度 (-128℃~127℃)，要保证CPU温度小于75℃*/
		float Tpcb;			/**< 主板温度(-128℃~127℃)，主板温度大于50℃时，仪
							*	器会发出两声警报*/
		UCHAR LeftCamFps;	/**< 左相机实时帧率（fps）*/
		UCHAR RightCamFps;	/**< 右相机实时帧率（fps）*/
		UCHAR ColorCamFps;	/**< 中间彩色相机实时帧率（fps）*/
		UCHAR LCDFps;		/**< 触控显示屏实时帧率（fps）*/
		USHORT ExposureTimeLeftCam;
							/**< 左相机实时曝光值(0x30-0x4000)*/
		USHORT ExposureTimeRightCam;
							/**< 右相机实时曝光值(0x30-0x4000)*/
		E_CollisionStatus CollisionStatus;	/**< 当前碰撞状态 */
		E_HardwareStatus HardwareStatus;	/**< 当前硬件状态 */
	};
	/**指示标记点的位置靠近相机视场边缘的等级*/


	/**
	*	包含标记点坐标信息
	*/
	struct T_MarkerInfo
	{
		/** 标记点坐标编号，系统更新一次坐标，编号自动加1，
		*连续采集模式下相邻两帧的采集时间间隔为1/60 秒*/
		UINT ID;	

		/** 检测到的标记点数量，0表示未检测到标记点，标记
		   *	点数量最大为200个*/
		int MarkerNumber;	

		/** 系统的坐标点集：存放标记点的三维坐标，数组类型为double，数组长
		*	度为PtMaxNUM*3（其中PtMaxNUM =200），每个标记点
		*	占3个长度，坐标顺序为X->Y->Z；*/					
		double MarkerCoordinate[PtMaxNUM * 3];

		/**幻影点警告数组：令value=PhantomMarkerWarning[i]，
		* 当value>0时，表示第i个点有可能是幻影点
		* value值表示该点所属的幻影点分组组号*/
		int PhantomMarkerWarning[PtMaxNUM];

		/**幻影点分组的总数，若不存在幻影点，则该值为0 */
		int PhantomMarkerGroupNumber;

		/** 指示当前双目相机的背景亮度是否正常，在亮度不正
		*   常的情况下获取到的标记点坐标可能不准确 */
		E_BackgroundLightStatus MarkerBGLightStatus;

		/**指示每个标记点靠近相机视场边缘的等级*/
		E_MarkWarnType MarkWarn[PtMaxNUM];
		/**左相机识别到的所有标记点中，最靠近视场边缘的点的等级*/
		E_MarkWarnType bLeftOutWarnning;
		/**右相机识别到的所有标记点中，最靠近视场边缘的点的等级*/
		E_MarkWarnType bRightOutWarning;
	};

	struct T_ToolTipPivotInfo
	{
		bool isToolFind;			/**工具是否被找到*/
		bool isPivotFinished;	/**绕点旋转过程是否结束*/
		float pivotRate;		/**绕点旋转进度*/
		float pivotMeanError;		/**尖端获取的平均误差*/
	};

	struct t_ToolTipCalProInfo
	{
		bool isBoardFind;			/**标定板工具是否被找到*/
		bool isToolFind;			/**定位工具是否被找到*/
		bool isValidCalibrate;		/**当次标定是否有效*/
		bool isCalibrateFinished;	/**标定过程是否结束*/
		float CalibrateError;		/**标定的平均误差*/
		float CalibrateRate;		/**标定进度*/
		float CalRMSError;			/**标定的RMS误差*/
	};

	struct t_ToolFixProInfo//工具识别信息校准
	{
		bool isToolFind;	//当次校准是否找到工具
		bool validfixflag;//当次校准是否有效
		int  isValidFixCnt;//有效校准次数，达到100次时结束
		bool isCalibrateFinished;//校准是否结束
		float MatchError;//校准后的匹配误差
		bool *mpMarkMatchStatus;//此变量不再使用，指示当次校准，工具上的点是否匹配
		int totalmarkcnt;//所选工具的标记点数
	};
	struct t_ToolMadeProInfo//工具识别信息文件制作
	{
		bool unValidMarkerFlag;//当次采集出现无效点
		float madeRate;//进度
		bool isMadeProFinished;
		float MadeError;//制作误差
	
	};
	struct t_ToolBaseInfo
	{
		char  *name;
		char  isBoard;
		char  markcnt;
	};

	struct T_AimToolDataResult
	{
		E_AimToolType type;			//工具类型，目前有定位工具和标定板工具
		bool validflag;				//当前识别的工具是否有效
		char  toolname[PtMaxNUM];	//工具名称（对应工具文件的第一行）
		float MeanError;				//工具上点距偏差的绝对平均误差
		float Rms;					//工具点距离误差的标准差

		float rotationvector[3];	//从工具坐标系到系统坐标系的旋转向量
		float Qoxyz[4];				//从工具坐标系到系统坐标系的四元数，顺序为Qx->Qy->Qz->Qo
		float Rto[3][3];			//从工具坐标系到系统坐标系的旋转矩阵
		float Tto[3];				//从工具坐标系到系统坐标系的平移矩阵

		float OriginCoor[3];		//工具在系统坐标系下原点坐标
		float tooltip[3];			//工具在系统坐标系下尖端坐标
		float toolmid[3];			//工具在系统坐标系下尖端方向上另一点坐标
		float toolCstip[3];			//工具坐标系尖端坐标(即该工具的工具文件上的针尖坐标)
		float toolCsmid[3];			//工具坐标系尖端方向上另一点坐标
		std::vector<int>toolptidx;	//工具上的点在系统的坐标点集下的索引，对于不匹配的点，其索引为-1
		T_AimToolDataResult*next;
	};

	struct T_AimToolDataResultSingle
	{
		E_AimToolType type;			//工具类型，目前有定位工具和标定板工具
		bool validflag;				//当前识别的工具是否有效
		char  toolname[PtMaxNUM];	//工具名称（对应工具文件的第一行）
		float MeanError;				//工具上点距偏差的绝对平均误差
		float Rms;					//工具点距离误差的标准差

		float rotationvector[3];	//从工具坐标系到系统坐标系的旋转向量
		float Qoxyz[4];				//从工具坐标系到系统坐标系的四元数，顺序为Qx->Qy->Qz->Qo
		float Rto[3][3];			//从工具坐标系到系统坐标系的旋转矩阵
		float Tto[3];				//从工具坐标系到系统坐标系的平移矩阵

		float OriginCoor[3];		//工具在系统坐标系下原点坐标
		float tooltip[3];			//工具在系统坐标系下尖端坐标
		float toolmid[3];			//工具在系统坐标系下尖端方向上另一点坐标
		float toolCstip[3];			//工具坐标系尖端坐标(即该工具的工具文件上的针尖坐标)
		float toolCsmid[3];			//工具坐标系尖端方向上另一点坐标
		int toolPtNum;		//工具上的总点数
		int toolPtId[16];	//工具上的点在系统的坐标点集下的索引，对于不匹配的点，其索引为-1，工具上的点不能超过16个，若超出，只显示前16个。

	};


	//精度工具上，左右两个工具的测量结果
	struct T_AccuracyToolResult
	{	
		bool validflag; //当次检测是否有效
		float Dis;//左右工具原点的距离
		float Angle[3];//左右工具法向量的夹角，单位为弧度
	};
	struct T_ToolFileData
	{
		char  toolname[PtMaxNUM];//工具名
		char tooType;
		int markerNumbers;
		float MarkerCoordinate[16 * 3];//若markerNumbers>16则时显示前16个标记点坐标；
		int  constTipNum;
		float tipHeadCoordinate[3];
		float tipBodyCoordinate[3];
	};


#ifdef __cplusplus
}
#endif





