#include "PKADataBase.h"

PKADataBase::PKADataBase(PKADataBase&& other) noexcept : m_map_landMark(std::move(other.m_map_landMark)),
m_map_axis(std::move(other.m_map_axis)),
m_map_plane(std::move(other.m_map_plane)),
m_map_result(std::move(other.m_map_result))
{
}

PKADataBase& PKADataBase::operator=(const PKADataBase& other)
{
	if (this == &other)
		return *this;
	m_map_landMark = other.m_map_landMark;
	m_map_axis = other.m_map_axis;
	m_map_plane = other.m_map_plane;
	m_map_result = other.m_map_result;
	return *this;
}

PKADataBase& PKADataBase::operator=(PKADataBase&& other) noexcept
{
	if (this == &other)
		return *this;
	m_map_landMark = std::move(other.m_map_landMark);
	m_map_axis = std::move(other.m_map_axis);
	m_map_plane = std::move(other.m_map_plane);
	m_map_result = std::move(other.m_map_result);
	return *this;
}

void PKADataBase::SetLandMark(PKALandmarks name, double* data)
{
	try
	{
		Eigen::Vector3d d(data);
		m_map_landMark.insert_or_assign(name, d);
	}
	catch (...)
	{
		std::cout << "err" << std::endl;
	}
}

bool PKADataBase::GetLandMark(PKALandmarks name, Eigen::Vector3d& oup_value)
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

void PKADataBase::SetAxis(PKAAxes name, double* p_start, double* direction)
{
	AxisType axis{};
	memcpy(axis.startPoint._Elems, p_start, 3 * sizeof(double));
	memcpy(axis.direction._Elems, direction, 3 * sizeof(double));

	m_map_axis.insert_or_assign(name, axis);
}

bool PKADataBase::GetAxis(PKAAxes name, AxisType& outp_axis)
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

void PKADataBase::SetPlane(PKAPlanes name, double* center, double* normal)
{
	PlaneType plane{};
	memcpy(plane.normal.startPoint._Elems, center, 3 * sizeof(double));
	memcpy(plane.normal.direction._Elems, normal, 3 * sizeof(double));

	m_map_plane.insert_or_assign(name, plane);
}

bool PKADataBase::GetPlane(PKAPlanes name, PlaneType& outp_plane)
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

void PKADataBase::SetResult(PKAResult name, double res)
{
	m_map_result.insert_or_assign(name, res);
}

bool PKADataBase::GetResult(PKAResult name, double& outp_res)
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


