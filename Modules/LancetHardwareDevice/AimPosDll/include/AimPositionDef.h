
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
	#define PtMaxNUM 200			/**< ��ǵ��������ֵ�������޸� */
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
	*	ͨѶ�ӿ�����
	*/
	enum E_Interface
	{
		I_USB=0,			/**< USBͨѶ�ӿ� */
		I_ETHERNET,	/**< ��̫��ͨѶ�ӿ� */
		I_WIFI,		/**< WIFIͨѶ�ӿ� */
	};

	/**
	*	��λ��ϵͳָ��
	*/
	enum E_SystemCommand
	{
		SC_COLLISION_DISABLE,	/**< �ر���ײ��⣨Ĭ�ϴ򿪣������е���ײ����������Զ���� */
		SC_COLLISION_ENABLE,	/**< ����ײ��⣨Ĭ�ϴ򿪣�����⵽��ײʱ��ϵͳ״̬��Ϣ��
								*	�ܶ�ȡ����ײ�����ľ��棬��ʱ��ײ�����ͣ��ֻ����ײ����
								*	�ľ��汻�������ܼ�������ײ��⡣���⣬��ͨ��
								*	Aim_SetCollisinoDetectLevel()����������ײ���������ȡ� */
		SC_COLLISION_INFO_CLEAR,/**< �����ײ�����ľ��� */
		SC_IRLED_ON,			/**< �򿪺���������Ĭ�ϴ򿪣�, ���ٶ�λ��ǵ�ʱ��Ҫ�򿪺���
								*	���� */
		SC_IRLED_OFF,			/**< �رպ���������Ĭ�ϴ򿪣����ڲ���Ҫ��������ʱ���ص�����
								*	���ɼ��ٹ��ģ��ӳ����� */
		SC_LASER_ON,			/**< �򿪶�λ���⣨Ĭ�Ϲرգ� */
		SC_LASER_OFF,			/**< �رն�λ���⣨Ĭ�Ϲرգ� */
		SC_LCD_PAGE_SUBPIXEL,	/**< ������ʾ���л�����ǵ�������ٽ��棨����Ĭ�Ͻ��棩 */
		SC_LCD_PAGE_COLOR,		/**< ������ʾ���л�����ɫͼ����ʾ���� */
		SC_LCD_ON,				/**< �򿪴�����ʾ�������(Ĭ�ϴ�) */
		SC_LCD_OFF,				/**< �رմ�����ʾ�������(Ĭ�ϴ�)���ڲ���Ҫ��Ļ��ʾʱ����
								*	����Ļ����ƿɼ��ٹ��ģ��ӳ����� */
		SC_AF_CONTINUOUSLY_ON,	/**< �м��ɫ��������Զ��Խ�������Ĭ�Ϲرգ���������ÿ��
								*	��ɫ���ʧȥ����ͻ�����Զ��Խ� */
		SC_AF_CONTINUOUSLY_OFF,	/**< �м��ɫ��������Զ��Խ��رգ�Ĭ�Ϲرգ����رպ��ɫ��
								*	�������ٽ��г����Զ��Խ� */
		SC_AF_SINGLE,			/**< �м��ɫ��������Զ��Խ���
								*	�·�ָ����м��ɫ��������е����Զ��Խ�����ʾ���ڴ�
								*	����ʾ����ɫͼ����ʾ���棬�����ɫͼ��Ҳ���Դ�����ɫ��
								*	�����е����Զ��Խ��� */
		SC_AF_FIX_INFINITY,		/**< �м��ɫ����̶����ൽ����Զ������Ĭ�϶Խ�����Զ�� */
		SC_AF_EXP_AUTO_ON,		/**< ���м��ɫ����Զ��ع⣨Ĭ�ϴ򿪣������ú�ϵͳ����
								*	��ǰ��ɫͼ�����ȵ����ع�ֵ���������� */
		SC_AF_EXP_AUTO_OFF,		/**< �ر��м��ɫ����Զ��ع⣨Ĭ�ϴ򿪣����رպ󣬱��ֹر�
								*	ʱ�̵��ع�ֵ���������� */
		SC_AF_RESTART,/**������ɫ���*/
		SC_DUALCAM_AUTO_EXP_ON,/**����˫Ŀ����Զ��ع�*/
		SC_DUALCAM_AUTO_EXP_OFF,/**�ر�˫Ŀ����Զ��ع�*/
		SC_DUALCAM_POWER_ON, /**����˫Ŀ���������Ĭ������*/
		SC_DUALCAM_POWER_OFF, /**�ر�˫Ŀ���������Ĭ������*/
		SC_DUALCAM_SYNC_TRIG,/**˫Ŀ����ɼ�ͬ��*/

		SC_NONE
	};

	/**
	*	ָʾ��ǰ��ײ״̬����ͨ�� Aim_SetCollisinoDetectLevel()����������ײ���������ȡ�
	*/
	enum E_CollisionStatus
	{
		COLLISION_NOT_OCCURRED,	/**< ��ײδ���� */
		COLLISION_OCCURRED,		/**< ��ײ��������ʱ��ͨ�� Aim_SetSystemCommand()�����·�
								*	SC_COLLISION_INFO_CLEARָ�������ײ���档�ڷ�����ײʱ��
								*	ײ��⽫��ͣ��ֻ�������ǰ����ײ������ܿ�����һ����ײ
								*	��⡣ */
		COLLISION_NOT_START,	/**< ��ײ���δ���� */
	};

	/**
	*	ָʾ��ǰ˫Ŀ����ı��������Ƿ������������Ȳ�����������»�ȡ���ı�ǵ�������ܲ�׼ȷ��
	*/
	enum E_BackgroundLightStatus
	{
		BG_LIGHT_OK,		/**< ���������� */
		BG_LIGHT_ABNORMAL,	/**< �������쳣 */
	};

	/**
	*	ָʾ��ǰӲ��״̬���������쳣ʱ�����ȳ������������������ܽ�����뾡����ϵ����ά�ޡ�
	*/
	enum E_HardwareStatus
	{
		HW_OK,							/**< Ӳ��״̬���� */
		HW_LCD_VOLTAGE_TOO_LOW,			/**< ������ʾ����ѹ���� */
		HW_LCD_VOLTAGE_TOO_HIGH,		/**< ������ʾ����ѹ���� */
		HW_IR_LEFT_VOLTAGE_TOO_LOW,		/**< ���������ϵͳ��ѹ���� */
		HW_IR_LEFT_VOLTAGE_TOO_HIGH,	/**< ���������ϵͳ��ѹ���� */
		HW_IR_RIGHT_VOLTAGE_TOO_LOW,	/**< ���������ϵͳ��ѹ���� */
		HW_IR_RIGHT_VOLTAGE_TOO_HIGH,	/**< ���������ϵͳ��ѹ���� */
		HW_GET_INITIAL_DATA_ERROR,		/**< ��ȡ��ʼ�����ݴ��� */
	};

	/**
	*	API�Ӷ�λ�Ƕ�ȡ�����ݡ�
	*/
	enum E_DataType
	{
		DT_NONE,			/**< ����ȡ���ݣ�����Ĭ�ϣ� */			
		DT_INFO,			/**< ����ȡ��Ϣ���ݣ�����ϵͳ״̬����ά���꣩*/
		DT_MARKER_INFO_WITH_WIFI,		/**< ����ȡ��ά�������ݣ���ʹ��wifiͨѶʱ��Ч */
		DT_STATUS_INFO,		/**< ����ȡϵͳ״̬���ݣ���ʹ��wifiͨѶʱ��Ч */
		DT_IMGDUAL,			/**< ����ȡ˫Ŀͼ�����ݣ�ʹ��WiFiͨѶʱ��Ч */
		DT_IMGCOLOR,		/**< ����ȡ��ɫͼ�����ݣ�ʹ��WiFiͨѶʱ��Ч */
		DT_INFO_IMGDUAL,	/**< ͬʱ��ȡ��Ϣ���ݣ�����ϵͳ״̬����ά���꣩��˫Ŀͼ������ */
		DT_INFO_IMGCOLOR,	/**< ͬʱ��ȡ��Ϣ���ݣ�����ϵͳ״̬����ά���꣩�Ͳ�ɫͼ������ */
		DT_INFO_IMGDUAL_IMGCOLOR,	
						/**< ͬʱ��ȡ��Ϣ���ݣ�����ϵͳ״̬����ά���꣩��˫Ŀͼ�����ݺͲ�ɫͼ������*/
	};

	/**
	*	API�����ķ���ֵ
	*/
	enum E_ReturnValue
	{
		AIMOOE_ERROR = -1,		/**< δ֪������ȷ�Ϻ��������Ƿ���ȷ */
		AIMOOE_OK=0,				/**< ����ִ�гɹ� */
		AIMOOE_CONNECT_ERROR,	/**< �豸�򿪴�����ȷ���豸�����Ƿ���ȷ */
		AIMOOE_NOT_CONNECT,		/**< �豸δ���ӣ����������豸 */
		AIMOOE_READ_FAULT,		/**< ���豸������ȷ���豸�����Ƿ����� */
		AIMOOE_WRITE_FAULT,		/**< д�豸������ȷ���豸�����Ƿ�����  */
		AIMOOE_NOT_REFLASH,		/**< ������Ϣδ���£�����Ҫ��ȡ����������ȷ���ã������� */
		AIMOOE_INITIAL_FAIL,		/**<��ʼ��ʧ��*/
		AIMOOE_HANDLE_IS_NULL,	/**<δ���ó�ʼ��������ȡ���*/
	};

	enum E_MarkWarnType
	{
		eWarn_None = 0,//δ�����ӳ���Ե
		eWarn_Common = 1,//�����ӳ���Ե
		eWarn_Critical = 2,//�ǳ������ӳ���Ե
	};

	enum E_AcquireMode
	{
		ContinuousMode = 0,//�����ɼ�
		SingleMasterMode = 1,//���������ɼ�
		SingleSlaveMode = 2,//���δӶ��ɼ�
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
		unsigned int width;//ͼ����
		unsigned int height;//ͼ��߶�
		UCHAR channel;//�Ҷ�ͼ��Ϊ1ͨ��8bit����ɫͼ��Ϊ2ͨ��16bit��RGB565��

	};

	struct T_AIMPOS_DATAPARA
	{

		AIMPOS_TYPE devtype;
		T_Img_Info dualimg;
		T_Img_Info colorimg;
		char hardwareinfo[20] = { 0 };//�ͺ���Ϣ

	};

	/**
	*	������λ�ǳ�����Ϣ
	*/
	struct T_ManufactureInfo
	{
		USHORT Year;			/**< �������ڣ��� */
		UCHAR Month;			/**< �������ڣ��� */
		UCHAR Day;			/**< �������ڣ��� */
		char Version[20];	/**< �汾�� */
		UCHAR VersionLength;	/**< �汾�ŵ��ַ����� */
	};

	/**
	*	����ϵͳ״̬��Ϣ
	*/
	struct T_AimPosStatusInfo
	{
		float Tcpu;			/**< CPU�¶� (-128��~127��)��Ҫ��֤CPU�¶�С��75��*/
		float Tpcb;			/**< �����¶�(-128��~127��)�������¶ȴ���50��ʱ����
							*	���ᷢ����������*/
		UCHAR LeftCamFps;	/**< �����ʵʱ֡�ʣ�fps��*/
		UCHAR RightCamFps;	/**< �����ʵʱ֡�ʣ�fps��*/
		UCHAR ColorCamFps;	/**< �м��ɫ���ʵʱ֡�ʣ�fps��*/
		UCHAR LCDFps;		/**< ������ʾ��ʵʱ֡�ʣ�fps��*/
		USHORT ExposureTimeLeftCam;
							/**< �����ʵʱ�ع�ֵ(0x30-0x4000)*/
		USHORT ExposureTimeRightCam;
							/**< �����ʵʱ�ع�ֵ(0x30-0x4000)*/
		E_CollisionStatus CollisionStatus;	/**< ��ǰ��ײ״̬ */
		E_HardwareStatus HardwareStatus;	/**< ��ǰӲ��״̬ */
	};
	/**ָʾ��ǵ��λ�ÿ�������ӳ���Ե�ĵȼ�*/


	/**
	*	������ǵ�������Ϣ
	*/
	struct T_MarkerInfo
	{
		/** ��ǵ������ţ�ϵͳ����һ�����꣬����Զ���1��
		*�����ɼ�ģʽ��������֡�Ĳɼ�ʱ����Ϊ1/60 ��*/
		UINT ID;	

		/** ��⵽�ı�ǵ�������0��ʾδ��⵽��ǵ㣬���
		   *	���������Ϊ200��*/
		int MarkerNumber;	

		/** ϵͳ������㼯����ű�ǵ����ά���꣬��������Ϊdouble�����鳤
		*	��ΪPtMaxNUM*3������PtMaxNUM =200����ÿ����ǵ�
		*	ռ3�����ȣ�����˳��ΪX->Y->Z��*/					
		double MarkerCoordinate[PtMaxNUM * 3];

		/**��Ӱ�㾯�����飺��value=PhantomMarkerWarning[i]��
		* ��value>0ʱ����ʾ��i�����п����ǻ�Ӱ��
		* valueֵ��ʾ�õ������Ļ�Ӱ��������*/
		int PhantomMarkerWarning[PtMaxNUM];

		/**��Ӱ�������������������ڻ�Ӱ�㣬���ֵΪ0 */
		int PhantomMarkerGroupNumber;

		/** ָʾ��ǰ˫Ŀ����ı��������Ƿ������������Ȳ���
		*   ��������»�ȡ���ı�ǵ�������ܲ�׼ȷ */
		E_BackgroundLightStatus MarkerBGLightStatus;

		/**ָʾÿ����ǵ㿿������ӳ���Ե�ĵȼ�*/
		E_MarkWarnType MarkWarn[PtMaxNUM];
		/**�����ʶ�𵽵����б�ǵ��У�����ӳ���Ե�ĵ�ĵȼ�*/
		E_MarkWarnType bLeftOutWarnning;
		/**�����ʶ�𵽵����б�ǵ��У�����ӳ���Ե�ĵ�ĵȼ�*/
		E_MarkWarnType bRightOutWarning;
	};

	struct T_ToolTipPivotInfo
	{
		bool isToolFind;			/**�����Ƿ��ҵ�*/
		bool isPivotFinished;	/**�Ƶ���ת�����Ƿ����*/
		float pivotRate;		/**�Ƶ���ת����*/
		float pivotMeanError;		/**��˻�ȡ��ƽ�����*/
	};

	struct t_ToolTipCalProInfo
	{
		bool isBoardFind;			/**�궨�幤���Ƿ��ҵ�*/
		bool isToolFind;			/**��λ�����Ƿ��ҵ�*/
		bool isValidCalibrate;		/**���α궨�Ƿ���Ч*/
		bool isCalibrateFinished;	/**�궨�����Ƿ����*/
		float CalibrateError;		/**�궨��ƽ�����*/
		float CalibrateRate;		/**�궨����*/
		float CalRMSError;			/**�궨��RMS���*/
	};

	struct t_ToolFixProInfo//����ʶ����ϢУ׼
	{
		bool isToolFind;	//����У׼�Ƿ��ҵ�����
		bool validfixflag;//����У׼�Ƿ���Ч
		int  isValidFixCnt;//��ЧУ׼�������ﵽ100��ʱ����
		bool isCalibrateFinished;//У׼�Ƿ����
		float MatchError;//У׼���ƥ�����
		bool *mpMarkMatchStatus;//�˱�������ʹ�ã�ָʾ����У׼�������ϵĵ��Ƿ�ƥ��
		int totalmarkcnt;//��ѡ���ߵı�ǵ���
	};
	struct t_ToolMadeProInfo//����ʶ����Ϣ�ļ�����
	{
		bool unValidMarkerFlag;//���βɼ�������Ч��
		float madeRate;//����
		bool isMadeProFinished;
		float MadeError;//�������
	
	};
	struct t_ToolBaseInfo
	{
		char  *name;
		char  isBoard;
		char  markcnt;
	};

	struct T_AimToolDataResult
	{
		E_AimToolType type;			//�������ͣ�Ŀǰ�ж�λ���ߺͱ궨�幤��
		bool validflag;				//��ǰʶ��Ĺ����Ƿ���Ч
		char  toolname[PtMaxNUM];	//�������ƣ���Ӧ�����ļ��ĵ�һ�У�
		float MeanError;				//�����ϵ��ƫ��ľ���ƽ�����
		float Rms;					//���ߵ�������ı�׼��

		float rotationvector[3];	//�ӹ�������ϵ��ϵͳ����ϵ����ת����
		float Qoxyz[4];				//�ӹ�������ϵ��ϵͳ����ϵ����Ԫ����˳��ΪQx->Qy->Qz->Qo
		float Rto[3][3];			//�ӹ�������ϵ��ϵͳ����ϵ����ת����
		float Tto[3];				//�ӹ�������ϵ��ϵͳ����ϵ��ƽ�ƾ���

		float OriginCoor[3];		//������ϵͳ����ϵ��ԭ������
		float tooltip[3];			//������ϵͳ����ϵ�¼������
		float toolmid[3];			//������ϵͳ����ϵ�¼�˷�������һ������
		float toolCstip[3];			//��������ϵ�������(���ù��ߵĹ����ļ��ϵ��������)
		float toolCsmid[3];			//��������ϵ��˷�������һ������
		std::vector<int>toolptidx;	//�����ϵĵ���ϵͳ������㼯�µ����������ڲ�ƥ��ĵ㣬������Ϊ-1
		T_AimToolDataResult*next;
	};

	struct T_AimToolDataResultSingle
	{
		E_AimToolType type;			//�������ͣ�Ŀǰ�ж�λ���ߺͱ궨�幤��
		bool validflag;				//��ǰʶ��Ĺ����Ƿ���Ч
		char  toolname[PtMaxNUM];	//�������ƣ���Ӧ�����ļ��ĵ�һ�У�
		float MeanError;				//�����ϵ��ƫ��ľ���ƽ�����
		float Rms;					//���ߵ�������ı�׼��

		float rotationvector[3];	//�ӹ�������ϵ��ϵͳ����ϵ����ת����
		float Qoxyz[4];				//�ӹ�������ϵ��ϵͳ����ϵ����Ԫ����˳��ΪQx->Qy->Qz->Qo
		float Rto[3][3];			//�ӹ�������ϵ��ϵͳ����ϵ����ת����
		float Tto[3];				//�ӹ�������ϵ��ϵͳ����ϵ��ƽ�ƾ���

		float OriginCoor[3];		//������ϵͳ����ϵ��ԭ������
		float tooltip[3];			//������ϵͳ����ϵ�¼������
		float toolmid[3];			//������ϵͳ����ϵ�¼�˷�������һ������
		float toolCstip[3];			//��������ϵ�������(���ù��ߵĹ����ļ��ϵ��������)
		float toolCsmid[3];			//��������ϵ��˷�������һ������
		int toolPtNum;		//�����ϵ��ܵ���
		int toolPtId[16];	//�����ϵĵ���ϵͳ������㼯�µ����������ڲ�ƥ��ĵ㣬������Ϊ-1�������ϵĵ㲻�ܳ���16������������ֻ��ʾǰ16����

	};


	//���ȹ����ϣ������������ߵĲ������
	struct T_AccuracyToolResult
	{	
		bool validflag; //���μ���Ƿ���Ч
		float Dis;//���ҹ���ԭ��ľ���
		float Angle[3];//���ҹ��߷������ļнǣ���λΪ����
	};
	struct T_ToolFileData
	{
		char  toolname[PtMaxNUM];//������
		char tooType;
		int markerNumbers;
		float MarkerCoordinate[16 * 3];//��markerNumbers>16��ʱ��ʾǰ16����ǵ����ꣻ
		int  constTipNum;
		float tipHeadCoordinate[3];
		float tipBodyCoordinate[3];
	};


#ifdef __cplusplus
}
#endif





