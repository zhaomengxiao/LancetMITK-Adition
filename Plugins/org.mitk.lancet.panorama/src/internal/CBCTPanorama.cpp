#include "CBCTPanorama.h"

Panorama::Panorama(std::string resultDir,
                   std::string mipFile,
                   std::string otsuFile,
                   std::string morphFile,
                   std::string thinFile,
                   std::string splineFile,
                   std::string panoramaFile,
                   std::string looseROIFile,
                   std::string boxROIFile,
                   std::string toothMaskFile)
{
  this->resultDir = resultDir;
  this->mipFile = mipFile;
  this->otsuFile = otsuFile;
  this->morphFile = morphFile;
  this->thinFile = thinFile;
  this->splineFile = splineFile;
  this->panoramaFile = panoramaFile;
  this->looseROIFile = looseROIFile;
  this->boxROIFile = boxROIFile;
  this->toothMaskFile = toothMaskFile;
  //´´½¨ÎÄ¼þ¼Ð
  itksys::SystemTools::MakeDirectory(resultDir);

  //ÊµÀý»¯
  scalarImageToHistogramGenerator = ScalarImageToHistogramGeneratorType::New();
  calculator = CalculatorType::New();
  binaryFilter = BinaryFilterType::New();
  seriesReader = seriesReaderType::New();
  seriesWriter = seriesWriterType::New();
  dcmIO = DCMImageIO::New();
  pngIO = PNGImageIO::New();
  namesGenerator = NamesGeneratorType::New();
  binaryDilate = DilateFilterType::New();
  binaryErode = ErodeFilterType::New();
  mipFilter = MipFilterType::New();
  smoothing2DFilter = Smoothing2DFilterType::New();
  extractFilter = ExtractFilterType::New();
  dcmReader = DCMReaderType::New();
  dcmWriter = DCMWriterType::New();
  binaryThinningImageFilter = BinaryThinningImageFilterType::New();
  rescaler = RescaleType::New();
  pngWriter = PNGWriterType::New();
  pngReader = PNGReaderType::New();
  connectedComponentFilter = ConnectedComponentFilter::New();
  labelShapeKeepNObjectsFilter = LabelShapeKeepNObjectsFilter::New();
  seriesFileNamesGenerator = SeriesFileNamesGeneratorType::New();
  niftiWriter = NiftiWriterType::New();
  labelWriter = LabelWriterType::New();
  niftiIO = NiftiImageIO::New();
}

//µäÐÍ´íÎó£¬smartPointer²»ÄÜÊ¹ÓÃdelete·½·¨
Panorama::~Panorama()
{
  scalarImageToHistogramGenerator = NULL;
  calculator = NULL;
  binaryFilter = NULL;
  seriesReader = NULL;
  seriesWriter = NULL;
  dcmIO = NULL;
  pngIO = NULL;
  namesGenerator = NULL;
  binaryDilate = NULL;
  binaryErode = NULL;
  mipFilter = NULL;
  smoothing2DFilter = NULL;
  extractFilter = NULL;
  dcmReader = NULL;
  dcmWriter = NULL;
  binaryThinningImageFilter = NULL;
  rescaler = NULL;
  pngWriter = NULL;
  pngReader = NULL;
  connectedComponentFilter = NULL;
  labelShapeKeepNObjectsFilter = NULL;
  seriesFileNamesGenerator = NULL;
  niftiIO = NULL;
  niftiWriter = NULL;
  labelWriter = NULL;
}

//×î´óÇ¿¶ÈÍ¶Ó°£¬»ñµÃÑÀ³ÝµÄÂÖÀªºÍºñ¶È
itk::SmartPointer<DCMImage2DType> Panorama::Mip(itk::SmartPointer<DCMImage3DType> teeth,
                                                int lowerSlice,
                                                int higherSlice)
{
  reset();
  extractFilter = ExtractFilterType::New();
  dcmWriter->SetImageIO(dcmIO);
  dcmWriter->SetFileName(mipFile);
  auto teethSize = teeth->GetLargestPossibleRegion().GetSize();

  //ÉèÖÃÑÀ³ÝÇÐÆ¬·¶Î§
  DCMImage3DType::IndexType start;
  start[0] = 0, start[1] = 0, start[2] = teethSize[2] - lowerSlice - 1;
  DCMImage3DType::SizeType size;
  size[0] = teethSize[0], size[1] = teethSize[1], size[2] = std::abs(lowerSlice - higherSlice);
  DCMImage3DType::RegionType extractRegion(start, size);
  extractFilter->SetExtractionRegion(extractRegion);
  //¹ÜµÀ
  extractFilter->SetInput(teeth);
  mipFilter->SetInput(extractFilter->GetOutput());
#if ITK_VERSION_MAJOR >= 4
  extractFilter->SetDirectionCollapseToIdentity(); // This is required.
#endif

  try
  {
    extractFilter->UpdateLargestPossibleRegion();
    mipFilter->UpdateLargestPossibleRegion();
    auto mip = mipFilter->GetOutput();
    using IteratorType = itk::ImageRegionIterator<DCMImage2DType>;
    IteratorType it(mip, mip->GetLargestPossibleRegion());
    it.GoToBegin();
    while (!it.IsAtEnd())
    {
      if (it.Get() >= 2000)
      {
        it.Set(2000);
      }
      ++it;
    }
    dcmWriter->SetInput(mip);
    dcmWriter->UpdateLargestPossibleRegion();
    return mip;
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    return nullptr;
  }
}

//´ó½òãÐÖµ·Ö¸î³öÑÀ³Ý
itk::SmartPointer<DCMImage2DType> Panorama::Otsu(itk::SmartPointer<DCMImage2DType> mip)
{
  reset();
  scalarImageToHistogramGenerator->SetNumberOfBins(128);
  calculator->SetNumberOfThresholds(2);

  //·Ö¸î³öµÄÄÚÍâÇ¿¶ÈÖµ
  const DCMPixelType outsideValue = 0;
  const DCMPixelType insideValue = 255;
  binaryFilter->SetOutsideValue(outsideValue);
  binaryFilter->SetInsideValue(insideValue);
  dcmWriter->SetImageIO(dcmIO);
  smoothing2DFilter->SetSigma(2);

  //¹ÜµÀ
  scalarImageToHistogramGenerator->SetInput(mip);
  calculator->SetInputHistogram(scalarImageToHistogramGenerator->GetOutput());
  smoothing2DFilter->SetInput(mip);
  binaryFilter->SetInput(smoothing2DFilter->GetOutput());
  dcmWriter->SetInput(binaryFilter->GetOutput());

  try
  {
    scalarImageToHistogramGenerator->Compute();
    calculator->Compute();
    //Ê¹ÓÃ GetOutput ·½Ê½¿ÉÒÔµÃµ½ÃÅÏÞÖµ
    CalculatorType::OutputType thresholdVector = calculator->GetOutput();
    thresholdVector.push_back(itk::NumericTraits<DCMPixelType>::max());

    smoothing2DFilter->UpdateLargestPossibleRegion();
    //µÃµ½ÑÀ³ÝµÄ·Ö¸îãÐÖµ
    DCMPixelType lowerThreshold = thresholdVector[1];
    DCMPixelType upperThreshold = thresholdVector[2];
    binaryFilter->SetLowerThreshold(lowerThreshold);
    binaryFilter->SetUpperThreshold(upperThreshold);
    binaryFilter->UpdateLargestPossibleRegion();
    dcmWriter->SetFileName(otsuFile);
    dcmWriter->UpdateLargestPossibleRegion();
    return binaryFilter->GetOutput();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    return nullptr;
  }
}

//¶ÔãÐÖµ·Ö¸î³öµÄÑÀ³ÝÐÎÌ¬Ñ§¿ªÔËËã
itk::SmartPointer<DCMImage2DType> Panorama::Morph(itk::SmartPointer<DCMImage2DType> otsu, int dilateR, int erodeR)
{
  reset();
  dcmWriter->SetImageIO(dcmIO);
  dcmWriter->SetFileName(morphFile);
  //¶þÖµÃÅÏÞ
  binaryFilter->SetUpperThreshold(255); //ÉÏÏÂãÐÖµ²ÎÊýÉè¶¨
  binaryFilter->SetLowerThreshold(250);
  binaryFilter->SetOutsideValue(0); //Êä³öÖµÉè¶¨£¬±³¾°Îª0£¬Ç°¾°Îª255
  binaryFilter->SetInsideValue(255);
  //½á¹¹Ìå
  StructuringElementType dilateElement;
  dilateElement.SetRadius(dilateR); //ÅòÕÍ°ë¾¶
  dilateElement.CreateStructuringElement();
  StructuringElementType erodeElement;
  erodeElement.SetRadius(erodeR); //¸¯Ê´°ë¾¶
  erodeElement.CreateStructuringElement();
  binaryDilate->SetKernel(dilateElement);
  binaryDilate->SetDilateValue(255); //ÅòÕÍºóÐÂÏñËØÏñËØÖµ
  binaryErode->SetKernel(erodeElement);
  binaryErode->SetErodeValue(255); //¸¯Ê´ºóÐÂÏñËØÏñËØÖµ
  //Á¬Í¨ÇøÓò±êÇ©
  labelShapeKeepNObjectsFilter->SetBackgroundValue(0);
  labelShapeKeepNObjectsFilter->SetNumberOfObjects(1);
  labelShapeKeepNObjectsFilter->SetAttribute(LabelShapeKeepNObjectsFilter::LabelObjectType::NUMBER_OF_PIXELS);
  //´°Öµ
  itk::RescaleIntensityImageFilter<DCMImage2DType, DCMImage2DType>::Pointer myRescaler =
    itk::RescaleIntensityImageFilter<DCMImage2DType, DCMImage2DType>::New();
  myRescaler->SetOutputMinimum(0);
  myRescaler->SetOutputMaximum(255);
  //¹ÜµÀ
  //ÏÈÅòÕÍÔÙ¸¯Ê´
  binaryDilate->SetInput(otsu);
  connectedComponentFilter->SetInput(binaryDilate->GetOutput());
  labelShapeKeepNObjectsFilter->SetInput(connectedComponentFilter->GetOutput());
  myRescaler->SetInput(labelShapeKeepNObjectsFilter->GetOutput());
  binaryErode->SetInput(myRescaler->GetOutput());
  binaryFilter->SetInput(binaryErode->GetOutput());
  dcmWriter->SetInput(binaryFilter->GetOutput());

  try
  {
    binaryDilate->UpdateLargestPossibleRegion();
    connectedComponentFilter->UpdateLargestPossibleRegion();
    labelShapeKeepNObjectsFilter->UpdateLargestPossibleRegion();
    myRescaler->UpdateLargestPossibleRegion();
    binaryErode->UpdateLargestPossibleRegion();
    binaryFilter->UpdateLargestPossibleRegion();
    dcmWriter->UpdateLargestPossibleRegion();
    return binaryFilter->GetOutput();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    return nullptr;
  }
}

//ÐÎÌ¬Ñ§Ï¸»¯£¬ÑùÌõÇúÏßÄâºÏ
std::vector<Panorama::Point> Panorama::SplineFitting(itk::SmartPointer<DCMImage2DType> morph)
{
  reset();
  pngWriter->SetFileName(thinFile);
  pngWriter->SetImageIO(pngIO);
  //ÉèÖÃÊä³öÏñËØÑÕÉ«Öµ£¬±ãÓÚ¹Û²ì
  rescaler->SetOutputMinimum(0);
  rescaler->SetOutputMaximum(255);
  //¹ÜµÀ
  binaryThinningImageFilter->SetInput(morph);
  rescaler->SetInput(binaryThinningImageFilter->GetOutput());
  pngWriter->SetInput(rescaler->GetOutput());

  try
  {
    binaryThinningImageFilter->UpdateLargestPossibleRegion();
    rescaler->UpdateLargestPossibleRegion();
    pngWriter->UpdateLargestPossibleRegion();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    std::cerr << "spline fitting" << std::endl;
    exit(-1);
  }
  auto arch = rescaler->GetOutput();
  //ÑùÌõÇúÏßÄâºÏ
  reset();
  std::vector<Point> leftPixelVector, rightPixelVector, archPixelVector;
  auto archSize = arch->GetLargestPossibleRegion().GetSize();
  int x_dim = archSize[0], y_dim = archSize[1];
  //±éÀúÑÀ¹­ÏßÍ¼Æ¬ÏñËØµã£¬»ñÈ¡ÏñËØµã×ø±ê
  PNGIteratorType arch_it(arch, arch->GetLargestPossibleRegion());
  arch_it.GoToBegin();
  int count = 0;
  bool arch_top = false;
  std::vector<int> pixel_top;
  while (!arch_it.IsAtEnd())
  {
    unsigned short value = arch_it.Get();
    int x_coor = count % x_dim;
    int y_coor = count / y_dim;
    if (value != 0)
    {
      Point coor;
      coor.x = x_coor;
      coor.y = y_coor;
      if (arch_top == false)
      {
        arch_top = true;
        pixel_top.push_back(x_coor);
        pixel_top.push_back(y_coor);
      }
      if (x_coor <= pixel_top[0])
      {
        leftPixelVector.push_back(coor);
      }
      else
      {
        rightPixelVector.push_back(coor);
      }
    }
    ++arch_it;
    count++;
  }
  //¶ÔÏñËØ½øÐÐÅÅÐò£¬ÒÔ±ãÔÚ¶þÎ¬Í¼ÏñÉÏÅÅ¿ª
  std::sort(leftPixelVector.begin(), leftPixelVector.end(), Panorama::leftCompare);
  std::sort(rightPixelVector.begin(), rightPixelVector.end(), Panorama::rightCompare);
  //ÅÅºÃÐòµÄÑÀ¹­ÏßÏñËØµã
  archPixelVector.insert(archPixelVector.begin(), leftPixelVector.begin(), leftPixelVector.end());
  archPixelVector.insert(archPixelVector.end(), rightPixelVector.begin(), rightPixelVector.end());
  thinPixelNum = archPixelVector.size();
  //²ÉÑùµÃµ½ÑùÌõÇúÏß¿ØÖÆµã£¬11¸ö¼´¿É
  auto controlPoints = getSplineControlPoints(archPixelVector, 11);
  return controlPoints;
}

//Éú³ÉÑÀ¹­ÏßÕ¹Ê¾
DCMImage2DType::Pointer Panorama::Arch(std::vector<Point> controlPoints,
                                       DCMImage2DType::Pointer morph,
                                       std::vector<Point> &result)
{
  reset();
  //»ñµÃÄâºÏµã
  result = spline(controlPoints, 2, 1.0 / thinPixelNum);
  //»ñµÃÑÀ¹­ÏßÇøÓò
  DCMImage2DType::Pointer image = DCMImage2DType::New();
  image->SetRegions(morph->GetBufferedRegion());
  image->Allocate();
  image->FillBuffer(0);
  image->CopyInformation(morph);
  //»­³öÄâºÏµÄÇúÏß
  for (auto point : result)
  {
    PNGImageType::IndexType index;
    index[0] = (int)point.x;
    index[1] = (int)point.y;
    image->SetPixel(index, 255);
  }
  try
  {
    dcmWriter->SetFileName(splineFile);
    dcmWriter->SetImageIO(dcmIO);
    dcmWriter->SetInput(image);
    dcmWriter->UpdateLargestPossibleRegion();
    return image;
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    return nullptr;
  }
}

//Éú³ÉÈ«¾°Í¼
DCMImage2DType::Pointer Panorama::GeneratePanorama(DCMImage3DType::Pointer teeth,
                                                   std::vector<Point> &result,
                                                   int thickness,
                                                   std::vector<std::vector<Panorama::Point>> &interpoMap)
{
  reset();
  DCMImage2DType::Pointer panoramaImg = DCMImage2DType::New();
  DCMImage2DType::IndexType panoramaStart;
  panoramaStart.Fill(0);
  DCMImage2DType::SizeType panoramSize;
  //È«¾°Í¼³¤¿í
  panoramSize[0] = result.size();
  panoramSize[1] = teeth->GetLargestPossibleRegion().GetSize()[2];
  DCMImage2DType::RegionType panoramRegion(panoramaStart, panoramSize);
  panoramaImg->SetRegions(panoramRegion);
  panoramaImg->Allocate();
  panoramaImg->FillBuffer(255);
  //Ë«ÏßÐÔ²åÖµ·¨
  //ÑÀ³Ýºñ¶È£¬µ¥Î»£ºÏñËØ
  for (int x = 0; x < panoramSize[0]; x++)
  {
    auto pixel = result[x];
    //»ñµÃµ±Ç°µãµÄÇ°5¸öµãºÍºó5¸öµã£¬¼ÆËãÇÐÏòÁ¿
    auto behind = x - 5 > 0 ? result[x - 5] : pixel;
    auto ahead = x + 5 < panoramSize[0] ? result[x + 5] : pixel;
    //ÑùÌõÇúÏßÄâºÏµã
    std::vector<Point> inputPoints, outputPoints;
    Point tmp1, tmp2;
    if (ahead.x == behind.x)
    {
      //´¹Ö±ÇÐÏß
      tmp1.x = pixel.x + thickness / 2;
      tmp1.y = pixel.y;
      tmp2.x = pixel.x - thickness / 2;
      tmp2.y = pixel.y;
    }
    else if (ahead.y == behind.y)
    {
      //Ë®Æ½ÇÐÏß
      tmp1.x = pixel.x;
      tmp1.y = pixel.y + thickness / 2;
      tmp2.x = pixel.x;
      tmp2.y = pixel.y - thickness / 2;
    }
    else
    {
      //ÆÕÍ¨ÇÐÏß
      double normal = -(ahead.x - behind.x) / (ahead.y - behind.y);
      auto angle = std::atan(normal);
      double xRange = std::abs(std::cos(angle)) * thickness;
      double yRange = std::abs(std::sin(angle)) * thickness;
      if (normal < 0)
      {
        tmp1.x = pixel.x - xRange / 2;
        tmp1.y = pixel.y + yRange / 2;
        tmp2.x = pixel.x + xRange / 2;
        tmp2.y = pixel.y - yRange / 2;
      }
      else
      {
        tmp1.x = pixel.x + xRange / 2;
        tmp1.y = pixel.y + yRange / 2;
        tmp2.x = pixel.x - xRange / 2;
        tmp2.y = pixel.y - yRange / 2;
      }
    }
    inputPoints.push_back(tmp1);
    inputPoints.push_back(pixel);
    inputPoints.push_back(tmp2);
    //·¨ÏßÉÏ²ÉÑùµã¸öÊý¾ÍÊÇÑÀ¹­ºñ¶È
    outputPoints = spline(inputPoints, 1, 1.0 / thickness);
    interpoMap.push_back(outputPoints);

    for (int z = 0; z < panoramSize[1]; z++)
    {
      //Èí×éÖ¯Ç¿¶ÈÖµãÐÖµ
      double S = 50;
      double sum = 0;
      for (auto samplePoint : outputPoints)
      {
        double value = interpo(teeth, samplePoint.x, samplePoint.y, z);
        sum += std::exp(value / S);
      }
      double resultPixel = S * std::log(sum);
      //Ó³Éä»Ò¶ÈÖµ
      resultPixel = resultPixel > 3000 ? 3000 : resultPixel;
      itk::Index<2> index_write;
      index_write[0] = x;
      //Í¼ÏñÉÏÏÂµßµ¹
      index_write[1] = panoramSize[1] - 1 - z;
      panoramaImg->SetPixel(index_write, (DCMPixelType)resultPixel);
    }
  }
  auto enhanceFilter = EnhancementFilterType::New();
  enhanceFilter->SetSigma(1);
  enhanceFilter->SetInput(panoramaImg);

  dcmWriter->SetFileName(resultDir + "\\enhance.dcm");
  dcmWriter->SetInput(enhanceFilter->GetOutput());
  dcmWriter->SetImageIO(dcmIO);

  try
  {
    enhanceFilter->UpdateLargestPossibleRegion();
    dcmWriter->UpdateLargestPossibleRegion();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    std::cerr << "image enhance error" << std::endl;
    exit(-1);
  }
  //ÌáÈ¡µ½µÄÔöÇ¿ÏßÌõ
  auto enhanceImage = enhanceFilter->GetOutput();
  reset();
  //ºÏ³É
  for (int x = 0; x < panoramSize[0]; x++)
  {
    for (int y = 0; y < panoramSize[1]; y++)
    {
      itk::Index<2> index = {x, y};
      DCMPixelType originPixel = panoramaImg->GetPixel(index);
      DCMPixelType enhancePixel = enhanceImage->GetPixel(index);
      DCMPixelType resultPixel = 0.7 * originPixel + 0.3 * enhancePixel;
      panoramaImg->SetPixel(index, resultPixel);
    }
  }
  dcmWriter->SetInput(panoramaImg);
  dcmWriter->SetImageIO(dcmIO);
  dcmWriter->SetFileName(panoramaFile);
  try
  {
    dcmWriter->UpdateLargestPossibleRegion();
    return panoramaImg;
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    return nullptr;
  }
}

//ÑùÌõÇúÏß
std::vector<Panorama::Point> Panorama::spline(std::vector<Point> controlPoints, const int degree, const double epsilon)
{
  //½Úµã
  const int numberOfPoints = controlPoints.size();
  const int numberOfKnots = numberOfPoints + degree + 1;
  std::vector<double> knot(numberOfKnots);

  //³õÊ¼»¯½ÚµãÔªËØ£¬Ê¹ÓÃclamped
  for (int i = 0; i < degree + 1; i++)
  {
    knot[i] = 0;
  }
  for (int i = numberOfKnots - 1; i > numberOfKnots - degree - 2; i--)
  {
    knot[i] = 1;
  }
  for (int i = degree + 1; i <= numberOfKnots - degree - 2; i++)
  {
    knot[i] = knot[i - 1] + (1.0 / ((long)numberOfKnots - 2 * degree - 1));
  }

  //ÇúÏß¾­¹ýµÄµã
  std::vector<Point> result;
  for (double t = 0; t <= 1; t += epsilon)
  {
    Point newPoint;
    for (int i = 0; i < numberOfPoints; i++)
    {
      double tmp = BaseFunc(i, degree, t, knot);
      newPoint.x += (tmp * controlPoints[i].x);
      newPoint.y += (tmp * controlPoints[i].y);
    }
    if (result.size() != 0)
    {
      Point lastPoint = result.back();
      int xDis = (int)newPoint.x - (int)lastPoint.x;
      int yDis = (int)newPoint.y - (int)lastPoint.y;
      if (xDis == 0 && yDis == 0)
      {
        //ÖØºÏµã
        continue;
      }
      else if (std::abs(xDis) <= 1 && std::abs(yDis) <= 1)
      {
        //ÏàÁÚµã
        result.push_back(newPoint);
      }
      else
      {
        while (xDis != 0 || yDis != 0)
        {
          Point lastPoint = result.back();
          Point tmp;
          tmp.x = lastPoint.x + (xDis > 0 ? 1 : xDis < 0 ? -1 : 0);
          tmp.y = lastPoint.y + (yDis > 0 ? 1 : yDis < 0 ? -1 : 0);
          xDis += xDis > 0 ? -1 : xDis < 0 ? 1 : 0;
          yDis += yDis > 0 ? -1 : yDis < 0 ? 1 : 0;
          result.push_back(tmp);
        }
        result.pop_back();
        result.push_back(newPoint);
      }
    }
    else
    {
      result.push_back(newPoint);
    }
  }
  return result;
}

//»ñµÃÑùÌõÇúÏßµÄ¿ØÖÆµã
std::vector<Panorama::Point> Panorama::getSplineControlPoints(std::vector<Point> pixels, int numberOfPoints)
{
  std::vector<Point> controlPoints;
  //¼ÆËãÒ»ÏÂ²ÉÑù²½³¤
  double step = pixels.size() / (numberOfPoints - 1);
  for (int count = 0; count < numberOfPoints - 1; count++)
  {
    Point p;
    p.x = pixels.at((long)(step * count)).x;
    p.y = pixels.at((long)(step * count)).y;
    controlPoints.push_back(p);
  }
  //×îºóÒ»¸öµã
  Point pEnd;
  pEnd.x = pixels.back().x;
  pEnd.y = pixels.back().y;
  controlPoints.push_back(pEnd);
  return controlPoints;
}

//¿íËÉROI
DCMImage3DType::Pointer Panorama::LooseROI(DCMImage3DType::Pointer teeth,
                                           std::vector<Panorama::Point> boxPoints,
                                           std::vector<std::vector<Panorama::Point>> interpoMap)
{
  //µÃµ½°üÎ§ºÐµÄ·¶Î§
  int minX = 0, minY = 0, maxX = 0, maxY = 0;
  Point point1 = boxPoints[1], point2 = boxPoints[3];
  minX = point1.x, minY = point1.y, maxX = point2.x, maxY = point2.y;
  reset();
  Point A = interpoMap[minX].front();
  Point B = interpoMap[minX].back();
  Point C = interpoMap[maxX].back();
  Point D = interpoMap[maxX].front();

  itk::Size<3> teethSize = teeth->GetLargestPossibleRegion().GetSize();
  for (int x = 0; x < teethSize[0]; x++)
  {
    for (int y = 0; y < teethSize[1]; y++)
    {
      double a = (B.x - A.x) * (y - A.y) - (B.y - A.y) * (x - A.x);
      double b = (C.x - B.x) * (y - B.y) - (C.y - B.y) * (x - B.x);
      double c = (D.x - C.x) * (y - C.y) - (D.y - C.y) * (x - C.x);
      double d = (A.x - D.x) * (y - D.y) - (A.y - D.y) * (x - D.x);
      if ((a > 0 && b > 0 && c > 0 && d > 0) || (a < 0 && b < 0 && c < 0 && d < 0))
      {
        continue;
      }
      for (int z = minY; z < maxY; z++)
      {
        itk::Index<3> index;
        index[0] = x, index[1] = y, index[2] = teethSize[2] - z - 1;
        teeth->SetPixel(index, 0);
      }
    }
  }
  //Êä³öµ½Í¼ÏñÐèÒªµÄ´óÐ¡
  int minBoxX = 1000, maxBoxX = 0, minBoxY = 1000, maxBoxY = 0;
  for (int idx = minX; idx < maxX; idx++)
  {
    std::vector<Panorama::Point> interPoints = interpoMap[idx];
    for (Point p : interPoints)
    {
      if (minBoxX > p.x)
        minBoxX = p.x;
      if (maxBoxX < p.x)
        maxBoxX = p.x;
      if (minBoxY > p.y)
        minBoxY = p.y;
      if (maxBoxY < p.y)
        maxBoxY = p.y;
    }
  }

  auto ROIFilter = ROIFilterType::New();
  DCMImage3DType::IndexType start;
  start[0] = minBoxX, start[1] = minBoxY, start[2] = teethSize[2] - maxY;
  DCMImage3DType::SizeType size;
  size[0] = maxBoxX - minBoxX, size[1] = maxBoxY - minBoxY, size[2] = maxY - minY;
  DCMImage3DType::RegionType ROIRegion(start, size);
  ROIFilter->SetRegionOfInterest(ROIRegion);
  //¹ÜµÀ
  ROIFilter->SetInput(teeth);
  niftiWriter->SetInput(ROIFilter->GetOutput());
  niftiWriter->SetImageIO(niftiIO);
  niftiWriter->SetFileName(looseROIFile);
  try
  {
    ROIFilter->UpdateLargestPossibleRegion();
    niftiWriter->UpdateLargestPossibleRegion();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    return nullptr;
  }

  auto looseROI = ROIFilter->GetOutput();
  //×î´óÇ¿¶ÈÍ¶Ó°
  reset();
  mipFilter->SetInput(looseROI);
  dcmWriter->SetImageIO(dcmIO);
  dcmWriter->SetInput(mipFilter->GetOutput());
  dcmWriter->SetFileName(resultDir + "//looseROI_mip.dcm");
  try
  {
    mipFilter->UpdateLargestPossibleRegion();
    dcmWriter->UpdateLargestPossibleRegion();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    return nullptr;
  }
  auto looseROI_mip = mipFilter->GetOutput();
  //ÒÖÖÆÇ¿¶ÈÖµ¹ý´óµÄ½ðÊôÎ±Ó°
  using IteratorType = itk::ImageRegionIterator<DCMImage2DType>;
  IteratorType it(looseROI_mip, looseROI_mip->GetLargestPossibleRegion());
  it.GoToBegin();
  while (!it.IsAtEnd())
  {
    if (it.Get() >= 2000)
    {
      it.Set(2000);
    }
    ++it;
  }
  reset();
  //ãÐÖµ·Ö¸î
  scalarImageToHistogramGenerator->SetNumberOfBins(128);
  calculator->SetNumberOfThresholds(2);

  //·Ö¸î³öµÄÄÚÍâÇ¿¶ÈÖµ
  const DCMPixelType outsideValue = 0;
  const DCMPixelType insideValue = 255;
  binaryFilter->SetOutsideValue(outsideValue);
  binaryFilter->SetInsideValue(insideValue);
  dcmWriter->SetImageIO(dcmIO);
  smoothing2DFilter->SetSigma(1);

  //¹ÜµÀ
  scalarImageToHistogramGenerator->SetInput(looseROI_mip);
  calculator->SetInputHistogram(scalarImageToHistogramGenerator->GetOutput());
  smoothing2DFilter->SetInput(looseROI_mip);
  binaryFilter->SetInput(smoothing2DFilter->GetOutput());
  dcmWriter->SetInput(binaryFilter->GetOutput());

  try
  {
    scalarImageToHistogramGenerator->Compute();
    calculator->Compute();
    CalculatorType::OutputType thresholdVector = calculator->GetOutput();
    thresholdVector.push_back(itk::NumericTraits<DCMPixelType>::max());

    smoothing2DFilter->UpdateLargestPossibleRegion();
    DCMPixelType lowerThreshold = thresholdVector[1];
    DCMPixelType upperThreshold = thresholdVector[2];
    binaryFilter->SetLowerThreshold(lowerThreshold);
    binaryFilter->SetUpperThreshold(upperThreshold);
    binaryFilter->UpdateLargestPossibleRegion();
    dcmWriter->SetFileName(resultDir + "\\looseROI_otsu.dcm");
    dcmWriter->UpdateLargestPossibleRegion();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    return nullptr;
  }

  // ·µ»ØlooseROI
  return looseROI;
}

//ÑÀ³Ý°üÎ§ºÐ
DCMImage3DType::Pointer Panorama::BoxROI(DCMImage3DType::Pointer teeth,
                                         std::vector<Panorama::Point> boxPoints,
                                         std::vector<std::vector<Panorama::Point>> interpoMap,
                                         int leftPadding,
                                         int rightPadding,
                                         int upPadding,
                                         int downPadding)
{
  //°üÎ§ºÐ·¶Î§
  int minX = 0, minY = 0, maxX = 0, maxY = 0;
  Point point1 = boxPoints[1], point2 = boxPoints[3];
  minX = point1.x, minY = point1.y, maxX = point2.x, maxY = point2.y;
  //Êä³öµ½Í¼ÏñÐèÒªµÄ´óÐ¡
  int minBoxX = 1000, maxBoxX = 0, minBoxY = 1000, maxBoxY = 0;
  for (int idx = minX; idx < maxX; idx++)
  {
    std::vector<Panorama::Point> interPoints = interpoMap[idx];
    for (Point p : interPoints)
    {
      if (minBoxX > p.x)
        minBoxX = p.x;
      if (maxBoxX < p.x)
        maxBoxX = p.x;
      if (minBoxY > p.y)
        minBoxY = p.y;
      if (maxBoxY < p.y)
        maxBoxY = p.y;
    }
  }
  reset();
  dcmReader->SetFileName(resultDir + "\\looseROI_otsu.dcm");
  dcmReader->SetImageIO(dcmIO);
  try
  {
    dcmReader->UpdateLargestPossibleRegion();
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    return nullptr;
  }
  DCMImage2DType::Pointer looseROI_otsu = dcmReader->GetOutput();
  itk::Size<2> otsuSize = looseROI_otsu->GetLargestPossibleRegion().GetSize();
  int otsuMinX = 1000, otsuMaxX = 0, otsuMinY = 1000, otsuMaxY = 0;
  for (int i = 0; i < otsuSize[0]; i++)
  {
    for (int j = 0; j < otsuSize[1]; j++)
    {
      itk::Index<2> index = {i, j};
      DCMPixelType pixel = looseROI_otsu->GetPixel(index);
      if (pixel == 255)
      {
        otsuMinX = otsuMinX > i ? i : otsuMinX;
        otsuMaxX = otsuMaxX < i ? i : otsuMaxX;
        otsuMinY = otsuMinY > j ? j : otsuMinY;
        otsuMaxY = otsuMaxY < j ? j : otsuMaxY;
      }
    }
  }

  itk::Size<3> teethSize = teeth->GetLargestPossibleRegion().GetSize();
  DCMImage3DType::IndexType start;
  start[0] = minBoxX + otsuMinX - leftPadding, start[1] = minBoxY + otsuMinY - upPadding,
  start[2] = teethSize[2] - maxY;
  DCMImage3DType::SizeType size;
  size[0] = otsuMaxX - otsuMinX + leftPadding + rightPadding, size[1] = otsuMaxY - otsuMinY + upPadding + downPadding,
  size[2] = maxY - minY;
  DCMImage3DType::RegionType toothBoxRegion(start, size);
  auto ROIFilter = ROIFilterType::New();
  ROIFilter->SetRegionOfInterest(toothBoxRegion);
  //¹ÜµÀ
  ROIFilter->SetInput(teeth);
  niftiWriter->SetInput(ROIFilter->GetOutput());
  niftiWriter->SetImageIO(niftiIO);
  niftiWriter->SetFileName(boxROIFile);
  try
  {
    ROIFilter->UpdateLargestPossibleRegion();
    niftiWriter->UpdateLargestPossibleRegion();
    auto boxROI = ROIFilter->GetOutput();
    return boxROI;
  }
  catch (itk::ExceptionObject &e)
  {
    std::cerr << e << std::endl;
    return nullptr;
  }
}

//¶ÔÈýÎ¬CBCTÊý¾Ý²åÖµ
double Panorama::interpo(DCMImage3DType::Pointer teeth, double x, double y, double z)
{
  int int_x = (int)x;
  int int_y = (int)y;
  double alpha = x - int_x;
  double beta = y - int_y;
  //Ë®Æ½·½Ïò²åÖµ
  double horiInterpo1, horiInterpo2;
  itk::Index<3> index_read;
  index_read[0] = int_x;
  index_read[1] = int_y;
  index_read[2] = z;
  short tmp1 = teeth->GetPixel(index_read);
  index_read[0] = int_x + 1;
  index_read[1] = int_y;
  index_read[2] = z;
  short tmp2 = teeth->GetPixel(index_read);
  horiInterpo1 = tmp1 + alpha * (tmp2 - tmp1);
  index_read[0] = int_x;
  index_read[1] = int_y + 1;
  index_read[2] = z;
  short tmp3 = teeth->GetPixel(index_read);
  index_read[0] = int_x + 1;
  index_read[1] = int_y + 1;
  index_read[2] = z;
  short tmp4 = teeth->GetPixel(index_read);
  horiInterpo2 = tmp3 + alpha * (tmp4 - tmp3);
  //´¹Ö±·½Ïò²åÖµ
  double value = horiInterpo1 + beta * (horiInterpo2 - horiInterpo1);
  return value;
}