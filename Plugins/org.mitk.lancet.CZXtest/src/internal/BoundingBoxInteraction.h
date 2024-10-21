#pragma once
#include <vtkMatrix4x4.h>
#include <mitkBoundingShapeObjectFactory.h>
#include <mitkDataStorage.h>
#include <mitkBoundingShapeInteractor.h>
#include <mitkNodePredicateFunction.h>
#include <mitkWorkbenchUtil.h>
#include <mitkIRenderWindowPart.h>
#include <usModuleRegistry.h>
#include <mitkSurface.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>
#include <vtkClipClosedSurface.h>
#include "PrintDataHelper.h"
#include "PKARenderHelper.h"
class BoundingBoxInteraction
{
public:
	BoundingBoxInteraction(mitk::DataStorage* dataStorage, mitk::IRenderWindowPart* renderWindowPart);

	void DisplayBoundingBox(std::string nodeName, std::string boundingBoxNmae);

	void CutModel(std::string ClippedNodeName);

private:
	void CreateBoundingShapeInteractor(bool rotationEnabled);
	mitk::Geometry3D::Pointer InitializeWithImageGeometry(const mitk::BaseGeometry* geometry) const;

private:
	mitk::DataStorage* m_DataStorage;
	std::string m_CurrentModelNodeName;
	mitk::BoundingShapeInteractor::Pointer m_BoundingShapeInteractor;
	mitk::IRenderWindowPart* m_RenderWindowPart;
};

