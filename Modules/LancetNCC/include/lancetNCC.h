#ifndef LANCETNCC_H
#define LANCETNCC_H
#include "MitkLANCETNCCExports.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>
#include <math.h>

#include "mitkPointSet.h"

// https://www.codeproject.com/Articles/99457/Edge-Based-Template-Matching#_comments

class MITKLANCETNCC_EXPORT GeoMatch
{
private:
	int				noOfCordinates;		//Number of elements in coordinate array
	CvPoint* cordinates;		//Coordinates array to store model points	
	int				modelHeight;		//Template height
	int				modelWidth;			//Template width
	double* edgeMagnitude;		//gradient magnitude
	double* edgeDerivativeX;	//gradient in X direction
	double* edgeDerivativeY;	//radient in Y direction	
	CvPoint			centerOfGravity;	//Center of gravity of template 

	bool			modelDefined;

	void CreateDoubleMatrix(double**& matrix, CvSize size);
	void ReleaseDoubleMatrix(double**& matrix, int size);

	int NumOfDetectedTargets{ 0 };

public:
	GeoMatch(void);
	GeoMatch(const void* templateArr);
	~GeoMatch(void);

	int CreateGeoMatchModel(const void* templateArr, double, double);

	// Find the best match
	double FindGeoMatchModel(const void* srcarr, double minScore, double greediness, CvPoint* resultPoint);

	// zzhou: Find the best match while avoiding the existent found points
	double FindGeoMatchModelEnhanced(const void* srcarr, double minScore, 
		double greediness, mitk::PointSet::Pointer foundPts, 
		double pixelSize_x, double pixelSize_y, 
		double steelballSize /*mm*/,CvPoint* resultPoint);



	// zzhou: Find the first n best matches
	bool FindAllGeoMatchModel(const void* srcarr, double minScore, double greediness, 
		int matchNum, double pixelSize_x, 
		double pixelSize_y, double steelballSize /*mm*/, 
		mitk::PointSet::Pointer resultPointSet);

	void DrawContours(IplImage* pImage, CvPoint COG, CvScalar, int);
	void DrawContours(IplImage* pImage, CvScalar, int);
};

#endif // LANCETNCC_H