set(H_FILES
  src/mitkGizmo_noscaleMapper2D.h
  src/mitkGizmo_noscaleObjectFactory.h
  include/mitkGizmo_noscale.h
  include/mitkGizmo_noscaleInteractor.h
)

set(CPP_FILES
  mitkGizmo_noscale.cpp
  mitkGizmo_noscaleInteractor.cpp
  mitkGizmo_noscaleObjectFactory.cpp
  mitkGizmo_noscaleMapper2D.cpp
)

set(RESOURCE_FILES
  # "Interactions" prefix forced by mitk::StateMachine
  Interactions/Gizmo3DStates.xml
  Interactions/Gizmo3DConfig.xml
)
