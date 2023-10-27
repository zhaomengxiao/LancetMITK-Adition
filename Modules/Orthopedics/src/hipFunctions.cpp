/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "hipFunctions.h"


namespace othopedics
{
	void CupAngle(double* direction, double& ResultAnteversion, double& ResultInclination, ECupAngleType type)
	{
		switch (type)
		{
		case ECupAngleType::RADIO_GRAPHIC:
			cupAngleRadiographic(direction, ResultAnteversion, ResultInclination);
			break;
		case ECupAngleType::OPERATIVE:
			cupAngleOperative(direction, ResultAnteversion, ResultInclination);
			break;
		case ECupAngleType::ANATOMICAL:
			cupAngleAnatomical(direction, ResultAnteversion, ResultInclination);
			break;
		default:
			cupAngleRadiographic(direction, ResultAnteversion, ResultInclination);
		}
	}

	void cupAngleRadiographic(double* direction, double& ResultAnteversion, double& ResultInclination)
	{
		double Coronalnormal[3] = { 0, -1, 0 };
		double D_value = 90.0;
		double CupLineTransform[3] = { direction[0], direction[1], direction[2] };
		double VersionRadians = AngleBetween2Vector(CupLineTransform, Coronalnormal, true);
		ResultAnteversion = (double)(VersionRadians * (180.0 / EIGEN_PI)) - D_value;
		if (ResultAnteversion < 0.0)
		{
			ResultAnteversion = -ResultAnteversion;
		}
		//std::cout << "ResultAnteversion:" << ResultAnteversion << std::endl;
		double CupLineProject[3];
		double origin[] = { 0, 0, 0 };
		//Projected onto the coronal plane
		projectToPlane(CupLineTransform, origin, Coronalnormal, CupLineProject);
		double z[3] = { 0, 0, 1 };
		double InclinationRadians = AngleBetween2Vector(CupLineProject, z, true);
		ResultInclination = (double)(InclinationRadians * (180.0 / EIGEN_PI));
		if (ResultInclination > 90.0)
		{
			ResultInclination = 180.0 - ResultInclination;
		}
		//std::cout << "ResultInclination:" << ResultInclination << std::endl;
	}

	void cupAngleOperative(double* direction, double& ResultAnteversion, double& ResultInclination)
	{
		double Sagittalnormal[3] = { -1, 0, 0 };
		double z[3] = { 0, 0, 1 };
		double D_value = 90.0;
		double CupLineTransform[3] = { direction[0], direction[1], direction[2] };
		double CupLineProject[3];
		double origin[] = { 0, 0, 0 };
		//投影到矢状面上
		projectToPlane(CupLineTransform, origin, Sagittalnormal, CupLineProject);
		double VersionRadians = AngleBetween2Vector(z, CupLineProject, true);
		ResultAnteversion = (double)(VersionRadians * (180.0 / EIGEN_PI));
		if (ResultAnteversion > 90.0)
		{
			ResultAnteversion = 180.0 - ResultAnteversion;
		}
		//std::cout << "ResultAnteversion:" << ResultAnteversion << std::endl;

		double InclinationRadians = AngleBetween2Vector(CupLineTransform, Sagittalnormal, true);
		ResultInclination = (double)(InclinationRadians * (180.0 / EIGEN_PI)) - D_value;
		if (ResultInclination < 0.0)
		{
			ResultInclination = -ResultInclination;
		}
		//std::cout << "ResultInclination:" << ResultInclination << std::endl;
	}

	void cupAngleAnatomical(double* direction, double& ResultAnteversion, double& ResultInclination)
	{
		const double TransverseNormal[3] = { 0, 0, -1 };
		double z[3] = { 0, 0, 1 };
		double x[3] = { -1, 0, 0 };
		double CupLineTransform[3] = { direction[0], direction[1], direction[2] };
		double CupLineProject[3];
		double origin[] = { 0, 0, 0 };
		//投影到横断面上
		projectToPlane(CupLineTransform, origin, TransverseNormal, CupLineProject);
		double VersionRadians = AngleBetween2Vector(x, CupLineProject, true);
		ResultAnteversion = (double)(VersionRadians * (180.0 / EIGEN_PI));
		if (ResultAnteversion > 90.0)
		{
			ResultAnteversion = 180.0 - ResultAnteversion;
		}
		//std::cout << "ResultAnteversion:" << ResultAnteversion << std::endl;

		double InclinationRadians = AngleBetween2Vector(CupLineTransform, z, true);
		ResultInclination = (double)(InclinationRadians * (180.0 / EIGEN_PI));
		if (ResultInclination > 90.0)
		{
			ResultInclination = 180.0 - ResultInclination;
		}
		//std::cout << "ResultInclination:" << ResultInclination << std::endl;
	}

	double FemoralVersionAngle(ESide side, double* FHC, double* FNC, double* ME, double* LE, double* DFCA, double* PFCA)
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

	double PelvicTilt(double* pelvicYAxis)
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

	double HipLength(double* LT, double* ASIS_L, double* ASIS_R)
	{
		return GetPointToLineDistance(LT, ASIS_L, ASIS_R);
	}

	double CombinedOffset(double* PFCA, double* DFCA, double* MidLine)
	{
		return GetPointToLineDistance(MidLine, PFCA, DFCA);
	}

	Eigen::Matrix4d CalPelvisCorrectionMatrix(Eigen::Vector3d ASIS_R, Eigen::Vector3d ASIS_L)
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

	Eigen::Matrix4d CalFemurCanalCorrectionMatrix(Eigen::Vector3d FHC, Eigen::Vector3d FNC, Eigen::Vector3d DFCA,
		Eigen::Vector3d PFCA, ESide side)
	{
		//cal axes
		Eigen::Matrix4d localFrame = CalFemurLocalFrame_canal(FHC, FNC, DFCA, PFCA, side);
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

	Eigen::Matrix4d CalFemurMechanicalCorrectionMatrix(Eigen::Vector3d FHC, Eigen::Vector3d FNC, Eigen::Vector3d ME,
		Eigen::Vector3d LE, ESide side)
	{
		//cal axes
		Eigen::Matrix4d localFrame = CalFemurLocalFrame_mechanical(FHC, FNC, ME, LE, side);
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

	Eigen::Matrix4d CalApplyAIAngleMatrix(Eigen::Vector3d center, double Anteversion, double Inclination, ESide side)
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

	Eigen::Matrix4d CalPelvisLocalFrame(Eigen::Vector3d& ASIS_L, Eigen::Vector3d& ASIS_R, Eigen::Vector3d& MidLine)
	{
		Eigen::Vector3d x = (ASIS_L - ASIS_R).normalized();
		Eigen::Vector3d y = (ASIS_R - MidLine).cross(ASIS_L - MidLine).normalized();
		Eigen::Vector3d z = x.cross(y).normalized();

		return ConvertCoordstoTransform(MidLine, x, y, z);
	}

	Eigen::Matrix4d CalFemurLocalFrame_canal(Eigen::Vector3d& FHC, Eigen::Vector3d& FNC, Eigen::Vector3d& DFCA, Eigen::Vector3d& PFCA, ESide side)
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

	Eigen::Matrix4d CalFemurLocalFrame_mechanical(Eigen::Vector3d& FHC, Eigen::Vector3d& FNC, Eigen::Vector3d& ME,
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

