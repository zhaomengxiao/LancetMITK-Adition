
// Qmitk
#include "DentalWidget.h"

// mitk image
#include <mitkImage.h>
#include <vtkCleanPolyData.h>
#include <vtkImplicitPolyDataDistance.h>
#include <ep/include/vtk-9.1/vtkTransformFilter.h>

#include "mitkImageToSurfaceFilter.h"
#include "mitkNodePredicateDataType.h"
#include "mitkSurface.h"
#include "surfaceregistraion.h"

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

	ExtractAllPlans();
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	m_Controls.textBrowser->append("------ Fine tuning succeeded ------");

}

void DentalWidget::ResetRegistration_()
{
	// Move ios to the initial position
	m_Controls.textBrowser->append("------ Reset started ------");

	if(!(m_MatrixRegistrationResult == nullptr) && GetDataStorage()->GetNamedNode("ios") != nullptr)
	{
		m_MatrixRegistrationResult = nullptr;
		GetDataStorage()->GetNamedNode("ios")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_InitialMatrix);
		ExtractAllPlans();
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

	if (GetDataStorage()->GetNamedNode("Precision_checkPoints") != nullptr)
	{
		GetDataStorage()->Remove(GetDataStorage()->GetNamedNode("Precision_checkPoints"));
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

	auto pset_precision = mitk::PointSet::New();
	auto node_pset_precision = mitk::DataNode::New();
	node_pset_precision->SetData(pset_precision);
	node_pset_precision->SetName("Precision_checkPoints");
	GetDataStorage()->Add(node_pset_precision);

	
	m_Controls.textBrowser->append("------ Reset succeeded ------");
}


void DentalWidget::CheckPrecision()
{
	if (m_MatrixRegistrationResult == nullptr)
	{
		m_Controls.textBrowser->append("!!!!!! Please register first !!!!!!");
		return;
	}

	auto node_targetPoints = GetDataStorage()->GetNamedNode("Precision_checkPoints");
	auto node_srcSurface = GetDataStorage()->GetNamedNode("ios");

	auto targetPoints = dynamic_cast<mitk::PointSet*>(node_targetPoints->GetData());
	auto srcSurface = dynamic_cast<mitk::Surface*>(node_srcSurface->GetData());

	if (targetPoints->IsEmpty())
	{
		m_Controls.textBrowser->append("!!!!!! Error: 'Precision_checkPoints' is empty !!!!!!");
		return;
	}
	
	m_Controls.textBrowser->append("------ Precision checking started ------");


	auto tmpTransform = vtkTransform::New();
	tmpTransform->Identity();
	tmpTransform->PostMultiply();
	tmpTransform->Concatenate(srcSurface->GetGeometry()->GetVtkMatrix());

	auto vtkFilter = vtkTransformFilter::New();
	vtkFilter->SetTransform(tmpTransform);
	vtkFilter->SetInputData(srcSurface->GetVtkPolyData());
	vtkFilter->Update();

	vtkNew<vtkImplicitPolyDataDistance> implicitPolyDataDistance;
	implicitPolyDataDistance->SetInput(vtkFilter->GetPolyDataOutput());
	//double d0 = implicitPolyDataDistance->EvaluateFunction(tmpCenter);
	for (int i{0}; i < targetPoints->GetSize(); i++ )
	{
		auto tmp = targetPoints->GetPoint(i);
		double p[3]
		{
			tmp[0],
			tmp[1],
			tmp[2],
		};
		double d = implicitPolyDataDistance->EvaluateFunction(p);

		m_Controls.textBrowser->append("Point " + QString::number(i) +" error: " + QString::number(d));

	}

	// auto icpRegistrator = mitk::SurfaceRegistration::New();
	//
	// icpRegistrator->SetSurfaceSrc(icp_srcSurface);
	// icpRegistrator->SetIcpPoints(icp_targetPoints);
	// icpRegistrator->ComputeIcpResult();
	// auto fixMatrix = icpRegistrator->GetResult();

	// Apply the fixMatrix to icp_src_Surface
	// auto tmpTransform = vtkTransform::New();
	// tmpTransform->Identity();
	// tmpTransform->PostMultiply();
	// tmpTransform->Concatenate(icp_srcSurface->GetGeometry()->GetVtkMatrix());
	// tmpTransform->Concatenate(fixMatrix);
	// tmpTransform->Update();
	//
	// icp_srcSurface->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(tmpTransform->GetMatrix());

	
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
	m_Controls.textBrowser->append("------ Precision checking started ------");
}
