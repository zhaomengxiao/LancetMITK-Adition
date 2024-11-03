#pragma once
#include <vtkCommand.h>
//#include <vtkDICOMImageReader.h>
class Utility
{
};

enum class TransformationType
{
	None,
	FlipHorizontal,
	FlipVertical,
	RotateLeft,
	RotateRight,
	Invert,
	ScrollMouse,
	Zoom,
	Pan,
	WindowLevel
};

enum class OverlayKey
{
	Zoom = 1001, 
	Series = 1003,
	Window = 2625616,
	Level = 2625617
};

enum vtkCustomEvents : unsigned long
{
	ChangeScrollValue = vtkCommand::UserEvent + 1,
	DefaultCursor = ChangeScrollValue+1,
	CursorMove = DefaultCursor+1,
	CursorFinishMovement = CursorMove + 1,
	CursorRotate = CursorFinishMovement + 1,
	ImageChanged = CursorRotate + 1,
	QualityLow = ImageChanged + 1,
	QualityHigh = QualityLow + 1,
};
