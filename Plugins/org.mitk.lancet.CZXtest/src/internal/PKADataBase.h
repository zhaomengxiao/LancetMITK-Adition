#pragma once
#ifndef PKADATABASE
#define PKADATABASE

#include "PKAData.h"
#include <physioModels.h>
#include <map>

//to import define LandMarkType and AxisType
using namespace lancetAlgorithm;

class PKADataBase
{
public:
	virtual ~PKADataBase() = default;

	PKADataBase() = default;
	PKADataBase(const PKADataBase& other) = default;

	PKADataBase(PKADataBase&& other) noexcept;

	PKADataBase& operator=(const PKADataBase& other);

	PKADataBase& operator=(PKADataBase&& other) noexcept;

	/**
	 * \brief Sets the position of a particular landmark in three-dimensional space.
	 *
	 * if a landmark has been set before, this method will overwrite the value.
	 *
	 * \param name key, listed in Enum ELandMarks.
	 * \param data value, The position of a point in three dimensions.
	 * \see physioConst.h
	 */
	virtual void SetLandMark(PKALandmarks name, double* data);

	/**
	 * \brief Get the position of a landmark by name.
	 * \param name [user input]
	 * \param oup_value [output]
	 * \return true if success.
	 */
	virtual bool GetLandMark(PKALandmarks name, Eigen::Vector3d& oup_value);

	virtual void SetAxis(PKAAxes name, double* p_start, double* direction);

	virtual bool GetAxis(PKAAxes name, AxisType & outp_axis);

	virtual void SetPlane(PKAPlanes name, double* center, double* normal);
	virtual bool GetPlane(PKAPlanes name, PlaneType & outp_plane);

	virtual void SetResult(PKAResult name, double res);
	virtual bool GetResult(PKAResult name, double& outp_res);
protected:

	std::map<PKALandmarks, Eigen::Vector3d> m_map_landMark{};
	std::map<PKAAxes, AxisType> m_map_axis{};
	std::map<PKAPlanes, PlaneType> m_map_plane{};
	std::map<PKAResult, double> m_map_result{};
};
#endif // !PKADATABASE

