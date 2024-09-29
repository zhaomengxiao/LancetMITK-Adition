#include "RobotFrame.h"
using namespace lancetAlgorithm;
RobotFrame::RobotFrame(mitk::DataStorage* dataStorage, std::vector<RobotJoint*> robotJoints, RobotBase* base, RobotTool* aRobotTool)
{
	m_dataStorage = dataStorage;
	m_RobotJoints = robotJoints;
	int size = m_RobotJoints.size();
	m_EndJoint = m_RobotJoints[size - 1];
	m_Base = base;
	m_RobotTool = aRobotTool;
}

void RobotFrame::AddRobot(std::string configPath)
{
	if(m_RobotJoints.size() == 0)
		return;
	PKARenderHelper::LoadSingleMitkFile(m_dataStorage, m_Base->GetDescription().FileName, m_Base->GetDescription().Name);
	m_dataStorage->GetNamedNode(m_Base->GetDescription().Name)->SetOpacity(0.4);
	for (auto joint : m_RobotJoints)
	{
		//std::cout << "JointPath: "<<joint->Path << std::endl;
		//std::cout << "JointName: " << joint->Name << std::endl;
		PKARenderHelper::LoadSingleMitkFile(m_dataStorage, joint->Path, joint->Name);
		m_dataStorage->GetNamedNode(joint->Name)->SetOpacity(0.4);
	}

	this->LoadMatricesFromJsonFile(configPath, m_RobotJoints);


	double directionY[3] = { 0,1,0 };
	double directionX[3] = { 1,0,0 };
	double directionZ[3] = { 0,0,1 };

	//Eigen::Vector3d Joint1Center(0, -49.25, 285.598);
	//vtkSmartPointer<vtkTransform> joint1Transform = vtkSmartPointer<vtkTransform>::New();
	//joint1Transform->Translate(Joint1Center.data());
	//vtkSmartPointer<vtkMatrix4x4> matrixJoint1 = vtkSmartPointer<vtkMatrix4x4>::New();
	//joint1Transform->GetMatrix(matrixJoint1);
	//m_dataStorage->GetNamedNode("Joint1")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixJoint1);
	//PKARenderHelper::RotateModel(directionZ, Joint1Center.data(), m_dataStorage->GetNamedNode("Joint1"), 180);
	//PKARenderHelper::RotateModel(directionX, Joint1Center.data(), m_dataStorage->GetNamedNode("Joint1"), -90);

	//Eigen::Vector3d Joint2Center(0, 0, 391.602);
	//vtkSmartPointer<vtkTransform> joint2Transform = vtkSmartPointer<vtkTransform>::New();
	//joint2Transform->Translate(Joint2Center.data());
	//vtkSmartPointer<vtkMatrix4x4> matrixJoint2 = vtkSmartPointer<vtkMatrix4x4>::New();
	//joint2Transform->GetMatrix(matrixJoint2);
	//m_dataStorage->GetNamedNode("Joint2")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixJoint2);

	//Eigen::Vector3d Joint3Center(65, 43.328, 744.202);
	//vtkSmartPointer<vtkTransform> joint3Transform = vtkSmartPointer<vtkTransform>::New();
	//joint3Transform->Translate(Joint3Center.data());
	//vtkSmartPointer<vtkMatrix4x4> matrixJoint3 = vtkSmartPointer<vtkMatrix4x4>::New();
	//joint3Transform->GetMatrix(matrixJoint3);
	//m_dataStorage->GetNamedNode("Joint3")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixJoint3);
	//PKARenderHelper::RotateModel(directionX, Joint3Center.data(), m_dataStorage->GetNamedNode("Joint3"), 90);

	//Eigen::Vector3d Joint4Center(12.2, 0, 843.602);
	//vtkSmartPointer<vtkTransform> joint4Transform = vtkSmartPointer<vtkTransform>::New();
	//joint4Transform->Translate(Joint4Center.data());
	//vtkSmartPointer<vtkMatrix4x4> matrixJoint4 = vtkSmartPointer<vtkMatrix4x4>::New();
	//joint4Transform->GetMatrix(matrixJoint4);
	//m_dataStorage->GetNamedNode("Joint4")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixJoint4);

	//Eigen::Vector3d Joint5Center(0, -50.3, 1199.602);
	//vtkSmartPointer<vtkTransform> joint5Transform = vtkSmartPointer<vtkTransform>::New();
	//joint5Transform->Translate(Joint5Center.data());
	//vtkSmartPointer<vtkMatrix4x4> matrixJoint5 = vtkSmartPointer<vtkMatrix4x4>::New();
	//joint5Transform->GetMatrix(matrixJoint5);
	//m_dataStorage->GetNamedNode("Joint5")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixJoint5);
	//PKARenderHelper::RotateModel(directionX, Joint5Center.data(), m_dataStorage->GetNamedNode("Joint5"), -90);

	//Eigen::Vector3d Joint6Center(-87, 0, 1287.002);
	//vtkSmartPointer<vtkTransform> joint6Transform = vtkSmartPointer<vtkTransform>::New();
	//joint6Transform->Translate(Joint6Center.data());
	//vtkSmartPointer<vtkMatrix4x4> matrixJoint6 = vtkSmartPointer<vtkMatrix4x4>::New();
	//joint6Transform->GetMatrix(matrixJoint6);
	//m_dataStorage->GetNamedNode("Joint6")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixJoint6);
	//PKARenderHelper::RotateModel(directionY, Joint6Center.data(), m_dataStorage->GetNamedNode("Joint6"), 180);
	//PKARenderHelper::RotateModel(directionX, Joint6Center.data(), m_dataStorage->GetNamedNode("Joint6"), 180);

	//Eigen::Vector3d Joint7Center(-87, 0, 1316.502);
	//vtkSmartPointer<vtkTransform> joint7Transform = vtkSmartPointer<vtkTransform>::New();
	//joint7Transform->Translate(Joint7Center.data());
	//vtkSmartPointer<vtkMatrix4x4> matrixJoint7 = vtkSmartPointer<vtkMatrix4x4>::New();
	//joint7Transform->GetMatrix(matrixJoint7);
	//m_dataStorage->GetNamedNode("Joint7")->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixJoint7);

	//vtkSmartPointer<vtkMatrix4x4> TImage2Base = vtkSmartPointer<vtkMatrix4x4>::New();
	//TImage2Base->DeepCopy(m_dataStorage->GetNamedNode("Base")->GetData()->GetGeometry()->GetVtkMatrix());
	//vtkSmartPointer<vtkMatrix4x4> TImage2Joint1 = vtkSmartPointer<vtkMatrix4x4>::New();
	//TImage2Joint1->DeepCopy(m_dataStorage->GetNamedNode("Joint1")->GetData()->GetGeometry()->GetVtkMatrix());
	//vtkSmartPointer<vtkMatrix4x4> TImage2Joint2 = vtkSmartPointer<vtkMatrix4x4>::New();
	//TImage2Joint2->DeepCopy(m_dataStorage->GetNamedNode("Joint2")->GetData()->GetGeometry()->GetVtkMatrix());
	//vtkSmartPointer<vtkMatrix4x4> TImage2Joint3 = vtkSmartPointer<vtkMatrix4x4>::New();
	//TImage2Joint3->DeepCopy(m_dataStorage->GetNamedNode("Joint3")->GetData()->GetGeometry()->GetVtkMatrix());
	//vtkSmartPointer<vtkMatrix4x4> TImage2Joint4 = vtkSmartPointer<vtkMatrix4x4>::New();
	//TImage2Joint4->DeepCopy(m_dataStorage->GetNamedNode("Joint4")->GetData()->GetGeometry()->GetVtkMatrix());
	//vtkSmartPointer<vtkMatrix4x4> TImage2Joint5 = vtkSmartPointer<vtkMatrix4x4>::New();
	//TImage2Joint5->DeepCopy(m_dataStorage->GetNamedNode("Joint5")->GetData()->GetGeometry()->GetVtkMatrix());
	//vtkSmartPointer<vtkMatrix4x4> TImage2Joint6 = vtkSmartPointer<vtkMatrix4x4>::New();
	//TImage2Joint6->DeepCopy(m_dataStorage->GetNamedNode("Joint6")->GetData()->GetGeometry()->GetVtkMatrix());
	//vtkSmartPointer<vtkMatrix4x4> TImage2Joint7 = vtkSmartPointer<vtkMatrix4x4>::New();
	//TImage2Joint7->DeepCopy(m_dataStorage->GetNamedNode("Joint7")->GetData()->GetGeometry()->GetVtkMatrix());
	

	//m_RobotJoints[0]->UpdateJointToLink(CalculateJointToLinkMatrix(TImage2Base, TImage2Joint1));  //base2Joint1
	//m_RobotJoints[1]->UpdateJointToLink(CalculateJointToLinkMatrix(TImage2Joint1, TImage2Joint2));  //Joint1ToJoint2
	//m_RobotJoints[2]->UpdateJointToLink(CalculateJointToLinkMatrix(TImage2Joint2, TImage2Joint3));  //Joint2ToJoint3
	//m_RobotJoints[3]->UpdateJointToLink(CalculateJointToLinkMatrix(TImage2Joint3, TImage2Joint4));  //Joint3ToJoint4
	//m_RobotJoints[4]->UpdateJointToLink(CalculateJointToLinkMatrix(TImage2Joint4, TImage2Joint5));  //Joint4ToJoint5
	//m_RobotJoints[5]->UpdateJointToLink(CalculateJointToLinkMatrix(TImage2Joint5, TImage2Joint6));  //Joint5ToJoint6
	//m_RobotJoints[6]->UpdateJointToLink(CalculateJointToLinkMatrix(TImage2Joint6, TImage2Joint7));  //Joint5ToJoint6

	UpdateJoints(0, 0);
	if (m_RobotTool)
	{
		PKARenderHelper::LoadSingleMitkFile(m_dataStorage, m_RobotTool->Path, m_RobotTool->Name);
	}
	if (m_RobotTool)
	{
		Eigen::Vector3d RobotToolCenter(-52, 183, 1391.002);
		vtkSmartPointer<vtkTransform> robotToolTransform = vtkSmartPointer<vtkTransform>::New();
		robotToolTransform->Translate(RobotToolCenter.data());
		vtkSmartPointer<vtkMatrix4x4> matrixRobotTool = vtkSmartPointer<vtkMatrix4x4>::New();
		robotToolTransform->GetMatrix(matrixRobotTool);
		m_dataStorage->GetNamedNode(m_RobotTool->Name)->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrixRobotTool);
		PKARenderHelper::RotateModel(directionZ, RobotToolCenter.data(), m_dataStorage->GetNamedNode(m_RobotTool->Name), 90);
	}
	vtkSmartPointer<vtkMatrix4x4> TImage2Joint7 = vtkSmartPointer<vtkMatrix4x4>::New();
	TImage2Joint7->DeepCopy(m_dataStorage->GetNamedNode("Joint7")->GetData()->GetGeometry()->GetVtkMatrix());
	vtkSmartPointer<vtkMatrix4x4> TImage2Tool = vtkSmartPointer<vtkMatrix4x4>::New();
	TImage2Tool->DeepCopy(m_dataStorage->GetNamedNode(m_RobotTool->Name)->GetData()->GetGeometry()->GetVtkMatrix());
	m_RobotTool->UpdateJointToTool(CalculateJointToLinkMatrix(TImage2Joint7, TImage2Tool));
	UpdateJoints(0, 0);
}

void RobotFrame::AddAxis(mitk::IRenderWindowPart* renderWindowPart)
{
	if (m_RobotJoints.size() == 0)
		return;
	PKARenderHelper::AddActor(renderWindowPart, m_Base->GetBaseAxesActor());
	for (auto joint : m_RobotJoints)
	{
		PKARenderHelper::AddActor(renderWindowPart, joint->GetJointAxesActor());
	}

	if (m_RobotTool)
	{
		PKARenderHelper::AddActor(renderWindowPart, m_RobotTool->GetToolAxesActor());
	}
}

void RobotFrame::UpdateJoints(int jointId, double aPosition)
{
	if (jointId >= m_RobotJoints.size())
		return;
	m_JointAngles[jointId] = aPosition;
	UpdateBaseToLink(jointId, aPosition);
	for (int i = jointId+1; i < m_RobotJoints.size(); ++i)
	{
		UpdateBaseToLink(i, m_JointAngles[i]);
	}
	Update(jointId);
}

void RobotFrame::UpdateJoints(double* aPosition)
{
	int size = m_RobotJoints.size();
	for (int i = 0; i < size; ++i)
	{
		m_JointAngles[i] = aPosition[i];
		UpdateBaseToLink(i, aPosition[i]);
	}
	Update();
}

void RobotFrame::DisplayRobot()
{
	for (auto joint : m_RobotJoints)
	{
		auto node = m_dataStorage->GetNamedNode(joint->Name);
		node->SetVisibility(true);
	}
}

void RobotFrame::HideRobot()
{
	for (auto joint : m_RobotJoints)
	{
		auto node = m_dataStorage->GetNamedNode(joint->Name);
		node->SetVisibility(false);
	}
}

void RobotFrame::DispalyAxes()
{
	for (auto joint : m_RobotJoints)
	{
		joint->GetJointAxesActor()->SetVisibility(true);
	}
}

void RobotFrame::HideAxes()
{
	for (auto joint : m_RobotJoints)
	{
		joint->GetJointAxesActor()->SetVisibility(false);
	}
}

void RobotFrame::DisplayTool()
{
	auto node = m_dataStorage->GetNamedNode("RobotTool");
	if (!node)
	{
		if (!m_RobotTool)
			return;
		PKARenderHelper::LoadSingleMitkFile(m_dataStorage, m_RobotTool->Path, m_RobotTool->Name);
	}
	else
	{
		node->SetVisibility(true);
	}
}

void RobotFrame::HideTool()
{
	auto node = m_dataStorage->GetNamedNode("RobotTool");
	if (!node)
		return;
}

vtkSmartPointer<vtkAxesActor> RobotFrame::GetJointAxesActor(int index)
{
	return  m_RobotJoints[index - 1]->GetJointAxesActor();
}

double* RobotFrame::GetCurrentJointAngles()
{
	return m_JointAngles;
}

double RobotFrame::GetJointAngleByIndex(int index)
{
	//int size =m_RobotJoints.size();
	//std::cout << "GetJointAngleByIndex: " << size << std::endl;
	//if (index >= size)
		//return 0.0;
	return m_JointAngles[index];
}

void RobotFrame::SaveJoints2Links(std::string fileName)
{
	this->SaveMatricesToJsonFile(m_RobotJoints, fileName);
}

void RobotFrame::UpdateBaseToLink(int jointID, double angle)
{
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkMatrix4x4> matrix1 = vtkSmartPointer<vtkMatrix4x4>::New();

	Eigen::Vector3d direction(0, 0, 1);
	if (jointID == 0)
	{
		matrix->Identity();
	}
	else
	{
		matrix1->DeepCopy(m_RobotJoints[jointID-1]->GetBaseToLink());
	}

	transform->PreMultiply();
	transform->SetMatrix(matrix1);
	transform->RotateWXYZ(angle, direction.data());
	transform->Concatenate(m_RobotJoints[jointID]->GetJointToLink());
	transform->Update();
	matrix1->Identity();
	transform->GetMatrix(matrix1);
	//m_dataStorage->GetNamedNode(m_RobotJoints[jointID + 1]->Name)->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(matrix1);
	m_RobotJoints[jointID]->UpdateBaseToLink(matrix1);
}

void RobotFrame::Update(int id)
{
	UpdateBaseToTool(id);
	UpdateForRender(id);
}

void RobotFrame::UpdateBaseToTool(int id)
{
	if (!m_RobotTool || !m_dataStorage->GetNamedNode(m_RobotTool->Name))
		return;
	vtkSmartPointer<vtkMatrix4x4> TImage2EndJoint = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkMatrix4x4> TEndJoint2Tool = vtkSmartPointer<vtkMatrix4x4>::New();
 	TImage2EndJoint->DeepCopy(this->m_EndJoint->GetBaseToLink());
	TEndJoint2Tool->DeepCopy(this->m_RobotTool->GetJointToTool());
	transform->PreMultiply();
	transform->SetMatrix(TImage2EndJoint);
	transform->Concatenate(TEndJoint2Tool);
	vtkSmartPointer<vtkMatrix4x4> TImage2RobotTool = vtkSmartPointer<vtkMatrix4x4>::New();
	transform->GetMatrix(TImage2RobotTool);
	//PrintDataHelper::CoutMatrix("TImage2RobotTool: ", TImage2RobotTool);
	m_RobotTool->UpdateBaseToTool(TImage2RobotTool);
}

void RobotFrame::UpdateForRender(int startId)
{
	for (int jointID = startId; jointID<m_RobotJoints.size(); ++jointID)
	{
		auto joint = m_RobotJoints[jointID];
		auto jointGeometry = m_dataStorage->GetNamedNode(joint->Name)->GetData()->GetGeometry();
		jointGeometry->SetIndexToWorldTransformByVtkMatrix(joint->GetBaseToLink());
		joint->GetJointAxesActor()->SetUserMatrix(joint->GetBaseToLink());
	}
	//updatae Tools
	if (m_RobotTool && m_dataStorage->GetNamedNode(m_RobotTool->Name))
	{
		m_dataStorage->GetNamedNode(m_RobotTool->Name)->GetData()->GetGeometry()->SetIndexToWorldTransformByVtkMatrix(m_RobotTool->GetBaseToTool());
		m_RobotTool->GetToolAxesActor()->SetUserMatrix(m_RobotTool->GetBaseToTool());
	}
}

vtkSmartPointer<vtkMatrix4x4> RobotFrame::CalculateJointToLinkMatrix(vtkMatrix4x4* TImage2PreJoint, vtkMatrix4x4* TImage2Joint)
{
	vtkSmartPointer<vtkMatrix4x4> TPreJoint2Image = vtkSmartPointer<vtkMatrix4x4>::New();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkMatrix4x4> TPreJoint2Joint = vtkSmartPointer<vtkMatrix4x4>::New();
	transform->PreMultiply();
	TPreJoint2Image->DeepCopy(TImage2PreJoint);
	TPreJoint2Image->Invert();
	transform->SetMatrix(TPreJoint2Image);
	transform->Concatenate(TImage2Joint);
	transform->Update();
	transform->GetMatrix(TPreJoint2Joint);
	return TPreJoint2Joint;
}

nlohmann::json RobotFrame::MatrixToJsonArray(vtkSmartPointer<vtkMatrix4x4> matrix)
{
	nlohmann::json jMatrix = nlohmann::json::array();
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			jMatrix.push_back(matrix->GetElement(i, j));
		}
	}
	return jMatrix;
}

void RobotFrame::SaveMatricesToJsonFile(const std::vector<RobotJoint*>& robotJoints, const std::string& filename)
{
	nlohmann::json j;
	for (size_t i = 0; i < robotJoints.size(); ++i) {
		j["Matrix" + std::to_string(i + 1)] = MatrixToJsonArray(robotJoints[i]->GetJointToLink());
	}

	std::ofstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return;
	}
	file << j.dump(4); // 将 JSON 数据写入文件，缩进4个空格
	file.close();
}

vtkSmartPointer<vtkMatrix4x4> RobotFrame::JsonArrayToMatrix(const nlohmann::json& jMatrix)
{
	vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			matrix->SetElement(i, j, jMatrix[i * 4 + j].get<double>());
		}
	}
	return matrix;
}

void RobotFrame::LoadMatricesFromJsonFile(const std::string& filename, std::vector<RobotJoint*>& roboJoints)
{
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error opening file: " << filename << std::endl;
		return;
	}

	nlohmann::json j;
	file >> j;
	file.close();

	size_t i = 0;
	for (nlohmann::json::iterator it = j.begin(); it != j.end() && i < roboJoints.size(); ++it, ++i) {
		vtkSmartPointer<vtkMatrix4x4> matrix = JsonArrayToMatrix(it.value());
		roboJoints[i]->UpdateJointToLink(matrix);
	}
}

vtkSmartPointer<vtkMatrix4x4> RobotFrame::CalculateTFemurImage2BaseMatrix()
{
	vtkSmartPointer<vtkMatrix4x4> TFemurImage2FemurRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemurImage2FemurRF->DeepCopy(PKAData::m_TFemurRF2FemurImage);
	TFemurImage2FemurRF->Invert();

	vtkSmartPointer<vtkMatrix4x4> TFemurRF2Camera = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemurRF2Camera->DeepCopy(PKAData::m_TCamera2FemurRF);
	TFemurRF2Camera->Invert();

	vtkSmartPointer<vtkMatrix4x4> TCamera2Base = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2Base->DeepCopy(CalculateTCamera2BaseMatrix());

	vtkSmartPointer<vtkMatrix4x4> TFemurImage2Base = vtkSmartPointer<vtkMatrix4x4>::New();
	TFemurImage2Base->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TFemurImage2FemurRF, TFemurRF2Camera, TCamera2Base));

	return TFemurImage2Base;
}

vtkSmartPointer<vtkMatrix4x4> RobotFrame::CalculateTCamera2BaseMatrix()
{
	vtkSmartPointer<vtkMatrix4x4> TCamera2BaseRF = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2BaseRF->DeepCopy(PKAData::m_TCamera2BaseRF);

	vtkSmartPointer<vtkMatrix4x4> TBaseRF2Base = vtkSmartPointer<vtkMatrix4x4>::New();
	TBaseRF2Base->DeepCopy(PKAData::m_TBaseRF2Base);

	vtkSmartPointer<vtkMatrix4x4> TCamera2Base = vtkSmartPointer<vtkMatrix4x4>::New();
	TCamera2Base->DeepCopy(CalculationHelper::PreConcatenateMatrixs(TCamera2BaseRF, TBaseRF2Base));
	return TCamera2Base;
}
