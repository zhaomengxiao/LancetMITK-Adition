#ifndef reduction_h
#define reduction_h

#include <MitkOrthopedicsExports.h>
#include "body.h"

namespace othopedics
{
	class MITKORTHOPEDICS_EXPORT Reduction :public itk::Object
	{
	public:
		mitkClassMacroItkParent(Reduction, itk::Object);
		itkNewMacro(Reduction);

		itkSetMacro(Pelvis, Pelvis::Pointer);
		itkSetMacro(Femur_L, Femur::Pointer);
		itkSetMacro(Femur_R, Femur::Pointer);
		itkSetMacro(Cup, Cup::Pointer);
		itkSetMacro(Stem, Stem::Pointer);

		itkGetMacro(Pelvis, Pelvis::Pointer);
		itkGetMacro(Femur_L, Femur::Pointer);
		itkGetMacro(Femur_R, Femur::Pointer);

		virtual void SetResult(EResult name, double res);
		virtual bool GetResult(EResult name, double& outp_res);

		//move femur head center to hip center of rotation
		void FemurPreopReduction_Canal(Pelvis::Pointer pelvis, Femur::Pointer femur);

		void PreOperativeReduction_Canal();
		void PreOperativeReduction_Mechanical();
		void CalPreopOffset();
		void CalPreopHipLength();

		void PlanReduction_Canal(Eigen::Matrix4d cupPlanMatrix, Eigen::Matrix4d stemPlanMatrix);
		void PlanReduction_Mechanical(Eigen::Matrix4d cupPlanMatrix, Eigen::Matrix4d stemPlanMatrix);
		void CalPlanedOffset();
		void CalPlanedHipLength();
		ESide m_OperationSide{ESide::right};
	private:
		Pelvis::Pointer m_Pelvis;
		Femur::Pointer m_Femur_L;
		Femur::Pointer m_Femur_R;
		Cup::Pointer m_Cup;
		Stem::Pointer m_Stem;

		std::map<EResult, double> m_results{};
	};
}



#endif
