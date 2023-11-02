#ifndef body_h
#define body_h

#include <MitkOrthopedicsExports.h>
#include <mitkDataNode.h>
#include <mitkSurface.h>

#include "hipFunctions.h"
#include "hipTermDefinition.h"
#include "mitkImage.h"

namespace othopedics
{
	using PointType = Eigen::Vector3d;
	using VectorType = Eigen::Vector3d;

	class AxisType
	{
	public:


		PointType startPoint{};
		VectorType direction{};
	};

	class PlaneType
	{
	public:
		double size{ 1 };
		AxisType normal{};
	};

	class SegmentType
	{
	public:

		PointType point_1;
		PointType point_2;
	};

	//helper functions

	/**
	 * \brief Determine the intersection between lines.
	 * \param p1 a point on line 1
	 * \param v1 direction vector of line 1
	 * \param p2 a point on line 2
	 * \param v2 direction vector of line 2
	 * \param intersectP intersection point
	 * \return true,if intersection
	 */
	inline bool lineIntersect3d(const Eigen::Vector3d& p1, const Eigen::Vector3d& v1, const Eigen::Vector3d& p2, const Eigen::Vector3d& v2, Eigen::Vector3d& intersectP)
	{
		{
			if (v1.dot(v2) == 1)
			{
				// 两线平行
				return false;
			}

			Eigen::Vector3d startPointSeg = p2 - p1;
			Eigen::Vector3d vecS1 = v1.cross(v2);            // 有向面积1
			Eigen::Vector3d vecS2 = startPointSeg.cross(v2); // 有向面积2
			double num = startPointSeg.dot(vecS1);

			// 判断两这直线是否共面
			if (num >= 1E-05f || num <= -1E-05f)
			{
				return false;
			}

			// 有向面积比值，利用点乘是因为结果可能是正数或者负数　　　　　　　　　　　　　　　　if (qFuzzyIsNull(vecS1.lengthSquared())) {        　　　　　　return false;    　　　　　　}　
			float num2 = vecS2.dot(vecS1) / vecS1.squaredNorm();
			if (num2 > 1 || num < 0) {
				return false;//num2的大小还可以判断是延长线相交还是线段相交
			}

			intersectP = p1 + v1 * num2;
			return true;
		}
	}

	

	inline Eigen::Matrix4d vtkMatrix4x4ToEigen(const vtkSmartPointer<vtkMatrix4x4>& vtkMatrix)
	{
		Eigen::Matrix4d eigenMatrix;
		// Get a pointer to the VTK matrix data array
		const double* vtkData = vtkMatrix->GetData();

		// Copy the VTK matrix data to the Eigen matrix
		std::memcpy(eigenMatrix.data(), vtkData, 16 * sizeof(double));

		return eigenMatrix.transpose();
	}
	inline void EigenToVtkMatrix4x4(const Eigen::Matrix4d& matrix, vtkSmartPointer<vtkMatrix4x4>& vtkmatrix)
	{
		std::memcpy(vtkmatrix->GetData(), matrix.data(), 16 * sizeof(double));
		vtkmatrix->Transpose();
	}
//	static Eigen::Matrix4d ConvertCoordstoTransform(Eigen::Vector3d& o, Eigen::Vector3d& x, Eigen::Vector3d& y, Eigen::Vector3d& z);
	static void TransformPoint(Eigen::Matrix4d& transform, PointType& input, PointType& output)
	{
		Eigen::Isometry3d T;
		T = transform;
		output = T * input;
	}

	static void TransformAxis(const Eigen::Matrix4d& transform, const AxisType& input, AxisType& output)
	{
		Eigen::Isometry3d T;
		T = transform;
		output.startPoint = T * input.startPoint;
		Eigen::Vector3d endPoint = input.startPoint + input.direction;
		endPoint = T * endPoint;
		output.direction = endPoint - output.startPoint;
	}

	static void TransformPlane(Eigen::Matrix4d& transform, PlaneType& input, PlaneType& output)
	{
		Eigen::Isometry3d T;
		T = transform;
		output = input;
		TransformAxis(transform, input.normal, output.normal);
	}


	class MITKORTHOPEDICS_EXPORT Body :public itk::Object
  {
  public:
		mitkClassMacroItkParent(Body, itk::Object);
		itkFactorylessNewMacro(Self);


		itkSetMacro(Surface, mitk::Surface::Pointer);
		itkGetMacro(Surface, mitk::Surface::Pointer);

		itkSetMacro(Image, mitk::Image::Pointer);
		itkGetMacro(Image, mitk::Image::Pointer);

		virtual bool Init();

		void SetIndexToWorldTransform(Eigen::Matrix4d T);

		void AppendTransform(Eigen::Matrix4d T);

		void FlushTransform();
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
		virtual bool GetPlane(EPlanes name, PlaneType& outp_plane);
		virtual bool GetGlobalPlane(EPlanes name, PlaneType& outp_plane);

		virtual void SetResult(EResult name, double res);
		virtual bool GetResult(EResult name, double& outp_res);

		virtual void TransformAllInternalData(Eigen::Matrix4d transform);

		Eigen::Matrix4d m_T_world_local;
	protected:
		Body();
		~Body() override;

		void listenSurfaceGeoModify();
		void onSurfaceGeoModified();
		std::string m_name;
		std::map<ELandMarks, Eigen::Vector3d> m_landMarks{};
		std::map<EAxes, AxisType> m_axes{};
		std::map<EPlanes, PlaneType> m_planes{};
		std::map<EResult, double> m_results{};
		mitk::Surface::Pointer m_Surface{};
		mitk::Image::Pointer m_Image{};

		mitk::BaseGeometry::Pointer m_FollowedGeometry = nullptr;
		//! ITK tag for the observing of m_FollowedGeometry
		unsigned long m_FollowerTag{};

		Eigen::Matrix4d m_T_Local_Image;
	};

	// class Bone:public Body
	// {
	// public:
	// public:
	// 	mitkClassMacro(Bone, Body);
	// 	itkNewMacro(Bone);
	//
	// 	bool Init() override;
	//
	// protected:
	//
	// 	virtual  void InitLocalFrame() =0;
	// };

  class MITKORTHOPEDICS_EXPORT Pelvis:public Body
  {
  public:
		mitkClassMacro(Pelvis, Body);
		itkNewMacro(Pelvis);

		bool Init() override;

  private:
		//static Eigen::Matrix4d calPelvisLocalFrame(Eigen::Vector3d& ASIS_L, Eigen::Vector3d& ASIS_R, Eigen::Vector3d& MidLine);

		bool InitPelvisLocalFrame();
  };

	class MITKORTHOPEDICS_EXPORT Femur :public Body
	{
	public:
		mitkClassMacro(Femur, Body);
		itkNewMacro(Femur);

		ESide m_Side{ ESide::right };

		bool Init() override;

	private:
		bool InitFemurLocalFrame();
		bool InitFemurLocalFrame_Mechanical();
	};

	class MITKORTHOPEDICS_EXPORT Cup :public Body
	{
	public:
		mitkClassMacro(Cup, Body);
		itkNewMacro(Cup);

		ESide m_Side{ ESide::right };

		bool Init() override;

	private:
		void InitCupLocalFrame();
	};

	class MITKORTHOPEDICS_EXPORT Stem :public Body
	{
	public:
		mitkClassMacro(Stem, Body);
		itkNewMacro(Stem);

		ESide m_Side{ ESide::right };

		bool Init() override;

	private:
		void InitStemLocalFrame();
	};
}



#endif
