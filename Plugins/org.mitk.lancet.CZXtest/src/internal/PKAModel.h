#pragma once
#ifndef PKAMODEL
#define PKAMODEL
#include <mitkRenderingManager.h>
#include "PKADataBase.h"
#include "PKAPrePlanBoneModel.h"
#include "PKAData.h"
#include "PKAProthesisModel.h"
#include "PKAEnum.h"
#define PKA_Model PKAModel::Instance()
namespace lancetAlgorithm
{
	class PKAModel : public PKADataBase
	{
	public:
		PKAModel() = default;
		~PKAModel();

		PKAModel(const PKAModel&) = delete;
		PKAModel(const PKAModel&&) = delete;
		PKAModel& operator = (const PKAModel&) = delete;

		void Delete();

		/** \brief Build the femur model with VA List,Writes parameters in a specific order.
		 *
		 * 1.femur medial epicondyle  2.femur lateral epicondyle 3.femur posterior medial 4.femur posterior lateral
		 * 5.femur medial distal point 6.femur lateral distal point 7.femur posterior medial point 8.femur posterior lateral point
		 * 9.hip center 10.femur knee center
		*/
		void BuildFemur(unsigned int n, ...);

		/** \brief Build the femur model with VA List,Writes parameters in a specific order.
		 *
		 * 1.PCL center  2.tibia knee center 3.tibia ankle center 4.Medial 1/3 of the tubercle 5.tibial proximal medial 6.tibia proximal lateral
		*/
		void BuildTibia(unsigned int n, ...);
		/** \brief Build the femur model with VA List,Writes parameters in a specific order.
		*
		* 1.anterior normal start 2. anterior normal end 3.anter.cham normal start  4.anter.chamfer normal end
		* 5.distal normal start 6.distal normal end 7.post.cham normal start 8.post.cham normal end
		* 9.posterior normal start 10.posterior normal end 11.resection line medial 12.resection line lateral
		*/
		void BuildFemurImplant(unsigned int n, ...);
		/** \brief Build the tibia model with VA List,Writes parameters in a specific order.
		*
		* 1.tibia implant proximal start 2. tibia implant proximal end 3.tibia symmetry axis start  4.tibia symmetry axis end
		*/
		void BuildTibiaImplant(unsigned int n, ...);

		void SetOperationSide(PKASurgicalSide side);

		void UpdateResultSymbol();

		void CalTransformToStandardPlane(double* p1, double* p2, double* p3, double* p4);

		void CalFemurVarus();
		void CalFemurRotation();
		void CalFemurFlexion();

		void CalTibiaVarus();
		void CalTibiaExternal();
		void CalTibiaPostSlope();

		void CalFemurDistalResectionDepth();
		void CalFemurPosteriorResectionDepth();
		void CalTibiaResectionDepth();

		void CalExtensionGap();

		void CalFlexionGap();

		void CalLimbFlexionAndVarus();

		void CalPlanned_Varus();

		void CalPrePlanning();

		void CalIntraPlanning();

		PKAPrePlanBoneModel* Femur();

		PKAPrePlanBoneModel* Tibia();

		PKAProthesisModel* FemurProsthesis();

		PKAProthesisModel* TibiaProsthesis();

		PKADataBase* PKA();

		static PKAModel& Instance();


	private:
		PKAPrePlanBoneModel* m_Femur{ nullptr };
		PKAPrePlanBoneModel* m_Tibia{ nullptr };

		PKAProthesisModel* m_FemurProsthesis{ nullptr };
		PKAProthesisModel* m_TibiaProsthesis{ nullptr };

		PKADataBase* m_PKA{ nullptr };

		PKASurgicalSide m_opSide{ PKASurgicalSide::Left };
		double m_resultSymbol{ 1.0 };
		Eigen::Vector3d CoronalNormal = Eigen::Vector3d(0, -1, 0);
		Eigen::Vector3d TransverseNormal = Eigen::Vector3d(0, 0, -1);
		Eigen::Vector3d SagittalNormal = Eigen::Vector3d(-1, 0, 0);

		//std::array<double, 9> m_transform;
		Eigen::Matrix3d m_transform;
	};
}
#endif // !PKAMODEL

