#pragma once
#include <vtkAbstractWidget.h>
#include <vtkSmartPointer.h>
#include <vtkImageResliceToColors.h>
#include <vtkRenderWindow.h>
#include "vtkReslicePlaneCursorWidget.h"
#include <vtkImageActor.h>

class vtkResliceCallBack;

class vtkResliceWidget final : public vtkAbstractWidget
{
public:
	static vtkResliceWidget* New();
	vtkTypeMacro(vtkResliceWidget, vtkAbstractWidget);
	vtkResliceWidget() = default;
	~vtkResliceWidget() = default;

	[[nodiscard]] vtkRenderWindow* GetRenderWindow() const { return m_RenderWindow; }
	[[nodiscard]] int GetIsCameraCentered() const { return m_IsCameraCentered; }

	void SetEnabled(int) override;
	void SetImageReslicers(
		const vtkSmartPointer<vtkImageResliceToColors>& m_First,
		const vtkSmartPointer<vtkImageResliceToColors>& m_Second,
		const vtkSmartPointer<vtkImageResliceToColors>& m_Third);

	vtkSmartPointer<vtkImageResliceToColors>* GetImageReslicers() { return m_ImageReslices; }

	void SetRenderWindows(vtkSmartPointer<vtkRenderWindow>* aRenderWindows);
	void RefreshWindows(int aRenderWindowNumber);
	void SetVisible(bool aVisibility);
	void SetCameraCentered(int aCentered);
	void SetHighQuality(int highQuality, int aPlane);

	void CreateDefaultRepresentation() override;
	void CenterImageActors(int aExcludedCursor);
	void ResetResliceCursor();

	enum widgetState{Start = 0, Rotate, Translate};
private:
	void ResetCamera(vtkRenderWindow* aRenderWindow);
	double* GetImageActorCenterPosition(vtkRenderWindow* aRenderWindow);
	void SetQualityToHigh(int aRenderWindowNumber, vtkImageActor* aActor);
	void SetQualityToLow(int aRenderWindowNumber, vtkImageActor* aActor);

private:
	vtkSmartPointer<vtkRenderWindow> m_RenderWindows[3] = {};
	vtkSmartPointer<vtkImageResliceToColors> m_ImageReslices[3] = {};
	vtkSmartPointer<vtkReslicePlaneCursorWidget> m_ReslicePlaneCursorWidget[3] = {};
	vtkSmartPointer<vtkResliceCallBack> m_ResliceCallBack[3] = {};
	vtkRenderWindow* m_RenderWindow = nullptr;

	int m_IsCameraCentered = 0;

protected:
	friend class vtkResliceCallBack;
};

