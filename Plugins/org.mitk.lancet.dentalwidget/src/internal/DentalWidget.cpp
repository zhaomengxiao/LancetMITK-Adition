/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


// Blueberry
#include <berryISelectionService.h>
#include <berryIWorkbenchWindow.h>

// Qmitk
#include "DentalWidget.h"


// mitk image
#include <mitkImage.h>
#include <vtkImageCast.h>
#include <vtkPointData.h>

#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateAnd.h"
#include "mitkNodePredicateDataType.h"
#include "mitkNodePredicateNot.h"
#include "mitkNodePredicateOr.h"
#include "mitkNodePredicateProperty.h"
#include "mitkPointSet.h"
#include "mitkSurface.h"

const std::string DentalWidget::VIEW_ID = "org.mitk.views.dentalwidget";

void DentalWidget::SetFocus()
{
  //m_Controls.buttonPerformImageProcessing->setFocus();
}

void DentalWidget::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/, const QList<mitk::DataNode::Pointer>& nodes)
{
	std::string nodeName;
	// iterate all selected objects, adjust warning visibility
	foreach(mitk::DataNode::Pointer node, nodes)
	{
		if (node.IsNull())
		{
			return;
		}

		node->GetName(nodeName);

		if (node != nullptr)
		{
			m_currentSelectedNode = node;
			m_baseDataToMove = node->GetData();
		}

	}

}

void DentalWidget::InitImageSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::Image>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select an image"));
	widget->SetPopUpTitel(QString("Select image"));
}

void DentalWidget::InitPointSetSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::PointSet>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a point set"));
	widget->SetPopUpTitel(QString("Select point set"));
}

void DentalWidget::InitSurfaceSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateAnd::New(
		mitk::TNodePredicateDataType<mitk::Surface>::New(),
		mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(mitk::NodePredicateProperty::New("helper object"),
			mitk::NodePredicateProperty::New("hidden object")))));

	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a surface"));
	widget->SetPopUpTitel(QString("Select surface"));
}

void DentalWidget::InitNodeSelector(QmitkSingleNodeSelectionWidget* widget)
{
	widget->SetDataStorage(GetDataStorage());
	widget->SetNodePredicate(mitk::NodePredicateNot::New(mitk::NodePredicateOr::New(
		mitk::NodePredicateProperty::New("helper object"), mitk::NodePredicateProperty::New("hidden object"))));
	widget->SetSelectionIsOptional(true);
	widget->SetAutoSelectNewNodes(true);
	widget->SetEmptyInfo(QString("Please select a node"));
	widget->SetPopUpTitel(QString("Select node"));
}

void DentalWidget::CreateQtPartControl(QWidget *parent)
{
  // create GUI widgets from the Qt Designer's .ui file
  m_Controls.setupUi(parent);

  InitImageSelector(m_Controls.mitkNodeSelectWidget_intraopCt);
  InitImageSelector(m_Controls.mitkNodeSelectWidget_preopCT_reg);
  InitImageSelector(m_Controls.mitkNodeSelectWidget_intraopCT_reg);
  InitNodeSelector(m_Controls.mitkNodeSelectWidget_appendMatrix);

  m_NodetreeModel = new QmitkDataStorageTreeModel(this->GetDataStorage());

  //connect(m_Controls.buttonPerformImageProcessing, &QPushButton::clicked, this, &DentalWidget::DoImageProcessing);
  connect(m_Controls.checkBox_useSmooth, &QCheckBox::stateChanged, this, &DentalWidget::CheckUseSmoothing);
  connect(m_Controls.pushButton_reconstructSurface, &QPushButton::clicked, this, &DentalWidget::ReconstructSurface);

  connect(m_Controls.pushButton_xp, &QPushButton::clicked, this, &DentalWidget::TranslatePlusX);
  connect(m_Controls.pushButton_yp, &QPushButton::clicked, this, &DentalWidget::TranslatePlusY);
  connect(m_Controls.pushButton_zp, &QPushButton::clicked, this, &DentalWidget::TranslatePlusZ);
  connect(m_Controls.pushButton_xm, &QPushButton::clicked, this, &DentalWidget::TranslateMinusX);
  connect(m_Controls.pushButton_ym, &QPushButton::clicked, this, &DentalWidget::TranslateMinusY);
  connect(m_Controls.pushButton_zm, &QPushButton::clicked, this, &DentalWidget::TranslateMinusZ);
  connect(m_Controls.pushButton_rxp, &QPushButton::clicked, this, &DentalWidget::RotatePlusX);
  connect(m_Controls.pushButton_ryp, &QPushButton::clicked, this, &DentalWidget::RotatePlusY);
  connect(m_Controls.pushButton_rzp, &QPushButton::clicked, this, &DentalWidget::RotatePlusZ);
  connect(m_Controls.pushButton_rxm, &QPushButton::clicked, this, &DentalWidget::RotateMinusX);
  connect(m_Controls.pushButton_rym, &QPushButton::clicked, this, &DentalWidget::RotateMinusY);
  connect(m_Controls.pushButton_rzm, &QPushButton::clicked, this, &DentalWidget::RotateMinusZ);
  connect(m_Controls.pushButton_registerIos_, &QPushButton::clicked, this, &DentalWidget::RegisterIos_);
  connect(m_Controls.pushButton_fixIcp_, &QPushButton::clicked, this, &DentalWidget::FineTuneRegister_);
  connect(m_Controls.pushButton_regReset_, &QPushButton::clicked, this, &DentalWidget::ResetRegistration_);
  connect(m_Controls.pushButton_extractBallCenters, &QPushButton::clicked, this, &DentalWidget::GetSteelballCenters_modelCBCT);
  connect(m_Controls.pushButton_checkPrecision, &QPushButton::clicked, this, &DentalWidget::CheckPrecision);
  connect(m_Controls.pushButton_modeltoCBCT, &QPushButton::clicked, this, &DentalWidget::RegisterModeltoCBCT);
  connect(m_Controls.pushButton_regResetModel, &QPushButton::clicked, this, &DentalWidget::ResetModelRegistration);
  connect(m_Controls.pushButton_preprocessCt, &QPushButton::clicked, this, &DentalWidget::PrepareCbctImages);
  connect(m_Controls.pushButton_retrieveMatrix, &QPushButton::clicked, this, &DentalWidget::RetrieveRegistrationMatrix);
  connect(m_Controls.pushButton_appendOffsetMatrix, &QPushButton::clicked, this, &DentalWidget::AppendMatrix_regisCbct);
  connect(m_Controls.pushButton_assembleRegistrationPoints, &QPushButton::clicked, this, &DentalWidget::AssembleRegistrationPoints);

  connect(m_Controls.pushButton_testSharpen, &QPushButton::clicked, this, &DentalWidget::on_pushButton_testSharpen_clicked);



  // Prepare some empty pointsets for registration purposes
  if (GetDataStorage()->GetNamedNode("landmark_src") == nullptr)
  {
	  auto pset_landmark_src = mitk::PointSet::New();
	  auto node_pset_landmark_src = mitk::DataNode::New();
	  node_pset_landmark_src->SetData(pset_landmark_src);
	  node_pset_landmark_src->SetName("landmark_src");
	  GetDataStorage()->Add(node_pset_landmark_src);
  }
  
  if (GetDataStorage()->GetNamedNode("landmark_target") == nullptr)
  {
	  auto pset_landmark_target = mitk::PointSet::New();
	  auto node_pset_landmark_target = mitk::DataNode::New();
	  node_pset_landmark_target->SetData(pset_landmark_target);
	  node_pset_landmark_target->SetName("landmark_target");
	  GetDataStorage()->Add(node_pset_landmark_target);
  }

  if (GetDataStorage()->GetNamedNode("icp_fineTuning") == nullptr)
  {
	  auto pset_icp_fineTuning = mitk::PointSet::New();
	  auto node_pset_icp_fineTuning = mitk::DataNode::New();
	  node_pset_icp_fineTuning->SetData(pset_icp_fineTuning);
	  node_pset_icp_fineTuning->SetName("icp_fineTuning");
	  GetDataStorage()->Add(node_pset_icp_fineTuning);
  }

  if (GetDataStorage()->GetNamedNode("Precision_checkPoints") == nullptr)
  {
	  auto pset_precision = mitk::PointSet::New();
	  auto node_pset_precision = mitk::DataNode::New();
	  node_pset_precision->SetData(pset_precision);
	  node_pset_precision->SetName("Precision_checkPoints");
	  GetDataStorage()->Add(node_pset_precision);
  }

	
  m_eigenMatrixInitialOffset.setIdentity();

}


void DentalWidget::on_pushButton_testSharpen_clicked()
{
	auto inputImage = dynamic_cast<mitk::Image*>(m_Controls.mitkNodeSelectWidget_intraopCt->GetSelectedNode()->GetData());

	//------------white image------------
	vtkNew<vtkImageData> whiteImage;
	double imageBounds[6]{ 0 };
	double imageSpacing[3]{ 1, 1, 1 };
	whiteImage->SetSpacing(imageSpacing);

	auto geometry = inputImage->GetGeometry();
	auto surfaceBounds = geometry->GetBounds();
	for (int n = 0; n < 6; n++)
	{
		imageBounds[n] = surfaceBounds[n];
	}

	int dim[3];
	for (int i = 0; i < 3; i++)
	{
		dim[i] = static_cast<int>(ceil((imageBounds[i * 2 + 1] - imageBounds[i * 2]) / imageSpacing[i]));
	}

	whiteImage->SetDimensions(dim);
	//whiteImage->SetExtent(0, dim[0] , 0, dim[1] , 0, dim[2] );

	cout << "Printing dim: " << dim[0] << ", " << dim[1] << ", " << dim[2] << endl;

	double origin[3];
	origin[0] = imageBounds[0] + imageSpacing[0] / 2;
	origin[1] = imageBounds[2] + imageSpacing[1] / 2;
	origin[2] = imageBounds[4] + imageSpacing[2] / 2;
	whiteImage->SetOrigin(origin);
	whiteImage->AllocateScalars(VTK_SHORT, 1);

	// fill the image with foreground voxels:
	short insideValue = 2000;
	// short outsideValue = 0;
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, insideValue);
	}

	//-----------------


	// Test: set the boundary voxel of the image to 
	auto inputVtkImage = inputImage->GetVtkImageData(0,0);
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	cout << "Printing dims: " << dims[0] << ", " << dims[1] << ", " << dims[2] <<endl;

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToInt();
	caster->Update();

	auto castVtkImage = caster->GetOutput();

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int* n = static_cast<int*>(castVtkImage->GetScalarPointer(x, y, z));

				int* m = static_cast<int*>(whiteImage->GetScalarPointer(x, y, z));

				m[0] = n[0];

			}
		}
	}



	auto newMitkImage = mitk::Image::New();
	
	newMitkImage->Initialize(whiteImage,1,-1,dim[2],dim[1]);
	
	newMitkImage->SetVolume(whiteImage->GetScalarPointer());

	// newMitkImage->SetGeometry(inputImage->GetGeometry());

	auto tmpNode = mitk::DataNode::New();
	tmpNode->SetName("testNode");
	tmpNode->SetData(newMitkImage);
	GetDataStorage()->Add(tmpNode);
	//
	// m_Controls.textBrowser->append("new button connected!");
}















