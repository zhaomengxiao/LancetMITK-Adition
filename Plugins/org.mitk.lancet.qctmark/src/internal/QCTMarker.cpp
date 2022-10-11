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
#include "QCTMarker.h"

// VTK
#include <vtkPointSet.h>

// Qt
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QCheckBox>
#include <QmitkRenderingManager.h>

// mitk image
#include <mitkImage.h>

// THA
//#include "Log.h"
//#include "IGTController.h"

const std::string QCTMarker::VIEW_ID = "org.mitk.views.qctmarker";

void QCTMarker::SetFocus()
{
}

void QCTMarker::CreateQtPartControl(QWidget* parent)
{
	// create GUI widgets from the Qt Designer's .ui file
	m_Controls.setupUi(parent);
	auto test_dir = QDir(":/org.mitk.lancet.qctmark/");
	QFile qss(test_dir.absoluteFilePath("qctmark.qss"));
	if (!qss.open(QIODevice::ReadOnly))
	{
		qWarning() << __func__ << __LINE__ << ":" << "error load file "
			<< test_dir.absoluteFilePath("qctmark.qss") << "\n"
			<< "error: " << qss.errorString();
	}
	// pos
	qInfo() << "log.file.pos " << qss.pos();
	m_Controls.widget->setStyleSheet(QLatin1String(qss.readAll()));
	qss.close();
	lastPoint = 0;
	checkBoxList << m_Controls.checkBox_1 << m_Controls.checkBox_2
		<< m_Controls.checkBox_3 << m_Controls.checkBox_4
		<< m_Controls.checkBox_5 << m_Controls.checkBox_6
		<< m_Controls.checkBox_7 << m_Controls.checkBox_8;
	for (int i = 0; i < checkBoxList.size(); i++)
	{
		connect(checkBoxList[i], SIGNAL(clicked(bool)), this, SLOT(toggled(bool)));
		checkBoxList[i]->setEnabled(true);
	}

	connect(m_Controls.pushButtonCapturePoint, &QPushButton::clicked, this, &QCTMarker::onButtonCapturePoint_clicked);
	connect(m_Controls.pushButtonClean, &QPushButton::clicked, this, &QCTMarker::onButtonClean_clicked);
	connect(m_Controls.pushButtonShowDistance, &QPushButton::clicked, this, &QCTMarker::onButtonShowDistance_clicked);
	connect(m_Controls.pushButtonClearDistance, &QPushButton::clicked, this, &QCTMarker::onButtonClearDistance_clicked);
}

void QCTMarker::OnSelectionChanged(berry::IWorkbenchPart::Pointer /*source*/,
	const QList<mitk::DataNode::Pointer>& nodes)
{
	// iterate all selected objects, adjust warning visibility
}

void QCTMarker::onButtonCapturePoint_clicked()
{
	for (int i = 0; i < checkBoxList.size(); i++)
	{
		checkBoxList[i]->setEnabled(true);
	}
	MITK_INFO << "QCTMarker:" << __func__ << ": log";

	for (int i = 0; i < checkBoxList.size(); i++)
	{
		if (checkBoxList[i]->isChecked())
		{
			mitk::RenderingManager* pManager = mitk::RenderingManager::GetInstance();
			mitk::DataStorage* pDs = pManager->GetDataStorage();
			mitk::DataNode::Pointer mypointSetNode = pDs->GetNamedNode("mypointSet");
			mitk::Vector3D myposition;
			mitk::PointSet::Pointer posteriorNode = nullptr;
			mitk::PointSet::Pointer anteriorNode = nullptr;
			mitk::PointSet::Pointer superiorNode = nullptr;
			//if (IGTController::side == RIGHT)
			//{
			//	posteriorNode = pDs->GetNamedObject<mitk::PointSet>("posterior_landmark_right");
			//	anteriorNode = pDs->GetNamedObject<mitk::PointSet>("anterior_landmark_right");
			//	superiorNode = pDs->GetNamedObject<mitk::PointSet>("superior_landmark_right");
			//}
			//else
			//{
			//	posteriorNode = pDs->GetNamedObject<mitk::PointSet>("posterior_landmark_left");
			//	anteriorNode = pDs->GetNamedObject<mitk::PointSet>("anterior_landmark_left");
			//	superiorNode = pDs->GetNamedObject<mitk::PointSet>("superior_landmark_left");
			//}
			mitk::PointSet::Pointer AsisPoint1 = pDs->GetNamedObject<mitk::PointSet>("AsisPoint1");
			mitk::PointSet::Pointer AsisPoint2 = pDs->GetNamedObject<mitk::PointSet>("AsisPoint2");
			mitk::PointSet::Pointer asisNode = pDs->GetNamedObject<mitk::PointSet>("ASIS");
			mitk::PointSet::Pointer trochanter = pDs->GetNamedObject<mitk::PointSet>("trochanter");
			mitk::PointSet::Pointer PLAN_trochanter1 = pDs->GetNamedObject<mitk::PointSet>("PLAN_trochanter1");
			mitk::PointSet::Pointer PLAN_trochanter2 = pDs->GetNamedObject<mitk::PointSet>("PLAN_trochanter2");
			mitk::PointSet::Pointer mypointSet = pDs->GetNamedObject<mitk::PointSet>("mypointSet");
			mitk::PointSet::Pointer pelvis_midline = pDs->GetNamedObject<mitk::PointSet>("pelvis_midline");
			mitk::PointSet::Pointer hip_length_right = pDs->GetNamedObject<mitk::PointSet>("hip_length_right");
			mitk::PointSet::Pointer hip_length_left = pDs->GetNamedObject<mitk::PointSet>("hip_length_left");
			if (mypointSet->GetPoint(0)[0] == 0 && mypointSet->GetPoint(0)[1] == 0 && mypointSet->GetPoint(0)[2] == 0)
			{
				QMessageBox::warning(nullptr, tr("错误"), tr("捕获点不正确"));
				mypointSet->Clear();
				return;
			}
			switch (i)
			{
			case 0:
			{
				myposition[0] = mypointSet->GetPoint(0)[0];
				myposition[1] = mypointSet->GetPoint(0)[1];
				myposition[2] = mypointSet->GetPoint(0)[2];

				//AsisPoint1->InsertPoint(0, myposition);
				double myposition_stem_tempAsisPoint[3]{ myposition[0], myposition[1], myposition[2] };
				AsisPoint1->InsertPoint(0, myposition_stem_tempAsisPoint);

				//asisNode->SetPoint(1, myposition);
				double myasisNode_stem_tempAsisNode[3]{ myposition[0], myposition[1], myposition[2] };
				asisNode->InsertPoint(1, myasisNode_stem_tempAsisNode);
				mypointSet->Clear();
				QMessageBox::about(nullptr, tr("成功"), tr("捕获点成功"));

				lastPoint = 1;
			}
			break;
			case 1:
			{
				myposition[0] = mypointSet->GetPoint(0)[0];
				myposition[1] = asisNode->GetPoint(1)[1];
				myposition[2] = asisNode->GetPoint(1)[2];

				//AsisPoint2->InsertPoint(0, myposition);
				double myposition_stem_tempAsisPoint[3]{ myposition[0], myposition[1], myposition[2] };
				AsisPoint2->InsertPoint(0, myposition_stem_tempAsisPoint);

				//asisNode->SetPoint(0, myposition);
				double myasisNode_stem_tempAsisNode[3]{ myposition[0], myposition[1], myposition[2] };
				asisNode->InsertPoint(0, myasisNode_stem_tempAsisNode);

				if (AsisPoint2->GetPoint(0)[0] - asisNode->GetPoint(1)[0] <= 1)
				{

					mypointSet->Clear();
					checkBoxList[1]->setChecked(false);
					QMessageBox::about(nullptr, tr("成功"), tr("捕获点成功"));
				}
				else
				{
					mypointSet->Clear();
					QMessageBox::about(nullptr, tr("失败"), tr("两个点不在一条直线上"));
				}
				lastPoint = 2;
			}
			break;
			case 2:
			{
				myposition[0] = mypointSet->GetPoint(0)[0];
				myposition[1] = asisNode->GetPoint(0)[1];
				myposition[2] = asisNode->GetPoint(0)[2];

				//pelvis_midline->SetPoint(0, myposition);
				double myposition_stem_tempPelvis_midline_one[3]{ myposition[0], myposition[1], myposition[2] };
				pelvis_midline->InsertPoint(0, myposition_stem_tempPelvis_midline_one);


				myposition[0] = mypointSet->GetPoint(0)[0];
				myposition[1] = asisNode->GetPoint(0)[1];
				myposition[2] = asisNode->GetPoint(0)[2] - 100;
				//pelvis_midline->SetPoint(1, myposition);
				double myposition_stem_tempPelvis_midline[3]{ myposition[0], myposition[1], myposition[2] };
				pelvis_midline->InsertPoint(1, myposition_stem_tempPelvis_midline);

				myposition[0] = hip_length_right->GetPoint(0)[0];
				myposition[2] = asisNode->GetPoint(0)[2];

				//hip_length_right->SetPoint(0, myposition);
				double myposition_stem_tempPelvis_Hip_length_right_one[2]{ myposition[0], myposition[2] };
				hip_length_right->InsertPoint(0, myposition_stem_tempPelvis_Hip_length_right_one);

				myposition[0] = hip_length_right->GetPoint(1)[0];
				myposition[2] = asisNode->GetPoint(0)[2] - 104;
				//hip_length_right->SetPoint(1, myposition);
				double myposition_stem_tempPelvis_Hip_length_right[2]{ myposition[0], myposition[2] };
				hip_length_right->InsertPoint(1, myposition_stem_tempPelvis_Hip_length_right);

				myposition[0] = hip_length_left->GetPoint(0)[0];
				myposition[2] = asisNode->GetPoint(0)[2];

				//hip_length_left->SetPoint(0, myposition);
				double myposition_stem_tempPelvis_Hip_length_left_one[2]{ myposition[0], myposition[2] };
				hip_length_left->InsertPoint(0, myposition_stem_tempPelvis_Hip_length_left_one);

				myposition[0] = hip_length_left->GetPoint(1)[0];
				myposition[2] = asisNode->GetPoint(0)[2] - 104;

				//hip_length_left->SetPoint(1, myposition);
				double myposition_stem_tempPelvis_Hip_length_left[2]{ myposition[0], myposition[2] };
				hip_length_left->InsertPoint(1, myposition_stem_tempPelvis_Hip_length_left);


				mypointSet->Clear();
				checkBoxList[2]->setCheckState(Qt::Unchecked);
				QMessageBox::about(nullptr, tr("成功"), tr("捕获点成功"));

				lastPoint = 3;
			}
			break;
			case 3:
			{
				myposition[0] = mypointSet->GetPoint(0)[0];
				myposition[1] = mypointSet->GetPoint(0)[1];
				myposition[2] = mypointSet->GetPoint(0)[2];

				//posteriorNode->SetPoint(0, myposition);
				double myposition_stem_tempPosteriorNode[3]{ myposition[0], myposition[1], myposition[2] };
				posteriorNode->InsertPoint(0, myposition_stem_tempPosteriorNode);
				mypointSet->Clear();
				checkBoxList[3]->setChecked(false);
				QMessageBox::about(nullptr, tr("成功"), tr("捕获点成功"));

				lastPoint = 4;
			}
			break;
			case 4:
			{
				myposition[0] = mypointSet->GetPoint(0)[0];
				myposition[1] = mypointSet->GetPoint(0)[1];
				myposition[2] = mypointSet->GetPoint(0)[2];
				//anteriorNode->SetPoint(0, myposition);
				double myposition_stem_tempAnteriorNode[3]{ myposition[0], myposition[1], myposition[2] };
				anteriorNode->InsertPoint(0, myposition_stem_tempAnteriorNode);
				mypointSet->Clear();
				checkBoxList[4]->setChecked(false);
				QMessageBox::about(nullptr, tr("成功"), tr("捕获点成功"));

				lastPoint = 5;
			}
			break;
			case 5:
			{
				myposition[0] = mypointSet->GetPoint(0)[0];
				myposition[1] = mypointSet->GetPoint(0)[1];
				myposition[2] = mypointSet->GetPoint(0)[2];
				//superiorNode->SetPoint(0, myposition);
				double myposition_stem_tempSuperiorNode[3]{ myposition[0], myposition[1], myposition[2] };
				superiorNode->InsertPoint(0, myposition_stem_tempSuperiorNode);

				mypointSet->Clear();
				checkBoxList[5]->setChecked(false);
				QMessageBox::about(nullptr, tr("成功"), tr("捕获点成功"));

				lastPoint = 6;
			}
			break;
			case 6:
			{
				myposition[0] = mypointSet->GetPoint(0)[0];
				myposition[1] = mypointSet->GetPoint(0)[1];
				myposition[2] = mypointSet->GetPoint(0)[2];

				//PLAN_trochanter1->InsertPoint(0, myposition);
				double myposition_stem_tempPLAN_trochanter1[3]{ myposition[0], myposition[1], myposition[2] };
				PLAN_trochanter1->InsertPoint(0, myposition_stem_tempPLAN_trochanter1);

				//trochanter->SetPoint(1, myposition);
				double myposition_stem_tempTrochanter[3]{ myposition[0], myposition[1], myposition[2] };
				trochanter->InsertPoint(1, myposition_stem_tempTrochanter);
				mypointSet->Clear();
				checkBoxList[6]->setChecked(false);
				QMessageBox::about(nullptr, tr("成功"), tr("捕获点成功"));

				lastPoint = 7;
			}
			break;
			case 7:
			{
				myposition[0] = mypointSet->GetPoint(0)[0];
				myposition[1] = mypointSet->GetPoint(0)[1];
				myposition[2] = mypointSet->GetPoint(0)[2];
				//PLAN_trochanter2->InsertPoint(0, myposition);
				double myposition_stem_tempPLAN_trochanter2[3]{ myposition[0], myposition[1], myposition[2] };
				PLAN_trochanter2->InsertPoint(0, myposition_stem_tempPLAN_trochanter2);

				//trochanter->SetPoint(0, myposition);
				double myposition_stem_tempTrochanter[3]{ myposition[0], myposition[1], myposition[2] };
				trochanter->InsertPoint(0, myposition_stem_tempTrochanter);
				mypointSet->Clear();
				checkBoxList[7]->setChecked(false);
				lastPoint++;
				QMessageBox::about(nullptr, tr("成功"), tr("捕获点成功"));

			}
			break;
			default:
				break;
			}
			pDs->Remove(mypointSetNode);
		}
	}
	//IGTController::getInStance()->CreateCalculateMode();
	//IGTController::getInStance()->GetPlanHipLength();
	//IGTController::getInStance()->GetPlanCombinedOffset();
}

void QCTMarker::onButtonClean_clicked()
{
	MITK_INFO << "QCTMarker:" << __func__ << ": log";
	for (int i = 0; i < checkBoxList.size(); i++)
	{
		checkBoxList[i]->setEnabled(false);
		if (checkBoxList[i]->isChecked())
		{
			checkBoxList[i]->setEnabled(true);
			mitk::RenderingManager* mypManager = mitk::RenderingManager::GetInstance();
			;			mitk::DataStorage* pDs = mypManager->GetDataStorage();
			mitk::PointSet::Pointer posteriorNode = nullptr;
			mitk::PointSet::Pointer anteriorNode = nullptr;
			mitk::PointSet::Pointer superiorNode = nullptr;
			//if (IGTController::side == RIGHT)
			//{
			//	posteriorNode = pDs->GetNamedObject<mitk::PointSet>("posterior_landmark_right");
			//	anteriorNode = pDs->GetNamedObject<mitk::PointSet>("anterior_landmark_right");
			//	superiorNode = pDs->GetNamedObject<mitk::PointSet>("superior_landmark_right");
			//}
			//else
			//{
			//	posteriorNode = pDs->GetNamedObject<mitk::PointSet>("posterior_landmark_left");
			//	anteriorNode = pDs->GetNamedObject<mitk::PointSet>("anterior_landmark_left");
			//	superiorNode = pDs->GetNamedObject<mitk::PointSet>("superior_landmark_left");
			//}
			mitk::PointSet::Pointer asisNode = pDs->GetNamedObject<mitk::PointSet>("ASIS");
			mitk::PointSet::Pointer trochanter = pDs->GetNamedObject<mitk::PointSet>("trochanter");
			mitk::PointSet::Pointer AsisPoint1 = pDs->GetNamedObject<mitk::PointSet>("AsisPoint1");
			mitk::PointSet::Pointer AsisPoint2 = pDs->GetNamedObject<mitk::PointSet>("AsisPoint2");
			mitk::PointSet::Pointer PLAN_trochanter1 = pDs->GetNamedObject<mitk::PointSet>("PLAN_trochanter1");
			mitk::PointSet::Pointer PLAN_trochanter2 = pDs->GetNamedObject<mitk::PointSet>("PLAN_trochanter2");
			mitk::PointSet::Pointer mypointSet = pDs->GetNamedObject<mitk::PointSet>("mypointSet");
			mitk::PointSet::Pointer pelvis_midline = pDs->GetNamedObject<mitk::PointSet>("pelvis_midline");
			switch (i)
			{
			case 0:
				AsisPoint1->Clear();
				break;
			case 1:
				AsisPoint2->Clear();
				break;
			case 2:
				pelvis_midline->Clear();
				break;
			case 3:
				posteriorNode->Clear();
				break;
			case 4:
				anteriorNode->Clear();
				break;
			case 5:
				superiorNode->Clear();
				break;
			case 6:
				PLAN_trochanter1->Clear();
				break;
			case 7:
				PLAN_trochanter2->Clear();
				break;
			default:
				break;
			}
		}
	}
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}

void QCTMarker::on_initial()
{
	bool mRet = false;
	mitk::PointSetDataInteractor::Pointer m_interactor = mitk::PointSetDataInteractor::New();
	//x = itk::SmartPointer<uFunAISinteractive>(uFunAISinteractive::New());
	//x->f_SetUID("true");
	try
	{
		mRet = m_interactor->LoadStateMachine("PointSet.xml");
		mRet = m_interactor->SetEventConfig("PointSetConfig.xml");
	}
	catch (const mitk::Exception& e)
	{
		qDebug() << QString::fromStdString(e.GetDescription());
	}
	mitk::PointSet::Pointer mypointSet = mitk::PointSet::New();
	mitk::DataNode::Pointer mypointSetNode = mitk::DataNode::New();
	mypointSetNode->SetData(mypointSet);
	mypointSetNode->SetName("mypointSet");
	mypointSetNode->SetFloatProperty("pointsize", 6);
	mypointSetNode->SetProperty("selectedcolor", mitk::ColorProperty::New(1.0f, 0.0f, 0.0f));
	mitk::RenderingManager* mypManager = mitk::RenderingManager::GetInstance();
	mitk::DataStorage* pDs = mypManager->GetDataStorage();
	mypointSetNode->SetData(mypointSet);
	pDs->Add(mypointSetNode);
	m_interactor->SetDataNode(mypointSetNode);
}
void QCTMarker::toggled(bool)
{
	mitk::RenderingManager* mypManager = mitk::RenderingManager::GetInstance();
	mitk::DataStorage* pDs = mypManager->GetDataStorage();
	mitk::PointSet::Pointer mypointSet = pDs->GetNamedObject<mitk::PointSet>("mypointSet");
	mitk::DataNode::Pointer mypointSetNode = pDs->GetNamedNode("mypointSet");
	if (mypointSet)
	{
		pDs->Remove(mypointSetNode);
	}
	on_initial();

	for (int i = 0; i < checkBoxList.size(); i++)
	{
		if (checkBoxList[i]->isChecked())
		{
			switch (i)
			{
			case 0:
				emit signalCapture(0, 0);
				break;
			case 1:
				emit signalCapture(1, 1);
				break;
			case 2:
				emit signalCapture(2, 2);
				break;
			case 3:
				emit signalCapture(3, 3);
				break;
			case 4:
				emit signalCapture(4, 4);
				break;
			case 5:
				emit signalCapture(5, 5);
				break;
			case 6:
				emit signalCapture(6, 6);
				break;
			case 7:
				emit signalCapture(7, 7);
				break;
			default:
				break;
			}
		}
	}
}

void QCTMarker::onButtonShowDistance_clicked()
{
	MITK_INFO << "QCTMarker:" << __func__ << ": log";
	QMessageBox::warning(nullptr, tr("提示"), tr("请选择两个点进行距离测量"));
	mitk::RenderingManager* mypManager = mitk::RenderingManager::GetInstance();
	mitk::DataStorage* pDs = mypManager->GetDataStorage();
	on_initial();
	mitk::DataNode::Pointer mypointSetNode = pDs->GetNamedNode("mypointSet");
	//mitk::DataNode::Pointer mypointSetNode = pDs->GetNamedNode("mypointSet");
	mypointSetNode->SetProperty("show contour", mitk::BoolProperty::New(true));
	mypointSetNode->SetProperty("show distances", mitk::BoolProperty::New(true));
	mypointSetNode->SetProperty("show distant lines", mitk::BoolProperty::New(true));
}

void QCTMarker::onButtonClearDistance_clicked()
{
	MITK_INFO << "QCTMarker:" << __func__ << ": log";
	QMessageBox::warning(nullptr, tr("提示"), tr("请选择两个点进行距离测量"));
	mitk::RenderingManager* mypManager = mitk::RenderingManager::GetInstance();
	mitk::DataStorage* pDs = mypManager->GetDataStorage();
	mitk::PointSet::Pointer mypointSet = pDs->GetNamedObject<mitk::PointSet>("mypointSet");
	mitk::DataNode::Pointer mypointSetNode = pDs->GetNamedNode("mypointSet");
	if (mypointSetNode)
	{
		mypointSetNode->SetProperty("show contour", mitk::BoolProperty::New(false));
		mypointSetNode->SetProperty("show distances", mitk::BoolProperty::New(false));
		mypointSetNode->SetProperty("show distant lines", mitk::BoolProperty::New(false));
		pDs->Remove(mypointSetNode);
	}
	mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}