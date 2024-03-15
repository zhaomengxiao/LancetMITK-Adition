
#include <lancetThaPelvisCupStencilObject.h>

#include "vtkTransformFilter.h"
#include "vtkPointData.h"
#include "mitkSurfaceToImageFilter.h"
#include "vtkImageCast.h"
#include "vtkImageData.h"
#include "mitkCLUtil.h"
#include "mitkTransferFunctionProperty.h"
#include "mitkTransferFunctionPropertySerializer.h"
#include "mitkProperties.h"

lancet::ThaPelvisCupStencilObject::ThaPelvisCupStencilObject() :
	m_flyingEdges3D(vtkSmartPointer<vtkDiscreteFlyingEdges3D>::New()),
	m_windowedSincPolyData(vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New()),
	m_TransformPolyDataFilter(vtkSmartPointer<vtkTransformPolyDataFilter>::New())
{

}

lancet::ThaPelvisCupStencilObject::~ThaPelvisCupStencilObject()
{
}

mitk::DataNode::Pointer lancet::ThaPelvisCupStencilObject::GenerateReamingImage(mitk::Image::Pointer pelvisOverlay, mitk::Surface::Pointer PelvisClipped,
	 mitk::Surface::Pointer cup, std::string VolumeRenderingConfigFile)
{
	//1.generate pre reaming pelvis image
	auto preReamingImageNode = ApplyBoneStencil(pelvisOverlay, PelvisClipped);
	auto preReamingImage = dynamic_cast<mitk::Image*>(preReamingImageNode->GetData());
	//2.generate implant stencil image
	auto ImplantStencilImageNode = ApplyImplantStencil(preReamingImage, cup);
	auto ImplantStencilImage = dynamic_cast<mitk::Image*>(ImplantStencilImageNode->GetData());
	//3.generate color layer
	auto whiteNode = mitk::DataNode::New();
	auto redNode = mitk::DataNode::New();
	auto greenNode = mitk::DataNode::New();
	GenerateReamingColorLayer(ImplantStencilImage, whiteNode, redNode, greenNode);
	auto whiteImage = dynamic_cast<mitk::Image*>(whiteNode->GetData());
	auto redImage = dynamic_cast<mitk::Image*>(redNode->GetData());
	auto greenImage = dynamic_cast<mitk::Image*>(greenNode->GetData());
	//4.combine different layers
	auto combinedReamingImageNode = CombineReamingImage(ImplantStencilImage, whiteImage, redImage, greenImage);
	//5.volume rendering
	SetVolumeRenderingConfig(combinedReamingImageNode, VolumeRenderingConfigFile);
	return combinedReamingImageNode;
}

void lancet::ThaPelvisCupStencilObject::DiscreteFlyingEdges3D(mitk::Image::Pointer image, vtkPolyData* surface)
{
	double scale[3];
	image->GetGeometry()->GetSpacing().ToArray(scale);
	scale[0] = 1 / scale[0];
	scale[1] = 1 / scale[1];
	scale[2] = 1 / scale[2];

	vtkSmartPointer<vtkTransform> Transform = vtkSmartPointer<vtkTransform>::New();
	Transform->SetMatrix(image->GetGeometry()->GetVtkMatrix());
	Transform->Scale(scale);
	Transform->Update();

	m_flyingEdges3D = vtkSmartPointer<vtkDiscreteFlyingEdges3D>::New();
	m_flyingEdges3D->SetInputData(image->GetVtkImageData());
	m_flyingEdges3D->SetValue(0, 1);
	m_flyingEdges3D->SetValue(1, 2);
	m_flyingEdges3D->SetValue(2, 3);

	m_flyingEdges3D->SetComputeGradients(false);
	m_flyingEdges3D->SetComputeNormals(false);
	m_flyingEdges3D->SetComputeScalars(true);
	m_flyingEdges3D->Update();

	m_windowedSincPolyData = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
	m_windowedSincPolyData->SetInputConnection(m_flyingEdges3D->GetOutputPort());
	m_windowedSincPolyData->SetNumberOfIterations(10);
	m_windowedSincPolyData->SetFeatureEdgeSmoothing(false);
	m_windowedSincPolyData->SetBoundarySmoothing(false);
	m_windowedSincPolyData->SetEdgeAngle(15);
	m_windowedSincPolyData->SetFeatureAngle(50);
	m_windowedSincPolyData->SetPassBand(0.01);
	//this->windowedSincPolyData->SetNonManifoldSmoothing(true);
	m_windowedSincPolyData->SetNonManifoldSmoothing(false);   //no open
	m_windowedSincPolyData->SetNormalizeCoordinates(true);
	m_windowedSincPolyData->Update();

	m_TransformPolyDataFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	m_TransformPolyDataFilter->SetInputConnection(m_windowedSincPolyData->GetOutputPort());
	m_TransformPolyDataFilter->SetTransform(Transform);
	m_TransformPolyDataFilter->Update();

	surface->ShallowCopy(m_TransformPolyDataFilter->GetOutput());
}



mitk::DataNode::Pointer lancet::ThaPelvisCupStencilObject::ApplyBoneStencil(mitk::Image::Pointer image, mitk::Surface::Pointer surface)
{
	vtkNew<vtkPolyData> surfacePolyData;

	DiscreteFlyingEdges3D(image, surfacePolyData);
	vtkNew<vtkTransformFilter> tmpTransFilter;
	vtkNew<vtkTransform> tmpTransform;
	tmpTransform->SetMatrix(surface->GetGeometry()->GetVtkMatrix());
	tmpTransFilter->SetTransform(tmpTransform);
	tmpTransFilter->SetInputData(surfacePolyData);
	tmpTransFilter->Update();

	auto objectSurface = mitk::Surface::New();
	objectSurface->SetVtkPolyData(tmpTransFilter->GetPolyDataOutput());

	vtkNew<vtkImageData> whiteImage;
	double imageBounds[6]{ 0 };
	double imageSpacing[3]{ 0.4, 0.4, 0.4 };
	whiteImage->SetSpacing(imageSpacing);

	auto geometry = objectSurface->GetGeometry();
	auto surfaceBounds = geometry->GetBounds();
	for (int n = 0; n < 6; n++)
	{
		imageBounds[n] = surfaceBounds[n];
	}

	int dim[3];
	for (int i = 0; i < 3; i++)
	{
		dim[i] = static_cast<int>(ceil((imageBounds[i * 2 + 1] - imageBounds[i * 2]) / imageSpacing[i]));
	}
	whiteImage->SetDimensions(dim);
	whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

	double origin[3];
	origin[0] = imageBounds[0] + imageSpacing[0] / 2;
	origin[1] = imageBounds[2] + imageSpacing[1] / 2;
	origin[2] = imageBounds[4] + imageSpacing[2] / 2;
	whiteImage->SetOrigin(origin);
	whiteImage->AllocateScalars(VTK_SHORT, 1);

	// fill the image with foreground voxels:
	short insideValue = 2000;
	// short outsideValue = 0;
	vtkIdType count = whiteImage->GetNumberOfPoints();
	for (vtkIdType i = 0; i < count; ++i)
	{
		whiteImage->GetPointData()->GetScalars()->SetTuple1(i, insideValue);
	}

	auto imageToCrop = mitk::Image::New();
	imageToCrop->Initialize(whiteImage);
	imageToCrop->SetVolume(whiteImage->GetScalarPointer());//用待剪切表面生成whiteImage

	mitk::SurfaceToImageFilter::Pointer surfaceToImageFilter = mitk::SurfaceToImageFilter::New();
	surfaceToImageFilter->SetBackgroundValue(0);
	surfaceToImageFilter->SetImage(imageToCrop);
	// Apply the stencil
	surfaceToImageFilter->SetInput(objectSurface);
	surfaceToImageFilter->SetReverseStencil(false);
	surfaceToImageFilter->Update();
	mitk::Image::Pointer convertedImage = mitk::Image::New();
	convertedImage = surfaceToImageFilter->GetOutput();

	// Test: set the boundary voxel of the image to 
	auto inputVtkImage = convertedImage->GetVtkImageData();
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	int tmpRegion[6]{ 0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1 };

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToInt();
	caster->Update();

	auto castVtkImage = caster->GetOutput();

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int x_p = ((x + 1) > (dims[0] - 1)) ? (dims[0] - 1) : (x + 1);
				int x_m = ((x - 1) < 0) ? 0 : (x - 1);

				int y_p = ((y + 1) > (dims[1] - 1)) ? (dims[1] - 1) : (y + 1);
				int y_m = ((y - 1) < 0) ? 0 : (y - 1);

				int z_p = ((z + 1) > (dims[2] - 1)) ? (dims[2] - 1) : (z + 1);
				int z_m = ((z - 1) < 0) ? z : (z - 1);

				int* n = static_cast<int*>(castVtkImage->GetScalarPointer(x, y, z));
				int* n1 = static_cast<int*>(castVtkImage->GetScalarPointer(x_m, y, z));
				int* n2 = static_cast<int*>(castVtkImage->GetScalarPointer(x_p, y, z));
				int* n3 = static_cast<int*>(castVtkImage->GetScalarPointer(x, y_m, z));
				int* n4 = static_cast<int*>(castVtkImage->GetScalarPointer(x, y_p, z));
				int* n5 = static_cast<int*>(castVtkImage->GetScalarPointer(x, y, z_m));
				int* n6 = static_cast<int*>(castVtkImage->GetScalarPointer(x, y, z_p));

				if (n[0] == 2000)
				{
					if (n1[0] == 0 || n2[0] == 0 || n3[0] == 0 || n4[0] == 0 || n5[0] == 0 || n6[0] == 0)
					{
						n[0] = 3000;
					}

					if (x == dims[0] - 1 || x == 0 || y == dims[1] - 1 || y == 0 || z == dims[2] - 1 || z == 0)
					{
						n[0] = 3000;
					}
				}

			}
		}
	}
	auto resultMitkImage = mitk::Image::New();
	resultMitkImage->Initialize(castVtkImage);
	resultMitkImage->SetVolume(castVtkImage->GetScalarPointer());
	resultMitkImage->SetGeometry(convertedImage->GetGeometry());

	mitk::DataNode::Pointer newNode = mitk::DataNode::New();
	newNode->SetData(resultMitkImage);
	return newNode;
}

mitk::DataNode::Pointer lancet::ThaPelvisCupStencilObject::ApplyImplantStencil(mitk::Image::Pointer image, mitk::Surface::Pointer surface)
{
	mitk::SurfaceToImageFilter::Pointer surfaceToImageFilter = mitk::SurfaceToImageFilter::New();
	surfaceToImageFilter->SetBackgroundValue(0);
	surfaceToImageFilter->SetImage(image);
	surfaceToImageFilter->SetInput(surface);
	surfaceToImageFilter->SetReverseStencil(false);

	mitk::Image::Pointer convertedImage = mitk::Image::New();
	surfaceToImageFilter->Update();
	convertedImage = surfaceToImageFilter->GetOutput();//用surface生成image


	// Test: set the boundary voxel of the image to 
	auto inputVtkImage = convertedImage->GetVtkImageData();
	int dims[3];
	inputVtkImage->GetDimensions(dims);

	int tmpRegion[6]{ 0, dims[0] - 1, 0, dims[1] - 1, 0, dims[2] - 1 };

	auto caster = vtkImageCast::New();
	caster->SetInputData(inputVtkImage);
	caster->SetOutputScalarTypeToInt();
	caster->Update();

	auto implantStencilImage = caster->GetOutput();//Implant的image

	auto boneVtkImage = image->GetVtkImageData();//骨头的image


	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int* n = static_cast<int*>(implantStencilImage->GetScalarPointer(x, y, z));
				int* m = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y, z));

				if (m[0] > 0 && n[0] != 0)
				{
					m[0] = 1000;
				}

			}
		}
	}//将骨头与假体相交的地方置1000


	// 1-voxel layer white boundary
	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int x_p = ((x + 1) > (dims[0] - 1)) ? (dims[0] - 1) : (x + 1);
				int x_m = ((x - 1) < 0) ? 0 : (x - 1);

				int y_p = ((y + 1) > (dims[1] - 1)) ? (dims[1] - 1) : (y + 1);
				int y_m = ((y - 1) < 0) ? 0 : (y - 1);

				int z_p = ((z + 1) > (dims[2] - 1)) ? (dims[2] - 1) : (z + 1);
				int z_m = ((z - 1) < 0) ? z : (z - 1);

				int* n = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y, z));
				int* n1 = static_cast<int*>(boneVtkImage->GetScalarPointer(x_m, y, z));
				int* n2 = static_cast<int*>(boneVtkImage->GetScalarPointer(x_p, y, z));
				int* n3 = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y_m, z));
				int* n4 = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y_p, z));
				int* n5 = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y, z_m));
				int* n6 = static_cast<int*>(boneVtkImage->GetScalarPointer(x, y, z_p));

				if (n[0] == 2000)
				{
					if (n1[0] == 1000 || n2[0] == 1000 || n3[0] == 1000 || n4[0] == 1000 || n5[0] == 1000 || n6[0] == 1000)
					{
						n[0] = 3000;
					}

				}

			}
		}
	}//如果邻域有1000，将2000的点设为3000

	auto resultMitkImage = mitk::Image::New();
	resultMitkImage->Initialize(boneVtkImage);
	resultMitkImage->SetVolume(boneVtkImage->GetScalarPointer());
	resultMitkImage->SetGeometry(convertedImage->GetGeometry());

	mitk::DataNode::Pointer newNode = mitk::DataNode::New();
	newNode->SetData(resultMitkImage);
	return newNode;
}

void lancet::ThaPelvisCupStencilObject::GenerateReamingColorLayer(mitk::Image::Pointer implantStencilImage, 
	mitk::DataNode::Pointer whiteNode, mitk::DataNode::Pointer redNode, mitk::DataNode::Pointer greenNode)
{
	auto caster = vtkImageCast::New();
	caster->SetInputData(implantStencilImage->GetVtkImageData());
	caster->SetOutputScalarTypeToInt();
	caster->Update();

	auto vtkImage = caster->GetOutput();

	int dims[3];

	vtkImage->GetDimensions(dims);

	vtkNew<vtkImageData> whiteLayer_image;
	whiteLayer_image->DeepCopy(vtkImage);

	vtkNew<vtkImageData> redPart_image;
	redPart_image->DeepCopy(vtkImage);

	vtkNew<vtkImageData> greenPart_image;
	greenPart_image->DeepCopy(vtkImage);

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int* n = static_cast<int*>(vtkImage->GetScalarPointer(x, y, z));
				int* w = static_cast<int*>(whiteLayer_image->GetScalarPointer(x, y, z));
				int* r = static_cast<int*>(redPart_image->GetScalarPointer(x, y, z));
				int* g = static_cast<int*>(greenPart_image->GetScalarPointer(x, y, z));

				if (n[0] == 3000)
				{
					r[0] = 0;
					g[0] = 0;

					g[0] = 1700;
				}

				if (n[0] == 2000)
				{
					w[0] = 0;
					g[0] = 0;

					w[0] = 3000;
					g[0] = 1700;
				}

				if (n[0] == 1000)
				{
					w[0] = 0;
					r[0] = 0;

					w[0] = 2600;
				}

				if (n[0] == 0)
				{
					w[0] = 2600;
					g[0] = 1700;
				}

			}
		}
	}//为每张图设置颜色

	auto whiteMitkImage = mitk::Image::New();
	whiteMitkImage->Initialize(whiteLayer_image);
	whiteMitkImage->SetVolume(whiteLayer_image->GetScalarPointer());
	whiteMitkImage->SetGeometry(implantStencilImage->GetGeometry());

	auto whiteMitkImage_smoothed = mitk::Image::New();
	mitk::CLUtil::GaussianFilter(whiteMitkImage, whiteMitkImage_smoothed, 0.1);
	whiteNode->SetData(whiteMitkImage_smoothed);

	auto redMitkImage = mitk::Image::New();
	redMitkImage->Initialize(redPart_image);
	redMitkImage->SetVolume(redPart_image->GetScalarPointer());
	redMitkImage->SetGeometry(implantStencilImage->GetGeometry());

	redNode->SetData(redMitkImage);

	auto greenMitkImage = mitk::Image::New();
	greenMitkImage->Initialize(greenPart_image);
	greenMitkImage->SetVolume(greenPart_image->GetScalarPointer());
	greenMitkImage->SetGeometry(implantStencilImage->GetGeometry());

	auto greenMitkImage_smoothed = mitk::Image::New();
	mitk::CLUtil::GaussianFilter(greenMitkImage, greenMitkImage_smoothed, 0.1);

	greenNode->SetData(greenMitkImage_smoothed);
}

mitk::DataNode::Pointer lancet::ThaPelvisCupStencilObject::CombineReamingImage(mitk::Image::Pointer implantStencilImage,
	mitk::Image::Pointer whiteImage, mitk::Image::Pointer redImage, mitk::Image::Pointer greenImage)
{
	auto caster_red = vtkImageCast::New();
	caster_red->SetInputData(redImage->GetVtkImageData());
	caster_red->SetOutputScalarTypeToInt();
	caster_red->Update();

	auto redVtkImage = caster_red->GetOutput();
	int dims[3];
	redVtkImage->GetDimensions(dims);

	auto caster_white = vtkImageCast::New();
	caster_white->SetInputData(whiteImage->GetVtkImageData());
	caster_white->SetOutputScalarTypeToInt();
	caster_white->Update();

	auto whiteVtkImage = caster_white->GetOutput();

	auto caster_green = vtkImageCast::New();
	caster_green->SetInputData(greenImage->GetVtkImageData());
	caster_green->SetOutputScalarTypeToInt();
	caster_green->Update();

	auto greenVtkImage = caster_green->GetOutput();

	vtkNew<vtkImageData> combinedVtkImage;
	combinedVtkImage->DeepCopy(redVtkImage);

	auto caster = vtkImageCast::New();
	caster->SetInputData(implantStencilImage->GetVtkImageData());
	caster->SetOutputScalarTypeToInt();
	caster->Update();

	auto vtkImage = caster->GetOutput();

	for (int z = 0; z < dims[2]; z++)
	{
		for (int y = 0; y < dims[1]; y++)
		{
			for (int x = 0; x < dims[0]; x++)
			{
				int* n = static_cast<int*>(vtkImage->GetScalarPointer(x, y, z));
				int* m = static_cast<int*>(combinedVtkImage->GetScalarPointer(x, y, z));
				int* w = static_cast<int*>(whiteVtkImage->GetScalarPointer(x, y, z));
				int* r = static_cast<int*>(redVtkImage->GetScalarPointer(x, y, z));
				int* g = static_cast<int*>(greenVtkImage->GetScalarPointer(x, y, z));

				m[0] = n[0];

				// green part
				if (g[0] < 1695)
				{
					if (n[0] == 0 || n[0] == 1000)
					{
						m[0] = g[0];
					}
				}

				// white part
				if (/*n[0] != 2000 &&*/ n[0] != 1000 && w[0] > 2600)
				{
					m[0] = w[0];
				}

				if (n[0] == 0 && m[0] == 0)
				{
					m[0] = 2150;
				}

				if (n[0] == 2000 && m[0] >= 2995)
				{
					m[0] = 1964;
				}

			}
		}
	}

	auto combinedMitkImage = mitk::Image::New();
	combinedMitkImage->Initialize(combinedVtkImage);
	combinedMitkImage->SetVolume(combinedVtkImage->GetScalarPointer());
	combinedMitkImage->SetGeometry(implantStencilImage->GetGeometry());


	mitk::DataNode::Pointer newNode = mitk::DataNode::New();
	newNode->SetData(combinedMitkImage);
	return newNode;
}

void lancet::ThaPelvisCupStencilObject::SetVolumeRenderingConfig(mitk::DataNode::Pointer imageToCut, std::string configFileName)
{
	mitk::TransferFunctionProperty::Pointer tfpToChange = dynamic_cast<mitk::TransferFunctionProperty*>(imageToCut->GetProperty("TransferFunction", nullptr));

	if (!tfpToChange)
	{
		if (!dynamic_cast<mitk::Image*>(imageToCut->GetData()))
		{
			MITK_WARN << "QmitkTransferFunctionWidget::SetDataNode called with non-image node";
			return;
		}

		imageToCut->SetProperty("TransferFunction", tfpToChange = mitk::TransferFunctionProperty::New());
	}

	mitk::TransferFunction::Pointer tf =
		mitk::TransferFunctionPropertySerializer::DeserializeTransferFunction(configFileName.c_str());

	if (tf.IsNotNull())
	{
		tfpToChange->SetValue(tf);
		imageToCut->SetProperty("volumerendering", mitk::BoolProperty::New(true));
		imageToCut->SetVisibility(true);
	}
	else
	{
		return;
	}
}
