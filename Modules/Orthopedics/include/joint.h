#ifndef joint_h
#define joint_h

#include <MitkOrthopedicsExports.h>
#include <itkeigen/Eigen/Eigen>
namespace othopedics
{
  class MITKORTHOPEDICS_EXPORT Joint
  {
  public:
    std::string name;

    std::string parentName;
    std::string childName;
    Eigen::Matrix4d T_parent;
    Eigen::Matrix4d T_child;
  };
}


#endif