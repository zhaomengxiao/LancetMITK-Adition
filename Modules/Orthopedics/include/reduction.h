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
		itkSetMacro(Femur_op, Femur::Pointer);
		itkSetMacro(Femur_contra, Femur::Pointer);
		itkSetMacro(Cup, Cup::Pointer);
		itkSetMacro(Stem, Stem::Pointer);
		itkSetMacro(Liner, Body::Pointer);
		itkSetMacro(Head, Body::Pointer);

		itkGetMacro(Pelvis, Pelvis::Pointer);
		itkGetMacro(Femur_op, Femur::Pointer);
		itkGetMacro(Femur_contra, Femur::Pointer);

		virtual void SetResult(EResult name, double res);
		virtual bool GetResult(EResult name, double& outp_res);

		//Femur Canal align and move femur head center to hip center of rotation
		void FemurPreopReduction_Canal(Pelvis::Pointer pelvis, Femur::Pointer femur);
		void FemurPreopReduction_Mechanical(Pelvis::Pointer pelvis, Femur::Pointer femur);

		//move stem HeadAssemblyPoint to cup center
		Eigen::Matrix4d GetStemReductionTransform(Cup::Pointer cup, Stem::Pointer stem, ELandMarks HeadAssemblyPoint);
		void StemReduction(Cup::Pointer cup, Stem::Pointer stem, ELandMarks HeadAssemblyPoint);

		void HeadReduction(Body::Pointer head, Stem::Pointer stem, ELandMarks HeadAssemblyPoint);

		bool GetCombinedOffset(Femur::Pointer femur, Pelvis::Pointer pelvis, double& offset);
		bool GetHipLength(Femur::Pointer femur, Pelvis::Pointer pelvis, double& hipLength);

		//pre
		void PreOperativeReduction_Canal();
		void PreOperativeReduction_Mechanical();
		void CalPreopOffset();
		void CalPreopHipLength();

		//plan
		void PlanReduction_Canal(Eigen::Matrix4d cupPlanMatrix, Eigen::Matrix4d stemPlanMatrix);
		void PlanReduction_Mechanical(Eigen::Matrix4d cupPlanMatrix, Eigen::Matrix4d stemPlanMatrix);
		void CalPlanedOffset();
		void CalPlanedHipLength();

	private:
		Pelvis::Pointer m_Pelvis;
		Femur::Pointer m_Femur_op;
		Femur::Pointer m_Femur_contra;
		Cup::Pointer m_Cup;
		Stem::Pointer m_Stem;
		Body::Pointer m_Liner;
		Body::Pointer m_Head;

		std::map<EResult, double> m_results{};
	};
}



#endif
