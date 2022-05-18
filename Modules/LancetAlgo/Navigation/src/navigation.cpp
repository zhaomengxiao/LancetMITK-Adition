#include "navigation.h"

#include <iostream>

#include "basic.h"
#include "leastsquaresfit.h"

namespace lancetAlgorithm
{
	void RegistVerifier::AddVerifyPoint(double* point)
	{
		m_PSet_Verify.push_back(LandMarkType{ point[0],point[1] ,point[2] });
	}

	void RegistVerifier::Reset()
	{
		m_rays.clear();
		m_errMap.clear();
		m_PSet_Record.clear();
		m_PSet_Verify.clear();
	}

	int RegistVerifier::IsOnVerifyPoint(double* position, double tolerance)
	{	
		int index = 0;
		for (auto ray : m_rays)
		{
			if (IsPointOnRay(position, ray.startPoint.data(), ray.direction.data(), tolerance))
			{
				return index;
			}
			index++;
		}
		return -1;
	}

	void RegistVerifier::RecordPosition(int index, double* position)
	{
		auto vPoint = m_PSet_Verify[index];
		double err = pow(vPoint[0] - position[0], 2) + pow(vPoint[1] - position[1], 2) + pow(vPoint[2] - position[2], 2);
		m_PSet_Record.insert_or_assign(index, LandMarkType{ position[0],position[1] ,position[2] });
		m_errMap.insert_or_assign(index, err);
	}

	double RegistVerifier::GetErr(int index)
	{
		auto iter = m_errMap.find(index);
		if (iter == m_errMap.end())
		{
			return -1;
		}
		return iter->second;
	}

	double RegistVerifier::GetRMSD()
	{
		double err = 0.0;
		for (int i =0;i < m_errMap.size();i++)
		{
			err += GetErr(i);
		}
		err = sqrt(err / double(m_errMap.size()));
		return err;
	}

	bool RegistVerifier::GenerateRays()
	{
		double r{ 0 };

		if (fit_sphere(m_PSet_Verify, m_raySource, r))
		{
			
			for (auto verifyPoint : m_PSet_Verify)
			{
				m_rays.emplace_back(AxisType{ m_raySource, verifyPoint,AxisType::EConstractType::TWO_POINT });
			}
			
			return true;
		}
		return false;
	}
}
