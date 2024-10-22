#pragma once
#include <QmitkAbstractView.h>
#include <mitkWorkbenchUtil.h>
#include <QmitkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkLineSource.h>
#include <vtkProperty.h>
#include <mitkIRenderWindowPart.h>
#include <mitkDataStorage.h>
#include <vtkPlaneSource.h>
#include <mitkSurface.h>
#include <vtkAppendPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkTubeFilter.h>
#include <vtkConeSource.h>
#include <mitkDataStorage.h>
#include <mitkIOUtil.h>
#include <mitkInteractionConst.h>
#include <mitkRotationOperation.h>
#include <vtkAxesActor.h>
#include <vtkRenderWindow.h>
#include <vtkRendererCollection.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <QmitkSingleNodeSelectionWidget.h>
#include <mitkNodePredicateAnd.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateProperty.h>
#include <mitkNodePredicateCompositeBase.h>
#include <mitkSurface.h>
#include <mitkNodePredicateBase.h>
#include <mitkNodePredicateDataType.h>
#include <mitkNodePredicateOr.h>
#include <mitkColorProperty.h>

	class PKARenderHelper
	{
	public:
		static void DrawLine(double* p1, double* p2, double* color, double lineWidth, vtkRenderer* render);


		static void DrawPlane(mitk::DataStorage* dataStorage, std::string planeNodeName);
		static vtkSmartPointer<vtkPolyData> BuildAxis(const mitk::Point3D& center, const mitk::Vector3D& axis, double halflength = 10);

		static void DrawLoaclCoordinateSystem(mitk::DataStorage* dataStorage, vtkMatrix4x4* matrix, const char* name, double* color);

		static vtkSmartPointer<vtkPolyData> DisplayDirection(mitk::DataStorage* dataStorage, Eigen::Vector3d start, Eigen::Vector3d end, const char* name, double* color, int length = 20);

		static void LoadSingleMitkFile(mitk::DataStorage* dataStorage, std::string filePath, std::string nodeName);

		static void AddPointSetToMitk(mitk::DataStorage* dataStorage, std::vector<Eigen::Vector3d> Points, std::string Name, int Size, double* Color);
		static void AddvtkPointsToMitk(mitk::DataStorage* dataSrorage, vtkPoints* points, std::string name, int Size, double* Color, bool visibility = true);

		static void TransformModel(mitk::DataStorage* dataStorage, const char* name, vtkMatrix4x4* matrix);
		static mitk::DataNode::Pointer AddPolyData2DataStorage(mitk::DataStorage* dataStorage, std::string name,
			vtkPolyData* polyData, double r = 1.0, double g = 1.0, double b = 1.0, mitk::DataNode* parent = nullptr);
		/// <summary>
		/// 
		/// </summary>
		/// <param name="direction">方向   其中方向不能为0</param>
		/// <param name="dataNode"></param>
		static void TranslateModel(double* direction, mitk::DataNode::Pointer dataNode, double length);

		static void TranslateModel(mitk::DataNode::Pointer dataNode, double* pos);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="direction">方向  其中方向不能为0</param>
		/// <param name="center"></param>
		/// <param name="dataNode"></param>
		static void RotateModel(double* direction, double* center, mitk::DataNode::Pointer dataNode, double angle);

		static void AddActor(mitk::IRenderWindowPart* renderWindowPart3D, vtkActor* actor);
		static void AddActor(mitk::IRenderWindowPart* renderWindowPart3D, vtkAxesActor* actor);
		static void RemoveActor(mitk::IRenderWindowPart* renderWindowPart3d, vtkAxesActor* actor);
		static vtkSmartPointer<vtkAxesActor> GenerateAxesActor(double axexLength = 30);

		/// <summary>
	   /// 初始化影像配准界面 mitkSingleNodeSelectionWidget,若不初始化则不能添加
	   /// </summary>
	   /// <param name="widget">UI mitkSingleNodeSelectionWidget SurfaceObjectName</param>
		static void InitSurfaceSelector(mitk::DataStorage* dataStorage, QmitkSingleNodeSelectionWidget* widget);

		/// <summary>
		/// 初始化影像配准界面 mitkSingleNodeSelectionWidget,若不初始化则不能添加
		/// </summary>
		/// <param name="widget">UI mitkSingleNodeSelectionWidget ImageObjectName</param>
		static void InitImageSelector(mitk::DataStorage* dataStorage, QmitkSingleNodeSelectionWidget* widget);

		/// <summary>
		/// 初始化影像配准界面 mitkSingleNodeSelectionWidget,若不初始化则不能添加
		/// </summary>
		/// <param name="widget">UI mitkSingleNodeSelectionWidget LandmarkObjectName</param>
		static void InitPointSetSelector(mitk::DataStorage* dataStorage, QmitkSingleNodeSelectionWidget* widget);

		static void InitBoundingBoxSelector(mitk::DataStorage* dataStorage, QmitkSingleNodeSelectionWidget* widget);

		static mitk::Surface::Pointer GetSurfaceNodeByName(mitk::DataStorage* dataStorage, const std::string nodeName);

		static void RemoveNodeIsExist(mitk::DataStorage* dataStorage, std::string name);

		static void RemoveAllNode(mitk::DataStorage* dataStorage);

		static Eigen::Vector3d GetFirstPointFromDataStorage(mitk::DataStorage* dataStorage, std::string nodeName);

		static bool IsContainsNamedNode(mitk::DataStorage* dataStorage, std::string nodeName);

		static void HideAllNode(mitk::DataStorage* dataStorage);
		static void DisplaySingleNode(mitk::DataStorage* dataStorage, std::string nodeName);
		static void DisplaySingleNode(mitk::DataStorage* dataStorage, mitk::DataNode::Pointer node);

		static void ResetAllRenderWindow(mitk::IRenderWindowPart* renderWindowPart);
	};


