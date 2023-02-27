#ifndef DRR_H
#define DRR_H
#include "mitkImage.h"
#include "MitkLancetDRRExports.h"
#include "mitkImageToImageFilter.h"
mitk::Image::Pointer MITKLANCETDRR_EXPORT DRR(mitk::Image::Pointer input);

class MITKLANCETDRR_EXPORT DrrFilter:public mitk::ImageToImageFilter
{
public:
	mitkClassMacro(DrrFilter, ImageToImageFilter);
	itkFactorylessNewMacro(Self)
	itkCloneMacro(Self)

 
	itkSetMacro(tx, double)
	itkSetMacro(ty, double)
	itkSetMacro(tz, double)

	itkSetMacro(rx, double)
	itkSetMacro(ry, double)
	itkSetMacro(rz, double)

	itkSetMacro(cx, double)
	itkSetMacro(cy, double)
	itkSetMacro(cz, double)

	itkSetMacro(threshold,double);
	itkSetMacro(sid,double);
	itkSetMacro(sx, double);
	itkSetMacro(sy, double);

	itkSetMacro(dx,int);
	itkSetMacro(dy,int);

	itkSetMacro(o2Dx,double);
	itkSetMacro(o2Dy, double);

	itkSetMacro(verbose, bool);
	/*!
	\brief easy way to set obj trans at once
	*/
  void SetObjTranslate(double tx, double ty, double tz);
  /*!
	\brief easy way to set obj rotate at once
	*/
	void SetObjRotate(double rx, double ry, double rz);

protected:
	/*!
	\brief standard constructor
	*/
	DrrFilter();
	/*!
	\brief standard destructor
	*/
	~DrrFilter() override;
	/*!
	\brief Method generating the output information of this filter (e.g. image dimension, image type, etc.).
	The interface ImageToImageFilter requires this implementation. Everything is taken from the input image.
	*/
	void GenerateOutputInformation() override;
	/*!
	\brief Method generating the output of this filter. Called in the updated process of the pipeline.
	This method generates the smoothed output image.
	*/
	void GenerateData() override;

	/*!
	\brief Internal templated method calling the ITK drr filter. Here the actual filtering is performed.
	*/
	template <typename TPixel, unsigned int VDimension>
	void ItkImageProcessing(const itk::Image<TPixel, VDimension>* itkImage);

	// template <typename TPixel, unsigned int VDimension>
	// mitk::Image::Pointer GenDRR(const itk::Image<TPixel, VDimension>* input_image, DrrFilter* generator);
	
private:
  // C-arm rotation
	double m_rx{0.0};
	double m_ry{0.0};
	double m_rz{0.0};

  // C-arm translation
	double m_tx{0.0};
	double m_ty{0.0};
	double m_tz{0.0};

  // C-arm gantry rotation center offset
	double m_cx{0.0};
	double m_cy{ 0.0 };
	double m_cz{ 0.0 };

  //drr para
	//double m_focalpoint{ 0.0 };
	double m_threshold{0.0};
	double m_sid{ 1024 };
	double m_sx{ 0.75 } ;// pixel spacing along X of the 2D DRR image[mm]
	double m_sy{ 0.75 };// pixel spacing along Y of the 2D DRR image[mm]
	
  //output image size
	int m_dx = 512;
	int m_dy = 512;

	double m_o2Dx { 0.0 };
	double m_o2Dy{ 0.0 };
	//double m_direction;
	bool m_verbose { true };
};





#endif // DRR_H
