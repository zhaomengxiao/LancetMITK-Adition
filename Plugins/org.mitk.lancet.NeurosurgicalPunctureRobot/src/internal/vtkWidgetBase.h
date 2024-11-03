#pragma once
#include <vtkRenderWindow.h>
#include <vtkDICOMReader.h>
class vtkWidgetBase
{
public:
	vtkWidgetBase() = default;
	virtual ~vtkWidgetBase() = default;

	vtkSmartPointer<vtkRenderWindow>* GetRenderWindows() { return m_RenderWindows; };
	vtkSmartPointer<vtkRenderWindow> GetActiveRenderWindow() const { return m_ActiveRenderWindow; };

	void SetRenderWindow(const vtkSmartPointer<vtkRenderWindow>& renderWindow) { m_RenderWindows[0] = renderWindow; };
	void SetRenderWindowMPR(const vtkSmartPointer<vtkRenderWindow>& sagittal, const vtkSmartPointer<vtkRenderWindow>& coronal,
		const vtkSmartPointer<vtkRenderWindow>& axial);

	virtual void SetInteractor(const vtkSmartPointer<vtkRenderWindowInteractor>& interactor) = 0;

	virtual void RenderWindow(vtkSmartPointer<vtkDICOMReader> reader) = 0;

private:
	vtkSmartPointer<vtkRenderWindow> m_RenderWindows[3] = {};
	vtkSmartPointer<vtkRenderWindow>  m_ActiveRenderWindow = nullptr;
};

