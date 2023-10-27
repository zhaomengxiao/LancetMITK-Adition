
#include "body.h"

#include <vtkTransformPolyDataFilter.h>

using namespace othopedics;

void Body::SetIndexToWorldTransform(Eigen::Matrix4d T)
{
	m_T_world_local = T;
	vtkSmartPointer<vtkMatrix4x4> vtkmatrix = vtkMatrix4x4::New();
	EigenToVtkMatrix4x4(T, vtkmatrix);
	m_Surface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkmatrix);
	this->Modified();
}

void Body::Init()
{
	m_T_world_local = Eigen::Matrix4d::Identity();
	m_name = "body";

	//listenSurfaceGeoModify();
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
	AxisType local;
	if (GetAxis(name, local))
	{
		TransformAxis(m_T_world_local, local, outp_axis);

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

// void Body::SetWorldTransform(Eigen::Matrix4d transform)
// {
// 	m_T_world_local = transform;
// }

void Body::TransformAllInternalData(Eigen::Matrix4d transform)
{
	//landmarks
	for (auto it : m_landMarks) {
		std::cout << to_String(it.first) << std::endl;
		Eigen::Vector3d pt;
		//TransformPoint(m_T_body_image, it.second, pt);
		TransformPoint(transform, it.second, pt);
		SetLandMark(it.first, pt.data());
	}
	//axis
	for (auto it : m_axes) {
		std::cout << to_string(it.first) << std::endl;
		AxisType axis;
		TransformAxis(transform, it.second, axis);
		SetAxis(it.first, axis.startPoint.data(), axis.direction.data());
	}
	//planes
	for (auto it : m_planes) {
		std::cout << to_String(it.first) << std::endl;
		PlaneType plane;
		TransformPlane(transform, it.second, plane);
		SetPlane(it.first, plane.normal.startPoint.data(), plane.normal.direction.data());
	}

	//todo surface
	if (m_Surface!=nullptr)
	{
		vtkSmartPointer<vtkMatrix4x4> vtkmatrix = vtkMatrix4x4::New();
		EigenToVtkMatrix4x4(transform, vtkmatrix);
		
		vtkNew<vtkTransform> trans;
		trans->SetMatrix(vtkmatrix);

		vtkNew<vtkTransformPolyDataFilter> transFilter;
		transFilter->SetTransform(trans);
		transFilter->SetInputData(m_Surface->GetVtkPolyData());
		transFilter->Update();

		m_Surface->SetVtkPolyData(transFilter->GetOutput());
	}
	//todo image
}

Body::Body()
{
}

Body::~Body()
{
}

void Body::onSurfaceGeoModified()
{
	m_T_world_local = vtkMatrix4x4ToEigen(m_FollowedGeometry->GetVtkMatrix());
}

void Body::listenSurfaceGeoModify()
{
	auto observer = itk::SimpleMemberCommand<Body>::New();
	observer->SetCallbackFunction(this, &Body::onSurfaceGeoModified);

	if (m_FollowedGeometry.IsNotNull())
	{
		m_FollowedGeometry->RemoveObserver(m_FollowerTag);
	}

	m_FollowedGeometry = m_Surface->GetGeometry();
	m_FollowerTag = m_FollowedGeometry->AddObserver(itk::ModifiedEvent(), observer);

	// initial adjustment
	//onSurfaceGeoModified();
}


void Pelvis::Init()
{
	Body::Init();
	m_name = "Pelvis";
	//cal local coords
	InitPelvisLocalFrame();
	//Transform all data into local coords
	TransformAllInternalData(m_T_world_local.inverse());

	//recover transform
	vtkSmartPointer<vtkMatrix4x4> vtkmatrix = vtkMatrix4x4::New();
	EigenToVtkMatrix4x4(m_T_world_local, vtkmatrix);
	m_Surface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkmatrix);

	//listenSurfaceGeoModify();
}


void Pelvis::InitPelvisLocalFrame()
{
	PointType ASIS_L{};
	PointType ASIS_R{};
	PointType MidLine{};

	if (GetLandMark(ELandMarks::p_ASIS_L, ASIS_L) && GetLandMark(ELandMarks::p_ASIS_R, ASIS_R) && GetLandMark(ELandMarks::p_MidLine, MidLine))
	{
		//origin
		SetLandMark(ELandMarks::p_O, MidLine.data());
		//cal axes

		Eigen::Matrix4d localFrame = CalPelvisLocalFrame(ASIS_L, ASIS_R, MidLine);
		VectorType x = localFrame.block<3, 1>(0, 0);
		VectorType y = localFrame.block<3, 1>(0, 1);
		VectorType z = localFrame.block<3, 1>(0, 2);
		SetAxis(EAxes::p_X, MidLine.data(), x.data());
		SetAxis(EAxes::p_Y, MidLine.data(), y.data());
		SetAxis(EAxes::p_Z, MidLine.data(), z.data());

		//transform
		m_T_world_local = localFrame;
		//std::cout << m_T_image_body << std::endl;
	}
	else
	{
		std::cout << "InitPelvisLocalFrame Error: Missing LandMarks:" << std::endl;
	}
	std::cout << "exit InitPelvisLocalFrame" << std::endl;
}

void Femur::Init()
{
	Body::Init();
	m_name = "Femur";
	//cal local coords
	InitFemurLocalFrame();
	//Transform all data into local coords
	TransformAllInternalData(m_T_world_local.inverse());

	//recover surface transform
	vtkSmartPointer<vtkMatrix4x4> vtkmatrix = vtkMatrix4x4::New();
	EigenToVtkMatrix4x4(m_T_world_local, vtkmatrix);
	m_Surface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(vtkmatrix);
}

void Femur::InitFemurLocalFrame()
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

		Eigen::Matrix4d	localFrame = CalFemurLocalFrame_canal(FHC, FNC, DFCA, PFCA, m_Side);

		VectorType x = localFrame.block<3, 1>(0, 0);
		VectorType y = localFrame.block<3, 1>(0, 1);
		VectorType z = localFrame.block<3, 1>(0, 2);
		SetAxis(EAxes::f_X_canal, FHC.data(), x.data());
		SetAxis(EAxes::f_Y_canal, FHC.data(), y.data());
		SetAxis(EAxes::f_Z_canal, FHC.data(), z.data());

		//transform
		m_T_world_local = localFrame;
		//std::cout << m_T_image_body << std::endl;

		//project FHC and FNC on plan(canal and y axis)
		// double fhc_projected[3], fnc_projected[3];
		// projectToPlane(FHC.data(), PFCA.data(), y.data(), fhc_projected);
		// projectToPlane(FNC.data(), PFCA.data(), y.data(), fnc_projected);
		// SetLandMark(ELandMarks::stem_CutPoint, fhc_projected);
		// SetLandMark(ELandMarks::stem_EndPoint, fnc_projected);
		//
		// //find intersection
		// Eigen::Vector3d intersection;
		//
		// if (!lineIntersect3d_2points(Eigen::Vector3d(fhc_projected), Eigen::Vector3d(fnc_projected), PFCA, DFCA, intersection))
		// {
		// 	MITK_WARN << "line not intersection";
		// 	return;
		// }

		//Eigen::Vector3d intersectP;
		double intersectP[3]{0,0,0};
		if (!StemInitPosition(FHC.data(), FNC.data(), PFCA.data(), DFCA.data(), intersectP))
		{
			MITK_ERROR << "StemInitPosition Error: no intersection:" << std::endl;
		}
		MITK_WARN << "intersectP" << intersectP[0]<<"," << intersectP[1] << "," << intersectP[2];
		SetLandMark(ELandMarks::f_NeckCanalIntersectPoint, intersectP/*.data()*/);
	}
	else
	{
		MITK_ERROR << "InitFemurLocalFrame Error: Missing LandMarks:" << std::endl;
	}
}

void Femur::InitFemurLocalFrame_Mechanical()
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

		Eigen::Matrix4d	localFrame = CalFemurLocalFrame_mechanical(FHC, FNC, ME, LE, m_Side);

		VectorType x = localFrame.block<3, 1>(0, 0);
		VectorType y = localFrame.block<3, 1>(0, 1);
		VectorType z = localFrame.block<3, 1>(0, 2);
		SetAxis(EAxes::f_X_mechanical, FHC.data(), x.data());
		SetAxis(EAxes::f_Y_mechanical, FHC.data(), y.data());
		SetAxis(EAxes::f_Z_mechanical, FHC.data(), z.data());

		//transform

		m_T_world_local = localFrame;
		//std::cout << m_T_world_local << std::endl;
	}
	else
	{
		std::cout << "InitFemurLocalFrame_Mechanical Error: Missing LandMarks:" << std::endl;
	}
	std::cout << "exit InitFemurLocalFrame_Mechanical" << std::endl;
}

void Cup::Init()
{
	Body::Init();
	m_name = "Cup";
	InitCupLocalFrame();
}

void Cup::InitCupLocalFrame()
{
	Eigen::Vector3d o, x, y, z;
	o << 0, 0, 0;
	x << 1, 0, 0;
	y << 0, 1, 0;
	z << 0, 0, 1;
	SetLandMark(ELandMarks::cup_COR,o.data());

	SetAxis(EAxes::cup_X, o.data(), x.data());
	SetAxis(EAxes::cup_Y, o.data(), y.data());
	SetAxis(EAxes::cup_Z, o.data(), z.data());

	m_T_world_local = Eigen::Matrix4d::Identity();
}

void Stem::Init()
{
	Body::Init();
	m_name = "Stem";
	InitStemLocalFrame();
}

void Stem::InitStemLocalFrame()
{
	Eigen::Vector3d o, x, y, z;
	o << 0, 0, 0;
	x << 1, 0, 0;
	y << 0, 1, 0;
	z << 0, 0, 1;

	SetLandMark(ELandMarks::stem_O, o.data());

	SetAxis(EAxes::stem_X, o.data(), x.data());
	SetAxis(EAxes::stem_Y, o.data(), y.data());
	SetAxis(EAxes::stem_Z, o.data(), z.data());
}
