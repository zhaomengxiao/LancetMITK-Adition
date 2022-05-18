#pragma once


#include "physioModels.h"

/**
 * \brief get the instance of RegistVerifier singleton.
 *
 * remember to Call the Delete Method when the RegistVerifier is useless;
 */
#define REGIST_VERIFIER lancetAlgorithm::RegistVerifier::Instance()

namespace lancetAlgorithm
{
	class RegistVerifier
	{
	public:

		static RegistVerifier& Instance()
		{
			static RegistVerifier instance;
			return instance;
		}

		RegistVerifier(const RegistVerifier &) = delete;
		RegistVerifier(const RegistVerifier &&) = delete;
		RegistVerifier &operator=(const RegistVerifier &) = delete;

		void Delete()
		{
		}

		/**
		 * \brief 
		 * \param point 
		 */
		void AddVerifyPoint(double* point);

		void Reset();
		/**
		 * \brief use verify points to generate rays.
		 *
		 * first fit a sphere by verify points,use the center as the ray source.
		 * then generate n rays from source to each verify point.
		 * \return true if generate success.
		 */
		bool GenerateRays();
		/**
		 * \brief Check whether the probe has reached a verification point
		 * \param position prob position
		 * \param tolerance  mm
		 * \return int index of verify point, if not on any verify point return -1.
		 * 
		 */
		int IsOnVerifyPoint(double* position, double tolerance);
		
		void RecordPosition(int index, double* position);

		double GetErr(int index);
		double GetRMSD();

		LandMarkType GetRaySource()
		{
			return m_raySource;
		}

		LandMarkType GetVerifyPoint(int index)
		{
			return m_PSet_Verify[index];
		}

	protected:
		RegistVerifier(){}
	private:
		
		std::vector<LandMarkType> m_PSet_Verify;
		std::map<int, LandMarkType> m_PSet_Record;
		std::map<int, double> m_errMap;
		std::vector<AxisType> m_rays;
		LandMarkType m_raySource{};
	};
}

