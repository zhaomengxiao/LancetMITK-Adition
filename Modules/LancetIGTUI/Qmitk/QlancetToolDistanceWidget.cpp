/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#include "QlancetToolDistanceWidget.h"

#include <QGridLayout>
#include <itkDataObject.h>
#include <mitkNavigationDataSource.h>
#include <cmath>




QlancetToolDistanceWidget::QlancetToolDistanceWidget(QWidget* parent)
: QWidget(parent), m_Controls(nullptr), m_DistanceLabels(nullptr)
{
  this->CreateQtPartControl( this );
}

QlancetToolDistanceWidget::~QlancetToolDistanceWidget()
{
  ClearDistanceMatrix();
  delete m_DistanceLabels;
  m_DistanceLabels = nullptr;
  m_Controls = nullptr;
}

void QlancetToolDistanceWidget::CreateQtPartControl(QWidget *parent)
{
  if (!m_Controls)
  {
    // create GUI widgets
    m_Controls = new Ui::QmitkToolDistanceWidgetControls;
    m_Controls->setupUi(parent);
    m_Controls->m_StatusLabel->setText(QString("No tracking tools connected. Please set up a connection first."));

    this->CreateConnections();
  }
}

void QlancetToolDistanceWidget::CreateConnections()
{

}

void QlancetToolDistanceWidget::CreateToolDistanceMatrix(itk::ProcessObject::DataObjectPointerArray & outputs)
{

  if(outputs.size() > 1)
  {
    this->show();

    mitk::NavigationData* navData;


    if(m_DistanceLabels == nullptr)
    {
      m_DistanceLabels = new DistanceLabelType;
    }

    if(m_DistanceLabels->isEmpty())
    {
      this->m_Controls->m_StatusLabel->setText("");

      QLabel* label;


      // labeling of matrix
      for (unsigned int i = 0; i < outputs.size()-1; i++)
      {
        navData = dynamic_cast<mitk::NavigationData*>(outputs.at(i).GetPointer());
        label = new QLabel(navData->GetName(),this);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        this->m_Controls->m_GridLayout->addWidget(label,i+1,0);

        navData = dynamic_cast<mitk::NavigationData*>(outputs.at(i+1).GetPointer());
        label = new QLabel(navData->GetName(),this);
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        this->m_Controls->m_GridLayout->addWidget(label,0,i+1);
      }


      for(unsigned int i = 0; i < outputs.size()-1; i++)
      {
        QVector<QLabel*>* rowDistances = new QVector<QLabel*>();

        for(unsigned int j = i+1; j < outputs.size(); j++)
        {
          // distance labels initializing
          label = new QLabel(QString("---"), this);
          label->setFrameStyle(QFrame::Box | QFrame::Sunken);
          label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
          rowDistances->append(label);

          //initial distance label adding to the QGridLayout
          this->m_Controls->m_GridLayout->addWidget(label,i+1,j);
        }
        this->m_DistanceLabels->append(*rowDistances);
      }
    }
  }
  else
  {
    this->m_Controls->m_StatusLabel->setText(QString("For distance information please connect at least two tools"));
  }

}

void QlancetToolDistanceWidget::CreateToolDistanceMatrix(std::vector<mitk::NavigationData::Pointer>* navDatas)
{
  if (navDatas->size() > 1)
  {
    this->show();

    mitk::NavigationData* navData;


    if (m_DistanceLabels == nullptr)
    {
      m_DistanceLabels = new DistanceLabelType;
    }

    if (m_DistanceLabels->isEmpty())
    {
      this->m_Controls->m_StatusLabel->setText("");

      QLabel* label;


      // labeling of matrix
      for (unsigned int i = 0; i < navDatas->size() - 1; i++)
      {
        navData = navDatas->at(i);
        label = new QLabel(navData->GetName(), this);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        this->m_Controls->m_GridLayout->addWidget(label, i + 1, 0);

        navData = navDatas->at(i+1);
        label = new QLabel(navData->GetName(), this);
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        this->m_Controls->m_GridLayout->addWidget(label, 0, i + 1);
      }


      for (unsigned int i = 0; i < navDatas->size() - 1; i++)
      {
        QVector<QLabel*>* rowDistances = new QVector<QLabel*>();

        for (unsigned int j = i + 1; j < navDatas->size(); j++)
        {
          // distance labels initializing
          label = new QLabel(QString("---"), this);
          label->setFrameStyle(QFrame::Box | QFrame::Sunken);
          label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
          rowDistances->append(label);

          //initial distance label adding to the QGridLayout
          this->m_Controls->m_GridLayout->addWidget(label, i + 1, j);
        }
        this->m_DistanceLabels->append(*rowDistances);
      }
    }
  }
  else
  {
    this->m_Controls->m_StatusLabel->setText(QString("For distance information please connect at least two tools"));
  }
}


void QlancetToolDistanceWidget::ShowDistanceValues(itk::ProcessObject::DataObjectPointerArray & outputs)
  {

    mitk::NavigationData* navData;
    mitk::NavigationData* nextNavData;

    for(int i=0; i < m_DistanceLabels->size(); i++)
    {
      int j = i+1;

      for(int k=0; k < m_DistanceLabels->at(i).size(); k++)
      {
        navData = dynamic_cast<mitk::NavigationData*>(outputs.at(i).GetPointer());
        nextNavData = dynamic_cast<mitk::NavigationData*>(outputs.at(j++).GetPointer());

        if(navData == nullptr || nextNavData == nullptr)
          return;

        mitk::NavigationData::PositionType::RealType distance =  navData->GetPosition().EuclideanDistanceTo(nextNavData->GetPosition());
        distance = floor(distance * 10.) / 10.;
        QString distanceStr;
        if(!navData->IsDataValid() || !nextNavData->IsDataValid())
        {
          distanceStr = "---";
        }
        else
        {
          distanceStr.setNum(distance);
          distanceStr += " mm";
        }
        this->m_DistanceLabels->at(i).at(k)->setText(distanceStr);
      }
    }
  }

void QlancetToolDistanceWidget::ShowDistanceValues(std::vector<mitk::NavigationData::Pointer>* navDatas)
{
  mitk::NavigationData* navData;
  mitk::NavigationData* nextNavData;

  for (int i = 0; i < m_DistanceLabels->size(); i++)
  {
    int j = i + 1;

    for (int k = 0; k < m_DistanceLabels->at(i).size(); k++)
    {
      navData = navDatas->at(i);
      nextNavData = navDatas->at(j++);

      if (navData == nullptr || nextNavData == nullptr)
        return;

      mitk::NavigationData::PositionType::RealType distance = navData->GetPosition().EuclideanDistanceTo(nextNavData->GetPosition());
      distance = floor(distance * 10.) / 10.;
      QString distanceStr;
      if (!navData->IsDataValid() || !nextNavData->IsDataValid())
      {
        distanceStr = "---";
      }
      else
      {
        distanceStr.setNum(distance);
        distanceStr += " mm";
      }
      this->m_DistanceLabels->at(i).at(k)->setText(distanceStr);
    }
  }
}

void QlancetToolDistanceWidget::ClearDistanceMatrix()
  {

    while(m_Controls->m_GridLayout->count() > 0)
    {
      QWidget* widget = m_Controls->m_GridLayout->itemAt(0)->widget();
      m_Controls->m_GridLayout->removeWidget(widget);
      delete widget;
    }
   delete this->m_DistanceLabels;
   this->m_DistanceLabels = nullptr;

   this->m_Controls->m_StatusLabel->setText(QString("For distance information please set up the connection again."));

  }

  void QlancetToolDistanceWidget::SetDistanceLabelValuesInvalid()
  {
    for(int i = 0; i < m_DistanceLabels->size(); i++)
    {
      for(int j= 0; j < m_DistanceLabels->at(i).size(); j++)
      {
        this->m_DistanceLabels->at(i).at(j)->setText(QString("---"));
      }
    }
  }




