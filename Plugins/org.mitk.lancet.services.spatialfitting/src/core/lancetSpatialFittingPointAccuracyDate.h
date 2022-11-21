/**
 * \par Copyright (C)2022-2023 Hangzhou Lancet Robot Co., Ltd. All rights reserved.
 *
 * \brief This is the declaration header file of space point calculation.
 * 
 * \ingroup org_mitk_lancet_services_spatialfitting
 * \version V1.0.0
 * \date 2022-11-18 10:18:52
 * 
 * \par Modify History
 *		-# Sun initialization the version for 2022-11-18 10:18:52.
 *
 * \author Sun
 * \remark Non
 */
#ifndef LancetSpatialFittingPointAccuracyDate_H
#define LancetSpatialFittingPointAccuracyDate_H

#include "lancetSpatialFittingGlobal.h"

// mitk
#include <mitkPoint.h>

BEGIN_SPATIAL_FITTING_NAMESPACE

/**
 * \ingroup org_mitk_lancet_services_spatialfitting
 *
 * \brief Represent or calculate the relationship and data between points.     
 *
 * The 3D point data has been clearly defined in mitk. The point calculation 
 * here is based on the extension calculation or processing of the mitk::Point3D 
 * type.
 * 
 * <b>Tips:</b><br>
 * In terms of actual use, this function class should not exist. Its significa-
 * nce lies in constraining the calculation or operation methods of external 
 * point-to-point.
 *
 * <b>Example:</b><br>
 * \code
 * std::array<double, 3> srcPoint = { 0.0,1.0,2.0 };
 * std::array<double, 3> targetPoint = { 0.0,1.0,2.0 };
 * PointAccuracyDate pointAccuracyDate;
 * // PointAccuracyDate pointAccuracyDate(srcPoint.data(), targetPoint.data());
 * pointAccuracyDate.SetSourcePoint(srcPoint.data());
 * pointAccuracyDate.SetTargetPoint(targetPoint.data());
 * pointAccuracyDate.Compute(); // return 0.0
 * \endcode
 *
 * \author Sun
 * \date 2022-11-18 11:06:46
 * \remark Non
 *
 * Contact: sh4a01@163.com
 *
 */
class ORG_MITK_LANCET_SERVICES_SPATIALFITTING_PLUGIN
	PointAccuracyDate
{
public:
	PointAccuracyDate();

	/**
	 * \brief Parametric construction method of point processing.
	 *
	 * \param src
	 *              3D origin data object.
	 * \param target
	 *              3D target point data object.
	 */
	PointAccuracyDate(const mitk::Point3D& src, const mitk::Point3D& target);

	virtual mitk::Point3D GetSourcePoint() const;
	virtual void SetSourcePoint(const mitk::Point3D&);

	virtual mitk::Point3D GetTargetPoint() const;
	virtual void SetTargetPoint(const mitk::Point3D&);

	/**
	 * \brief Calculate the distance from the source point to the target point.
	 *
	 * Because no similar method is provided in mitk::Point3D, it has the meaning 
	 * of existence.
	 *
	 * \return Returns the linear distance between 3D space point and 3D space.
	 */
	virtual double Compute() const;
private:
	struct PointAccuracyDatePrivateImp;
	std::shared_ptr<PointAccuracyDatePrivateImp> imp;
};

END_SPATIAL_FITTING_NAMESPACE

#endif // !LancetSpatialFittingPointAccuracyDate_H
