#include "lancetMatrixConvert.h"

void lancet::ConvertMitkAffineTransform3DTo3x4Array(mitk::AffineTransform3D::Pointer input, double(&output)[3][4])
{
  if (input == nullptr)
  {
    return;
  }
  auto matrix = input->GetMatrix();
  auto translate = input->GetOffset();
  output[0][0] = matrix[0][0]; output[0][1] = matrix[0][1]; output[0][2] = matrix[0][2]; output[0][3] = translate[0];
  output[1][0] = matrix[1][0]; output[1][1] = matrix[1][1]; output[1][2] = matrix[1][2]; output[1][3] = translate[1];
  output[2][0] = matrix[2][0]; output[2][1] = matrix[2][1]; output[2][2] = matrix[2][2]; output[2][3] = translate[2];
}
