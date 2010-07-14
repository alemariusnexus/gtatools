ADD_SUBDIRECTORY(impl)

SET(SRCS ${SRCS} src/formats/FormatHandler.cpp src/formats/FormatManager.cpp PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS} src/formats/FormatHandler.h PARENT_SCOPE)
