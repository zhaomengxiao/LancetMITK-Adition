//#pragma once
//#include <mitkVtkMapper.h>
//
//#include <vtkPolyDataMapper.h>
//class ModelUserControl;
//using namespace mitk;
//class ModelMapper2D : public mitk::VtkMapper
//{
//public:
//    mitkClassMacro(ModelMapper2D, VtkMapper);
//    itkFactorylessNewMacro(Self);
//    itkCloneMacro(Self);
//
//    //! Provides given node with a set of default properties.
//    //!
//    //! \param node The DataNode to decorate with properties.
//    //! \param renderer When not nullptr, generate specific properties for given renderer
//    //! \param overwrite Whether already existing properties shall be overwritten.
//    static void SetDefaultProperties(mitk::DataNode* node,
//        mitk::BaseRenderer* renderer = nullptr,
//        bool overwrite = false);
//
//    //! Return the vtkProp that represents the "rendering result".
//    vtkProp* GetVtkProp(mitk::BaseRenderer* renderer) override { return m_LSH.GetLocalStorage(renderer)->m_Actor; }
//    //! "Resets" the mapper, setting its result to invisible.
//    void ResetMapper(mitk::BaseRenderer* renderer) override;
//
//    //! Return the internal vtkPolyData for given renderer.
//    //! This serves for picking by the associated interactor class.
//    vtkPolyData* GetVtkPolyData(mitk::BaseRenderer* renderer);
//
//private:
//    //! Provide the mapping input as a specific RawMesh_C
//    const ModelUserControl* GetInput();
//
//    //! Update the vtkProp, i.e. the contours of a slice through our RawMesh_C.
//    void GenerateDataForRenderer(mitk::BaseRenderer* renderer) override;
//
//    //! Apply visual properties
//    void ApplyVisualProperties(BaseRenderer* renderer);
//
//    //! (RenderWindow) Instance specific data.
//    class LocalStorage : public Mapper::BaseLocalStorage
//    {
//    public:
//        //! The overall rendering result.
//        vtkSmartPointer<vtkActor> m_Actor;
//
//        //! The mapper of the resulting vtkPolyData
//        //! (3D polygons for 3D mapper, 2D contours for 2D mapper)
//        vtkSmartPointer<vtkPolyDataMapper> m_VtkPolyDataMapper;
//
//        //! Last time this storage has been updated.
//        itk::TimeStamp m_LastUpdateTime;
//
//        LocalStorage();
//    };
//
//    //! (RenderWindow) Instance specific data.
//    LocalStorageHandler<LocalStorage> m_LSH;
//};
//
//
