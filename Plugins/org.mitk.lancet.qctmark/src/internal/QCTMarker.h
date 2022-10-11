/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/


#ifndef QCTMarker_h
#define QCTMarker_h

#include <berryISelectionListener.h>

#include <QmitkAbstractView.h>

#include "ui_QCTMarkerControls.h"

#include <mitkPointSetDataInteractor.h>

#include "mitkPointSet.h"
/**
  \brief QCTMarker

  \warning  This class is not yet documented. Use "git blame" and ask the author to provide basic documentation.

  \sa QmitkAbstractView
  \ingroup ${plugin_target}_internal
*/
class QCTMarker : public QmitkAbstractView
{
	// this is needed for all Qt objects that should have a Qt meta-object
	// (everything that derives from QObject and wants to have signal/slots)
	Q_OBJECT

public:
	static const std::string VIEW_ID;

	void on_initial();

Q_SIGNALS:
	void signalCapture(int, int);

public Q_SLOTS:
	void onButtonCapturePoint_clicked();
	void onButtonClean_clicked();
	void onButtonShowDistance_clicked();
	void onButtonClearDistance_clicked();
	void toggled(bool);
protected:
	virtual void CreateQtPartControl(QWidget* parent) override;

	virtual void SetFocus() override;

	/// \brief called by QmitkFunctionality when DataManager's selection has changed
	virtual void OnSelectionChanged(berry::IWorkbenchPart::Pointer source,
		const QList<mitk::DataNode::Pointer>& nodes) override;

	/// \brief Called when the user clicks the GUI button

	Ui::QCTMarkerControls m_Controls;
	QList<QCheckBox*> checkBoxList;
	int lastPoint;
};

#endif // QCTMarker_h
