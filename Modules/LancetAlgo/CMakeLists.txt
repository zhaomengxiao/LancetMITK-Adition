MITK_CREATE_MODULE(
  INCLUDE_DIRS
    PUBLIC Geometry/include Navigation/include Physiology/include
  DEPENDS MitkCore
  PACKAGE_DEPENDS Eigen
  FORCE_STATIC
)
add_subdirectory(cmdapps)
#add_subdirectory(autoload)