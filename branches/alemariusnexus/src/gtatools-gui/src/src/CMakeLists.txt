SET(CONFIG_DIR "/etc/gtatools-gui" CACHE PATH "The configuration directory of gtatools-gui")

ADD_SUBDIRECTORY(gui)
ADD_SUBDIRECTORY(formats)

CONFIGURE_FILE(config.cmake.h config.h)

SET(SRCS ${SRCS} src/main.cpp src/Profile.cpp src/ProfileManager.cpp src/System.cpp src/StringMatcher.cpp PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS} src/ProfileManager.h src/Profile.h src/System.h PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS} PARENT_SCOPE)
SET(INCS ${INCS} PARENT_SCOPE)
