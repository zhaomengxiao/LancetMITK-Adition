#ifndef PHYSIOMODELS_H
#define PHYSIOMODELS_H
#include <array>
#include <vector>
#include <map>
#include "physioConst.h"
#include <memory>

using LandMarkType = std::array<double, 3>;
using VectorType = std::array<double, 3>;
using LandMarkPair = std::pair<ELandMarks, LandMarkType>;

namespace lancetAlgorithm
{
	class AxisType
	{
	public:
		enum class EConstractType
		{
			TWO_POINT,
			POINT_DIRECTION
		};

		AxisType() = default;

		AxisType(const std::array<double, 3>& data1, const std::array<double, 3>& data2, EConstractType type)
		{
			if (EConstractType::TWO_POINT == type)
			{
				startPoint = data1;
				direction = std::array<double, 3>{data2[0] - data1[0], data2[1] - data1[1], data2[2] - data1[2]};
			}
			else
			{
				startPoint = data1;
				direction = data2;
			}
		}

		LandMarkType startPoint{};
		VectorType direction{};
	};

	class PlaneType
	{
	public:
		double size{1};
		AxisType normal;
	};

	class SegmentType
	{
	public:

		LandMarkType point_1;
		LandMarkType point_2;
	};

	enum ECorrection
	{
		ORIGIN,
		MECHANICS,
		CANAL,
		CHECKPOINT_PRE,
		CHECKPOINT_POST
	};


	enum EIVAngelType
	{
		RADIO_GRAPHIC,//常用，影像学定义
		OPERATIVE, //机械轴的定义
		ANATOMICAL //解剖学的定义
	};

	/**
	 * \brief base class to access data for all physio model.
	 *
	 * DataBase use map to store the data.
	 * 
	 */
	class DataBase
	{
	public:
		virtual ~DataBase() = default;

		DataBase() = default;
		DataBase(const DataBase& other) = default;

		DataBase(DataBase&& other) noexcept;

		DataBase& operator=(const DataBase& other);

		DataBase& operator=(DataBase&& other) noexcept;

		/**
		 * \brief Sets the position of a particular landmark in three-dimensional space.
		 *
		 * if a landmark has been set before, this method will overwrite the value.
		 * 
		 * \param name key, listed in Enum ELandMarks.
		 * \param data value, The position of a point in three dimensions.
		 * \see physioConst.h
		 */
		virtual void SetLandMark(ELandMarks name, double* data);
		
		/**
		 * \brief Get the position of a landmark by name.
		 * \param name [user input]
		 * \param oup_value [output]
		 * \return true if success.
		 */
		virtual bool GetLandMark(ELandMarks name, LandMarkType& oup_value);

		virtual void SetAxis(EAxes name, double* p_start, double* direction);
		
		virtual bool GetAxis(EAxes name, AxisType& outp_axis);

		virtual void SetPlane(EPlanes name, double* center, double* normal);
		virtual bool GetPlane(EPlanes name,PlaneType& outp_plane);

		virtual void SetResult(EResult name, double res);
		virtual bool GetResult(EResult name, double& outp_res);
	protected:

		std::map<ELandMarks, LandMarkType> m_map_landMark{};
		std::map<EAxes, AxisType> m_map_axis{};
		std::map<EPlanes, PlaneType> m_map_plane{};
		std::map<EResult, double> m_map_result{};
	};


	/** \brief Base class for storing physiological and anatomical landmarks and data about the pelvis.
		*/
	class PelvisModel : public DataBase
	{
	public:
		typedef std::unique_ptr<PelvisModel> Ptr;
		virtual ~PelvisModel() = default;

		void Update();
		void AddAcetabulumPoint_Edge(ESide side, double* data);
		void AddAcetabulumPoint_Surface(ESide side, double* data);

		bool UpdateAcetabularCenter(ESide side);
		bool UpdateAcetabularAxis(ESide side);
		bool updateAIAngle(ESide side, EIVAngelType type = RADIO_GRAPHIC);

	private:
		void updatePHA();
		void updatePSA();
		void updatePLA();
		void updateMidPlane();

		std::vector<double> m_PSet_RAC{};
		std::vector<double> m_PSet_LAC{};

		std::vector<double> m_PSet_RAA{};
		std::vector<double> m_PSet_LAA{};
	};

	/** \brief Base class for storing physiological and anatomical landmarks and data about the femur.
		*/
	class femurModel : public DataBase
	{
	public:
		explicit femurModel(ESide _side)
			: side(_side)
		{
		}

		
		
		ESide side;

		void Update();

		LandMarkType CorrectMechanAxis(LandMarkType inp);
		LandMarkType CorrectCanalAxis(LandMarkType inp);

		LandMarkType GetLT(ECorrection correction);
		LandMarkType GetPFCA(ECorrection correction);

		/** \brief [ModelOutput] The 4x4 transform matrix that rotate the femur model,make canal or mechanic axis Vertical alignment.
		 *
		 *[Notice] use the output array carefully,the data is RowMajor format.
		 *
		 *[vtk usage example]
		 *@verbatim
		 *vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
		 *transform->Concatenate([return].data());
		 *transform->Update();
		 *@endverbatim
		 */
		virtual std::array<double, 16> GetMatrix(ECorrection correction);

	protected:
		std::array<double, 16> calMechanCorrection(LandMarkType FHC, LandMarkType DFCA, LandMarkType PFCA);
		std::array<double, 16> calCanalCorrection(LandMarkType FHC, LandMarkType DFCA, LandMarkType PFCA);
		std::array<double, 16> m_Matrix_canal{};
		std::array<double, 16> m_Matrix_mechan{};
	};

	class femurModel_OpSide final : public femurModel
	{
	public:

		explicit femurModel_OpSide(const ESide _side)
			: femurModel(_side)
		{
		}

		bool Update_inOp();

		std::array<double, 16>GetMatrix(ECorrection correction) override;
		std::array<double, 16> GetCheckPCorrectMarix_pre() const;

		std::array<double, 16> GetCheckPCorrectMarix_post() const;

		void SetEpicondylarPoints(double* Medial, double* Lateral);
		double CalFemoralVersion();
	private:
		void handleError(const char* what);
		/** \brief store the canal correction matrix of the pre-Operation CheckPoints
		 */
		std::array<double, 16> m_Matrix_checkPoints_pre{};
		/** \brief store the canal correction matrix of the post-Operation CheckPoints
		 */
		std::array<double, 16> m_Matrix_checkPoints_post{};
	};

	/**
	 * compute the Anteversion angle and the Inclination Angle
	 *
	 * @param direction [Input]The direction of the Acetabular grinding rod.
	 * @param ResultAnteversion [Output] The compute result of Anteversion.
	 * @param ResultInclination [Output] The compute result of Inclination.
	 * @param type [Input] different type of Anteversion/Inclination angle, default:Radiographic
	 */
	void AnteversionAndInclinationAngle(double* direction, double& ResultAnteversion, double& ResultInclination,
	                                    EIVAngelType type = EIVAngelType::RADIO_GRAPHIC);
	void AIAngleRadiographic(double* direction, double& ResultAnteversion, double& ResultInclination);
	void AIAngleOperative(double* direction, double& ResultAnteversion, double& ResultInclination);
	void AIAngleAnatomical(double* direction, double& ResultAnteversion, double& ResultInclination);

		/**
	* compute the Femoral Version Angle
	*
	* @param MFC [Input] Medial Femoral Epicondyle.
	* @param LFC [Input] Lateral Femoral Epicondyle.
	* @param GT [Input] Greater Trochanter.
	* @param FHC [Input] Femur Head Center.
	* @param DFCA [Input] Distal point of Femoral canal axis.
	* @param PFCA [Input] Proximal point of Femoral canal axis.
	* @return  Femoral Version Angle
	*/
	double FemoralVersionAngle(double* MFC, double* LFC, double* GT, double* FHC, double* DFCA, double* PFCA);

	class TKADataBase
	{
	public:
		virtual ~TKADataBase() = default;

		TKADataBase() = default;
		TKADataBase(const TKADataBase& other) = default;

		TKADataBase(TKADataBase&& other) noexcept;

		TKADataBase& operator=(const TKADataBase& other);

		TKADataBase& operator=(TKADataBase&& other) noexcept;

		/**
		 * \brief Sets the position of a particular landmark in three-dimensional space.
		 *
		 * if a landmark has been set before, this method will overwrite the value.
		 *
		 * \param name key, listed in Enum ELandMarks.
		 * \param data value, The position of a point in three dimensions.
		 * \see physioConst.h
		 */
		virtual void SetLandMark(TKALandmarks name, double* data);

		/**
		 * \brief Get the position of a landmark by name.
		 * \param name [user input]
		 * \param oup_value [output]
		 * \return true if success.
		 */
		virtual bool GetLandMark(TKALandmarks name, LandMarkType& oup_value);

		virtual void SetAxis(TKAAxes name, double* p_start, double* direction);

		virtual bool GetAxis(TKAAxes name, AxisType& outp_axis);

		virtual void SetPlane(TKAPlanes name, double* center, double* normal);
		virtual bool GetPlane(TKAPlanes name, PlaneType& outp_plane);

		virtual void SetResult(TKAResult name, double res);
		virtual bool GetResult(TKAResult name, double& outp_res);
	protected:

		std::map<TKALandmarks, LandMarkType> m_map_landMark{};
		std::map<TKAAxes, AxisType> m_map_axis{};
		std::map<TKAPlanes, PlaneType> m_map_plane{};
		std::map<TKAResult, double> m_map_result{};
	};

	class TKAFemurModel :public TKADataBase
	{
	public:
		void update();
	private:
		void updateMechanicalAxis();
		void updatePCA();
		void updateTEA();
		void updateFemurCoordinate();
	};

	class TKATibiaModel :public TKADataBase
	{
	public:
		void update();
	private:
		void updateMechanicalAxis();
		void updateAPAixs();
		void updateTibiaCoordinate();
	};

	class TKAFemurImplantModel :public TKADataBase
	{
	public:
		void update();
	private:
		void updateResectionLine();
		void updateAnteriorCut();
		void updateAnteriorChamCut();
		void updateDistalCut();
		void updatePosteriorChamCut();
		void updatePosteriorCut();
	};

	class TKATibiaImplantModel :public TKADataBase
	{
	public:
		void update();
	private:
		void updateAxis();
		void updatePlaneSymmetryAxis();
	};
}

#endif