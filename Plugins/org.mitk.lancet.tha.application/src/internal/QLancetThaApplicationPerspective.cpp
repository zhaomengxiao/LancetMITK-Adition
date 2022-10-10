#include "QLancetThaApplicationPerspective.h"

QLancetThaApplicationPerspective::QLancetThaApplicationPerspective(QObject* parent)
	: QObject(parent)
{

}

void QLancetThaApplicationPerspective::CreateInitialLayout(berry::IPageLayout::Pointer layout)
{
	// Editors are placed for free.
	QString editorAreaId = layout->GetEditorArea();
	layout->SetFixed(true);
	
	// create folder with alignment "right"
	berry::IFolderLayout::Pointer right = layout->CreateFolder("right", berry::IPageLayout::RIGHT, 0.5f, editorAreaId);
	// add emptyview1 to the folder
	right->AddView("org.mitk.views.qctmarker");
	right->AddView("org.mitk.views.qimplantplanning");
	right->AddView("org.mitk.views.qlinkinghardware");
	right->AddView("org.mitk.views.qdisclaimeragreement");
	right->AddView("org.mitk.views.qmedicalrecordmanagement");
	right->AddView("org.mitk.views.qroboticsregistrations");

	right->AddView("org.mitk.views.qroboticsregistrationsaccuracy");
	right->AddView("org.mitk.views.qfemurmarkerpoint");
	right->AddView("org.mitk.views.qfemurroughregistrations");
	right->AddView("org.mitk.views.qfemurprecisionregistrations");
	right->AddView("org.mitk.views.qpelvismarkerpoint");
	right->AddView("org.mitk.views.qpelvisroughregistrations");
	right->AddView("org.mitk.views.qpelvisprecisionregistrations");
	right->AddView("org.mitk.views.qcontusionaccuracy");
	right->AddView("org.mitk.views.qcontusion");
	right->AddView("org.mitk.views.qputmolarcupprosthesis");
	right->AddView("org.mitk.views.qputmolarcupprosthesisaccuracy");
	right->AddView("org.mitk.views.qfemurcontusion");
	right->AddView("org.mitk.views.qputfemurprosthesisaccuracy");
	right->AddView("org.mitk.views.qpostoperativeoutcome");

	layout->AddPerspectiveShortcut("org.mitk.views.qctmarker");
	layout->AddPerspectiveShortcut("org.mitk.views.qimplantplanning");
	layout->AddPerspectiveShortcut("org.mitk.views.qlinkinghardware");
	layout->AddPerspectiveShortcut("org.mitk.views.qdisclaimeragreement");
	layout->AddPerspectiveShortcut("org.mitk.views.qroboticsregistrations");
	layout->AddPerspectiveShortcut("org.mitk.views.qmedicalrecordmanagement");
	layout->AddPerspectiveShortcut("org.mitk.views.qroboticsregistrationsaccuracy");
	layout->AddPerspectiveShortcut("org.mitk.views.qfemurmarkerpoint");
	layout->AddPerspectiveShortcut("org.mitk.views.qfemurroughregistrations");
	layout->AddPerspectiveShortcut("org.mitk.views.qfemurprecisionregistrations");
	layout->AddPerspectiveShortcut("org.mitk.views.qpelvismarkerpoint");
	layout->AddPerspectiveShortcut("org.mitk.views.qpelvisroughregistrations");
	layout->AddPerspectiveShortcut("org.mitk.views.qpelvisprecisionregistrations");
	layout->AddPerspectiveShortcut("org.mitk.views.qcontusionaccuracy");
	layout->AddPerspectiveShortcut("org.mitk.views.qcontusion");
	layout->AddPerspectiveShortcut("org.mitk.views.qputmolarcupprosthesis");
	layout->AddPerspectiveShortcut("org.mitk.views.qputmolarcupprosthesisaccuracy");
	layout->AddPerspectiveShortcut("org.mitk.views.qfemurcontusion");
	layout->AddPerspectiveShortcut("org.mitk.views.qputfemurprosthesisaccuracy");
	layout->AddPerspectiveShortcut("org.mitk.views.qpostoperativeoutcome");

	if (layout->GetViewLayout("org.mitk.views.qctmarker").IsNotNull())
	{
		layout->GetViewLayout("org.mitk.views.qctmarker")->SetMoveable(false);
		layout->GetViewLayout("org.mitk.views.qctmarker")->SetCloseable(false);
	}
}
