#include "hip.h"

#include <iostream>


#include "basic.h"


namespace FuturTecAlgorithm
{

	Body::Body(Body&& other) noexcept : m_landMarks(std::move(other.m_landMarks)),
		m_axes(std::move(other.m_axes)),
		m_planes(std::move(other.m_planes)),
		m_results(std::move(other.m_results))
	{
	}

	Body& Body::operator=(const Body& other)
	{
		if (this == &other)
			return *this;
		m_landMarks = other.m_landMarks;
		m_axes = other.m_axes;
		m_planes = other.m_planes;
		m_results = other.m_results;
		return *this;
	}

	Body& Body::operator=(Body&& other) noexcept
	{
		if (this == &other)
			return *this;
		m_landMarks = std::move(other.m_landMarks);
		m_axes = std::move(other.m_axes);
		m_planes = std::move(other.m_planes);
		m_results = std::move(other.m_results);
		return *this;
	}

	void Body::SetLandMark(ELandMarks name, double* data)
	{
		try
		{
			PointType point;
			memcpy(point.data(), data, 3 * sizeof(double));
			m_landMarks.insert_or_assign(name, point);
		}
		catch (...)
		{
			std::cout << "err" << std::endl;
		}

	}

	bool Body::GetLandMark(ELandMarks name, PointType& oup_value)
	{
		auto iter = m_landMarks.find(name);
		if (iter != m_landMarks.end())
		{
			oup_value = iter->second;
			return true;
		}
		std::cout << "Error: Cant find LandMark:" << to_String(name) << std::endl;
		return false;
	}

	bool Body::GetGlobalLandMark(ELandMarks name, PointType& oup_value)
	{
		Eigen::Vector3d pt;
		if (GetLandMark(name, pt))
		{
			TransformPoint(m_T_world_local, pt, pt);
			oup_value = pt;
			return true;
		}
		return false;
	}

	void Body::SetAxis(EAxes name, double* p_start, double* direction)
	{
		PointType p1{ p_start };
		PointType p2{ direction };
		AxisType axis;
		axis.startPoint = p1;
		axis.direction = p2;

		m_axes.insert_or_assign(name, axis);
	}

	bool Body::GetAxis(EAxes name, AxisType& outp_axis)
	{
		auto iter = m_axes.find(name);
		if (iter != m_axes.end())
		{
			outp_axis = iter->second;
			return true;
		}
		std::cout << "Error: Cant find Axis:" << to_string(name) << std::endl;
		return false;
	}

	bool Body::GetGlobalAxis(EAxes name, AxisType& outp_axis)
	{
		AxisType tmp;
		if (GetAxis(name, tmp))
		{
			TransformAxis(m_T_world_local, tmp, tmp);
			outp_axis.startPoint = tmp.startPoint;
			outp_axis.direction = tmp.direction;
			return true;
		}
		return false;
	}

	void Body::SetPlane(EPlanes name, double* center, double* normal)
	{
		PointType p{ center };
		PointType v{ normal };
		PlaneType plane{};
		plane.normal.startPoint = p;
		plane.normal.direction = v;

		m_planes.insert_or_assign(name, plane);
	}

	bool Body::GetPlane(EPlanes name, PlaneType& outp_plane)
	{
		auto iter = m_planes.find(name);
		if (iter != m_planes.end())
		{
			outp_plane = iter->second;
			return true;
		}
		std::cout << "Error: Cant find Plane:" << to_String(name) << std::endl;
		return false;
	}

	bool Body::GetGlobalPlane(EPlanes name, PlaneType& outp_plane)
	{
		PlaneType tmp;
		if (GetPlane(name, tmp))
		{
			TransformPlane(m_T_world_local, tmp, tmp);
			outp_plane = tmp;
			return true;
		}
		return false;
	}

	void Body::SetResult(EResult name, double res)
	{
		m_results.insert_or_assign(name, res);
	}

	bool Body::GetResult(EResult name, double& outp_res)
	{
		auto iter = m_results.find(name);
		if (iter != m_results.end())
		{
			outp_res = iter->second;
			return true;
		}
		std::cout << "Error: Cant find Result:" << to_string(name) << std::endl;
		return false;
	}

	void Body::SetWorldTransform(Eigen::Matrix4d transform)
	{
		m_T_world_local = transform;
	}

	void Body::convertToLocal()
	{
		//landmarks
		for (auto it : m_landMarks) {
			std::cout << to_String(it.first) << std::endl;
			Eigen::Vector3d pt;
			TransformPoint(m_T_body_image, it.second, pt);
			SetLandMark(it.first, pt.data());
		}
		//axis
		for (auto it : m_axes) {
			std::cout << to_string(it.first) << std::endl;
			AxisType axis;
			TransformAxis(m_T_body_image, it.second, axis);
			SetAxis(it.first, axis.startPoint.data(),axis.direction.data());
		}
		//planes
		for (auto it : m_planes) {
			std::cout << to_String(it.first) << std::endl;
			PlaneType plane;
			TransformPlane(m_T_body_image, it.second, plane);
			SetPlane(it.first, plane.normal.startPoint.data(), plane.normal.direction.data());
		}
	}

	void Pelvis::calTransformWorldToPelvisISB()
	{
		PointType ASIS_L{};
		PointType ASIS_R{};
		PointType MidLine{};

		if (GetLandMark(ELandMarks::p_ASIS_L, ASIS_L) && GetLandMark(ELandMarks::p_ASIS_R, ASIS_R) && GetLandMark(ELandMarks::p_MidLine, MidLine))
		{
			//origin
			SetLandMark(ELandMarks::p_O, MidLine.data());
			//cal axes
			VectorType z = (ASIS_R - ASIS_L).normalized();
			VectorType x = (ASIS_L - MidLine).cross(ASIS_R - MidLine).normalized();
			VectorType y = z.cross(x).normalized();
			SetAxis(EAxes::p_X, MidLine.data(), x.data());
			SetAxis(EAxes::p_Y, MidLine.data(), y.data());
			SetAxis(EAxes::p_Z, MidLine.data(), z.data());
			//transform
			m_T_image_body = ConvertCoordstoTransform(MidLine, x, y, z);
			m_T_body_image = m_T_image_body.inverse();
			//std::cout << m_T_image_body << std::endl;

		}
		else
		{
			std::cout << "calTransformWorldToPelvisISB Error: Missing LandMarks:" << std::endl;
		}
		std::cout << "exit calTransformWorldToPelvisISB" << std::endl;
	}

	void Pelvis::calTransformImageToPelvisCT()
	{
		PointType ASIS_L{};
		PointType ASIS_R{};
		PointType MidLine{};

		if (GetLandMark(ELandMarks::p_ASIS_L, ASIS_L) && GetLandMark(ELandMarks::p_ASIS_R, ASIS_R) && GetLandMark(ELandMarks::p_MidLine, MidLine))
		{
			//origin
			SetLandMark(ELandMarks::p_O, MidLine.data());
			//cal axes
			
			Eigen::Matrix4d localFrame = calPelvisLocalFrame(ASIS_L,ASIS_R,MidLine);
			VectorType x = localFrame.block<3, 1>(0, 0);
			VectorType y = localFrame.block<3, 1>(0, 1);
			VectorType z = localFrame.block<3, 1>(0, 2);
			SetAxis(EAxes::p_X, MidLine.data(), x.data());
			SetAxis(EAxes::p_Y, MidLine.data(), y.data());
			SetAxis(EAxes::p_Z, MidLine.data(), z.data());

			//transform
			m_T_image_body = ConvertCoordstoTransform(MidLine, x, y, z);
			m_T_body_image = m_T_image_body.inverse();
			m_T_world_local = m_T_image_body;
			//std::cout << m_T_image_body << std::endl;
		}
		else
		{
			std::cout << "calTransformImageToPelvisCT Error: Missing LandMarks:" << std::endl;
		}
		std::cout << "exit calTransformImageToPelvisCT" << std::endl;
	}

	void Pelvis::calTransformImageToBodyISB()
	{
		calTransformWorldToPelvisISB();
	}

	void Pelvis::calTransformImageToBody()
	{
		calTransformImageToPelvisCT();
	}

	void Femur::calTransformImageToBodyISB()
	{
	}

	void Femur::calTransformImageToBody()
	{
		//calTransformImageToBody_Mechanical();
		calTransformImageToBody_Canal();
	}

	void Femur::calTransformImageToBody_Canal()
	{
		PointType FHC{};
		PointType FNC{};
		PointType DFCA{};
		PointType PFCA{};
		if (GetLandMark(ELandMarks::f_FHC, FHC) && GetLandMark(ELandMarks::f_FNC, FNC)
			&& GetLandMark(ELandMarks::f_DFCA, DFCA) && GetLandMark(ELandMarks::f_PFCA, PFCA))
		{
			//set origin to Femur Head Center
			SetLandMark(ELandMarks::f_O, FHC.data());
			//cal axes
			//femur canal axis
			VectorType canal = (PFCA - DFCA).normalized();
			SetAxis(EAxes::f_Canal, DFCA.data(), canal.data());
			//femur neck axis
			VectorType neck = (FHC - FNC).normalized();
			SetAxis(EAxes::f_Neck, FNC.data(), neck.data());

			Eigen::Matrix4d	localFrame = calFemurLocalFrame_canal(FHC, FNC, DFCA, PFCA, m_side);

			VectorType x = localFrame.block<3,1>(0,0);
			VectorType y = localFrame.block<3, 1>(0, 1);
			VectorType z = localFrame.block<3, 1>(0, 2);
			SetAxis(EAxes::f_X_canal, FHC.data(), x.data());
			SetAxis(EAxes::f_Y_canal, FHC.data(), y.data());
			SetAxis(EAxes::f_Z_canal, FHC.data(), z.data());

			//transform
			m_T_image_body = localFrame;
			m_T_body_image = m_T_image_body.inverse();
			m_T_world_local = m_T_image_body;
			std::cout << m_T_image_body << std::endl;
		}
		else
		{
			std::cout << "calTransformImageToBody(Femur) Error: Missing LandMarks:" << std::endl;
		}
		std::cout << "exit calTransformImageToBody(Femur)" << std::endl;
	}

	void Femur::calTransformImageToBody_Mechanical()
	{
		PointType FHC{};
		PointType FNC{};
		PointType ME{};
		PointType LE{};
		if (GetLandMark(ELandMarks::f_FHC, FHC) && GetLandMark(ELandMarks::f_FNC, FNC)
			&& GetLandMark(ELandMarks::f_ME, ME) && GetLandMark(ELandMarks::f_LE, LE))
		{
			//origin
			SetLandMark(ELandMarks::f_O, FHC.data());

			//midEEs
			PointType midEEs = (ME + LE) / 2.0;
			SetLandMark(ELandMarks::f_MidEEs, midEEs.data());
			//cal axes
			//femur mechanical axis
			VectorType mechanical = (FHC - midEEs).normalized();
			SetAxis(EAxes::f_Mechanics, midEEs.data(), mechanical.data());
			//epicondylar axis
			VectorType  epicondylar = (LE - ME).normalized();
			SetAxis(EAxes::f_Epicondyla, ME.data(), epicondylar.data());

			Eigen::Matrix4d	localFrame = calFemurLocalFrame_mechanical(FHC, FNC, ME, LE, m_side);

			VectorType x = localFrame.block<3, 1>(0, 0);
			VectorType y = localFrame.block<3, 1>(0, 1);
			VectorType z = localFrame.block<3, 1>(0, 2);
			SetAxis(EAxes::f_X_mechanical, FHC.data(), x.data());
			SetAxis(EAxes::f_Y_mechanical, FHC.data(), y.data());
			SetAxis(EAxes::f_Z_mechanical, FHC.data(), z.data());

			//transform
			m_T_image_body = ConvertCoordstoTransform(FHC, x, y, z);
			m_T_body_image = m_T_image_body.inverse();
			m_T_world_local = m_T_image_body;
			std::cout << m_T_image_body << std::endl;
		}
		else
		{
			std::cout << "calTransformImageToBody(Femur) Error: Missing LandMarks:" << std::endl;
		}
		std::cout << "exit calTransformImageToBody(Femur)" << std::endl;
	}


	Eigen::Matrix4d ConvertCoordstoTransform(Eigen::Vector3d& o, Eigen::Vector3d& x, Eigen::Vector3d& y, Eigen::Vector3d& z)
	{
		Eigen::Matrix4d transform;
		transform <<
			x(0), y(0), z(0), o(0),
			x(1), y(1), z(1), o(1),
			x(2), y(2), z(2), o(2),
			0, 0, 0, 1;
		return transform;
	}

	void Cup::init()
	{
		Eigen::Vector3d o, x, y, z;
		o << 0, 0, 0;
		x << 1, 0, 0;
		y << 0, 1, 0;
		z << 0, 0, 1;

		SetAxis(EAxes::cup_X, o.data(), x.data());
		SetAxis(EAxes::cup_Y, o.data(), y.data());
		SetAxis(EAxes::cup_Z, o.data(), z.data());

		m_T_world_local = Eigen::Matrix4d::Identity();
	}

	void Cup::calTransformImageToBodyISB()
	{
	}

	void Cup::calTransformImageToBody()
	{
		init();
	}

	 void AnteversionAndInclinationAngle(double* direction, double& ResultAnteversion, double& ResultInclination, EIVAngelType type)
	 {
		 switch (type)
		 {
		 case RADIO_GRAPHIC:
			 AIAngleRadiographic(direction, ResultAnteversion, ResultInclination);
			 break;
		 case OPERATIVE:
			 AIAngleOperative(direction, ResultAnteversion, ResultInclination);
			 break;
		 case ANATOMICAL:
			 AIAngleAnatomical(direction, ResultAnteversion, ResultInclination);
			 break;
		 default:
			 AIAngleRadiographic(direction, ResultAnteversion, ResultInclination);
		 }
	 }
	
	 void AIAngleRadiographic(double* direction, double& ResultAnteversion, double& ResultInclination)
	 {
	 	double Coronalnormal[3] = { 0, -1, 0 };
	 	double D_value = 90.0;
	 	double CupLineTransform[3] = { direction[0], direction[1], direction[2] };
	 	double VersionRadians = lancetAlgorithm::AngleBetween2Vector(CupLineTransform, Coronalnormal, true);
	 	ResultAnteversion = (double)(VersionRadians * (180.0 / EIGEN_PI)) - D_value;
	 	if (ResultAnteversion < 0.0)
	 	{
	 		ResultAnteversion = -ResultAnteversion;
	 	}
	 	//std::cout << "ResultAnteversion:" << ResultAnteversion << std::endl;
	 	double CupLineProject[3];
	 	double origin[] = { 0, 0, 0 };
	 	//投影到冠状面上
	 	lancetAlgorithm::projectToPlane(CupLineTransform, origin, Coronalnormal, CupLineProject);
	 	double z[3] = { 0, 0, 1 };
	 	double InclinationRadians = lancetAlgorithm::AngleBetween2Vector(CupLineProject, z, true);
	 	ResultInclination = (double)(InclinationRadians * (180.0 / EIGEN_PI));
	 	if (ResultInclination > 90.0)
	 	{
	 		ResultInclination = 180.0 - ResultInclination;
	 	}
	 	//std::cout << "ResultInclination:" << ResultInclination << std::endl;
	 }
	
	 void AIAngleOperative(double* direction, double& ResultAnteversion, double& ResultInclination)
	 {
	 	double Sagittalnormal[3] = { -1, 0, 0 };
	 	double z[3] = { 0, 0, 1 };
	 	double D_value = 90.0;
	 	double CupLineTransform[3] = { direction[0], direction[1], direction[2] };
	 	double CupLineProject[3];
	 	double origin[] = { 0, 0, 0 };
	 	//投影到矢状面上
	 	lancetAlgorithm::projectToPlane(CupLineTransform, origin, Sagittalnormal, CupLineProject);
	 	double VersionRadians = lancetAlgorithm::AngleBetween2Vector(z, CupLineProject, true);
	 	ResultAnteversion = (double)(VersionRadians * (180.0 / EIGEN_PI));
	 	if (ResultAnteversion > 90.0)
	 	{
	 		ResultAnteversion = 180.0 - ResultAnteversion;
	 	}
	 	//std::cout << "ResultAnteversion:" << ResultAnteversion << std::endl;
	
	 	double InclinationRadians = lancetAlgorithm::AngleBetween2Vector(CupLineTransform, Sagittalnormal, true);
	 	ResultInclination = (double)(InclinationRadians * (180.0 / EIGEN_PI)) - D_value;
	 	if (ResultInclination < 0.0)
	 	{
	 		ResultInclination = -ResultInclination;
	 	}
	 	//std::cout << "ResultInclination:" << ResultInclination << std::endl;
	 }
	
	 void AIAngleAnatomical(double* direction, double& ResultAnteversion, double& ResultInclination)
	 {
		 const double TransverseNormal[3] = { 0, 0, -1 };
		 double z[3] = { 0, 0, 1 };
		 double x[3] = { -1, 0, 0 };
		 double CupLineTransform[3] = { direction[0], direction[1], direction[2] };
		 double CupLineProject[3];
		 double origin[] = { 0, 0, 0 };
	 	//投影到横断面上
		 lancetAlgorithm::projectToPlane(CupLineTransform, origin, TransverseNormal, CupLineProject);
		 double VersionRadians = lancetAlgorithm::AngleBetween2Vector(x, CupLineProject, true);
		 ResultAnteversion = (double)(VersionRadians * (180.0 / EIGEN_PI));
		 if (ResultAnteversion > 90.0)
		 {
			 ResultAnteversion = 180.0 - ResultAnteversion;
		 }
	 	//std::cout << "ResultAnteversion:" << ResultAnteversion << std::endl;
	
		 double InclinationRadians = lancetAlgorithm::AngleBetween2Vector(CupLineTransform, z, true);
		 ResultInclination = (double)(InclinationRadians * (180.0 / EIGEN_PI));
		 if (ResultInclination > 90.0)
		 {
			 ResultInclination = 180.0 - ResultInclination;
		 }
	 	//std::cout << "ResultInclination:" << ResultInclination << std::endl;
	 }
	
	 double FemoralVersionAngle(ESide side,double* FHC, double* FNC, double* ME, double* LE, double* DFCA, double* PFCA)
	 {
		 // Compose neck axis: neck center + femurCOR
		 Eigen::Vector3d neckAxis;
		 neckAxis[0] = FHC[0] - FNC[0];
		 neckAxis[1] = FHC[1] - FNC[1];
		 neckAxis[2] = FHC[2] - FNC[2];
		 // Compose epicondylar axis
		 Eigen::Vector3d epicondylarAxis;
		 epicondylarAxis[0] = ME[0] - LE[0];
		 epicondylarAxis[1] = ME[1] - LE[1];
		 epicondylarAxis[2] = ME[2] - LE[2];
		 // Compose femur canal axis
		 Eigen::Vector3d canalAxis;
		 canalAxis[0] = PFCA[0] - DFCA[0];
		 canalAxis[1] = PFCA[1] - DFCA[1];
		 canalAxis[2] = PFCA[2] - DFCA[2];

		 canalAxis.normalize();

		 // Project the neckAxis onto the canal axis
		 Eigen::Vector3d neckAxis_ontoCanalAxis = neckAxis.dot(canalAxis) * canalAxis;

		 // neckAxis projection onto the perpendicular plane 
		 Eigen::Vector3d neckAxis_ontoPlane = neckAxis - neckAxis_ontoCanalAxis;

		 // Project the epicondylarAxis onto the canal axis
		 Eigen::Vector3d epicondylarAxis_ontoCanalAxis = epicondylarAxis.dot(canalAxis) * canalAxis;

		 // epicondylarAxis projection onto the perpendicular plane
		 Eigen::Vector3d epicondylarAxis_ontoPlane = epicondylarAxis - epicondylarAxis_ontoCanalAxis;

		 double femoralVersion = (180 / EIGEN_PI) * acos(epicondylarAxis_ontoPlane.dot(neckAxis_ontoPlane)
			 / (epicondylarAxis_ontoPlane.norm() * neckAxis_ontoPlane.norm()));

		 // Determine anteversion or retroversion; if ante, assign femoralVersion as (+); if retro, assign as (-)
		 double tmpValue = epicondylarAxis_ontoPlane.cross(neckAxis_ontoPlane).dot(canalAxis);

		 if (side == ESide::left)
		 {
			 if (tmpValue < 0)
			 {
				 femoralVersion = -femoralVersion;
			 }
		 }
		 else
		 {
			 if (tmpValue > 0)
			 {
				 femoralVersion = -femoralVersion;
			 }
		 }

		 return femoralVersion;
	 }

	 Eigen::Matrix4d calPelvisLocalFrame(Eigen::Vector3d& ASIS_L, Eigen::Vector3d& ASIS_R, Eigen::Vector3d& MidLine)
	 {
		 Eigen::Vector3d x = (ASIS_L - ASIS_R).normalized();
		 Eigen::Vector3d y = (ASIS_R - MidLine).cross(ASIS_L - MidLine).normalized();
		 Eigen::Vector3d z = x.cross(y).normalized();

		 return ConvertCoordstoTransform(MidLine, x, y, z);
	 }

	 Eigen::Matrix4d calFemurLocalFrame_canal(Eigen::Vector3d& FHC, Eigen::Vector3d& FNC, Eigen::Vector3d& DFCA, Eigen::Vector3d& PFCA, ESide side)
	 {
		 Eigen::Vector3d x, y, z;
		 if (side == ESide::right)
		 {
			 z = (PFCA - DFCA).normalized();
			 y = z.cross(FHC - FNC).normalized();
			 x = y.cross(z).normalized();
		 }
		 else
		 {
			 z = (PFCA - DFCA).normalized();
			 y = (FHC - FNC).cross(z).normalized();
			 x = y.cross(z).normalized();
		 }
		 return ConvertCoordstoTransform(FHC, x, y, z);
	 }

	 Eigen::Matrix4d calFemurLocalFrame_mechanical(Eigen::Vector3d& FHC, Eigen::Vector3d& FNC, Eigen::Vector3d& ME,
		 Eigen::Vector3d& LE, ESide side)
	 {
		 Eigen::Vector3d midEEs = (ME + LE) / 2.0;
		 Eigen::Vector3d x, y, z;
		 if (side == ESide::right)
		 {
			 z = (FHC - midEEs).normalized();
			 y = z.cross(FHC - FNC).normalized();
			 x = y.cross(z).normalized();
		 }
		 else
		 {
			 z = (FHC - midEEs).normalized();
			 y = (FHC - FNC).cross(z).normalized();
			 x = y.cross(z).normalized();
		 }
		 return ConvertCoordstoTransform(FHC, x, y, z);
	 }
}
