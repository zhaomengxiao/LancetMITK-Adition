#ifndef LANCETMATRIXCONVET_H
#define LANCETMATRIXCONVET_H

#include "mitkAffineTransform3D.h"

namespace lancet
{
  /**
  * \brief Convert Mitk::AffineTransform3D To c style 3x4 Array
  *
  *@param input A mitk::AffineTransform3D::Pointer
  *@param output A transformation matrix has 3x4 elements. It consists of a rotational matrix (3x3 elements)
  * and a translational vector (3x1 elements). The complete transformation matrix has the
  * following structure: <br>
  * [Transformation(3x4)] = [Rotation(3x3) | Translation(3x1)]
 */
  void ConvertMitkAffineTransform3DTo3x4Array(mitk::AffineTransform3D::Pointer input, double(&output)[3][4]);
}

#endif // LANCETMATRIXCONVET_H
