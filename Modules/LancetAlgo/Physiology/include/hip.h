#ifndef HIP_H
#define HIP_H

#include <array>
#include <iostream>
#include <map>
#include <memory>
#include <vector>

#include <itkeigen/Eigen/Eigen>
namespace FuturTecAlgorithm
{
	
	enum class ESide
	{
		right,
		left
	};

	inline const char* to_String(ESide e)
	{
		switch (e)
		{
		case ESide::right: return "right";
		case ESide::left: return "left";
		default: return "unknown";
		}
	}

	enum class ELandMarks
	{
		//==========================Pelvis=======================
		/** \brief [UserInput] A registration point located near the lowermost border of the operative posterior acetabulum.*/
		p_PostHorn = 0,
		/** \brief [UserInput] A registration point located near the lowermost border of the operative anterior acetabulum..*/
		p_AntHorn,
		/** \brief [UserInput] A depressed area located lateral to the anterior margin of the operative acetabulum,
		 * easily palpable or identifiable during surgery.
		 * Used only as an alternative choice for the anterior horn landmark in direct anterior pelvic registration.*/
		p_AntNotch,

		/** \brief [UserInput] Right ASIS(Anterior superior iliac spine) */
		p_ASIS_R,
		/** \brief [UserInput] Left ASIS(Anterior superior iliac spine) */
		p_ASIS_L,
		/** \brief [UserInput] Right Pubic tubercle,The rounded eminence located at the upper border of the right pubis near the pubic symphysis. */
		p_PT_R,
		/** \brief [UserInput] Left Pubic tubercle */
		p_PT_L,
		/** \brief [UserInput] Leaving the boneand centrally located in the symphysis pubis, it is used to determine the midlineand plane of segmentation of the pelvis. */
		p_MidLine,
		/** \brief [UserInput]The top line of the sacral plate (S1) defined on the sagittal CT image, used to determine the angle between the sacral plate and the anterior-posterior axis.	 */
		p_SS_A,
		/** \brief [UserInput]The top line of the sacral plate (S1) defined on the sagittal CT image, used to determine the angle between the sacral plate and the anterior-posterior axis.	 */
		p_SS_P,
		/** \brief [UserInput]Hip Center of Rotation	(Operation Side) */
		p_COR,

		/** \brief [ModelOutput] the Origin of Pelvis local coordinates */
		p_O,

		// /** \brief [ModelOutput] the Center of Right Acetabular */
		// p_RAC,
		// /** \brief [ModelOutput] the Center of Left Acetabular */
		// p_LAC,


		//==========================Femur=======================

		/** \brief [UserInput] Femoral Neck Saddle */
		f_FNS,
		/** \brief [UserInput] Femur Landmark 2 */
		f_FL2,
		/** \brief [UserInput] Greater Trochanter */
		f_GT,

		/** \brief [UserInput] Femur Head Center */
		f_FHC,
		/** \brief [UserInput] Femur Neck center */
		f_FNC,
		/** \brief [UserInput] Proximal point of Femoral canal axis */
		f_PFCA,
		/** \brief [UserInput] Distal point of Femoral canal axis  */
		f_DFCA,
		/** \brief [UserInput] Medial Femoral Epicondyle */
		f_ME,
		/** \brief [UserInput] Lateral Femoral Epicondyle  */
		f_LE,
		/** \brief [UserInput] Lesser Trochanter */
		f_LT,
		/** \brief [ModelOutput] Midpoint of Medial and Lateral Epicondyles */
		f_MidEEs,

		f_O,

		/** \brief [UserInput] Proximal Checkpoint,The position before the operation   */
		f_CheckPointP_pre,
		/** \brief [UserInput] Distal Checkpoint,The position before the operation    */
		f_CheckPointD_pre,
		/** \brief [UserInput] Proximal Checkpoint,The position after the operation   */
		f_CheckPointP_post,
		/** \brief [UserInput] Distal Checkpoint,The position after the operation    */
		f_CheckPointD_post,
		/** \brief [UserInput] Femur Head Center During the operation, the surgeon will place four points on the cup to verify its placement.  */
		f_FHC_inOp
	};

	inline const char* to_String(ELandMarks e)
	{
		switch (e)
		{
		case ELandMarks::p_PostHorn: return "p_PostHorn";
		case ELandMarks::p_AntHorn: return "p_AntHorn";
		case ELandMarks::p_AntNotch: return "p_AntNotch";
		case ELandMarks::p_ASIS_R: return "p_ASIS_R";
		case ELandMarks::p_ASIS_L: return "p_ASIS_L";
		case ELandMarks::p_PT_R: return "p_PT_R";
		case ELandMarks::p_PT_L: return "p_PT_L";
		case ELandMarks::p_MidLine: return "p_MidLine";
		case ELandMarks::p_SS_A: return "p_SS_A";
		case ELandMarks::p_SS_P: return "p_SS_P";
		case ELandMarks::p_COR: return "p_COR";
		case ELandMarks::p_O: return "p_O";
		case ELandMarks::f_FNS: return "f_FNS";
		case ELandMarks::f_FL2: return "f_FL2";
		case ELandMarks::f_GT: return "f_GT";
		case ELandMarks::f_FHC: return "f_FHC";
		case ELandMarks::f_FNC: return "f_FNC";
		case ELandMarks::f_PFCA: return "f_PFCA";
		case ELandMarks::f_DFCA: return "f_DFCA";
		case ELandMarks::f_ME: return "f_ME";
		case ELandMarks::f_LE: return "f_LE";
		case ELandMarks::f_LT: return "f_LT";
		case ELandMarks::f_CheckPointP_pre: return "f_CheckPointP_pre";
		case ELandMarks::f_CheckPointD_pre: return "f_CheckPointD_pre";
		case ELandMarks::f_CheckPointP_post: return "f_CheckPointP_post";
		case ELandMarks::f_CheckPointD_post: return "f_CheckPointD_post";
		case ELandMarks::f_FHC_inOp: return "f_FHC_inOp";
		case ELandMarks::f_MidEEs: return "f_MidEEs";
		case ELandMarks::f_O: return "f_O";
		default: return "unknown";
		}
	}

	enum class EAxes
	{
		/** \brief [ModelOutput] Pelvic local coords X axis,
		 * pointing to anterior,Perpendicular to the plane defined by the bilateral anterior superior iliac spines and midpoint, perpendicular to the Z-axis, . */
		p_X = 0,
		/** \brief [ModelOutput] Pelvic local coords Y axis,
		 * pointing to head,Perpendicular to both the X-axis and Z-axis. */
		p_Y,
		/** \brief [ModelOutput] Pelvic local coords Z axis,
		 * pointing to the right. Parallel to the line connecting the left and right anterior superior iliac spines */
		p_Z,
		/** \brief [ModelOutput] Pelvic Horizontal Axis, The line connecting LASI and RASI */

		/** \brief [ModelOutput] Femur Mechanical Axis*/
		f_Mechanics,
		/** \brief [ModelOutput] Femur Canal Axis*/
		f_Canal,
		/** \brief [ModelOutput] Femur Epicondylar Axis*/
		f_Epicondyla,
		/** \brief [ModelOutput] Femur Neck Axis*/
		f_Neck,

		f_X_canal,
		f_Y_canal,
		f_Z_canal,

		f_X_mechanical,
		f_Y_mechanical,
		f_Z_mechanical,


		//prothesis
		cup_X,
		cup_Y,
		cup_Z
	};

	inline  const char* to_string(EAxes e)
	{
		switch (e)
		{
		case EAxes::p_X: return "p_X";
		case EAxes::p_Y: return "p_Y";
		case EAxes::p_Z: return "p_Z";
		case EAxes::f_Mechanics: return "f_Mechanics";
		case EAxes::f_Canal: return "f_Canal";
		case EAxes::f_Epicondyla: return "f_Epicondyla";
		case EAxes::f_Neck: return "f_Neck";
		case EAxes::f_X_canal: return "f_X_canal";
		case EAxes::f_Y_canal: return "f_Y_canal";
		case EAxes::f_Z_canal: return "f_Z_canal";
		case EAxes::f_X_mechanical: return "f_X_mechanical";
		case EAxes::f_Y_mechanical: return "f_Y_mechanical";
		case EAxes::f_Z_mechanical: return "f_Z_mechanical";
		default: return "unknown";
		}
	}

	enum class EPlanes
	{
		MIDPLANE = 0,
	};

	inline const char* to_String(EPlanes e)
	{
		switch (e)
		{
		case EPlanes::MIDPLANE: return "MIDPLANE";
		default: return "unknown";
		}
	}

	enum class EResult
	{
		/** \brief [ModelOutput] 骨盆前倾角*/
		f_PT = 0,
		/** \brief [ModelOutput] 在双侧股骨机械轴对齐，股骨偏心距最大处与冠状面平行的前提下，从小转子到髂前上棘连线（即骨盆横轴）的距离*/
		f_HipLength,
		/** \brief [ModelOutput] 在股骨干垂直，股骨偏心距最大处与冠状面平行，股骨干轴到骨盆中轴面的距离*/
		f_Offset,

		f_HipLength_post,
		f_Offset_post,
		/** \brief [ModelOutput] Difference(mm) in hip length between the operative and contralateral sides before operation*/
		f_HipLengthDiff_preOp2Contral,
		/** \brief [ModelOutput] Difference(mm) in hip length between the operative and contralateral sides after operation*/
		f_HipLengthDiff_Op2Contralateral,
		/** \brief [ModelOutput] Difference(mm) in hip length before and after operation */
		f_HipLengthDiff_PrePostOp,
		/** \brief[ModelOutput] Difference(mm) in Combined Offset between the operative and contralateral sides before operation */
		f_OffsetDiff_preOp2Contral,
		/** \brief[ModelOutput] Difference(mm) in Combined Offset between the operative and contralateral sides after operation*/
		f_OffsetDiff_Op2Contralateral,
		/** \brief[ModelOutput] Difference(mm) in Combined Offset before and after operation */
		f_OffsetDiff_PrePostOp,
		/** \brief [ModelOutput] 术侧股骨倾角*/
		f_OpVersion,

		/** \brief [ModelOutput] 右侧臼杯前倾角*/
		p_RightAnteversion,
		/** \brief [ModelOutput] 右侧臼杯外展角*/
		p_RightInclination,
		/** \brief [ModelOutput] 左侧臼杯前倾角*/
		p_LeftAnteversion,
		/** \brief [ModelOutput] 左侧臼杯外展角*/
		p_LeftInclination
	};

	inline  const char* to_string(EResult e)
	{
		switch (e)
		{
		case EResult::f_PT: return "f_PT";
		case EResult::f_HipLength: return "f_HipLength";
		case EResult::f_Offset: return "f_Offset";
		case EResult::f_HipLength_post: return "f_HipLength_post";
		case EResult::f_Offset_post: return "f_Offset_post";
		case EResult::f_HipLengthDiff_preOp2Contral: return "f_HipLengthDiff_preOp2Contral";
		case EResult::f_HipLengthDiff_Op2Contralateral: return "f_HipLengthDiff_Op2Contralateral";
		case EResult::f_HipLengthDiff_PrePostOp: return "f_HipLengthDiff_PrePostOp";
		case EResult::f_OffsetDiff_preOp2Contral: return "f_OffsetDiff_preOp2Contral";
		case EResult::f_OffsetDiff_Op2Contralateral: return "f_OffsetDiff_Op2Contralateral";
		case EResult::f_OffsetDiff_PrePostOp: return "f_OffsetDiff_PrePostOp";
		case EResult::f_OpVersion: return "f_OpVersion";
		case EResult::p_RightAnteversion: return "p_RightAnteversion";
		case EResult::p_RightInclination: return "p_RightInclination";
		case EResult::p_LeftAnteversion: return "p_LeftAnteversion";
		case EResult::p_LeftInclination: return "p_LeftInclination";
		default: return "unknown";
		}
	}

	using PointType = Eigen::Vector3d;
	using VectorType = Eigen::Vector3d;
	//using LandMarkPair = std::pair<ELandMarks, PointType>;
	

	class AxisType
	{
	public:
		// enum class EConstractType
		// {
		// 	TWO_POINT,
		// 	POINT_DIRECTION
		// };

		// AxisType() = default;

		// AxisType(const PointType& p_start, const PointType& p_d, EConstractType type)
		// {
		// 	if (EConstractType::TWO_POINT == type)
		// 	{
		// 		startPoint = p_start;
		// 		direction = p_d - p_start;
		// 	}
		// 	else
		// 	{
		// 		startPoint = p_start;
		// 		direction = p_d;
		// 	}
		// }

		PointType startPoint{};
		VectorType direction{};
	};

	class PlaneType
	{
	public:
		double size{1};
		AxisType normal{};
	};

	class SegmentType
	{
	public:

		PointType point_1;
		PointType point_2;
	};

	enum ECorrection
	{
		ORIGIN,
		MECHANICS,
		CANAL,
		CHECKPOINT_PRE,
		CHECKPOINT_POST
	};


	enum EIVAngelType
	{
		RADIO_GRAPHIC,//常用，影像学定义
		OPERATIVE, //机械轴的定义
		ANATOMICAL //解剖学的定义
	};

	//helper functions
	static Eigen::Matrix4d ConvertCoordstoTransform(Eigen::Vector3d& o, Eigen::Vector3d& x, Eigen::Vector3d& y, Eigen::Vector3d& z);
	static void TransformPoint(Eigen::Matrix4d& transform, PointType& input, PointType& output)
	{
		Eigen::Isometry3d T;
		T = transform;
		output = T * input;
	}
	static void TransformAxis(Eigen::Matrix4d& transform, AxisType& input, AxisType& output)
	{
		Eigen::Isometry3d T;
		T = transform;
		output.startPoint = T * input.startPoint;
		output.direction = T * input.direction;
	}

	static void TransformPlane(Eigen::Matrix4d& transform, PlaneType& input, PlaneType& output)
	{
		Eigen::Isometry3d T;
		T = transform;
		output = input;
		TransformAxis(transform, input.normal, output.normal);
	}

	class Joint
	{
	public:
		Eigen::Matrix4d transform_parent;
		Eigen::Matrix4d transform_child;
		Eigen::Matrix4d transform_interJonit;
	};

	/**
	 * \brief base class to access data for all physio model.
	 *
	 * Body use map to store the data.
	 * 
	 */
	class Body
	{
	public:
		virtual ~Body() = default;

		Body() = default;

		Body(Body&& other) noexcept;
		Body& operator=(const Body& other);
		Body& operator=(Body&& other) noexcept;
		/**
		 * \brief Sets the position of a particular landmark in three-dimensional space.
		 *
		 * if a landmark has been set before, this method will overwrite the value.
		 * 
		 * \param name key, listed in Enum ELandMarks.
		 * \param data value, The position of a point in three dimensions.
		 * \see physioConst.h
		 */
		virtual void SetLandMark(ELandMarks name, double* data);
		
		/**
		 * \brief Get the position of a landmark by name.
		 * \param name [user input]
		 * \param oup_value [output]
		 * \return true if success.
		 */
		virtual bool GetLandMark(ELandMarks name, PointType& oup_value);
		virtual bool GetGlobalLandMark(ELandMarks name, PointType& oup_value);

		virtual void SetAxis(EAxes name, double* p_start, double* direction);
		virtual bool GetAxis(EAxes name, AxisType& outp_axis);
		virtual bool GetGlobalAxis(EAxes name, AxisType& outp_axis);

		virtual void SetPlane(EPlanes name, double* center, double* normal);
		virtual bool GetPlane(EPlanes name,PlaneType& outp_plane);
		virtual bool GetGlobalPlane(EPlanes name, PlaneType& outp_plane);

		virtual void SetResult(EResult name, double res);
		virtual bool GetResult(EResult name, double& outp_res);

		virtual void SetWorldTransform(Eigen::Matrix4d transform);

		virtual void calTransformImageToBodyISB() = 0;
		virtual void calTransformImageToBody() = 0;
		virtual void convertToLocal();
		Eigen::Matrix4d m_T_image_body;
		Eigen::Matrix4d m_T_body_image;
		Eigen::Matrix4d m_T_world_local;
	protected:
		std::string m_name;
		std::map<ELandMarks, PointType> m_landMarks{};
		std::map<EAxes, AxisType> m_axes{};
		std::map<EPlanes, PlaneType> m_planes{};
		std::map<EResult, double> m_results{};
	};


	/** \brief Base class for storing physiological and anatomical landmarks and data about the pelvis.
		*/
	class Pelvis : public Body
	{
	public:
		~Pelvis() override = default;

		//init
		void calTransformImageToBodyISB() override;
		void calTransformImageToBody() override;
	private:
		void calTransformWorldToPelvisISB();
		void calTransformImageToPelvisCT();
	};

	/** \brief Base class for storing physiological and anatomical landmarks and data about the right femur.
		*/
	class Femur : public Body
	{
	public:
		void calTransformImageToBodyISB() override;
		void calTransformImageToBody() override;
		void calTransformImageToBody_Canal();
		void calTransformImageToBody_Mechanical();
		Eigen::Matrix4d m_T_image_body_mechanical;
		ESide m_side{ESide::right};
	};

	class Cup : public Body
	{
	public:
		void init();
		ESide m_side{ ESide::right };

		void calTransformImageToBodyISB() override;
		void calTransformImageToBody() override;
	};

	/**
	 * compute the Anteversion angle and the Inclination Angle
	 *
	 * calculation is based on a radio_graphic coordinate system: x left , y backward , z head.
	 *
	 * @param direction [Input]The direction of the cup normal or Acetabular grinding rod.
	 * @param ResultAnteversion [Output] The compute result of Anteversion.
	 * @param ResultInclination [Output] The compute result of Inclination.
	 * @param type [Input] different type of Anteversion/Inclination angle, default:Radiographic
	 */
	void AnteversionAndInclinationAngle(double* direction, double& ResultAnteversion, double& ResultInclination,
		EIVAngelType type = EIVAngelType::RADIO_GRAPHIC);
	void AIAngleRadiographic(double* direction, double& ResultAnteversion, double& ResultInclination);
	void AIAngleOperative(double* direction, double& ResultAnteversion, double& ResultInclination);
	void AIAngleAnatomical(double* direction, double& ResultAnteversion, double& ResultInclination);

	inline Eigen::Matrix4d CalApplyAIAngleMatrix(Eigen::Vector3d center, double Anteversion, double Inclination,ESide side)
	{
		Anteversion = Anteversion / 180.0 * EIGEN_PI;
		Inclination = Inclination / 180.0 * EIGEN_PI;
		Eigen::Vector3d x, y, z;
		x << 1, 0, 0;
		y << 0, 1, 0;
		z << 0, 0, 1;

		Eigen::Isometry3d T;
		T.setIdentity();

		if (side == ESide::right)
		{
			const Eigen::AngleAxis rot(Inclination, y);
			T.rotate(rot);
		}
		else
		{
			const Eigen::AngleAxis rot(Inclination, -y);
			T.rotate(rot);
		}

		Eigen::Vector3d x_rot = T * x;
		Eigen::AngleAxis rot2(Anteversion, -x_rot);
		T.prerotate(rot2);

		Eigen::Vector3d center_rot = T * center;
		T.pretranslate(center - center_rot);

		return T.matrix();
	}

	 /**
	* Compute the Femoral Version Angle.
	*
	* can use either local or global points, all points are on Femur.
	*
	*	The native femoral version is the angle between the neck axis and epicondylar axis when
	* these 2 axes are projected on a plane perpendicular to the femur canal
	* a positive value (anteversion) is returned when the neck axis angled anteriorly relative to the
	* epicondylar axis.
	*
	* @param side [Input] define right or left femur.
	* @param FHC [Input] Femur Head Center.
	* @param FNC [Input] Femur Neck Center.
	* @param ME [Input] Medial Femoral Epicondyle.
	* @param LE [Input] Lateral Femoral Epicondyle.
	* @param DFCA [Input] Distal point of Femoral canal axis.
	* @param PFCA [Input] Proximal point of Femoral canal axis.
	* @return  Femoral Version Angle
	*/
	double FemoralVersionAngle(ESide side, double* FHC, double* FNC, double* ME, double* LE, double* DFCA, double* PFCA);

	/**
	 * \brief Pelvic Tilt is the angle between APP and coronal plane.
	 * when the line of the anterior superior iliac spine (X-axis) is virtually corrected
	 * to be parallel to the inner lateral (horizontal) direction.
	 * The Angle between the Y axis of the pelvis and the Y axis of the world coordinate system .
	 * \param pelvicYAxis Perpendicular to the plane defined by the ASIS and midLine point and pointing backward.
	 * \return Pelvic Tilt angle
	 */
	inline double PelvicTilt(double* pelvicYAxis)
	{
		// if the patient direction meets the requirements, the supine pelvis tilt is angle between pelvicFrame's
		// y axis and worldFrame's y axis

		Eigen::Vector3d y_pelvicFrame;
		y_pelvicFrame << pelvicYAxis[0], pelvicYAxis[1], pelvicYAxis[2];

		Eigen::Vector3d y_worldFrame;
		y_worldFrame << 0, 1, 0;

		double tmpDotProduct = y_pelvicFrame.dot(y_worldFrame);
		Eigen::Vector3d tmpCrossProduct = y_pelvicFrame.cross(y_worldFrame);
		double angle = (180.0 / EIGEN_PI) * acos(tmpDotProduct);
	
		if (tmpCrossProduct[0] > 0)
		{
			angle = -angle;
		}
		return angle;
	}

	inline double GetPointToLineDistance(double* point, double* linePoint_0, double* linePoint_1)
	{
		Eigen::Vector3d lineVector;
		lineVector[0] = linePoint_0[0] - linePoint_1[0];
		lineVector[1] = linePoint_0[1] - linePoint_1[1];
		lineVector[2] = linePoint_0[2] - linePoint_1[2];

		Eigen::Vector3d tmpVector;
		tmpVector[0] = linePoint_0[0] - point[0];
		tmpVector[1] = linePoint_0[1] - point[1];
		tmpVector[2] = linePoint_0[2] - point[2];

		double projection = lineVector.dot(tmpVector) / lineVector.norm();

		double distance = sqrt(pow(tmpVector.norm(), 2) - pow(projection, 2));

		return distance;
	}

	/**
	 * \brief hip length is the distance from the lesser trochanter to the ASIS line.
	 * when the femur is mechanically aligned and the maximum femural offset is parallel to the coronal plane.
	 *
	 * MUST use global points in aligned pose.
	 * \param LT Lesser Trochanter,Left or Right
	 * \param ASIS_L Anterior Superior lliac Spine（Left）
	 * \param ASIS_R Anterior Superior lliac Spine（Right）
	 * \return hip length(mm)
	 */
	inline double HipLength(double* LT, double* ASIS_L, double* ASIS_R)
	{
		return GetPointToLineDistance(LT, ASIS_L, ASIS_R);
	};


	//
	/**
	 * \brief CombinedOffset is the distance from the canal axis to the sagittal plane passing through the midline point.
	 * When the femoral canal is perpendicular and the femoral offset is at its maximum parallel to the coronal plane.
	 *
	 * \param PFCA Proximal point of Femoral canal axis.
	 * \param DFCA Distal point of Femoral canal axis
	 * \param MidLine Pelvic Middle Point.
	 * \return CombinedOffset(mm)
	 */
	inline double CombinedOffset(double* PFCA, double* DFCA, double* MidLine)
	{
		return GetPointToLineDistance(MidLine, PFCA, DFCA);
	};


	//Cal local Frame
	Eigen::Matrix4d calPelvisLocalFrame(Eigen::Vector3d& ASIS_L, Eigen::Vector3d& ASIS_R, Eigen::Vector3d& MidLine);

	Eigen::Matrix4d calFemurLocalFrame_canal(Eigen::Vector3d& FHC, Eigen::Vector3d& FNC, Eigen::Vector3d& DFCA, Eigen::Vector3d& PFCA, ESide side);

	Eigen::Matrix4d calFemurLocalFrame_mechanical(Eigen::Vector3d& FHC, Eigen::Vector3d& FNC, Eigen::Vector3d& ME,
		Eigen::Vector3d& LE, ESide side);

	//Virtual Corrrection
	inline Eigen::Matrix4d CalPelvisCorrectionMatrix(Eigen::Vector3d ASIS_R, Eigen::Vector3d ASIS_L)
	{
		Eigen::Vector3d x_world;
		x_world << 1, 0, 0;
		Eigen::Vector3d x_pelvis;
		x_pelvis = ASIS_L - ASIS_R;
		Eigen::Matrix3d rot = Eigen::Quaterniond().FromTwoVectors(x_pelvis, x_world).matrix();

		Eigen::Isometry3d T;
		T.setIdentity();
		T.rotate(rot);

		Eigen::Vector3d ASIS_R_roted = T * ASIS_R;
		Eigen::Vector3d t = ASIS_R - ASIS_R_roted;

		T.pretranslate(t);

		return T.matrix();
	}

	inline Eigen::Matrix4d CalFemurCanalCorrectionMatrix(Eigen::Vector3d FHC, Eigen::Vector3d FNC, Eigen::Vector3d DFCA, Eigen::Vector3d PFCA,ESide side)
	{
		//cal axes
		Eigen::Matrix4d localFrame = calFemurLocalFrame_canal(FHC, FNC, DFCA, PFCA, side);
		//rotate to align axes
		Eigen::Matrix3d rot;
		rot.setIdentity();
		rot = localFrame.block<3, 3>(0, 0);

		Eigen::Isometry3d T;
		T.setIdentity();
		T.rotate(rot.inverse());

		//translate back to fhc
		Eigen::Vector3d fhc_rot = T * FHC;
		Eigen::Vector3d t = FHC - fhc_rot;
		T.pretranslate(t);

		return T.matrix();
	}

	inline Eigen::Matrix4d CalFemurMechanicalCorrectionMatrix(Eigen::Vector3d FHC, Eigen::Vector3d FNC, Eigen::Vector3d ME, Eigen::Vector3d LE, ESide side)
	{
		//cal axes
		Eigen::Matrix4d localFrame = calFemurLocalFrame_mechanical(FHC, FNC, ME, LE, side);
		//rotate to align axes
		Eigen::Matrix3d rot;
		rot.setIdentity();
		rot = localFrame.block<3, 3>(0, 0);

		Eigen::Isometry3d T;
		T.setIdentity();
		T.rotate(rot.inverse());

		//translate back to fhc
		Eigen::Vector3d fhc_rot = T * FHC;
		Eigen::Vector3d t = FHC - fhc_rot;
		T.pretranslate(t);

		return T.matrix();
	}
}

#endif