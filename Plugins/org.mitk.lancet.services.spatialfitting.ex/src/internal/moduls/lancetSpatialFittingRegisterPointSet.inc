#ifndef lancetSpatialFittingRegisterPointSet_INC
#define lancetSpatialFittingRegisterPointSet_INC

#include <mitkPoint.h>
#include <mitkPointSet.h>

template<typename Point, unsigned int Number = 3U>
class RegisterPointSet
{
  struct PrivateImp
  {
	bool isModify;
    std::array<Point, Number> arrayStandardPointSet;
    std::array<Point, Number> arrayReferencePointSet;
  };
public:
  RegisterPointSet() : imp(std::make_shared<PrivateImp>()) {}

  RegisterPointSet(const RegisterPointSet& pointSet)
    : imp(std::make_shared<PrivateImp>())
  {
	this->SetStandardPointSet(pointSet.imp->arrayStandardPointSet);
	this->SetReferencePointSet(pointSet.imp->arrayReferencePointSet);
  }
  
  RegisterPointSet(const RegisterPointSet* pointSet)
    : RegisterPointSet(*pointSet)
  {
	this->SetStandardPointSet(pointSet->imp->arrayStandardPointSet);
	this->SetReferencePointSet(pointSet->imp->arrayReferencePointSet);
  }
  
  unsigned int GetNumber() const
  {
	return Number;
  }
  
  void SetStandardPointSet(int index, Point pt)
  {
    this->imp->arrayStandardPointSet[index] = pt;
  }
  
  void SetStandardPointSet(const std::array<Point, Number>& pointSet)
  {
    this->imp->arrayStandardPointSet = pointSet;
  }
  
  Point GetStandardPointSet(int index) const
  {
    return this->imp->arrayStandardPointSet[index];
  }
  
  std::array<Point, Number> GetStandardPointSetArray() const 
  { 
	return this->imp->arrayStandardPointSet; 
  }
  
  mitk::PointSet::Pointer GetMitkStandardPointSetArray() const 
  { 
	mitk::PointSet::Pointer mitkPointArray = mitk::PointSet::New();
	
	for(int index = 0; index < this->GetNumber(); ++index)
	{
	  mitkPointArray->SetPoint(index, this->GetStandardPointSet(index));
	}
	
	return mitkPointArray; 
  }
  
  void SetReferencePointSet(int index, Point pt)
  {
    this->imp->arrayReferencePointSet[index] = pt;
  }
  
  void SetReferencePointSet(const std::array<Point, Number>& pointSet)
  {
    this->imp->arrayReferencePointSet = pointSet;
  }
  
  Point GetReferencePointSet(int index) const
  {
    return this->imp->arrayReferencePointSet[index];
  }
  
  std::array<Point, Number> GetReferencePointSetArray() const 
  { 
	return this->imp->arrayReferencePointSet; 
  }
  
  std::array<Point, Number> GetMitkReferencePointSetArray() const 
  { 
	mitk::PointSet::Pointer mitkPointArray = mitk::PointSet::New();
	
	for(int index = 0; index < this->GetNumber(); ++index)
	{
	  mitkPointArray->SetPoint(index, this->GetReferencePointSet(index));
	}
	
	return mitkPointArray; 
  }

  bool IsModify() const 
  {
    return this->imp->isModify;
  }

  void SetModify(bool modify)
  {
	this->imp->isModify = modify;
  }
private:
  
  std::shared_ptr<PrivateImp> imp;
};




#endif // !lancetSpatialFittingRegisterPointSet_INC