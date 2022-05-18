#ifndef PHYSIOMODELFACTORY_H
#define PHYSIOMODELFACTORY_H

#include "physioModels.h"

/**
 * \brief get the instance of THA_Model singleton.
 * 
 * remember to Call the Delete Method when the model is useless;
 */
#define THA_MODEL lancetAlgorithm::THA_Model::Instance()
#define TKA_MODEL lancetAlgorithm::TKA_Model::Instance()
namespace lancetAlgorithm
{
	/*class PhysioModel
	{
	public:
		virtual ~PhysioModel() = default;
		virtual void BuildPelvis(double* data, ...);
		virtual void BuildFemur(double* data, ...);

		static PhysioModel* GetInstance()
		{
			return _instance;
		}

	protected:
		PhysioModel();

	private:
		static PhysioModel* _instance;
	};*/

	
	/**
	 * \brief A Singleton Builder for Total Hip Arthroplasty.
	 *
	 * This class provides the user with an interface
	 * between the hip model and the left and right femur model
	 * to create an entire clinical model based on a few feature points,
	 * and to obtain the required calculation results
	 */
	class THA_Model:public DataBase
	{
	public:

		~THA_Model();

		THA_Model(const THA_Model &) = delete;
		THA_Model(const THA_Model &&) = delete;
		THA_Model &operator=(const THA_Model &) = delete;

		void Delete();
		/** \brief Build the pelvis model with VA List,Writes parameters in a specific order.
		 *
		 * 1.RASI  2.LASI 3.PT
		*/
		void BuildPelvis(unsigned int n, double* data, ...) ;
		/** \brief Build the Femur model with VA List,Writes parameters in a specific order.
		 *
		 * 1.DFCA  2.PFCA 3.FHC 4.LT
		*/
		void BuildFemur(ESide side, ...) ;

		void SetFHCinOp(double* data);

		void SetPreCheckPoints(double* distal, double* proximal);

		void SetPostCheckPoints(double* distal, double* proximal);

		PelvisModel* Pelvis();

		femurModel* Femur() const;

		femurModel_OpSide* Femur_opSide() const;

		ESide GetOprationSide() const;

		void SetOprationSide(ESide opside);

		double CalHipLength(ESide side);
		double CalCombineOffset(ESide side);

		std::array<double, 2> CalHipLenAndOffset(ESide side);
		std::array<double, 4> CalHipLenAndOffsetBothSides();
		bool CalHipLengthAndOffsetPostOp();
		bool CalOffsetPostOp();
		bool CalHipLengthPostOp();
		bool CalOffsetDiff_preOp2Contral();
		bool CalHipLengthDiff_preOp2Contral();
		bool CalOffsetDiff_Op2Contralateral();
		bool CalHipLengthDiff_Op2Contralateral();

		static THA_Model& Instance();
	protected:
		THA_Model();
	private:
		PelvisModel* m_pelvis{nullptr};
		femurModel* m_femur{nullptr};
		femurModel_OpSide* m_femur_op{nullptr};
		ESide e_opSide{ESide::right};
	};

	class TKA_Model :public TKADataBase
	{
	public:

		TKA_Model() = default;
		~TKA_Model();

		TKA_Model(const TKA_Model &) = delete;
		TKA_Model(const TKA_Model&&) = delete;
		TKA_Model &operator=(const TKA_Model&) = delete;

		void Delete();

		/** \brief Build the femur model with VA List,Writes parameters in a specific order.
		 *
		 * 1.femur medial epicondyle  2.femur lateral epicondyle 3.femur posterior medial 4.femur posterior lateral
		 * 5.femur medial distal point 6.femur lateral distal point 7.femur posterior medial point 8.femur posterior lateral point
		 * 9.hip center 10.femur knee center
		*/
		void buildFemur(unsigned int n, ...);

		/** \brief Build the femur model with VA List,Writes parameters in a specific order.
		 *
		 * 1.PCL center  2.tibia knee center 3.tibia ankle center 4.Medial 1/3 of the tubercle 5.tibial proximal medial 6.tibia proximal lateral
		*/
		void buildTibia(unsigned int n, ...);
		/** \brief Build the femur model with VA List,Writes parameters in a specific order.
		*
		* 1.anterior normal start 2. anterior normal end 3.anter.cham normal start  4.anter.chamfer normal end
		* 5.distal normal start 6.distal normal end 7.post.cham normal start 8.post.cham normal end
		* 9.posterior normal start 10.posterior normal end 11.resection line medial 12.resection line lateral
		*/
		void buildFemurImplant(unsigned int n, ...);
		/** \brief Build the tibia model with VA List,Writes parameters in a specific order.
		*
		* 1.tibia implant proximal start 2. tibia implant proximal end 3.tibia symmetry axis start  4.tibia symmetry axis end
		*/
		void buildTibiaImplant(unsigned int n, ...);

		void SetOperationSide(TKASide side);

		void UpdateResultSymbol();

		void CalTransformToStandardPlane(double *p1, double *p2, double *p3, double *p4);

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

		TKAFemurModel *femur();

		TKATibiaModel *tibia();

		TKAFemurImplantModel *femurimplant();

		TKATibiaImplantModel *tibiaimplant();

		TKADataBase *TKA();

		static TKA_Model &Instance();


	private:
		TKAFemurModel *m_femur{ nullptr };
		TKATibiaModel *m_tibia{ nullptr };

		TKAFemurImplantModel *m_femurimplant{ nullptr };
		TKATibiaImplantModel *m_tibiaimplant{ nullptr };

		TKADataBase *m_TKA{ nullptr };

		TKASide m_opSide{ TKASide::left };
		double m_resultSymbol{ 1.0 };
		double CoronalNormal[3] = { 0,-1,0 };
		double TransverseNormal[3] = { 0,0,-1 };
		double SagittalNormal[3] = { -1,0,0 };

		std::array<double,9> m_transform;
	};
}

#endif
