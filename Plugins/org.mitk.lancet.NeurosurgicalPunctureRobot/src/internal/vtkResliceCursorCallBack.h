#pragma once
#include "vtkInclude.h"
class vtkResliceCursorCallBack : public vtkCommand
{
public:
	static vtkResliceCursorCallBack* New() { return new vtkResliceCursorCallBack; }


	void Execute(vtkObject* caller, unsigned long aEvent, void* callData) override
	{
		if (aEvent == vtkResliceCursorWidget::WindowLevelEvent || aEvent == vtkCommand::WindowLevelEvent
			|| aEvent == vtkResliceCursorWidget::ResliceThicknessChangedEvent)
		{
			for (int i = 0; i < 3; ++i)
			{
				this->ResliceCursorWidget[i]->Render();
			}
			this->ImagePlaneWidget[0]->GetInteractor()->GetRenderWindow()->Render();
			return;
		}

		vtkImagePlaneWidget* imagePlaneWidget = dynamic_cast<vtkImagePlaneWidget*>(caller);
		if (imagePlaneWidget)
		{
			double* wl = static_cast<double*>(callData);

			if (imagePlaneWidget == this->ImagePlaneWidget[0])
			{
				this->ImagePlaneWidget[1]->SetWindowLevel(wl[0], wl[1], 1);
				this->ImagePlaneWidget[2]->SetWindowLevel(wl[0], wl[1], 1);
			}
			else if (imagePlaneWidget == this->ImagePlaneWidget[1])
			{
				this->ImagePlaneWidget[0]->SetWindowLevel(wl[0], wl[1], 1);
				this->ImagePlaneWidget[2]->SetWindowLevel(wl[0], wl[1], 1);
			}
			else if (imagePlaneWidget == this->ImagePlaneWidget[2])
			{
				this->ImagePlaneWidget[0]->SetWindowLevel(wl[0], wl[1], 1);
				this->ImagePlaneWidget[1]->SetWindowLevel(wl[0], wl[1], 1);
			}
		}

		vtkResliceCursorWidget* resliceCursorWidget = dynamic_cast<vtkResliceCursorWidget*>(caller);
		if (ResliceCursorWidget)
		{
			vtkResliceCursorLineRepresentation* rep = dynamic_cast<vtkResliceCursorLineRepresentation*>(resliceCursorWidget->GetRepresentation());

			rep->GetResliceCursorActor()->GetCursorAlgorithm()->GetResliceCursor();
			for (int i = 0; i < 3; ++i)
			{
				vtkPlaneSource* ps = static_cast<vtkPlaneSource*>(this->ImagePlaneWidget[i]->GetPolyDataAlgorithm());
				ps->SetOrigin
				(this->ResliceCursorWidget[i]->GetResliceCursorRepresentation()->GetPlaneSource()->GetOrigin());
				ps->SetPoint1
				(this->ResliceCursorWidget[i]->GetResliceCursorRepresentation()->GetPlaneSource()->GetPoint1());
				ps->SetPoint2 
				(this->ResliceCursorWidget[i]->GetResliceCursorRepresentation()->GetPlaneSource()->GetPoint2());
			}
		}

		for (int i = 0; i < 3; ++i)
		{
			this->ResliceCursorWidget[i]->Render();
		}
		this->ImagePlaneWidget[0]->GetInteractor()->GetRenderWindow()->Render();
	}

public:
	vtkResliceCursorCallBack() {};
	vtkImagePlaneWidget* ImagePlaneWidget[3];
	vtkResliceCursorWidget* ResliceCursorWidget[3];
};

