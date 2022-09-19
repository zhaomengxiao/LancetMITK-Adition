#include "robotRegistration.h"
#include <vtkMatrix4x4.h>
#include <vtkNew.h>
#include <mitkLog.h>

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

using namespace Eigen;
void RobotRegistration::AddPose(mitk::NavigationData::Pointer nd_robot2flange, mitk::NavigationData::Pointer nd_Ndi2RegistFrame, bool translationOnly)
{
  mitk::Matrix3D Rrobot2flange = nd_robot2flange->GetRotationMatrix();
  mitk::Point3D Vrobot2flange = nd_robot2flange->GetPosition();
  mitk::Matrix3D Rndi2RegistFrame = nd_Ndi2RegistFrame->GetRotationMatrix();
  mitk::Point3D Vndi2RegistFrame = nd_Ndi2RegistFrame->GetPosition();

	Matrix3d r;
	r << Rrobot2flange(0, 0), Rrobot2flange(0, 1), Rrobot2flange(0, 2), Rrobot2flange(1, 0),
		Rrobot2flange(1, 1), Rrobot2flange(1, 2), Rrobot2flange(2, 0), Rrobot2flange(2, 1),
		Rrobot2flange(2, 2);

	Vector3d v;
	v << Vrobot2flange[0], Vrobot2flange[1], Vrobot2flange[2];

	Matrix3d rn;
	rn << Rndi2RegistFrame(0, 0), Rndi2RegistFrame(0, 1), Rndi2RegistFrame(0, 2), Rndi2RegistFrame(1, 0),
		Rndi2RegistFrame(1, 1), Rndi2RegistFrame(1, 2), Rndi2RegistFrame(2, 0), Rndi2RegistFrame(2, 1),
		Rndi2RegistFrame(2, 2);

	Vector3d vn;
  v << Vndi2RegistFrame[0], Vndi2RegistFrame[1], Vndi2RegistFrame[2];

	this->m_translationOnly = translationOnly;
	if (translationOnly)
	{
		R.emplace(R.begin(), r);
		V.emplace(V.begin(), v);
		Rn.emplace(Rn.begin(), rn);
		Vn.emplace(Vn.begin(), vn);
		m_numberOfFixR++;
	}
	else
	{
		R.push_back(r);
		V.push_back(v);
		Rn.push_back(rn);
		Vn.push_back(vn);
	}
	m_numberOfPose++;
}

bool RobotRegistration::PopLastPose()
{
	return this->PopLastPose(this->m_translationOnly);
}

bool RobotRegistration::PopLastPose(bool translationOnly)
{
	if (R.size() != V.size() || V.size() != Rn.size() || Rn.size() != Vn.size())
	{
		MITK_WARN << "Unknown error, the number of internal containers does not correspond.";
		return false;
	}

	if (R.size() == 0)
	{
		MITK_WARN << "Internal container quantity is empty.";
		return false;
	}

	if (true == translationOnly)
	{
		R.erase(R.begin());
		V.erase(V.begin());
		Rn.erase(Rn.begin());
		Vn.erase(Vn.begin());
		--m_numberOfFixR;
	}
	else
	{
		R.pop_back();
		V.pop_back();
		Rn.pop_back();
		Vn.pop_back();
	}
	--m_numberOfPose;
	return true;
}

void RobotRegistration::RemoveAllPose()
{
	Rn.clear();
	Vn.clear();

	R0 = Matrix3d();
	V0 = Vector3d();

	R.clear();
	V.clear();

	Re = Matrix3d();
	Ve = Vector3d();

	m_numberOfFixR = 0;
	m_numberOfPose = 0;

	m_translationOnly = true;
	m_calculateEndToolAttitude = false;
}

int RobotRegistration::PoseCount() const
{
	return m_numberOfPose;
}

bool RobotRegistration::GetRegistraionMatrix(vtkMatrix4x4* output)
{
	if (Regist() == false)
	{
		return false;
	}

	Matrix4d matrix;
	matrix.setIdentity();
	matrix.block(0, 0, 3, 3) = R0;
	matrix.block(0, 3, 3, 1) = V0;

	vtkNew<vtkMatrix4x4> res;
	res->DeepCopy(matrix.data());
	res->Transpose();

	output->DeepCopy(res);
	return true;
}

void RobotRegistration::GetTCP(std::array<double, 6>& output)
{
	//In case anyone forgets to do calculation
	Regist();

	output[0] = Ve(0);
	output[1] = Ve(1);
	output[2] = Ve(2);

	Eigen::Vector3d eulerAngle = Re.eulerAngles(2, 1, 0);
	output[3] = eulerAngle(0);
	output[4] = eulerAngle(1);
	output[5] = eulerAngle(2);
}


bool RobotRegistration::Regist()
{
	//check calculation pre requirements
	if (m_numberOfFixR < 4)
	{
		MITK_ERROR << "At least four translation only pose needed to regist";
		return false;
	}
	// check if truly translate only
	for (int i = 0; i < m_numberOfFixR; i++)
	{
		if (!R[i].isApprox(R[0], 0.001))
		{
			MITK_WARN << "There is rotation in Only translation pose, Registration Is not accurate. ";
			MITK_WARN << R[0];
			MITK_WARN << R[i];
		}
	}

	calculateR0();
	calculateVe();
	calculateV0();
	calculateRe();
	return true;
}

double RobotRegistration::RMS()
{
	double res = 0;
	for (int i = 0; i < m_numberOfPose; i++)
	{
		Vector3d err = Vn[i] - R0 * R[i] * Ve - R0 * V[i] - V0;
		res += err.cwiseProduct(err).sum();
	}
	return sqrt(res / m_numberOfPose);
}

void RobotRegistration::Print()
{
	for (int i = 0; i < Rn.size(); i++)
	{
		std::cout << "Robot Matrix in algo" << std::endl;
		std::cout << R[i] << std::endl;
		std::cout << V[i] << std::endl;
		std::cout << "tool Matrix in algo" << std::endl;
		std::cout << Rn[i] << std::endl;
		std::cout << Vn[i] << std::endl;
	}
}

void RobotRegistration::calculateR0()
{
	MatrixXd A;
	A.resize(3, m_numberOfFixR - 1);
	int col_index = 0;
	for (int i = 1; i < m_numberOfFixR; i++)
	{
		Vector3d dV = V[i] - V[i - 1];
		A.col(col_index) = dV;
		col_index++;
	}
	//std::cout << "A:\n" << A << std::endl;

	Matrix3Xd B;
	B.resize(3, m_numberOfFixR - 1);
	col_index = 0;
	for (int i = 1; i < m_numberOfFixR; i++)
	{
		Vector3d dV = Vn[i] - Vn[i - 1];
		B.col(col_index) = dV;
		col_index++;
	}
	//std::cout << "B:\n" << B << std::endl;

	MatrixXd ABt = A * B.transpose();
	//std::cout << "ABt:\n" << ABt << std::endl;

	JacobiSVD<MatrixXd> svd(ABt, ComputeThinU | ComputeThinV);
	auto v = svd.matrixV();
	auto u = svd.matrixU();
	//std::cout << "V:\n" << v << std::endl;
	//std::cout << "U:\n" << u << std::endl;
	R0 = v * u.transpose();

	//std::cout << "R0:\n" << R0 << std::endl;
}

void RobotRegistration::calculateRe()
{
	MatrixXd A;
	const int n = m_numberOfPose - m_numberOfFixR;
	A.resize(3 * n, 3);
	for (int i = 0; i < n; i++)
	{
		A.block(3 * i, 0, 3, 3) = R[i + m_numberOfFixR];
	}
	//std::cout << "A:\n" << A << std::endl;

	MatrixXd B;
	B.resize(3 * n, 3);
	for (int i = 0; i < n; i++)
	{
		B.block(3 * i, 0, 3, 3) = R0.transpose() * Rn[i + m_numberOfFixR];
	}
	//std::cout << "B:\n" << B << std::endl;
	Re = (A.transpose() * A).inverse() * A.transpose() * B;
	//std::cout << "Re:\n" << Re << std::endl;
}

void RobotRegistration::calculateVe()
{
	MatrixXd A;
	const int n = m_numberOfPose - m_numberOfFixR - 1;
	A.resize(3 * n, 3);
	for (int i = 0; i < n; i++)
	{
		Matrix3d a = R0 * (R[i + m_numberOfFixR + 1] - R[i + m_numberOfFixR]);
		A.block(3 * i, 0, 3, 3) = a;
	}
	//std::cout << "A:\n" << A << std::endl;

	MatrixXd B;
	B.resize(3 * n, 1);
	for (int i = 0; i < n; i++)
	{
		B.block(3 * i, 0, 3, 1) =
			Vn[i + m_numberOfFixR + 1] - Vn[i + m_numberOfFixR] - R0 * (V[i + m_numberOfFixR + 1] - V[i + m_numberOfFixR]);
	}
	//std::cout << "B:\n" << B << std::endl;
	Ve = (A.transpose() * A).inverse() * A.transpose() * B;
	//std::cout << "Ve:\n" << Ve << std::endl;
}

void RobotRegistration::calculateV0()
{
	V0.setZero();
	for (int i = 0; i < m_numberOfPose; i++)
	{
		V0 = V0 + (Vn[i] - R0 * R[i] * Ve - R0 * V[i]);
	}
	V0 = V0 / R.size();
	//std::cout << "V0:\n" << V0 << std::endl;
}

void RobotRegistration::genTestData()
{
	//Tm2b
	Transform<double, 3, Affine> transform = Translation3d(10, 10, 10) * AngleAxis<double>(M_PI / 6, Vector3d(0, 0, 1));
	Matrix4d Tm2b = transform.matrix();
	std::cout << "Tm2b:\n" << Tm2b << std::endl;
	//Tf2e TCP
	transform = Translation3d(5, 5, 5) * AngleAxis<double>(M_PI / 2, Vector3d(0, 1, 0));
	Matrix4d Tf2e = transform.matrix();
	std::cout << "Tf2e:\n" << Tf2e << std::endl;

	//10 pose of robot arm,first 5 fixR;
	std::vector<Matrix4d> Tb2f;
	//
	transform = Translation3d(5, 5, 5) * AngleAxis<double>(M_PI / 2, Vector3d(1, 2, 3).normalized());
	Tb2f.push_back(transform.matrix());
	transform = Translation3d(5, 3, 10) * AngleAxis<double>(M_PI / 2, Vector3d(1, 2, 3).normalized());
	Tb2f.push_back(transform.matrix());
	transform = Translation3d(5, 5, 4) * AngleAxis<double>(M_PI / 2, Vector3d(1, 2, 3).normalized());
	Tb2f.push_back(transform.matrix());
	transform = Translation3d(1, 5, 5) * AngleAxis<double>(M_PI / 2, Vector3d(1, 2, 3).normalized());
	Tb2f.push_back(transform.matrix());
	transform = Translation3d(5, 2, 6) * AngleAxis<double>(M_PI / 2, Vector3d(1, 2, 3).normalized());
	Tb2f.push_back(transform.matrix());
	//
	transform = Translation3d(1, 5, 7) * AngleAxis<double>(M_PI / 6, Vector3d(1, 2, 3).normalized());
	Tb2f.push_back(transform.matrix());
	transform = Translation3d(5, 3, 10) * AngleAxis<double>(M_PI / 2, Vector3d(2, 1, 3).normalized());
	Tb2f.push_back(transform.matrix());
	transform = Translation3d(2, 5, 4) * AngleAxis<double>(M_PI / 5, Vector3d(1, 2, 3).normalized());
	Tb2f.push_back(transform.matrix());
	transform = Translation3d(1, 8, 3) * AngleAxis<double>(M_PI / 9, Vector3d(1, 0, 0).normalized());
	Tb2f.push_back(transform.matrix());
	transform = Translation3d(5, 2, 6) * AngleAxis<double>(M_PI / 2, Vector3d(1, 2, 0).normalized());
	Tb2f.push_back(transform.matrix());

	std::vector<Matrix4d> Tm2e;
	for (auto t : Tb2f)
	{
		Tm2e.push_back(Tm2b * t * Tf2e);
	}

	for (int i = 0; i < 10; i++)
	{
		Matrix3d r = Tb2f[i].block(0, 0, 3, 3);
		Vector3d v = Tb2f[i].block(0, 3, 3, 1);

		Matrix3d rn = Tm2e[i].block(0, 0, 3, 3);
		Vector3d vn = Tm2e[i].block(0, 3, 3, 1);

		R.push_back(r);
		V.push_back(v);

		Rn.push_back(rn);
		Vn.push_back(vn);
	}

	m_numberOfFixR = 5;
}
