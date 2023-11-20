#include "physioModels.h"


#include <array>
#include "Eigen/Eigen"
#include <leastsquaresfit.h>
#include <basic.h>
#include <iostream>

namespace lancetAlgorithm
{
	DataBase::DataBase(DataBase&& other) noexcept: m_map_landMark(std::move(other.m_map_landMark)),
	                                               m_map_axis(std::move(other.m_map_axis)),
	                                               m_map_plane(std::move(other.m_map_plane)),
	                                               m_map_result(std::move(other.m_map_result))
	{
	}

	DataBase& DataBase::operator=(const DataBase& other)
	{
		if (this == &other)
			return *this;
		m_map_landMark = other.m_map_landMark;
		m_map_axis = other.m_map_axis;
		m_map_plane = other.m_map_plane;
		m_map_result = other.m_map_result;
		return *this;
	}

	DataBase& DataBase::operator=(DataBase&& other) noexcept
	{
		if (this == &other)
			return *this;
		m_map_landMark = std::move(other.m_map_landMark);
		m_map_axis = std::move(other.m_map_axis);
		m_map_plane = std::move(other.m_map_plane);
		m_map_result = std::move(other.m_map_result);
		return *this;
	}

	void DataBase::SetLandMark(ELandMarks name, double* data)
	{
		try
		{
			LandMarkType array{};
			memcpy(array._Elems, data, 3 * sizeof(double));
			m_map_landMark.insert_or_assign(name, array);
		}
		catch (...)
		{
			std::cout << "err" << std::endl;
		}
		
	}

	bool DataBase::GetLandMark(ELandMarks name, LandMarkType& oup_value)
	{
		auto iter = m_map_landMark.find(name);
		if (iter != m_map_landMark.end())
		{
			oup_value = iter->second;
			return true;
		}
		std::cout << "Error: Cant find LandMark:" << to_string(name) << std::endl;
		return false;
	}

	void DataBase::SetAxis(EAxes name, double* p_start,double* direction)
	{
		AxisType axis{};	
		memcpy(axis.startPoint._Elems, p_start, 3 * sizeof(double));
		memcpy(axis.direction._Elems, direction, 3 * sizeof(double));

		m_map_axis.insert_or_assign(name, axis);
	}

	bool DataBase::GetAxis(EAxes name, AxisType& outp_axis)
	{
		auto iter = m_map_axis.find(name);
		if (iter != m_map_axis.end())
		{
			outp_axis = iter->second;
			return true;
		}
		std::cout << "Error: Cant find Axis:" << to_string(name) << std::endl;
		return false;
	}

	void DataBase::SetPlane(EPlanes name, double* center,double* normal)
	{
		PlaneType plane{};
		memcpy(plane.normal.startPoint._Elems, center, 3 * sizeof(double));
		memcpy(plane.normal.direction._Elems, normal, 3 * sizeof(double));

		m_map_plane.insert_or_assign(name, plane);
	}

	bool DataBase::GetPlane(EPlanes name, PlaneType& outp_plane)
	{
		auto iter = m_map_plane.find(name);
		if (iter != m_map_plane.end())
		{
			outp_plane = iter->second;
			return true;
		}
		std::cout << "Error: Cant find Plane:" << to_string(name) << std::endl;
		return false;
	}

	void DataBase::SetResult(EResult name, double res)
	{
		m_map_result.insert_or_assign(name, res);
	}

	bool DataBase::GetResult(EResult name, double& outp_res)
	{
		auto iter = m_map_result.find(name);
		if (iter != m_map_result.end())
		{
			outp_res = iter->second;
			return true;
		}
		std::cout << "Error: Cant find Result:" << to_string(name) << std::endl;
		return false;
	}

	void PelvisModel::Update()
	{
		updatePHA();
		updatePSA();
		updatePLA();
		updateMidPlane();
	}

	void PelvisModel::updatePHA()//right positive
	{
		LandMarkType LASI{};
		LandMarkType RASI{};

		GetLandMark(ELandMarks::p_LASI, LASI);
		GetLandMark(ELandMarks::p_RASI, RASI);

		auto appcenter = GetMidPoint(RASI, LASI).data();
		SetLandMark(ELandMarks::p_APPCenter, appcenter);
		
		VectorType direction{ Normalize(std::array<double,3>{LASI[0] - RASI[0],LASI[1] - RASI[1] ,LASI[2] - RASI[2]}) };

		SetAxis(EAxes::p_PHA, appcenter, direction.data());
	}

	void PelvisModel::updatePSA()//out positve
	{
		LandMarkType PT{};
		GetLandMark(ELandMarks::p_PT, PT);
		AxisType axis_PHA{};
		GetAxis(EAxes::p_PHA, axis_PHA);
		LandMarkType APPCenter{};
		GetLandMark(ELandMarks::p_APPCenter, APPCenter);
		
		Eigen::Vector3d h_axis{axis_PHA.direction.data()};
		Eigen::Vector3d tmp;
		tmp << APPCenter[0] - PT[0], APPCenter[1] - PT[1], APPCenter[2] - PT[2];

		tmp = h_axis.cross(tmp).eval();
		tmp.normalize();
		SetAxis(EAxes::p_PSA, APPCenter.data(), tmp.data());
		//cal PT angle
		double vec_ny[3]{ 0,-1,0 };
		double angle = AngleBetween2Vector(tmp.data(), vec_ny);
		if (tmp.z()>0)
		{
			angle = -angle;
		}
		SetResult(EResult::f_PT, angle);
	}

	void PelvisModel::updatePLA()
	{
		AxisType axis_PHA{};
		GetAxis(EAxes::p_PHA, axis_PHA);
		AxisType axis_PSA{};
		GetAxis(EAxes::p_PSA, axis_PSA);
		Eigen::Vector3d h_axis{ axis_PHA.direction.data() };
		Eigen::Vector3d s_axis{ axis_PSA.direction.data() };
		LandMarkType APPCenter{};
		GetLandMark(ELandMarks::p_APPCenter, APPCenter);

		Eigen::Vector3d l_axes = h_axis.cross(s_axis).eval();

		SetAxis(EAxes::p_PLA, APPCenter.data(), l_axes.data());
	}

	void PelvisModel::updateMidPlane()
	{
		LandMarkType PT{};
		GetLandMark(ELandMarks::p_PT, PT);
		AxisType normal{};
		GetAxis(EAxes::p_PHA, normal);
		SetPlane(EPlanes::MIDPLANE, PT.data(), normal.direction.data());
	}

	bool PelvisModel::UpdateAcetabularCenter(ESide side)
	{
		std::vector<double>* data{nullptr};
		ELandMarks target;
		LandMarkType center{ 0,0,0 };
		double r{ 0 };

		if (ESide::left == side)
		{
			data = &m_PSet_LAC;
			target = ELandMarks::p_LAC;
		}
		else
		{
			data = &m_PSet_RAC;
			target = ELandMarks::p_RAC;
		}
		if (!fit_sphere(*data, center, r))
		{
			std::cout << "Cal AcetabularCenter Err:cant fit sphere" << std::endl;
			return false;
		}
		SetLandMark(target, center.data());
		//clear data vec after result get
		data->clear();
		data = nullptr;
		return true;
	}

	bool PelvisModel::UpdateAcetabularAxis(ESide side)
	{
		//not use this center,use LAC or RAC instead;
		LandMarkType center{ 0,0,0 };
		double r = 0;
		VectorType normal;
		
		std::vector<double>* data{ nullptr };
		EAxes e_AA;
		ELandMarks e_AC;
		if (ESide::left == side)
		{
			data = &m_PSet_LAA;
			e_AC = ELandMarks::p_LAC;
			e_AA = EAxes::p_LAA;
		}
		else
		{
			data = &m_PSet_RAA;
			e_AC = ELandMarks::p_RAC;
			e_AA = EAxes::p_RAA;
		}
		if (!fit_circle_3d(*data, center, r, normal))
		{
			std::cout << "Cal AcetabularAxes Err:cant fit plane" << std::endl;
			return false;
		}
		if (normal[2] > 0.0)
		{
			normal[0] = -normal[0];
			normal[1] = -normal[1];
			normal[2] = -normal[2];
		}
		LandMarkType AC{};
		if (!GetLandMark(e_AC, AC))
		{
			std::cout << "Cal AcetabularAxes Err:Cant get Acetabular Center.Run UpdateAcetabularCenter first" << std::endl;
			return false;
		}
		
		SetAxis(e_AA, AC.data(), normal.data());
		return true;
	}

	bool PelvisModel::updateAIAngle(ESide side, EIVAngelType type)
	{
		EAxes e_AA;
		EResult anteversion;
		EResult inclination;
		if (ESide::left == side)
		{
			e_AA = EAxes::p_LAA;
			anteversion = EResult::p_LeftAnteversion;
			inclination = EResult::p_LeftInclination;
		}
		else
		{
			e_AA = EAxes::p_RAA;
			anteversion = EResult::p_RightAnteversion;
			inclination = EResult::p_RightInclination;
		}
		AxisType AA{};
		if (!GetAxis(e_AA, AA))
		{
			std::cout << "updateAIAngle Err:Cant get Acetabular Axes.Run UpdateAcetabularAxis first" << std::endl;
			return false;
		}
		double Anteversion = 0;
		double Inclination = 0;
		AnteversionAndInclinationAngle(AA.direction.data(), Anteversion, Inclination, type);
		SetResult(anteversion, Anteversion);
		SetResult(inclination, Inclination);
		return true;
	}

	void PelvisModel::AddAcetabulumPoint_Edge(ESide side, double* data)
	{
		if (side == ESide::left)
		{
			m_PSet_LAA.push_back(data[0]);
			m_PSet_LAA.push_back(data[1]);
			m_PSet_LAA.push_back(data[2]);
		}
		else
		{
			m_PSet_RAA.push_back(data[0]);
			m_PSet_RAA.push_back(data[1]);
			m_PSet_RAA.push_back(data[2]);
		}
		
	}

	void PelvisModel::AddAcetabulumPoint_Surface(ESide side, double* data)
	{
		if (side == ESide::left)
		{
			m_PSet_LAC.push_back(data[0]);
			m_PSet_LAC.push_back(data[1]);
			m_PSet_LAC.push_back(data[2]);
		}
		else
		{
			m_PSet_RAC.push_back(data[0]);
			m_PSet_RAC.push_back(data[1]);
			m_PSet_RAC.push_back(data[2]);
		}
	}

	void femurModel::Update()
	{
		LandMarkType FHC{};
		GetLandMark(ELandMarks::f_FHC, FHC);
		LandMarkType DFCA{};
		GetLandMark(ELandMarks::f_DFCA, DFCA);
		LandMarkType PFCA{};
		GetLandMark(ELandMarks::f_PFCA, PFCA);
		
		m_Matrix_canal = calCanalCorrection(FHC,DFCA,PFCA);
		m_Matrix_mechan = calMechanCorrection(FHC, DFCA, PFCA);
	}

	std::array<double, 16> femurModel::calMechanCorrection(LandMarkType FHC, LandMarkType DFCA, LandMarkType PFCA)
	{
		std::array<double, 16> res{};
		Eigen::Vector3d axis_z, axis_x,axis_y;
		Eigen::Matrix<double, 4, 4, Eigen::RowMajor> transMatrix;
		switch (side)
		{
		case ESide::right:
			axis_z << FHC[0] - DFCA[0], FHC[1] - DFCA[1], FHC[2] - DFCA[2];
			axis_z.normalize();

			axis_y = Eigen::Vector3d{ PFCA[0] - FHC[0], PFCA[1] - FHC[1], PFCA[2] - FHC[2]}.cross(axis_z);
			axis_y.normalize();
			axis_x = axis_y.cross(axis_z);
			break;
		case ESide::left:
			axis_z << FHC[0] - DFCA[0], FHC[1] - DFCA[1], FHC[2] - DFCA[2];
			axis_z.normalize();

			axis_y = axis_z.cross(Eigen::Vector3d{ PFCA[0] - FHC[0], PFCA[1] - FHC[1], PFCA[2] - FHC[2] });
			axis_y.normalize();
			axis_x = axis_y.cross(axis_z);
			break;
		}
		//===========cal 4x4 transform matrix============
		Eigen::Matrix4d trans1;
		trans1.setIdentity();
		trans1(0, 3) = -FHC[0];
		trans1(1, 3) = -FHC[1];
		trans1(2, 3) = -FHC[2];

		Eigen::Matrix4d rotation;
		rotation.setIdentity();
		for (unsigned int i = 0; i < 3; i++)
		{
			rotation(i, 0) = axis_x[i];
			rotation(i, 1) = axis_y[i];
			rotation(i, 2) = axis_z[i];
		}
		rotation = rotation.inverse().eval();

		Eigen::Matrix4d trans2;
		trans2.setIdentity();
		trans2(0, 3) = FHC[0];
		trans2(1, 3) = FHC[1];
		trans2(2, 3) = FHC[2];

		transMatrix = trans2 * rotation*trans1;

		memcpy(res._Elems, transMatrix.data(), 16 * sizeof(double));
		//===========END============
		return res;
	}

	std::array<double, 16> femurModel::calCanalCorrection(LandMarkType FHC, LandMarkType DFCA, LandMarkType PFCA)
	{
		std::array<double, 16> res{};
		Eigen::Vector3d axis_z, axis_x, axis_y;
		Eigen::Matrix<double,4,4,Eigen::RowMajor> transMatrix;
		
		switch (side)
		{
		case ESide::right:
			
			axis_z << PFCA[0] - DFCA[0], PFCA[1] - DFCA[1], PFCA[2] - DFCA[2];
			axis_z.normalize();

			axis_y = Eigen::Vector3d{ PFCA[0] - FHC[0], PFCA[1] - FHC[1], PFCA[2] - FHC[2] }.cross(axis_z);
			axis_y.normalize();
			axis_x = axis_y.cross(axis_z);
			break;

		case ESide::left:
			axis_z << PFCA[0] - DFCA[0], PFCA[1] - DFCA[1], PFCA[2] - DFCA[2];
			axis_z.normalize();

			axis_y = axis_z.cross(Eigen::Vector3d{ PFCA[0] - FHC[0], PFCA[1] - FHC[1], PFCA[2] - FHC[2] });
			axis_y.normalize();
			axis_x = axis_y.cross(axis_z);
			break;
		}

		//===========cal 4x4 transform matrix============
		Eigen::Matrix4d trans1;
		trans1.setIdentity();
		trans1(0, 3) = -FHC[0];
		trans1(1, 3) = -FHC[1];
		trans1(2, 3) = -FHC[2];

		Eigen::Matrix4d rotation;
		rotation.setIdentity();
		for (unsigned int i = 0; i < 3; i++)
		{
			rotation(i, 0) = axis_x[i];
			rotation(i, 1) = axis_y[i];
			rotation(i, 2) = axis_z[i];
		}
		rotation = rotation.inverse().eval();

		Eigen::Matrix4d trans2;
		trans2.setIdentity();
		trans2(0, 3) = FHC[0];
		trans2(1, 3) = FHC[1];
		trans2(2, 3) = FHC[2];

		transMatrix = trans2 * rotation * trans1;

		memcpy(res._Elems, transMatrix.data(), 16 * sizeof(double));
		//===========END============
		return res;
	}

	LandMarkType femurModel::CorrectMechanAxis(LandMarkType inp)
	{
		Eigen::Matrix<double, 4, 4, Eigen::RowMajor> T{ m_Matrix_mechan.data() };
		Eigen::Vector4d inpV{inp[0],inp[1] ,inp[2] ,1};

		Eigen::Vector4d res = T * inpV;
		
		return LandMarkType{ res[0],res[1],res[2] };
	}

	LandMarkType femurModel::CorrectCanalAxis(LandMarkType inp)
	{
		Eigen::Matrix<double, 4, 4, Eigen::RowMajor> T{ m_Matrix_canal.data() };
		Eigen::Vector4d inpV{ inp[0],inp[1] ,inp[2] ,1 };

		Eigen::Vector4d res = T * inpV;
		return LandMarkType{ res[0],res[1],res[2] };
	}

	LandMarkType femurModel::GetLT(ECorrection correction)
	{
		LandMarkType LT{};
		GetLandMark(ELandMarks::f_LT, LT);
		switch (correction)
		{
		case MECHANICS:
			return CorrectMechanAxis(LT);
			
		case CANAL: 
			return CorrectCanalAxis(LT);

		case ORIGIN:
			return LT;
		}
		return LT;
	}

	LandMarkType femurModel::GetPFCA(ECorrection correction)
	{
		LandMarkType PFCA{};
		GetLandMark(ELandMarks::f_PFCA, PFCA);
		switch (correction)
		{
		case MECHANICS:
			return CorrectMechanAxis(PFCA);

		case CANAL:
			return CorrectCanalAxis(PFCA);

		case ORIGIN:
			return PFCA;
		}
		return PFCA;
	}

	std::array<double, 16> femurModel::GetMatrix(ECorrection correction)
	{
		switch (correction)
		{
		case MECHANICS: 
			return m_Matrix_mechan;
		case CANAL: 
			return m_Matrix_canal;
		default:
			Eigen::Matrix4d matrix4d;
			matrix4d.setIdentity();
			std::array<double, 16> identity{};
			memcpy(identity._Elems, matrix4d.data(), 16 * sizeof(double));
			return identity;
		}
	}

	bool femurModel_OpSide::Update_inOp()
	{
		LandMarkType FHC{};
		LandMarkType DFCA{};
		LandMarkType PFCA{};
		if (!GetLandMark(ELandMarks::f_FHC, FHC)
			|| !GetLandMark(ELandMarks::f_CheckPointD_pre, DFCA)
			|| !GetLandMark(ELandMarks::f_CheckPointP_pre, PFCA))
		{
			handleError("Err Update_inOp: Call SetPreCheckPoints First");
			return false;
		}

		LandMarkType FHC_post{};
		LandMarkType DFCA_post{};
		LandMarkType PFCA_post{};
		if (!GetLandMark(ELandMarks::f_FHC_inOp, FHC_post))
		{
			handleError("Err Update_inOp: Call SetFHCinOp First");
			return false;		
		}
		if (!GetLandMark(ELandMarks::f_CheckPointD_post, DFCA_post)
			||!GetLandMark(ELandMarks::f_CheckPointP_post, PFCA_post))
		{
			handleError("Err Update_inOp: Call SetPostCheckPoints First");
			return false;
		}
		m_Matrix_checkPoints_pre = calCanalCorrection(FHC, DFCA, PFCA);
		m_Matrix_checkPoints_post = calCanalCorrection(FHC_post, DFCA_post, PFCA_post);

		Eigen::Matrix<double, 4, 4, Eigen::RowMajor> T{ m_Matrix_checkPoints_pre.data() };
		Eigen::Vector4d inpV{ PFCA[0],PFCA[1] ,PFCA[2] ,1 };
		Eigen::Vector4d PFCA_pre_transed = T * inpV;

		Eigen::Matrix<double, 4, 4, Eigen::RowMajor> T_1{ m_Matrix_checkPoints_post.data() };
		Eigen::Vector4d inpV_1{ PFCA_post[0],PFCA_post[1] ,PFCA_post[2] ,1 };
		Eigen::Vector4d PFCA_post_transed = T_1 * inpV_1;

		double res_offset = 0.0;
		if (ESide::right == side)
		{
			res_offset =-( PFCA_post_transed[0] - PFCA_pre_transed[0]);
		}
		else res_offset = PFCA_post_transed[0] - PFCA_pre_transed[0];
		SetResult(EResult::f_OffsetDiff_PrePostOp, res_offset);
		//down - positive
		SetResult(EResult::f_HipLengthDiff_PrePostOp, -(PFCA_post_transed[2] - PFCA_pre_transed[2]));
		return true;
	}

	void femurModel_OpSide::SetEpicondylarPoints(double* Medial, double* Lateral)
	{
		SetLandMark(ELandMarks::f_MFC, Medial);
		SetLandMark(ELandMarks::f_LFC, Lateral);
	}

	double femurModel_OpSide::CalFemoralVersion()
	{
		LandMarkType MFC{};
		LandMarkType LFC{};
		LandMarkType GT{};
		LandMarkType FHC{};
		LandMarkType DFCA{};
		LandMarkType PFCA{};
		std::string lack = "";
		if (!GetLandMark(ELandMarks::f_MFC, MFC))
		{
			lack = lack + "CalFemoralVersion : Can't get Medial Femoral Epicondyle\n";
		}
		if (!GetLandMark(ELandMarks::f_LFC, LFC))
		{
			lack = lack + "CalFemoralVersion : Can't get Lateral Femoral Epicondyle";
		}
		if (!GetLandMark(ELandMarks::f_GT, GT))
		{
			lack = lack + "CalFemoralVersion : Can't get Greater Trochanter";
		}
		if (!GetLandMark(ELandMarks::f_FHC, FHC))
		{
			lack = lack + "CalFemoralVersion : Can't get Femur Head Center";
		}
		if (!GetLandMark(ELandMarks::f_DFCA, DFCA))
		{
			lack = lack + "CalFemoralVersion : Can't get Distal point of Femoral canal axis";
		}
		if (!GetLandMark(ELandMarks::f_PFCA, PFCA))
		{
			lack = lack + "CalFemoralVersion : Can't get Proximal point of Femoral canal axis";
		}

		double normal[3] = { DFCA[0] - PFCA[0],  DFCA[1] - PFCA[1] ,DFCA[2] - PFCA[2] };

		double p1[3], p2[3], p3[3], p4[3];

		projectToPlane(MFC.data(), PFCA.data(), normal, p1);
		projectToPlane(LFC.data(), PFCA.data(), normal, p2);
		projectToPlane(GT.data(), PFCA.data(), normal, p3);
		projectToPlane(FHC.data(), PFCA.data(), normal, p4);

		if (!lack.empty())
		{
			std::cout << lack << std::endl;
		}

		//double Version = AngleBetween2Line(MFC.data(), LFC.data(), GT.data(), FHC.data());
		double Version = AngleBetween2Line(p1, p2, p3, p4);

		SetResult(EResult::f_OpVersion, Version);

		return Version;
	}

	std::array<double, 16> femurModel_OpSide::GetMatrix(ECorrection correction)
	{
		switch (correction)
		{
		case MECHANICS:
			return m_Matrix_mechan;
		case CANAL:
			return m_Matrix_canal;
		case CHECKPOINT_PRE:
			return m_Matrix_checkPoints_pre;
		case CHECKPOINT_POST:
			return m_Matrix_checkPoints_post;
		default:
			Eigen::Matrix4d matrix4d;
			matrix4d.setIdentity();
			std::array<double, 16> identity{};
			memcpy(identity._Elems, matrix4d.data(), 16 * sizeof(double));
			return identity;
		}
	}

	std::array<double, 16> femurModel_OpSide::GetCheckPCorrectMarix_pre() const
	{
		return m_Matrix_checkPoints_pre;
	}

	std::array<double, 16> femurModel_OpSide::GetCheckPCorrectMarix_post() const
	{
		return m_Matrix_checkPoints_post;
	}

	void femurModel_OpSide::handleError(const char* what)
	{
		auto dummy = Eigen::Matrix<double, 4, 4>::Identity().eval();
		memcpy(m_Matrix_checkPoints_pre._Elems, dummy.data(), 16 * sizeof(double));
		memcpy(m_Matrix_checkPoints_post._Elems, dummy.data(), 16 * sizeof(double));
		std::cout << what << std::endl;
	}

	void AnteversionAndInclinationAngle(double *direction, double &ResultAnteversion, double &ResultInclination, EIVAngelType type)
	{
		switch (type)
		{
		case RADIO_GRAPHIC:
			AIAngleRadiographic(direction, ResultAnteversion, ResultInclination);

		case OPERATIVE:
			AIAngleOperative(direction, ResultAnteversion, ResultInclination);

		case ANATOMICAL:
			AIAngleAnatomical(direction, ResultAnteversion, ResultInclination);
		}
	}

	void AIAngleRadiographic(double *direction, double &ResultAnteversion, double &ResultInclination)
	{
		double Coronalnormal[3] = { 0, -1, 0 };
		double D_value = 90.0;
		double CupLineTransform[3] = { direction[0], direction[1], direction[2] };
		double VersionRadians = AngleBetween2Vector(CupLineTransform, Coronalnormal, true);
		ResultAnteversion = (double)(VersionRadians * (180.0f / EIGEN_PI)) - D_value;
		if (ResultAnteversion < 0.0)
		{
			ResultAnteversion = -ResultAnteversion;
		}
		//std::cout << "ResultAnteversion:" << ResultAnteversion << std::endl;
		double CupLineProject[3];
		double origin[] = { 0, 0, 0 };
		//ͶӰ����״����
		projectToPlane(CupLineTransform, origin, Coronalnormal, CupLineProject);
		double z[3] = { 0, 0, 1 };
		double InclinationRadians = AngleBetween2Vector(CupLineProject, z, true);
		ResultInclination = (double)(InclinationRadians * (180.0f / EIGEN_PI));
		if (ResultInclination > 90.0)
		{
			ResultInclination = 180.0 - ResultInclination;
		}
		//std::cout << "ResultInclination:" << ResultInclination << std::endl;
	}

	void AIAngleOperative(double *direction, double &ResultAnteversion, double &ResultInclination)
	{
		double Sagittalnormal[3] = { -1, 0, 0 };
		double z[3] = { 0, 0, 1 };
		double D_value = 90.0;
		double CupLineTransform[3] = { direction[0], direction[1], direction[2] };
		double CupLineProject[3];
		double origin[] = { 0, 0, 0 };
		//ͶӰ��ʸ״����
		projectToPlane(CupLineTransform, origin, Sagittalnormal, CupLineProject);
		double VersionRadians = AngleBetween2Vector(z, CupLineProject, true);
		ResultAnteversion = (double)(VersionRadians * (180.0f / EIGEN_PI));
		if (ResultAnteversion > 90.0)
		{
			ResultAnteversion = 180.0 - ResultAnteversion;
		}
		//std::cout << "ResultAnteversion:" << ResultAnteversion << std::endl;

		double InclinationRadians = AngleBetween2Vector(CupLineTransform, Sagittalnormal, true);
		ResultInclination = (double)(InclinationRadians * (180.0f / EIGEN_PI)) - D_value;
		if (ResultInclination < 0.0)
		{
			ResultInclination = -ResultInclination;
		}
		//std::cout << "ResultInclination:" << ResultInclination << std::endl;
	}

	void AIAngleAnatomical(double *direction, double &ResultAnteversion, double &ResultInclination)
	{
		double TransverseNormal[3] = { 0, 0, -1 };
		double z[3] = { 0, 0, 1 };
		double x[3] = { -1, 0, 0 };
		double CupLineTransform[3] = { direction[0], direction[1], direction[2] };
		double CupLineProject[3];
		double origin[] = { 0, 0, 0 };
		//ͶӰ���������
		projectToPlane(CupLineTransform, origin, TransverseNormal, CupLineProject);
		double VersionRadians = AngleBetween2Vector(x, CupLineProject, true);
		ResultAnteversion = (double)(VersionRadians * (180.0f / EIGEN_PI));
		if (ResultAnteversion > 90.0)
		{
			ResultAnteversion = 180.0 - ResultAnteversion;
		}
		//std::cout << "ResultAnteversion:" << ResultAnteversion << std::endl;

		double InclinationRadians = AngleBetween2Vector(CupLineTransform, z, true);
		ResultInclination = (double)(InclinationRadians * (180.0f / EIGEN_PI));
		if (ResultInclination > 90.0)
		{
			ResultInclination = 180.0 - ResultInclination;
		}
		//std::cout << "ResultInclination:" << ResultInclination << std::endl;
	}

	double FemoralVersionAngle(double* MFC, double* LFC, double* GT, double* FHC, double* DFCA, double* PFCA)
	{
		double normal[3] = { DFCA[0] - PFCA[0],  DFCA[1] - PFCA[1] ,DFCA[2] - PFCA[2] };

		double p1[3], p2[3], p3[3], p4[3];

		projectToPlane(MFC, PFCA, normal, p1);
		projectToPlane(LFC, PFCA, normal, p2);
		projectToPlane(GT, PFCA, normal, p3);
		projectToPlane(FHC, PFCA, normal, p4);

		double Version = AngleBetween2Line(p1, p2, p3, p4);

		return Version;
	}

	TKADataBase::TKADataBase(TKADataBase && other) noexcept : m_map_landMark(std::move(other.m_map_landMark)),
		m_map_axis(std::move(other.m_map_axis)),
		m_map_plane(std::move(other.m_map_plane)),
		m_map_result(std::move(other.m_map_result))
	{
	}

	TKADataBase & TKADataBase::operator=(const TKADataBase & other)
	{
		if (this == &other)
			return *this;
		m_map_landMark = other.m_map_landMark;
		m_map_axis = other.m_map_axis;
		m_map_plane = other.m_map_plane;
		m_map_result = other.m_map_result;
		return *this;
	}

	TKADataBase & TKADataBase::operator=(TKADataBase && other) noexcept
	{
		if (this == &other)
			return *this;
		m_map_landMark = std::move(other.m_map_landMark);
		m_map_axis = std::move(other.m_map_axis);
		m_map_plane = std::move(other.m_map_plane);
		m_map_result = std::move(other.m_map_result);
		return *this;
	}

	void TKADataBase::SetLandMark(TKALandmarks name, double * data)
	{
		try
		{
			LandMarkType array{};
			memcpy(array._Elems, data, 3 * sizeof(double));
			m_map_landMark.insert_or_assign(name, array);
		}
		catch (...)
		{
			std::cout << "err" << std::endl;
		}
	}

	bool TKADataBase::GetLandMark(TKALandmarks name, LandMarkType & oup_value)
	{
		auto iter = m_map_landMark.find(name);
		if (iter != m_map_landMark.end())
		{
			oup_value = iter->second;
			return true;
		}
		std::cout << "Error: Cant find LandMark:" << to_string(name) << std::endl;
		return false;
	}

	void TKADataBase::SetAxis(TKAAxes name, double * p_start, double * direction)
	{
		AxisType axis{};
		memcpy(axis.startPoint._Elems, p_start, 3 * sizeof(double));
		memcpy(axis.direction._Elems, direction, 3 * sizeof(double));

		m_map_axis.insert_or_assign(name, axis);
	}

	bool TKADataBase::GetAxis(TKAAxes name, AxisType & outp_axis)
	{
		auto iter = m_map_axis.find(name);
		if (iter != m_map_axis.end())
		{
			outp_axis = iter->second;
			return true;
		}
		std::cout << "Error: Cant find Axis:" << to_string(name) << std::endl;
		return false;
	}

	void TKADataBase::SetPlane(TKAPlanes name, double * center, double * normal)
	{
		PlaneType plane{};
		memcpy(plane.normal.startPoint._Elems, center, 3 * sizeof(double));
		memcpy(plane.normal.direction._Elems, normal, 3 * sizeof(double));

		m_map_plane.insert_or_assign(name, plane);
	}

	bool TKADataBase::GetPlane(TKAPlanes name, PlaneType & outp_plane)
	{
		auto iter = m_map_plane.find(name);
		if (iter != m_map_plane.end())
		{
			outp_plane = iter->second;
			return true;
		}
		std::cout << "Error: Cant find Plane:" << to_string(name) << std::endl;
		return false;
	}

	void TKADataBase::SetResult(TKAResult name, double res)
	{
		m_map_result.insert_or_assign(name, res);
	}

	bool TKADataBase::GetResult(TKAResult name, double & outp_res)
	{
		auto iter = m_map_result.find(name);
		if (iter != m_map_result.end())
		{
			outp_res = iter->second;
			return true;
		}
		std::cout << "Error: Cant find Result:" << to_string(name) << std::endl;
		return false;
	}

	void TKAFemurModel::update()
	{
		updateMechanicalAxis();
		updatePCA();
		updateTEA();
		updateFemurCoordinate();
	}

	void TKAFemurModel::updateMechanicalAxis()
	{
		LandMarkType HipCtr{};
		LandMarkType KCtr{};

		if (!(GetLandMark(TKALandmarks::HIP_CENTER, HipCtr)) || !(GetLandMark(TKALandmarks::fKNEE_CENTER, KCtr)))
		{
			return;
		}
		VectorType direction{ Normalize(std::array<double,3>{KCtr[0] - HipCtr[0],KCtr[1] - HipCtr[1],KCtr[2] - HipCtr[2]}) };
		SetAxis(TKAAxes::f_MA, HipCtr.data(), direction.data());
	}

	void TKAFemurModel::updatePCA()
	{
		LandMarkType PM{};
		LandMarkType PL{};

		if (!(GetLandMark(TKALandmarks::f_PM, PM)) || !(GetLandMark(TKALandmarks::f_PL, PL)))
		{
			return;
		}
		VectorType direction{ Normalize(std::array<double,3>{PM[0] - PL[0],PM[1] - PL[1],PM[2] - PL[2]}) };
		SetAxis(TKAAxes::f_PCA, PL.data(), direction.data());
	}

	void TKAFemurModel::updateTEA()
	{
		LandMarkType ME{};
		LandMarkType LE{};

		if (!(GetLandMark(TKALandmarks::f_ME, ME)) || !(GetLandMark(TKALandmarks::f_LE, LE)))
		{
			return;
		}

		VectorType direction{ Normalize(std::array<double,3>{ME[0] - LE[0],ME[1] - LE[1],ME[2] - LE[2]}) };
		SetAxis(TKAAxes::f_TEA, LE.data(), direction.data());
	}

	void TKAFemurModel::updateFemurCoordinate()
	{
		AxisType MA{};
		AxisType TEA{};

		if (!(GetAxis(TKAAxes::f_MA, MA)) || !(GetAxis(TKAAxes::f_TEA, TEA)))
		{
			return;
		}

		double coronal[3], transverse[3],sagittal[3];
		CommonVerticalVector(TEA.direction.data(), MA.direction.data(), coronal);
		SetAxis(TKAAxes::f_coronal, MA.startPoint.data(), coronal);
		CommonVerticalVector(MA.direction.data(), coronal, sagittal);
		SetAxis(TKAAxes::f_sagittal, MA.startPoint.data(), sagittal);
		SetAxis(TKAAxes::f_transverse, MA.startPoint.data(), MA.direction.data());
	}

	void TKATibiaModel::update()
	{
		updateMechanicalAxis();
		updateAPAixs();
		updateTibiaCoordinate();
	}

	void TKATibiaModel::updateMechanicalAxis()
	{
		LandMarkType KCtr{};
		LandMarkType ACtr{};

		if (!(GetLandMark(TKALandmarks::tKNEE_CENTER, KCtr)) || !(GetLandMark(TKALandmarks::tANKLE_CENTER, ACtr)))
		{
			return;
		}
		VectorType direcrion{ Normalize(std::array<double, 3>{ACtr[0] - KCtr[0], ACtr[1] - KCtr[1], ACtr[2] - KCtr[2]}) };
		SetAxis(TKAAxes::t_MA, KCtr.data(), direcrion.data());
	}

	void TKATibiaModel::updateAPAixs()
	{
		LandMarkType PCLCtr{};
		LandMarkType TUBERCLE{};

		if (!(GetLandMark(TKALandmarks::PCL_CENTER, PCLCtr)) || !(GetLandMark(TKALandmarks::TUBERCLE, TUBERCLE)))
		{
			return;
		}
		VectorType direction{ Normalize(std::array<double,3>{TUBERCLE[0] - PCLCtr[0],TUBERCLE[1] - PCLCtr[1],TUBERCLE[2] - PCLCtr[2]}) };
		SetAxis(TKAAxes::t_APA, PCLCtr.data(), direction.data());
	}

	void TKATibiaModel::updateTibiaCoordinate()
	{
		AxisType MA{};
		AxisType AP{};

		if (!(GetAxis(TKAAxes::t_MA, MA)) || !(GetAxis(TKAAxes::t_APA, AP)))
		{
			return;
		}

		double coronal[3], transverse[3], sagittal[3];
		CommonVerticalVector(AP.direction.data(), MA.direction.data(), sagittal);
		SetAxis(TKAAxes::t_sagittal, MA.startPoint.data(), sagittal);
		CommonVerticalVector(MA.direction.data(), sagittal, coronal);
		SetAxis(TKAAxes::t_coronal, MA.startPoint.data(), coronal);
		SetAxis(TKAAxes::t_transverse, MA.startPoint.data(), MA.direction.data());
	}

	void TKAFemurImplantModel::update()
	{
		updateResectionLine();
		updateAnteriorCut();
		updateAnteriorChamCut();
		updateDistalCut();
		updatePosteriorChamCut();
		updatePosteriorCut();
	}

	void TKAFemurImplantModel::updateResectionLine()
	{
		LandMarkType RM{};
		LandMarkType RL{};

		if (!(GetLandMark(TKALandmarks::fi_ResectionMedial, RM)) || !(GetLandMark(TKALandmarks::fi_ResectionLateral, RL)))
		{
			return;
		}
		VectorType direction{ Normalize(std::array<double,3>{RL[0] - RM[0],RL[1] - RM[1],RL[2] - RM[2]}) };
		SetAxis(TKAAxes::fi_BRL, RM.data(), direction.data());
	}

	void TKAFemurImplantModel::updateAnteriorCut()
	{
		LandMarkType Start{};
		LandMarkType End{};

		if (!(GetLandMark(TKALandmarks::fi_ANTERIORSTART, Start)) || !(GetLandMark(TKALandmarks::fi_ANTERIOREND, End)))
		{
			return;
		}
		VectorType direction{ Normalize(std::array<double,3>{End[0] - Start[0],End[1] - Start[1],End[2] - Start[2]}) };
		SetPlane(TKAPlanes::FEMURANTERIOR, Start.data(), direction.data());
	}

	void TKAFemurImplantModel::updateAnteriorChamCut()
	{
		LandMarkType Start{};
		LandMarkType End{};

		if (!(GetLandMark(TKALandmarks::fi_ANTERIORCHAMSTART, Start)) || !(GetLandMark(TKALandmarks::fi_ANTERIORCHAMEND, End)))
		{
			return;
		}
		VectorType direction{ Normalize(std::array<double,3>{End[0] - Start[0],End[1] - Start[1],End[2] - Start[2]}) };
		SetPlane(TKAPlanes::FEMURANTERIORCHAM, Start.data(), direction.data());
	}

	void TKAFemurImplantModel::updateDistalCut()
	{
		LandMarkType Start{};
		LandMarkType End{};

		if (!(GetLandMark(TKALandmarks::fi_DISTALSTART, Start)) || !(GetLandMark(TKALandmarks::fi_DISTALEND, End)))
		{
			return;
		}
		VectorType direction{ Normalize(std::array<double,3>{End[0] - Start[0],End[1] - Start[1],End[2] - Start[2]}) };
		SetAxis(TKAAxes::fi_A, Start.data(), direction.data());
		SetPlane(TKAPlanes::FEMURDISTAL, Start.data(), direction.data());
	}

	void TKAFemurImplantModel::updatePosteriorChamCut()
	{
		LandMarkType Start{};
		LandMarkType End{};

		if (!(GetLandMark(TKALandmarks::fi_POSTERIORCHAMSTART, Start)) || !(GetLandMark(TKALandmarks::fi_POSTERIORCHAMEND, End)))
		{
			return;
		}
		VectorType direction{ Normalize(std::array<double,3>{End[0] - Start[0],End[1] - Start[1],End[2] - Start[2]}) };
		SetPlane(TKAPlanes::FEMURPOSTERIORCHAM, Start.data(), direction.data());
	}

	void TKAFemurImplantModel::updatePosteriorCut()
	{
		LandMarkType Start{};
		LandMarkType End{};

		if (!(GetLandMark(TKALandmarks::fi_POSTERIORSTART, Start)) || !(GetLandMark(TKALandmarks::fi_POSTERIOREND, End)))
		{
			return;
		}
		VectorType direction{ Normalize(std::array<double,3>{End[0] - Start[0],End[1] - Start[1],End[2] - Start[2]}) };
		SetPlane(TKAPlanes::FEMURPOSTERIOR, Start.data(), direction.data());
	}

	void TKATibiaImplantModel::update()
	{
		updateAxis();
		updatePlaneSymmetryAxis();
	}

	void TKATibiaImplantModel::updateAxis()
	{
		LandMarkType Start{};
		LandMarkType End{};

		if (!(GetLandMark(TKALandmarks::ti_PROXIMALSTART, Start)) || !(GetLandMark(TKALandmarks::ti_PROXIMALEND, End)))
		{
			return;
		}
		VectorType direction{ Normalize(std::array<double,3>{End[0] - Start[0],End[1] - Start[1],End[2] - Start[2]}) };
		SetAxis(TKAAxes::ti_A, Start.data(), direction.data());
		SetPlane(TKAPlanes::TIBIAPROXIMAL, Start.data(), direction.data());
	}

	void TKATibiaImplantModel::updatePlaneSymmetryAxis()
	{
		LandMarkType Start{};
		LandMarkType End{};

		if (!(GetLandMark(TKALandmarks::ti_SYMMETRYSTART, Start)) || !(GetLandMark(TKALandmarks::ti_SYMMETRYEND, End)))
		{
			return;
		}
		VectorType direction{ Normalize(std::array<double,3>{End[0] - Start[0],End[1] - Start[1],End[2] - Start[2]}) };
		SetAxis(TKAAxes::ti_SA, Start.data(), direction.data());
	}
}
