set(CPP_FILES  
  Qmitk/QmitkLancetKukaWidget.cpp
  Qmitk/QlancetToolDistanceWidget.cpp
)

set(UI_FILES 
  Qmitk/QmitkLancetKukaWidget.ui
  Qmitk/QlancetToolDistanceWidgetControls.ui
)

set(MOC_H_FILES
  Qmitk/QmitkLancetKukaWidget.h
  Qmitk/QlancetToolDistanceWidget.h
)

# if(MITK_USE_POLHEMUS_TRACKER)
#   set(CPP_FILES ${CPP_FILES} Qmitk/QmitkPolhemusTrackerWidget.cpp)
#   set(UI_FILES ${UI_FILES} Qmitk/QmitkPolhemusTrackerWidget.ui)
#   set(MOC_H_FILES ${MOC_H_FILES} Qmitk/QmitkPolhemusTrackerWidget.h)
# endif(MITK_USE_POLHEMUS_TRACKER)

set(QRC_FILES
  #resources/IGTUI.qrc
)
