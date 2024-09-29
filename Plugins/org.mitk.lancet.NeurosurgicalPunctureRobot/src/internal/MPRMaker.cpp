#include "MPRMaker.h"
#include <vtkDICOMImageReader.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkImageProperty.h>
#include <vtkCamera.h>
#include <vtkInformation.h>
#include <vtkImageActor.h>
#include <vtkScalarsToColors.h>
#include <vtkMatrix4x4.h>
#include <vtkImageResliceMapper.h>
#include <vtkTextProperty.h>

void MPRMaker::Initialize()
{
	for (int i = 0; i < 3; ++i)
	{
		m_ReslicerToColor[i] = vtkSmartPointer<vtkImageResliceToColors>::New();
		m_OriginalValuesReslicer[i] = vtkSmartPointer<vtkImageReslice>::New();
		m_TextActor[i] = vtkSmartPointer<vtkTextActor>::New();
	}
	SetInitialMatrix();
}

void MPRMaker::SetInitialMatrix()
{
	vtkSmartPointer<vtkMatrix4x4> sagittalMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> coronalMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkMatrix4x4> axialMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
	sagittalMatrix->DeepCopy(m_SagittalMatrix);
	coronalMatrix->DeepCopy(m_CoronalMatrix);
	axialMatrix->DeepCopy(m_AxialMatrix);
	
	m_ReslicerToColor[0]->SetResliceAxes(sagittalMatrix);
	m_ReslicerToColor[1]->SetResliceAxes(coronalMatrix);
	m_ReslicerToColor[2]->SetResliceAxes(axialMatrix);
}

void MPRMaker::CreateMPRViews()
{
	for (int i = 0; i < 3; ++i)
	{
		RenderPlaneOffScreen(i);
	}
}

void MPRMaker::SetMiddleSlice(int aPlane, const vtkSmartPointer<vtkDICOMReader>& aReader)
{
	double spacing[3];
	double origin[3];
	double center[3];
	auto* const extent = m_Reader->GetOutput()->GetExtent();
	aReader->GetOutputInformation(0)->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), extent);
	aReader->GetOutput()->GetSpacing(spacing);
	aReader->GetOutput()->GetOrigin(origin);

	center[0] = origin[0] + spacing[0] * 0.5 * (extent[0] + extent[1]);
	center[1] = origin[1] + spacing[1] * 0.5 * (extent[2] + extent[3]);
	center[2] = origin[2] + spacing[2] * 0.5 * (extent[4] + extent[5]);
	m_ReslicerToColor[aPlane]->GetResliceAxes()->SetElement(0, 3, center[0]);
	m_ReslicerToColor[aPlane]->GetResliceAxes()->SetElement(1, 3, center[1]);
	m_ReslicerToColor[aPlane]->GetResliceAxes()->SetElement(2, 3, center[2]);
}

void MPRMaker::RenderPlaneOffScreen(int aPlane)
{
	const double level = m_InitialWindow == 0 ?
		m_Reader->GetMetaData()->Get(DC::WindowCenter).AsInt() : m_InitialWindow;
	const double window = m_InitialLevel == 0 ?
		m_Reader->GetMetaData()->Get(DC::WindowWidth).AsInt() : m_InitialLevel;
	SetMiddleSlice(aPlane, m_Reader);
	if (!m_ColorMap)
	{
		m_ColorMap = vtkSmartPointer<vtkScalarsToColors>::New();
		m_ColorMap->SetRange(level - 0.5 * window, level + 0.5 * window);
	}
	m_ReslicerToColor[aPlane]->SetInputData(m_Reader->GetOutput());
	m_ReslicerToColor[aPlane]->BypassOff();
	m_ReslicerToColor[aPlane]->SetInformationInput(m_Reader->GetOutput());
	m_ReslicerToColor[aPlane]->SetOutputDimensionality(2);
	m_ReslicerToColor[aPlane]->SetSlabNumberOfSlices(0);
	m_ReslicerToColor[aPlane]->SetLookupTable(m_ColorMap);
	m_ReslicerToColor[aPlane]->SetOutputFormatToRGB();
	m_ReslicerToColor[aPlane]->Update();
	m_OriginalValuesReslicer[aPlane]->SetInputData(m_Reader->GetOutput());
	m_OriginalValuesReslicer[aPlane]->SetInformationInput(m_Reader->GetOutput());
	m_OriginalValuesReslicer[aPlane]->SetOutputDimensionality(2);
	m_OriginalValuesReslicer[aPlane]->SetResliceAxes(m_ReslicerToColor[aPlane]->GetResliceAxes());
	m_OriginalValuesReslicer[aPlane]->Update();

	switch (aPlane)
	{
	case 0:
		m_TextActor[aPlane]->SetInput("Sagittal");
		m_TextActor[aPlane]->GetTextProperty()->SetColor(0, 1, 0);
		break;
	case 1:
		m_TextActor[aPlane]->SetInput("Coronal");
		m_TextActor[aPlane]->GetTextProperty()->SetColor(0, 0, 1);
		break;
	case 2:
		m_TextActor[aPlane]->SetInput("Axial");
		m_TextActor[aPlane]->GetTextProperty()->SetColor(1, 0, 0);
		break;
	default:
		break;
	}

	m_TextActor[aPlane]->GetTextProperty()->SetFontSize(20);
	vtkSmartPointer<vtkImageSlice> slice = vtkSmartPointer<vtkImageSlice>::New();
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	vtkSmartPointer <vtkImageResliceMapper> imageResliceMapper = vtkSmartPointer<vtkImageResliceMapper>::New();
	imageResliceMapper->SeparateWindowLevelOperationOff();
	imageResliceMapper->SetInputConnection(m_ReslicerToColor[aPlane]->GetOutputPort());
	slice->GetProperty()->SetInterpolationTypeToCubic();
	slice->SetMapper(imageResliceMapper);

	renderer->AddViewProp(slice);
	renderer->AddActor(m_TextActor[aPlane]);
	renderer->SetBackground(0, 0, 0);
	renderer->GetActiveCamera()->SetParallelProjection(1);
	renderer->ResetCamera();
	m_RenderWindow[aPlane]->AddRenderer(renderer);
	m_RenderWindow[aPlane]->Render();
}

void MPRMaker::SetRenderWindos(const vtkSmartPointer<vtkRenderWindow>& sagittalRenderWindow,
	const vtkSmartPointer<vtkRenderWindow>& coronalRenderWindow,
	const vtkSmartPointer<vtkRenderWindow>& axialRenderWindow)
{
	m_RenderWindow[0] = sagittalRenderWindow;
	m_RenderWindow[1] = coronalRenderWindow;
	m_RenderWindow[2] = axialRenderWindow;
}

vtkImageReslice* MPRMaker::GetOriginValueImageReslice(int plane)
{
	m_OriginalValuesReslicer[plane]->SetResliceAxes(m_ReslicerToColor[plane]->GetResliceAxes());
	return m_OriginalValuesReslicer[plane];
}

double MPRMaker::GetCenterSliceZPosition(int plane) const
{
	int* const extent = m_Reader->GetOutput()->GetExtent();
	return 0.5 * (extent[plane * 2] + extent[plane * 2 + 1]);
}

void MPRMaker::Create3DMatrix()
{
}

void MPRMaker::CreateMPR(vtkSmartPointer<vtkDICOMReader> reader)
{
	this->m_Reader = reader;
	CreateMPRViews();
}

void MPRMaker::ResetMatrixesToInitialPosition()
{
	SetInitialMatrix();
	for (auto i = 0; i < 3; ++i)
	{
		SetMiddleSlice(i, m_Reader);
	}
}

void MPRMaker::ResetWindowLevel()
{
	const double level = m_Reader->GetMetaData()->Get(DC::WindowCenter).AsInt();
	const double window = m_Reader->GetMetaData()->Get(DC::WindowWidth).AsInt();
	if (!m_ColorMap)
	{
		m_ColorMap = vtkSmartPointer<vtkScalarsToColors>::New();
	}
	m_ColorMap->SetRange(level - 0.5 * window, level + 0.5 * window);
}
