set(CPP_FILES  
  Qmitk/QmitkLancetKukaWidget.cpp
)

set(UI_FILES 
  Qmitk/QmitkLancetKukaWidget.ui
)

set(MOC_H_FILES
  Qmitk/QmitkLancetKukaWidget.h
)

# if(MITK_USE_POLHEMUS_TRACKER)
#   set(CPP_FILES ${CPP_FILES} Qmitk/QmitkPolhemusTrackerWidget.cpp)
#   set(UI_FILES ${UI_FILES} Qmitk/QmitkPolhemusTrackerWidget.ui)
#   set(MOC_H_FILES ${MOC_H_FILES} Qmitk/QmitkPolhemusTrackerWidget.h)
# endif(MITK_USE_POLHEMUS_TRACKER)

set(QRC_FILES
  #resources/IGTUI.qrc
)
