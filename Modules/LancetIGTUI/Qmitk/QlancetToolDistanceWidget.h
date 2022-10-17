/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef QLANCETTOOLDISTANCEWIDGET_H
#define QLANCETTOOLDISTANCEWIDGET_H

#include "ui_QmitkToolDistanceWidgetControls.h"
#include "MitkLancetIGTUIExports.h"

#include <itkProcessObject.h>
#include <QLabel>
#include <QVector>

#include "mitkNavigationData.h"

/*!
\brief QmitkToolDistanceWidget

Widget for setting up and controlling an update timer in an IGT-Pipeline.

*/
class MITKLANCETIGTUI_EXPORT QlancetToolDistanceWidget : public QWidget
{
  Q_OBJECT // this is needed for all Qt objects that should have a MOC object (everything that derives from QObject)
public:

  typedef QVector<QVector<QLabel*> > DistanceLabelType;

  /*!
  \brief default constructor
  */
  QlancetToolDistanceWidget( QWidget* parent );

  /*!
  \brief default destructor
  */
  ~QlancetToolDistanceWidget() override;

  /*!
  \brief This method displays the matrix with the distances between the tracking source's outputs in a QGridLayout.
  */
  void ShowDistanceValues(itk::ProcessObject::DataObjectPointerArray & outputs);

  /*!
  \brief This method displays the matrix with the distances between the tracking source's outputs in a QGridLayout.
  */
  void ShowDistanceValues(std::vector<mitk::NavigationData::Pointer>* navDatas);

  /*!
  \brief This method creates the initial distances matrix and labels it with the connected tool names.
  */
  void CreateToolDistanceMatrix(itk::ProcessObject::DataObjectPointerArray & outputs);

  /*!
  \brief This method creates the initial distances matrix and labels it with the connected tool names.
  */
  void CreateToolDistanceMatrix(std::vector<mitk::NavigationData::Pointer>* navDatas);

  /*!
  \brief This method clears the whole tool distances matrix
  */
  void ClearDistanceMatrix();

public slots:
  /*!
  \brief This method set's all distance entries in the matrix to "---". Can be used e.g. if tracking is stopped.
  */
  void SetDistanceLabelValuesInvalid();

protected:
  void CreateConnections();
  void CreateQtPartControl( QWidget *parent );
  Ui::QmitkToolDistanceWidgetControls* m_Controls;  ///< gui widgets


private:
  /*!
  \brief Double vector for all between tool distances labels.
  */
  DistanceLabelType* m_DistanceLabels;

  std::vector<mitk::NavigationData::Pointer>* m_NavDatas;

};

#endif // QLANCETTOOLDISTANCEWIDGET_H
