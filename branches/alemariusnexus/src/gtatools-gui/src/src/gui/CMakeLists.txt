SET(SRCS ${SRCS}
    src/gui/FileItemModel.cpp src/gui/StaticFile.cpp src/gui/MainWindow.cpp src/gui/FirstStartWizard.cpp
    src/gui/ProfileConfigWidget.cpp src/gui/ConfigWidget.cpp src/gui/GUIModule.cpp src/gui/DefaultGUIModule.cpp
    src/gui/FileSearchDialog.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    src/gui/FileItemModel.h src/gui/MainWindow.h src/gui/FirstStartWizard.h
    src/gui/ProfileConfigWidget.h src/gui/ConfigWidget.h src/gui/GUIModule.h
    src/gui/DefaultGUIModule.h src/gui/FileSearchDialog.h
    PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS}
    src/gui/MainFrame.ui src/gui/FirstStartWizard.ui src/gui/ProfileConfigWidget.ui src/gui/ConfigWidget.ui
    src/gui/FileSearchDialog.ui
    PARENT_SCOPE)
SET(INCS ${INCS} src/gui PARENT_SCOPE)
