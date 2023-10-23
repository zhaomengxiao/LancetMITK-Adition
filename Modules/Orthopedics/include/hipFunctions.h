/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef hipFunctions_h
#define hipFunctions_h

#include "hipTermDefinition.h"
#include <itkeigen/Eigen/Eigen>
#include <vtkMath.h>

namespace othopedics
{
	//todo should be move out
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

	inline Eigen::Matrix4d ConvertCoordstoTransform(Eigen::Vector3d& o, Eigen::Vector3d& x, Eigen::Vector3d& y, Eigen::Vector3d& z)
	{
		Eigen::Matrix4d transform;
		transform <<
			x(0), y(0), z(0), o(0),
			x(1), y(1), z(1), o(1),
			x(2), y(2), z(2), o(2),
			0, 0, 0, 1;
		return transform;
	}

	inline void projectToPlane(const double x[3], const double origin[3], const double normal[3], double xproj[3])
	{
		double t, xo[3];

		xo[0] = x[0] - origin[0];
		xo[1] = x[1] - origin[1];
		xo[2] = x[2] - origin[2];

		Eigen::Vector3d nl(normal);
		nl.normalize();
		Eigen::Vector3d xv(xo);

		t = xv.dot(nl);

		xproj[0] = x[0] - t * nl[0];
		xproj[1] = x[1] - t * nl[1];
		xproj[2] = x[2] - t * nl[2];
	}
	
	inline double AngleBetween2Vector(const double vec1[3], const double vec2[3], bool radian)
	{
		Eigen::Vector3d v1(vec1);
		Eigen::Vector3d v2(vec2);
		auto cross = v1.cross(v2);

		if (radian)
		{
			return atan2(cross.norm(), v1.dot(v2));
		}
		else
		{
			auto DegreesFromRadians = [](double x) -> double { return x * 57.2957795131; };
			double degreeAngle = DegreesFromRadians(atan2(cross.norm(), v1.dot(v2)));
			return degreeAngle;
		}
	}
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
	MITKORTHOPEDICS_EXPORT void CupAngle(double* direction, double& ResultAnteversion, double& ResultInclination,
	                                     ECupAngleType type = ECupAngleType::RADIO_GRAPHIC);

	MITKORTHOPEDICS_EXPORT void cupAngleRadiographic(double* direction, double& ResultAnteversion, double& ResultInclination);
	MITKORTHOPEDICS_EXPORT void cupAngleOperative(double* direction, double& ResultAnteversion, double& ResultInclination);
	MITKORTHOPEDICS_EXPORT void cupAngleAnatomical(double* direction, double& ResultAnteversion, double& ResultInclination);

	
	/**
	* \brief Compute the Femoral Version Angle.
	*
	* can use either local or global points, all points are on Femur.
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
	double MITKORTHOPEDICS_EXPORT FemoralVersionAngle(ESide side, double* FHC, double* FNC, double* ME, double* LE, double* DFCA, double* PFCA);

	/**
	 * \brief Pelvic Tilt is the angle between APP and coronal plane.
	 * when the line of the anterior superior iliac spine (X-axis) is virtually corrected
	 * to be parallel to the inner lateral (horizontal) direction.
	 * The Angle between the Y axis of the pelvis and the Y axis of the world coordinate system .
	 * \param pelvicYAxis Perpendicular to the plane defined by the ASIS and midLine point and pointing backward.
	 * \return Pelvic Tilt angle
	 */
	double MITKORTHOPEDICS_EXPORT PelvicTilt(double* pelvicYAxis);


	/**
	 * \brief hip length is the distance from the lesser trochanter to the ASIS line.
	 * when the femur is mechanically aligned and the maximum femural offset is parallel to the coronal plane.
	 *
	 * MUST use global points in aligned pose.
	 * \param LT Lesser Trochanter,Left or Right
	 * \param ASIS_L Anterior Superior lliac Spine£¨Left£©
	 * \param ASIS_R Anterior Superior lliac Spine£¨Right£©
	 * \return hip length(mm)
	 */
	double MITKORTHOPEDICS_EXPORT HipLength(double* LT, double* ASIS_L, double* ASIS_R);;


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
	double MITKORTHOPEDICS_EXPORT CombinedOffset(double* PFCA, double* DFCA, double* MidLine);

	//Cal local Frame
	Eigen::Matrix4d MITKORTHOPEDICS_EXPORT CalPelvisLocalFrame(Eigen::Vector3d& ASIS_L, Eigen::Vector3d& ASIS_R, Eigen::Vector3d& MidLine);

	Eigen::Matrix4d MITKORTHOPEDICS_EXPORT CalFemurLocalFrame_canal(Eigen::Vector3d& FHC, Eigen::Vector3d& FNC, Eigen::Vector3d& DFCA, Eigen::Vector3d& PFCA, ESide side);

	Eigen::Matrix4d MITKORTHOPEDICS_EXPORT CalFemurLocalFrame_mechanical(Eigen::Vector3d& FHC, Eigen::Vector3d& FNC, Eigen::Vector3d& ME,
		Eigen::Vector3d& LE, ESide side);

	//Virtual Correction
	Eigen::Matrix4d MITKORTHOPEDICS_EXPORT CalPelvisCorrectionMatrix(Eigen::Vector3d ASIS_R, Eigen::Vector3d ASIS_L);

	Eigen::Matrix4d MITKORTHOPEDICS_EXPORT CalFemurCanalCorrectionMatrix(Eigen::Vector3d FHC, Eigen::Vector3d FNC, Eigen::Vector3d DFCA, Eigen::Vector3d PFCA, ESide side);

	Eigen::Matrix4d MITKORTHOPEDICS_EXPORT CalFemurMechanicalCorrectionMatrix(Eigen::Vector3d FHC, Eigen::Vector3d FNC, Eigen::Vector3d ME, Eigen::Vector3d LE, ESide side);
};

#endif
