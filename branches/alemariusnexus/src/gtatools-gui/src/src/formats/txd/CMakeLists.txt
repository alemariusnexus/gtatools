SET(SRCS ${SRCS}
    src/formats/txd/TXDFormatHandler.cpp src/formats/txd/TXDGUIModule.cpp src/formats/txd/TXDWidget.cpp
    src/formats/txd/TextureDisplayer.cpp src/formats/txd/TextureSearchDialog.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    src/formats/txd/TXDFormatHandler.h src/formats/txd/TXDGUIModule.h src/formats/txd/TXDWidget.h
    src/formats/txd/TextureDisplayer.h src/formats/txd/TextureSearchDialog.h
    PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS} src/formats/txd/TXDWidget.ui src/formats/txd/TextureSearchDialog.ui PARENT_SCOPE)
SET(INCS ${INCS} src/formats/txd PARENT_SCOPE)
