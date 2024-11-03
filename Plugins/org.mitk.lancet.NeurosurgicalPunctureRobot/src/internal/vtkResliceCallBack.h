#pragma once
#include <vtkCommand.h>
#include "vtkResliceWidget.h"
class vtkResliceCallBack : public vtkCommand
{
public:
	static vtkResliceCallBack* New();
	vtkTypeMacro(vtkResliceCallBack, vtkCommand);

	vtkResliceCallBack() = default;
	~vtkResliceCallBack() = default;

	[[nodicard]] vtkResliceWidget* GetWidget() const { return nullptr; }



private:
	vtkResliceWidget* m_ResliceWidget = {};

	
};

