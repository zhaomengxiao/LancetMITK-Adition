#ifndef model_h
#define model_h

#include <itkObject.h>
#include <MitkOrthopedicsExports.h>
#include <itkeigen/Eigen/Eigen>

#include "mitkCommon.h"
#include "body.h"
#include "joint.h"

namespace othopedics
{
  class MITKORTHOPEDICS_EXPORT Model :public itk::Object
  {
  public:
    mitkClassMacroItkParent(Model, itk::Object);
    itkFactorylessNewMacro(Self)

    std::string name;

    std::vector<Body> m_bodies;
    std::vector<Joint> m_joints;
  };
}
#endif