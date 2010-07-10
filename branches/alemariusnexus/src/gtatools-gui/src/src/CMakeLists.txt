SET(CONFIG_DIR "/etc/gtatools-gui" CACHE PATH "The configuration directory of gtatools-gui")

ADD_SUBDIRECTORY(gui)
ADD_SUBDIRECTORY(formats)

CONFIGURE_FILE(config.cmake.h config.h)

SET(SRCS ${SRCS} src/main src/Profile src/ProfileManager PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS} src/ProfileManager.h PARENT_SCOPE)
