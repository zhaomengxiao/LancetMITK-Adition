#include "lancetSpatialFittingPelvicRegistrationsModel.h"

//BEGIN_SPATIAL_FITTING_NAMESPACE

template<typename RegisterPointSet>
struct PelvicRegistrationsModel::PelvicRegisterDataModel<RegisterPointSet>::PrivateImp
{
  RegisterPointSet vegaPointArray;
  RegisterPointSet imagePointArray;
};

template<typename RegisterPointSet>
PelvicRegistrationsModel::PelvicRegisterDataModel<RegisterPointSet>::PelvicRegisterDataModel()
  : imp(std::make_shared<PrivateImp>())
{
  
}

template<typename RegisterPointSet>
int PelvicRegistrationsModel::PelvicRegisterDataModel<RegisterPointSet>::GetMaxPointSize() const
{
  return this->imp->vegaPointArray.size();
}

template<typename RegisterPointSet>
mitk::Point3D PelvicRegistrationsModel::PelvicRegisterDataModel<RegisterPointSet>::GetImagePointOfRegister(int index) const
{
  if (index < this->imp->imagePointArray.size())
  {
    return this->imp->imagePointArray[index];
  }
  return mitk::Point3D();
}

template<typename RegisterPointSet>
void PelvicRegistrationsModel::PelvicRegisterDataModel<RegisterPointSet>::SetImagePointArrayOfRegister(int index,
   const mitk::Point3D& pt)
{
  if (index < this->imp->imagePointArray.size())
  {
    this->imp->imagePointArray[index] = pt;
  }
}

template<typename RegisterPointSet>
void PelvicRegistrationsModel::PelvicRegisterDataModel<RegisterPointSet>::SetImagePointArrayOfRegister(const RegisterPointSet& ptArray)
{
  for(int index = 0; index < this->GetMaxPointSize();  ++index)
  {
    if(ptArray.size() > index)
    {
      this->SetImagePointArrayOfRegister(index, ptArray[index]);
    }
  }
}

// Return -1 value to indicate that the database is complete, otherwise return null subscript.
template<typename RegisterPointSet>
int PelvicRegistrationsModel::PelvicRegisterDataModel<RegisterPointSet>::GetImagePointArrayWaitModifyIndex() const
{
  for(int index = 0; index < this->GetMaxPointSize();  ++index)
  {
    if(this->GetImagePointOfRegister(index) == mitk::Point3D())
    {
      return index;
    }
  }

  return -1;
}

template<typename RegisterPointSet>
mitk::Point3D PelvicRegistrationsModel::PelvicRegisterDataModel<RegisterPointSet>::GetVegaPointOfRegister(int index) const
{
  if (index < this->imp->vegaPointArray.size())
  {
    return this->imp->vegaPointArray[index];
  }
  return mitk::Point3D();
}

template<typename RegisterPointSet>
void PelvicRegistrationsModel::PelvicRegisterDataModel<RegisterPointSet>::SetVegaPointArrayOfRegister(int index, 
  const mitk::Point3D& pt)
{
  if (index < this->imp->imagePointArray.size())
  {
    this->imp->imagePointArray[index] = pt;
  }
}

template<typename RegisterPointSet>
void PelvicRegistrationsModel::PelvicRegisterDataModel<RegisterPointSet>::SetVegaPointArrayOfRegister(const RegisterPointSet& ptArray)
{
  for(int index = 0; index < this->GetMaxPointSize();  ++index)
  {
    if(ptArray.size() > index)
    {
      this->SetVegaPointArrayOfRegister(index, ptArray[index]);
    }
  }
}

// Return -1 value to indicate that the database is complete, otherwise return null subscript.
template<typename RegisterPointSet>
int PelvicRegistrationsModel::PelvicRegisterDataModel<RegisterPointSet>::GetVegaPointArrayWaitModifyIndex() const
{
  for(int index = 0; index < this->GetMaxPointSize();  ++index)
  {
    if(this->GetVegaPointOfRegister(index) == mitk::Point3D())
    {
      return index;
    }
  }

  return -1;
}

//END_SPATIAL_FITTING_NAMESPACE