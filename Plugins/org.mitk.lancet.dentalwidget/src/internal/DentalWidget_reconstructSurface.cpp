
// Qmitk
#include "DentalWidget.h"

// mitk image
#include <mitkImage.h>
#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateDataType.h"
#include "mitkSurface.h"


void DentalWidget::CheckUseSmoothing()
{
	if (m_Controls.lineEdit_smoothIteration->isEnabled())
	{
		m_Controls.lineEdit_smoothIteration->setDisabled(true);
		m_Controls.lineEdit_smoothRelaxtion->setDisabled(true);
	}
	else
	{
		m_Controls.lineEdit_smoothIteration->setEnabled(true);
		m_Controls.lineEdit_smoothRelaxtion->setEnabled(true);
	}
}

bool DentalWidget::ReconstructSurface()
{
	// auto inputCtImage = dynamic_cast<mitk::Image*>(m_CtImageDataNode->GetData());
	auto inputCtImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_intraopCt->GetSelectedNode()->GetData());

	if (inputCtImage != nullptr)
	{
		// The isosurface of the CT data into a single polydata
		double threshold = m_Controls.lineEdit_surfaceLowerBound->text().toDouble();
		auto mitkRecontructedSurfaces = mitk::Surface::New();
		mitk::ImageToSurfaceFilter::Pointer imageToSurfaceFilter = mitk::ImageToSurfaceFilter::New();

		// Set smoothing parameters
		if (m_Controls.checkBox_useSmooth->isChecked())
		{
			int iteration = m_Controls.lineEdit_smoothIteration->text().toInt();
			double relaxtion = m_Controls.lineEdit_smoothRelaxtion->text().toDouble();
			imageToSurfaceFilter->SmoothOn();
			imageToSurfaceFilter->SetSmoothRelaxation(relaxtion);
			imageToSurfaceFilter->SetSmoothIteration(iteration);
		}


		imageToSurfaceFilter->SetInput(inputCtImage);
		imageToSurfaceFilter->SetThreshold(threshold);
		mitkRecontructedSurfaces = imageToSurfaceFilter->GetOutput();
		

		// draw extracted surface
		auto nodeSteelballSurfaces = mitk::DataNode::New();
		nodeSteelballSurfaces->SetName("Reconstructed CBCT surface");
		nodeSteelballSurfaces->SetColor(1, 0.5, 0);
		// add new node
		nodeSteelballSurfaces->SetData(mitkRecontructedSurfaces);
		GetDataStorage()->Add(nodeSteelballSurfaces);
		return true;
	}
	return false;
}

bool DentalWidget::AutoReconstructSurface()
{
	if (GetDataStorage()->GetNamedNode("CBCT") == nullptr)
	{
		return false;
	}

	auto inputCtImage = dynamic_cast<mitk::Image*>(GetDataStorage()->GetNamedNode("CBCT")->GetData());

	if (inputCtImage != nullptr)
	{
		// The isosurface of the CT data into a single polydata
		double threshold = 1500;
		auto mitkRecontructedSurfaces = mitk::Surface::New();
		mitk::ImageToSurfaceFilter::Pointer imageToSurfaceFilter = mitk::ImageToSurfaceFilter::New();

		// Set smoothing parameters
		// if (m_Controls.checkBox_useSmooth->isChecked())
		// {
		// 	int iteration = m_Controls.lineEdit_smoothIteration->text().toInt();
		// 	double relaxtion = m_Controls.lineEdit_smoothRelaxtion->text().toDouble();
		// 	imageToSurfaceFilter->SmoothOn();
		// 	imageToSurfaceFilter->SetSmoothRelaxation(relaxtion);
		// 	imageToSurfaceFilter->SetSmoothIteration(iteration);
		// }


		imageToSurfaceFilter->SetInput(inputCtImage);
		imageToSurfaceFilter->SetThreshold(threshold);
		mitkRecontructedSurfaces = imageToSurfaceFilter->GetOutput();


		// draw extracted surface
		auto nodeSteelballSurfaces = mitk::DataNode::New();
		nodeSteelballSurfaces->SetName("Reconstructed CBCT surface");
		nodeSteelballSurfaces->SetColor(1, 0.5, 0);
		// add new node
		nodeSteelballSurfaces->SetData(mitkRecontructedSurfaces);
		GetDataStorage()->Add(nodeSteelballSurfaces);
		return true;
	}
	return false;
}




