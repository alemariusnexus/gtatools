CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(prj-gtatools-gui CXX)

ADD_SUBDIRECTORY(src)

IF(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "${prj-gtaformats_SOURCE_DIR}" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ELSE(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ENDIF(prj-gtaformats_SOURCE_DIR)

IF(UNIX)
	SET(GL_DEFAULT_LIB GL)
	SET(GLU_DEFAULT_LIB GLU)
ELSE(UNIX) # Windows...
	SET(GL_DEFAULT_LIB opengl32)
	SET(GLU_DEFAULT_LIB glu32)
ENDIF(UNIX)

SET(GTAFORMATS_LIB_PATH "gtaformats" CACHE PATH "Path to (or name of) the libgtaformats library file")
SET(SQUISH_INCLUDE_PATH "" CACHE PATH "Directory containing libsquish header files.")
SET(SQUISH_LIB_PATH "squish" CACHE PATH "Path to (or name of) libsquish library file")
SET(GL_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL header file(s)")
SET(GL_LIB_PATH "${GL_DEFAULT_LIB}" CACHE PATH "Path to (or name of) the OpenGL library file")
SET(GLU_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL Utility (GLU) header file(s)")
SET(GLU_LIB_PATH "${GLU_DEFAULT_LIB}" CACHE PATH "Path to (or name of) the OpenGL Utility (GLU) library file")

IF(UNIX)
	SET(wxWidgets_USE_UNICODE ON)
	SET(wxWidgets_DEBUG ON CACHE BOOL "Whether to use wxWidgets debug libraries")
	SET(wxWidgets_STATIC ON CACHE BOOL "Whether to use wxWidgets as a static library")
	SET(wxWidgets_UNIVERSAL OFF CACHE BOOL "Whether to use wxWidgets universal mode libraries")
ELSE(UNIX) # Windows...
	SET(wxWidgets_ROOT_DIR "" CACHE PATH "Path to wxWidgets root directory")
	SET(wxWidgets_LIB_DIR "" CACHE PATH "Path to wxWidgets library directory")
	SET(wxWidgets_CONFIGURATION "" CACHE STRING "wxWidgets configuration to use (e.g. msw, mswd, mswu, mswunivud etc.)")
ENDIF(UNIX)

FIND_PACKAGE(wxWidgets COMPONENTS base core gl)

INCLUDE(${wxWidgets_USE_FILE})

IF(NOT GTAFORMATS_LIB_PATH)
	SET(GTAFORMATS_LIB_PATH gtaformats)
ENDIF(NOT GTAFORMATS_LIB_PATH)
IF(NOT SQUISH_LIB_PATH)
	SET(SQUISH_LIB_PATH squish)
ENDIF(NOT SQUISH_LIB_PATH)

INCLUDE_DIRECTORIES(${GTAFORMATS_INCLUDE_PATH} ${SQUISH_INCLUDE_PATH} ${GL_INCLUDE_PATH} ${GLU_INCLUDE_PATH})

ADD_EXECUTABLE(gtatools-gui WIN32 ${SRCS})
TARGET_LINK_LIBRARIES(gtatools-gui ${GTAFORMATS_LIB_PATH} ${SQUISH_LIB_PATH} ${GL_LIB_PATH} ${GLU_LIB_PATH})

TARGET_LINK_LIBRARIES(gtatools-gui ${wxWidgets_LIBRARIES})

INSTALL(TARGETS gtatools-gui DESTINATION bin)

