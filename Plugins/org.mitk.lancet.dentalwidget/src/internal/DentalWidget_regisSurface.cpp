
// Qmitk
#include "DentalWidget.h"

// mitk image
#include <mitkImage.h>
#include <vtkAppendPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkClipPolyData.h>
#include <vtkPlanes.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>

#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateDataType.h"
#include "mitkSurface.h"
#include "surfaceregistraion.h"


void DentalWidget::UpdateUiRegistrationMatrix()
{
	m_Controls.lineEdit_RegistrationMatrix_0->setText(QString::number(m_eigenMatrixTmpRegistrationResult(0)));
	m_Controls.lineEdit_RegistrationMatrix_1->setText(QString::number(m_eigenMatrixTmpRegistrationResult(1)));
	m_Controls.lineEdit_RegistrationMatrix_2->setText(QString::number(m_eigenMatrixTmpRegistrationResult(2)));
	m_Controls.lineEdit_RegistrationMatrix_3->setText(QString::number(m_eigenMatrixTmpRegistrationResult(3)));
	m_Controls.lineEdit_RegistrationMatrix_4->setText(QString::number(m_eigenMatrixTmpRegistrationResult(4)));
	m_Controls.lineEdit_RegistrationMatrix_5->setText(QString::number(m_eigenMatrixTmpRegistrationResult(5)));
	m_Controls.lineEdit_RegistrationMatrix_6->setText(QString::number(m_eigenMatrixTmpRegistrationResult(6)));
	m_Controls.lineEdit_RegistrationMatrix_7->setText(QString::number(m_eigenMatrixTmpRegistrationResult(7)));
	m_Controls.lineEdit_RegistrationMatrix_8->setText(QString::number(m_eigenMatrixTmpRegistrationResult(8)));
	m_Controls.lineEdit_RegistrationMatrix_9->setText(QString::number(m_eigenMatrixTmpRegistrationResult(9)));
	m_Controls.lineEdit_RegistrationMatrix_10->setText(QString::number(m_eigenMatrixTmpRegistrationResult(10)));
	m_Controls.lineEdit_RegistrationMatrix_11->setText(QString::number(m_eigenMatrixTmpRegistrationResult(11)));
	m_Controls.lineEdit_RegistrationMatrix_12->setText(QString::number(m_eigenMatrixTmpRegistrationResult(12)));
	m_Controls.lineEdit_RegistrationMatrix_13->setText(QString::number(m_eigenMatrixTmpRegistrationResult(13)));
	m_Controls.lineEdit_RegistrationMatrix_14->setText(QString::number(m_eigenMatrixTmpRegistrationResult(14)));
	m_Controls.lineEdit_RegistrationMatrix_15->setText(QString::number(m_eigenMatrixTmpRegistrationResult(15)));
}

void DentalWidget::LandmarkRegistration()
{
	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
	m_LandmarkSourcePointset = m_Controls.mitkNodeSelectWidget_LandmarkSrcPointset->GetSelectedNode();
	m_LandmarkTargetPointset = m_Controls.mitkNodeSelectWidget_LandmarkTargetPointset->GetSelectedNode();


	MITK_INFO << "Proceeding Landmark registration";
	if (m_LandmarkSourcePointset != nullptr && m_LandmarkTargetPointset != nullptr)
	{
		auto sourcePointset = dynamic_cast<mitk::PointSet*>(m_LandmarkSourcePointset->GetData());
		auto targetPointset = dynamic_cast<mitk::PointSet*>(m_LandmarkTargetPointset->GetData());
		landmarkRegistrator->SetLandmarksSrc(sourcePointset);
		landmarkRegistrator->SetLandmarksTarget(targetPointset);
		landmarkRegistrator->ComputeLandMarkResult();



		Eigen::Matrix4d tmpRegistrationResult{ landmarkRegistrator->GetResult()->GetData() };
		tmpRegistrationResult.transposeInPlace();
		m_eigenMatrixTmpRegistrationResult = tmpRegistrationResult;
		MITK_INFO << m_eigenMatrixTmpRegistrationResult;
		UpdateUiRegistrationMatrix();
	}
	else
	{
		m_Controls.textBrowser->append("Landmark source or target are is not ready");
	}

	std::ostringstream os;
	landmarkRegistrator->GetResult()->Print(os);

	m_Controls.textBrowser->append("-------------Start landmark registration----------");
	// m_Controls.textBrowser_moveData->append(QString::fromStdString(os.str()));

	m_Controls.textBrowser->append("Max landmark registration error: " + QString::number(landmarkRegistrator->GetmaxLandmarkError()));
	m_Controls.textBrowser->append("Average landmark registration error: " + QString::number(landmarkRegistrator->GetavgLandmarkError()));

}

vtkMatrix4x4* DentalWidget::ObtainVtkMatrixFromRegistrationUi()
{
	auto tmpMatrix = vtkMatrix4x4::New();

	tmpMatrix->SetElement(0, 0, m_Controls.lineEdit_RegistrationMatrix_0->text().toDouble());
	tmpMatrix->SetElement(1, 0, m_Controls.lineEdit_RegistrationMatrix_1->text().toDouble());
	tmpMatrix->SetElement(2, 0, m_Controls.lineEdit_RegistrationMatrix_2->text().toDouble());
	tmpMatrix->SetElement(3, 0, m_Controls.lineEdit_RegistrationMatrix_3->text().toDouble());
	tmpMatrix->SetElement(0, 1, m_Controls.lineEdit_RegistrationMatrix_4->text().toDouble());
	tmpMatrix->SetElement(1, 1, m_Controls.lineEdit_RegistrationMatrix_5->text().toDouble());
	tmpMatrix->SetElement(2, 1, m_Controls.lineEdit_RegistrationMatrix_6->text().toDouble());
	tmpMatrix->SetElement(3, 1, m_Controls.lineEdit_RegistrationMatrix_7->text().toDouble());
	tmpMatrix->SetElement(0, 2, m_Controls.lineEdit_RegistrationMatrix_8->text().toDouble());
	tmpMatrix->SetElement(1, 2, m_Controls.lineEdit_RegistrationMatrix_9->text().toDouble());
	tmpMatrix->SetElement(2, 2, m_Controls.lineEdit_RegistrationMatrix_10->text().toDouble());
	tmpMatrix->SetElement(3, 2, m_Controls.lineEdit_RegistrationMatrix_11->text().toDouble());
	tmpMatrix->SetElement(0, 3, m_Controls.lineEdit_RegistrationMatrix_12->text().toDouble());
	tmpMatrix->SetElement(1, 3, m_Controls.lineEdit_RegistrationMatrix_13->text().toDouble());
	tmpMatrix->SetElement(2, 3, m_Controls.lineEdit_RegistrationMatrix_14->text().toDouble());
	tmpMatrix->SetElement(3, 3, m_Controls.lineEdit_RegistrationMatrix_15->text().toDouble());

	return tmpMatrix;
}

void DentalWidget::AppendRegistrationMatrix()
{
	m_currentSelectedNode = m_Controls.mitkNodeSelectWidget_MovingObject->GetSelectedNode();

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		if (m_baseDataToMove != nullptr)
		{
			auto tmpVtkTransform = vtkTransform::New();
			tmpVtkTransform->PostMultiply();
			tmpVtkTransform->Identity();
			tmpVtkTransform->SetMatrix(m_baseDataToMove->GetGeometry()->GetVtkMatrix());
			tmpVtkTransform->Concatenate(ObtainVtkMatrixFromRegistrationUi());

			m_baseDataToMove->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpVtkTransform->GetMatrix());
			mitk::RenderingManager::GetInstance()->RequestUpdateAll();


		}
		else
		{
			m_Controls.textBrowser->append("Moving object is empty ~~");
		}

	}

}

void DentalWidget::IcpRegistration()
{
	auto icpRegistrator = mitk::SurfaceRegistration::New();

	m_IcpSourceSurface = m_Controls.mitkNodeSelectWidget_IcpSrcSurface->GetSelectedNode();
	m_IcpTargetPointset = m_Controls.mitkNodeSelectWidget_IcpTargetPointset->GetSelectedNode();

	MITK_INFO << "Proceedinng ICP registration";

	if (m_IcpSourceSurface != nullptr && m_IcpTargetPointset != nullptr)
	{
		auto icpTargetPointset = dynamic_cast<mitk::PointSet*>(m_IcpTargetPointset->GetData());
		auto icpSrcSurface = dynamic_cast<mitk::Surface*>(m_IcpSourceSurface->GetData());
		icpRegistrator->SetIcpPoints(icpTargetPointset);
		icpRegistrator->SetSurfaceSrc(icpSrcSurface);
		icpRegistrator->ComputeIcpResult();

		Eigen::Matrix4d tmpRegistrationResult{ icpRegistrator->GetResult()->GetData() };
		tmpRegistrationResult.transposeInPlace();

		m_eigenMatrixTmpRegistrationResult = tmpRegistrationResult;
		MITK_INFO << m_eigenMatrixTmpRegistrationResult;

		UpdateUiRegistrationMatrix();
	}
	else
	{
		m_Controls.textBrowser->append("Icp source or target are is not ready");
	}

	std::ostringstream os;
	icpRegistrator->GetResult()->Print(os);
	m_Controls.textBrowser->append("-------------Start ICP registration----------");
	m_Controls.textBrowser->append(QString::fromStdString(os.str()));
};


void DentalWidget::RegisterIos()
{
	m_Controls.textBrowser->append("------- Registration started -------");

	auto node_landmark_src = GetDataStorage()->GetNamedNode("landmark_src");
	auto landmark_src = dynamic_cast<mitk::PointSet*>(node_landmark_src->GetData());
	int landmark_src_ptNum = landmark_src->GetSize();

	if (landmark_src_ptNum == 0)
	{
		m_Controls.textBrowser->append("!!!!! Landmark error: landmark_src is empty !!!!!");
		return;
	}

	auto node_landmark_target = GetDataStorage()->GetNamedNode("landmark_target");
	auto landmark_target = dynamic_cast<mitk::PointSet*>(node_landmark_target->GetData());
	int landmark_target_ptNum = landmark_target->GetSize();

	if (landmark_target_ptNum == 0)
	{
		m_Controls.textBrowser->append("!!!!! Landmark error: landmark_target is empty !!!!!");
		return;
	}


	auto node_icp_target = GetDataStorage()->GetNamedNode("icp_target");
	auto icp_target = dynamic_cast<mitk::PointSet*>(node_icp_target->GetData());
	int icp_target_ptNum = landmark_target->GetSize();

	if (icp_target_ptNum == 0)
	{
		m_Controls.textBrowser->append("!!!!! Landmark error: icp_target is empty !!!!!");
		return;
	}

	auto node_icp_surface = GetDataStorage()->GetNamedNode("ios");
	if(node_icp_surface==nullptr)
	{
		m_Controls.textBrowser->append("!!!!! ICP error: Please rename the intraoral scan surface as 'ios' !!!!!");
		return;
	}

	//landmark registration
	auto landmarkRegistrator = mitk::SurfaceRegistration::New();
	m_LandmarkSourcePointset = node_landmark_src;
	m_LandmarkTargetPointset = node_landmark_target;

	MITK_INFO << "Proceeding Landmark registration";
	if (m_LandmarkSourcePointset != nullptr && m_LandmarkTargetPointset != nullptr)
	{
		auto sourcePointset = dynamic_cast<mitk::PointSet*>(m_LandmarkSourcePointset->GetData());
		auto targetPointset = dynamic_cast<mitk::PointSet*>(m_LandmarkTargetPointset->GetData());
		landmarkRegistrator->SetLandmarksSrc(sourcePointset);
		landmarkRegistrator->SetLandmarksTarget(targetPointset);
		landmarkRegistrator->ComputeLandMarkResult();



		Eigen::Matrix4d tmpRegistrationResult{ landmarkRegistrator->GetResult()->GetData() };
		// tmpRegistrationResult.transposeInPlace();
		m_eigenMatrixTmpRegistrationResult = tmpRegistrationResult;
		MITK_INFO << m_eigenMatrixTmpRegistrationResult;
		
	}

	// Record the initial matrix offset
	Eigen::Matrix4d initialMatrixOffset{ node_icp_surface->GetData()->GetGeometry()->GetVtkMatrix()->GetData() };
	// tmpRegistrationResult.transposeInPlace();
	m_eigenMatrixInitialOffset = initialMatrixOffset;

	//std::ostringstream os;
	//landmarkRegistrator->GetResult()->Print(os);

	//m_Controls.textBrowser->append("-------------Start landmark registration----------");
	// m_Controls.textBrowser_moveData->append(QString::fromStdString(os.str()));

	m_Controls.textBrowser->append("Max landmark registration error: " + QString::number(landmarkRegistrator->GetmaxLandmarkError()));
	m_Controls.textBrowser->append("Average landmark registration error: " + QString::number(landmarkRegistrator->GetavgLandmarkError()));


	// First move
	m_currentSelectedNode = node_icp_surface;

	QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		if (m_baseDataToMove != nullptr)
		{
			auto tmpVtkTransform = vtkTransform::New();
			tmpVtkTransform->PostMultiply();
			tmpVtkTransform->Identity();
			tmpVtkTransform->SetMatrix(m_baseDataToMove->GetGeometry()->GetVtkMatrix());
			auto tmpVtkMatrix = vtkMatrix4x4::New();
			tmpVtkMatrix->DeepCopy(m_eigenMatrixTmpRegistrationResult.data());
			tmpVtkTransform->Concatenate(tmpVtkMatrix);

			m_baseDataToMove->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpVtkTransform->GetMatrix());
			mitk::RenderingManager::GetInstance()->RequestUpdateAll();


		}


	}



	// ICP registration
	auto icpRegistrator = mitk::SurfaceRegistration::New();

	m_IcpSourceSurface = node_icp_surface;
	m_IcpTargetPointset = node_icp_target;

	MITK_INFO << "Proceedinng ICP registration";

	if (m_IcpSourceSurface != nullptr && m_IcpTargetPointset != nullptr)
	{
		auto icpTargetPointset = dynamic_cast<mitk::PointSet*>(m_IcpTargetPointset->GetData());
		auto icpSrcSurface = dynamic_cast<mitk::Surface*>(m_IcpSourceSurface->GetData());
		icpRegistrator->SetIcpPoints(icpTargetPointset);
		icpRegistrator->SetSurfaceSrc(icpSrcSurface);
		icpRegistrator->ComputeIcpResult();

		Eigen::Matrix4d tmpRegistrationResult1{ icpRegistrator->GetResult()->GetData() };
		//tmpRegistrationResult.transposeInPlace();

		m_eigenMatrixTmpRegistrationResult = tmpRegistrationResult1;
		MITK_INFO << m_eigenMatrixTmpRegistrationResult;

		//UpdateUiRegistrationMatrix();
	}
	

	// std::ostringstream os;
	// icpRegistrator->GetResult()->Print(os);
	// m_Controls.textBrowser->append("-------------Start ICP registration----------");
	// m_Controls.textBrowser->append(QString::fromStdString(os.str()));



	// Apply the transformation



	// Move ios

	//m_currentSelectedNode = node_icp_surface;

	//QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

	//int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
	//int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

	for (int i = 0; i < (parentRowCount + 1); i++)
	{
		if (i == parentRowCount)
		{
			m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

		}
		else
		{
			QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
			m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
		}

		if (m_baseDataToMove != nullptr)
		{
			auto tmpVtkTransform = vtkTransform::New();
			tmpVtkTransform->PostMultiply();
			tmpVtkTransform->Identity();
			tmpVtkTransform->SetMatrix(m_baseDataToMove->GetGeometry()->GetVtkMatrix());
			auto tmpVtkMatrix = vtkMatrix4x4::New();
			tmpVtkMatrix->DeepCopy(m_eigenMatrixTmpRegistrationResult.data());
			tmpVtkTransform->Concatenate(tmpVtkMatrix);

			m_baseDataToMove->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpVtkTransform->GetMatrix());
			mitk::RenderingManager::GetInstance()->RequestUpdateAll();


		}
		

	}

	m_Controls.textBrowser->append("------- Registration succeeded -------");
}

void DentalWidget::FineTuneRegister()
{
	m_Controls.textBrowser->append("------ Fine tuning started ------");

	m_IcpTargetPointset = GetDataStorage()->GetNamedNode("icp_fineTuning");

	if (dynamic_cast<mitk::PointSet*>(m_IcpTargetPointset->GetData())->GetSize() == 0)
	{
		m_Controls.textBrowser->append("!!!! Error: 'icp_fineTuning' is empty !!!!");
		return;
	}

	if(m_IcpSourceSurface != nullptr)
	{
		auto icpRegistrator = mitk::SurfaceRegistration::New();
		

		MITK_INFO << "Proceedinng ICP registration";

		auto icpTargetPointset = dynamic_cast<mitk::PointSet*>(m_IcpTargetPointset->GetData());
		auto icpSrcSurface = dynamic_cast<mitk::Surface*>(m_IcpSourceSurface->GetData());
		icpRegistrator->SetIcpPoints(icpTargetPointset);
		icpRegistrator->SetSurfaceSrc(icpSrcSurface);
		icpRegistrator->ComputeIcpResult();

		Eigen::Matrix4d tmpRegistrationResult1{ icpRegistrator->GetResult()->GetData() };
		//tmpRegistrationResult.transposeInPlace();

		m_eigenMatrixTmpRegistrationResult = tmpRegistrationResult1;
		MITK_INFO << m_eigenMatrixTmpRegistrationResult;

		// Move data
		m_currentSelectedNode = m_IcpSourceSurface;

		QModelIndex parentIndex = m_NodetreeModel->GetIndex(m_currentSelectedNode);

		int parentRowCount = m_NodetreeModel->rowCount(parentIndex);
		int parentColumnCount = m_NodetreeModel->columnCount(parentIndex);

		for (int i = 0; i < (parentRowCount + 1); i++)
		{
			if (i == parentRowCount)
			{
				m_baseDataToMove = m_NodetreeModel->GetNode(parentIndex)->GetData();

			}
			else
			{
				QModelIndex tmpIndex = m_NodetreeModel->index(i, 0, parentIndex);
				m_baseDataToMove = m_NodetreeModel->GetNode(tmpIndex)->GetData();
			}

			if (m_baseDataToMove != nullptr)
			{
				auto tmpVtkTransform = vtkTransform::New();
				tmpVtkTransform->PostMultiply();
				tmpVtkTransform->Identity();
				tmpVtkTransform->SetMatrix(m_baseDataToMove->GetGeometry()->GetVtkMatrix());
				auto tmpVtkMatrix = vtkMatrix4x4::New();
				tmpVtkMatrix->DeepCopy(m_eigenMatrixTmpRegistrationResult.data());
				tmpVtkTransform->Concatenate(tmpVtkMatrix);

				m_baseDataToMove->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpVtkTransform->GetMatrix());
				mitk::RenderingManager::GetInstance()->RequestUpdateAll();


			}


		}
		

		m_Controls.textBrowser->append("------ Fine tuning succeeded ------");
	}else
	{
		m_Controls.textBrowser->append("!!!! Error: Please register once before fine tuning !!!!");
	}
}


void DentalWidget::FineTuneRegister_()
{
	if(m_MatrixRegistrationResult==nullptr)
	{
		m_Controls.textBrowser->append("!!!!!! Please register first !!!!!!");
		return;
	}
	   	
	auto node_icp_targetPoints = GetDataStorage()->GetNamedNode("icp_fineTuning");
	auto node_icp_srcSurface = GetDataStorage()->GetNamedNode("ios");

	auto icp_targetPoints = dynamic_cast<mitk::PointSet*>(node_icp_targetPoints->GetData());
	auto icp_srcSurface = dynamic_cast<mitk::Surface*>(node_icp_srcSurface->GetData());

	if (icp_targetPoints->IsEmpty())
	{
		m_Controls.textBrowser->append("!!!!!! Error: 'icp_fineTuning' is empty !!!!!!");
		return;
	}

	m_Controls.textBrowser->append("------ Fine tuning started ------");

	auto icpRegistrator = mitk::SurfaceRegistration::New();

	icpRegistrator->SetSurfaceSrc(icp_srcSurface);
	icpRegistrator->SetIcpPoints(icp_targetPoints);
	icpRegistrator->ComputeIcpResult();
	auto fixMatrix = icpRegistrator->GetResult();

	// Apply the fixMatrix to icp_src_Surface
	auto tmpTransform = vtkTransform::New();
	tmpTransform->Identity();
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(icp_srcSurface->GetGeometry()->GetVtkMatrix());
	tmpTransform->Concatenate(fixMatrix);
	tmpTransform->Update();

	icp_srcSurface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTransform->GetMatrix());

	m_Controls.textBrowser->append("Maximal ICP error: "+ QString::number(icpRegistrator->GetmaxIcpError()));
	m_Controls.textBrowser->append("Average ICP error: " + QString::number(icpRegistrator->GetavgIcpError()));


	// TestExtractPlan();
	ExtractAllPlans();
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	m_Controls.textBrowser->append("------ Fine tuning succeeded ------");


}

void DentalWidget::ResetRegistration()
{
	m_Controls.textBrowser->append("------ Reset started ------");


	auto tmpVtkMatrix = vtkMatrix4x4::New();
	tmpVtkMatrix->DeepCopy(m_eigenMatrixInitialOffset.data());
	GetDataStorage()->GetNamedNode("ios")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpVtkMatrix);

	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("icp_fineTuning"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("icp_target"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("landmark_src"));
	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("landmark_target"));
	

	// Prepare some empty pointsets for registration purposes
	auto pset_landmark_src = mitk::PointSet::New();
	auto node_pset_landmark_src = mitk::DataNode::New();
	node_pset_landmark_src->SetData(pset_landmark_src);
	node_pset_landmark_src->SetName("landmark_src");
	GetDataStorage()->Add(node_pset_landmark_src);

	auto pset_landmark_target = mitk::PointSet::New();
	auto node_pset_landmark_target = mitk::DataNode::New();
	node_pset_landmark_target->SetData(pset_landmark_target);
	node_pset_landmark_target->SetName("landmark_target");
	GetDataStorage()->Add(node_pset_landmark_target);

	auto pset_icp_target = mitk::PointSet::New();
	auto node_pset_icp_target = mitk::DataNode::New();
	node_pset_icp_target->SetData(pset_icp_target);
	node_pset_icp_target->SetName("icp_target");
	GetDataStorage()->Add(node_pset_icp_target);

	auto pset_icp_fineTuning = mitk::PointSet::New();
	auto node_pset_icp_fineTuning = mitk::DataNode::New();
	node_pset_icp_fineTuning->SetData(pset_icp_fineTuning);
	node_pset_icp_fineTuning->SetName("icp_fineTuning");
	GetDataStorage()->Add(node_pset_icp_fineTuning);

	m_Controls.textBrowser->append("------ Reset succeeded ------");

}

void DentalWidget::ResetRegistration_()
{
	// Move ios to the initial position
	m_Controls.textBrowser->append("------ Reset started ------");

	if(!(m_MatrixRegistrationResult == nullptr))
	{
		m_MatrixRegistrationResult = nullptr;
		GetDataStorage()->GetNamedNode("ios")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_InitialMatrix);
	}

	// Remove nodes if they exist 
	if (GetDataStorage()->GetNamedNode("icp_fineTuning") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("icp_fineTuning"));
	}

	if (GetDataStorage()->GetNamedNode("landmark_src") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("landmark_src"));
	}
	
	if (GetDataStorage()->GetNamedNode("landmark_target") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("landmark_target"));
	}

	
	// Prepare some empty pointsets for registration purposes
	auto pset_landmark_src = mitk::PointSet::New();
	auto node_pset_landmark_src = mitk::DataNode::New();
	node_pset_landmark_src->SetData(pset_landmark_src);
	node_pset_landmark_src->SetName("landmark_src");
	GetDataStorage()->Add(node_pset_landmark_src);

	auto pset_landmark_target = mitk::PointSet::New();
	auto node_pset_landmark_target = mitk::DataNode::New();
	node_pset_landmark_target->SetData(pset_landmark_target);
	node_pset_landmark_target->SetName("landmark_target");
	GetDataStorage()->Add(node_pset_landmark_target);

	auto pset_icp_fineTuning = mitk::PointSet::New();
	auto node_pset_icp_fineTuning = mitk::DataNode::New();
	node_pset_icp_fineTuning->SetData(pset_icp_fineTuning);
	node_pset_icp_fineTuning->SetName("icp_fineTuning");
	GetDataStorage()->Add(node_pset_icp_fineTuning);


	// TestExtractPlan();
	ExtractAllPlans();
	m_Controls.textBrowser->append("------ Reset succeeded ------");
}

void DentalWidget::RegisterIos_()
{
	m_Controls.textBrowser->append("------ Registration started ------");

	auto node_landmark_src = GetDataStorage()->GetNamedNode("landmark_src");

	auto node_landmark_target = GetDataStorage()->GetNamedNode("landmark_target");

	auto node_ios = GetDataStorage()->GetNamedNode("ios");

	auto node_icp_target = GetDataStorage()->GetNamedNode("Reconstructed CBCT surface");

	if (!(m_MatrixRegistrationResult == nullptr))
	{
		m_Controls.textBrowser->append("!!!!!! Error: Please reset before register again !!!!!!");
		return;
	}

	if(node_landmark_src==nullptr)
	{
		m_Controls.textBrowser->append("!!!!!! Error: 'landmark_src' is missing !!!!!!");
		return;
	}
	if (dynamic_cast<mitk::PointSet*>(node_landmark_src->GetData())->IsEmpty())
	{
		m_Controls.textBrowser->append("!!!!!! Error: 'landmark_src' is empty !!!!!!");
		return;
	}
	if (node_landmark_target == nullptr )
	{
		m_Controls.textBrowser->append("!!!!!! Error: 'landmark_target' is missing !!!!!!");
		return;
	}
	if (dynamic_cast<mitk::PointSet*>(node_landmark_target->GetData())->IsEmpty() )
	{
		m_Controls.textBrowser->append("!!!!!! Error: 'landmark_target' is empty !!!!!!");
		return;
	}
	if (node_ios == nullptr)
	{
		m_Controls.textBrowser->append("!!!!!! Error: 'ios' is missing!!!!!!");
		return;
	}
	if (node_icp_target == nullptr)
	{
		if(GetDataStorage()->GetNamedNode("CBCT")==nullptr)
		{
			m_Controls.textBrowser->append("!!!!!! Error: 'Reconstructed CBCT surface' is missing!!!!!!");
			return;
		}
		AutoReconstructSurface();
		node_icp_target = GetDataStorage()->GetNamedNode("Reconstructed CBCT surface");
	}

	ClipTeeth();

	auto node_icp_src = GetDataStorage()->GetNamedNode("Clipped data");

	// Landmark registration
	auto landmarkRegistrator = mitk::SurfaceRegistration::New();

	landmarkRegistrator->SetLandmarksSrc(dynamic_cast<mitk::PointSet*>(node_landmark_src->GetData()));
	landmarkRegistrator->SetLandmarksTarget(dynamic_cast<mitk::PointSet*>(node_landmark_target->GetData()));
	landmarkRegistrator->ComputeLandMarkResult();


	auto landmarkResult = landmarkRegistrator->GetResult();
	Eigen::Matrix4d a{ landmarkResult->GetData() };
	MITK_INFO << a;
	auto landmarkTransform = vtkTransform::New();
	landmarkTransform->Identity();
	landmarkTransform->PostMultiply();
	landmarkTransform->SetMatrix(landmarkResult);
	landmarkTransform->Update();

	// ICP registration
	auto icpRegistrator = mitk::SurfaceRegistration::New();

	// Prepare icp source: Clipped data
	vtkNew<vtkTransformFilter> sourceFilter;
	auto targetSurface = dynamic_cast<mitk::Surface*>(node_icp_src->GetData());
	sourceFilter->SetInputData(targetSurface->GetVtkPolyData());
	sourceFilter->SetTransform(landmarkTransform);
	sourceFilter->Update();
	
	auto icpSource = mitk::Surface::New();
	icpSource->SetVtkPolyData(sourceFilter->GetPolyDataOutput());
	
	icpRegistrator->SetSurfaceSrc(icpSource);
	icpRegistrator->SetSurfaceTarget(dynamic_cast<mitk::Surface*>(node_icp_target->GetData()));
	icpRegistrator->ComputeSurfaceIcpResult();
	auto icpResult = icpRegistrator->GetResult();
	
	// Apply landmark & icp results to ios
	auto combinedTransform = vtkTransform::New();
	combinedTransform->Identity();
	combinedTransform->PostMultiply();
	m_InitialMatrix = vtkMatrix4x4::New();
	m_InitialMatrix ->DeepCopy(node_ios->GetData()->GetGeometry()->GetVtkMatrix());
	combinedTransform->Concatenate(node_ios->GetData()->GetGeometry()->GetVtkMatrix());
	combinedTransform->Concatenate(landmarkResult);
	combinedTransform->Concatenate(icpResult);
	combinedTransform->Update();
	
	node_ios->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(combinedTransform->GetMatrix());

	m_MatrixRegistrationResult = combinedTransform->GetMatrix();

	GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Clipped data"));

	// TestExtractPlan();

	ExtractAllPlans();
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	m_Controls.textBrowser->append("------ Registration succeeded ------");
}

void DentalWidget::ClipTeeth()
{
	auto inputPolyData = dynamic_cast<mitk::Surface*>
		(GetDataStorage()->GetNamedNode("ios")->GetData());

	auto inputPointSet = dynamic_cast<mitk::PointSet*>
		(GetDataStorage()->GetNamedNode("landmark_src")->GetData());

	vtkSmartPointer<vtkAppendPolyData> appendFilter =
		vtkSmartPointer<vtkAppendPolyData>::New();

	vtkSmartPointer<vtkCleanPolyData> cleanFilter =
		vtkSmartPointer<vtkCleanPolyData>::New();

	int inputPointNum = inputPointSet->GetSize();

	for (int i{ 0 }; i < inputPointNum; i++)
	{
		double tmpPoint[3]
		{
			inputPointSet->GetPoint(i)[0],
			inputPointSet->GetPoint(i)[1],
			inputPointSet->GetPoint(i)[2],
		};

		vtkNew<vtkClipPolyData> clip;
		vtkNew<vtkPlanes> planes;
		
		double boxSize{ 6 };
		
		planes->SetBounds(
			tmpPoint[0] - boxSize / 2,
			tmpPoint[0] + boxSize / 2,
			tmpPoint[1] - boxSize / 2,
			tmpPoint[1] + boxSize / 2,
			tmpPoint[2] - boxSize / 2,
			tmpPoint[2] + boxSize / 2
		);

		clip->SetInputData(inputPolyData->GetVtkPolyData());
		clip->SetClipFunction(planes);
		clip->InsideOutOn();
		clip->GenerateClippedOutputOn();
		clip->Update();

		appendFilter->AddInputData(clip->GetOutput());
		appendFilter->Update();
	}

	cleanFilter->SetInputData(appendFilter->GetOutput());
	cleanFilter->Update();


	auto tmpNode = mitk::DataNode::New();
	auto tmpData = mitk::Surface::New();
	tmpData->SetVtkPolyData(cleanFilter->GetOutput());
	tmpNode->SetData(tmpData);
	tmpNode->SetName("Clipped data");
	GetDataStorage()->Add(tmpNode);
}

