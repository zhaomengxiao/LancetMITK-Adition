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
		itkFactorylessNewMacro(Self)


		itkSetMacro(Surface, mitk::Surface::Pointer)
		itkGetMacro(Surface, mitk::Surface::Pointer)

		

		virtual void Init();
		void SetIndexToWorldTransform(Eigen::Matrix4d T);
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
		// Eigen::Matrix4d m_T_image_body;
		// Eigen::Matrix4d m_T_body_image;
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
		mitk::Image::Pointer m_image{};

		mitk::BaseGeometry::Pointer m_FollowedGeometry = nullptr;
		//! ITK tag for the observing of m_FollowedGeometry
		unsigned long m_FollowerTag{};
	};


  class MITKORTHOPEDICS_EXPORT Pelvis:public Body
  {
  public:
		mitkClassMacro(Pelvis, Body);
		itkNewMacro(Pelvis);

		void Init() override;

  private:
		//static Eigen::Matrix4d calPelvisLocalFrame(Eigen::Vector3d& ASIS_L, Eigen::Vector3d& ASIS_R, Eigen::Vector3d& MidLine);

		void InitPelvisLocalFrame();
  };

	class MITKORTHOPEDICS_EXPORT Femur :public Body
	{
	public:
		mitkClassMacro(Femur, Body);
		itkNewMacro(Femur);

		ESide m_Side{ ESide::right };

		void Init() override;

	private:
		void InitFemurLocalFrame();
		void InitFemurLocalFrame_Mechanical();
	};
}



#endif
