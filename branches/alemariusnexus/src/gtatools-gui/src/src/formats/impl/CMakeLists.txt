SET(SRCS ${SRCS}
    src/formats/impl/IMGFormatHandler.cpp src/formats/impl/TXDFormatHandler.cpp
    src/formats/impl/IPLFormatHandler.cpp src/formats/impl/IDEFormatHandler.cpp
    src/formats/impl/DFFFormatHandler.cpp src/formats/impl/TXDGUIModule.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    src/formats/impl/IMGFormatHandler.h src/formats/impl/TXDFormatHandler.h
    src/formats/impl/IPLFormatHandler.h src/formats/impl/IDEFormatHandler.h
    src/formats/impl/DFFFormatHandler.h src/formats/impl/TXDGUIModule.h
    PARENT_SCOPE)
