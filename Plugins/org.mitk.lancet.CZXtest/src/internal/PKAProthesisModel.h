#pragma once
#ifndef PKAPROSTHESISIMODEL
#define PKAPROSTHESISIMODEL


#include <eigen3/Eigen/Dense>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>
#include "CalculationHelper.h"
#include "PKADataBase.h"

namespace lancetAlgorithm
{
	class PKAProthesisModel : public PKADataBase
	{
	public:
		/*void UpdateFemur();
		void UpdateTibie();*/

		//void UpdateModel(vtkMatrix4x4* matrix);
		void UpdateFemur();
		void UpdateTibia();
	private:

		void UpdateFemurResectionLine();
		void UpdateFemurAnteriorChamCut();
		void UpdateFemurDistalCut();
		void UpdateFemurPosteriorCut();
		/**************************************************************************************************************************************************************/
		void UpdateTibiaAxis();
		void UpdateTibiaPlaneSymmetryAxis();

	};
}
#endif // !PKAPROSTHESISIMODEL

