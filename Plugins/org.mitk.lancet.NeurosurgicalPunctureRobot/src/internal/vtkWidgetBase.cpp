#include "vtkWidgetBase.h"

void vtkWidgetBase::SetRenderWindowMPR(const vtkSmartPointer<vtkRenderWindow>& sagittal, const vtkSmartPointer<vtkRenderWindow>& coronal, const vtkSmartPointer<vtkRenderWindow>& axial)
{
	m_RenderWindows[0] = sagittal;
	m_RenderWindows[1] = coronal;
	m_RenderWindows[2] = axial;
}
