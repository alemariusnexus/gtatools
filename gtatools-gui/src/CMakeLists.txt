CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gtatools-gui CXX)

ADD_SUBDIRECTORY(dff)
ADD_SUBDIRECTORY(img)
ADD_SUBDIRECTORY(lang)
ADD_SUBDIRECTORY(txd)

SET(SRCS ${SRCS} AboutFrame Application DataSource DisplayManager FileDataSource FileDisplayer FormatProvider MainFrame WxGUI)

IF(UNIX)
	SET(wxWidgets_USE_UNICODE ON)
	SET(wxWidgets_DEBUG ON CACHE BOOL "Whether to use wxWidgets debug libraries")
	SET(wxWidgets_STATIC ON CACHE BOOL "Whether to use wxWidgets as a static library")
	SET(wxWidgets_UNIVERSAL OFF CACHE BOOL "Whether to use wxWidgets universal mode libraries")
ELSE(UNIX) # Windows...
	SET(wxWidgets_ROOT_DIR '' CACHE PATH "Path to wxWidgets root directory")
	SET(wxWidgets_LIB_DIR '' CACHE PATH "Path to wxWidgets library directory")
	SET(wxWidgets_CONFIGURATION '' CACHE STRING "wxWidgets configuration to use (e.g. msw, mswd, mswu, mswunivud etc.)")
ENDIF(UNIX)

FIND_PACKAGE(wxWidgets COMPONENTS base core gl)

INCLUDE(${wxWidgets_USE_FILE})

ADD_EXECUTABLE(gtatools-gui ${SRCS})
TARGET_LINK_LIBRARIES(gtatools-gui gtaformats squish)

TARGET_LINK_LIBRARIES(gtatools-gui ${wxWidgets_LIBRARIES})

INSTALL(TARGETS gtatools-gui DESTINATION bin)

