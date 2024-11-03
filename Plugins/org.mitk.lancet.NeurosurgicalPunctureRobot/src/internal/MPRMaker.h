#pragma once
#include <vtkDICOMReader.h>
#include <vtkDICOMMetaData.h>
#include <vtkImageResliceToColors.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkImageProperty.h>
#include <vtkCamera.h>
#include <vtkInformation.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtkImageActor.h>
#include <vtkScalarsToColors.h>
#include <vtkMatrix4x4.h>
#include <vtkImageResliceMapper.h>
#include <vtkTextActor.h>

class MPRMaker
{
public:
	MPRMaker() {};
	~MPRMaker() = default;

	void SetInitialWindow(const int& window) { m_InitialWindow = window; }
	void SetInitialLevel(const int& level) { m_InitialLevel = level; }
	void SetRenderWindos
	(
		const vtkSmartPointer<vtkRenderWindow>& sagittalRenderWindow,
		const vtkSmartPointer<vtkRenderWindow>& coronalRenderWindow,
		const vtkSmartPointer<vtkRenderWindow>& axialWindow
	);

	[[nodiscard]] int GetInitialWindow() const { return m_InitialWindow; }
	[[nodiscard]] int GetInitialLevel() const { return m_InitialLevel; }
	[[nodiscard]] vtkImageResliceToColors* GetImageReslice(const int plane) const { return m_ReslicerToColor[plane]; }
	[[nodiscard]] vtkImageReslice* GetOriginValueImageReslice(int plane);
	[[nodiscard]] vtkImageData* GetInputData() const { return m_Reader->GetOutput(); }
	[[nodiscard]] double GetCenterSliceZPosition(int plane) const;
	[[nodiscard]] vtkSmartPointer<vtkScalarsToColors> GetColorMapScalar() const { return m_ColorMap; }

	void Create3DMatrix();
	void CreateMPR(vtkSmartPointer<vtkDICOMReader> reader);
	void ResetMatrixesToInitialPosition();
	void ResetWindowLevel();

private:
	int m_InitialWindow = 0;
	int m_InitialLevel = 0;
	vtkSmartPointer<vtkDICOMReader> m_Reader = {};
	vtkSmartPointer<vtkImageResliceToColors> m_ReslicerToColor[3] = {};
	vtkSmartPointer<vtkImageReslice> m_OriginalValuesReslicer[3] = {};
	vtkSmartPointer<vtkRenderWindow> m_RenderWindow[3] = {};
	vtkSmartPointer<vtkTextActor> m_TextActor[3] = {};
	vtkSmartPointer<vtkScalarsToColors> m_ColorMap = {};

	double m_SagittalMatrix[16] =
	{
		0,0,1,0,
		-1,0,0,0,
		0,-1,0,0,
		0,0,0,1
	};

	double m_CoronalMatrix[16] =
	{
		1,0,0,0,
		0,0,1,0,
		0,-1,0,0,
		0,0,0,1
	};

	double m_AxialMatrix[16] =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	void Initialize();

	void SetInitialMatrix();

	void CreateMPRViews();

	void SetMiddleSlice(int aPlane, const vtkSmartPointer<vtkDICOMReader>& aReader);

	void RenderPlaneOffScreen(int aPlane);
};

