SET(SRC_FILES
	DFFFrame DFFGeometry DFFGeometryPart DffLoader DFFMaterial DffMesh DFFTexture gf_filetype gta
	IDEEntity IDEReader IDEStaticObject IMGArchive IMGException IMGVisitor TXDArchive TXDException TXDTexture TXDVisitor
	PARENT_SCOPE
	)
SET(INC_FILES
	DFFException DFFFrame DFFGeometry DFFGeometryPart DffLoader DFFMaterial DffMesh DFFTexture gf_filetype gta gtadff gtaimg
	gtatxd IDEEntity IDEReader IDEStaticObject IMGArchive IMGException IMGVisitor TXDArchive
	TXDException TXDTexture TXDVisitor gf_config
	PARENT_SCOPE
	)

IF(MSVC)
	SET(INC_FILES ${INC_FILES} msvcstdint PARENT_SCOPE)
ENDIF(MSVC)

OPTION(GF_USE_SQUISH "When turned on, DXT-compressed textures are supported. You'll need libsquish then." ON)
CONFIGURE_FILE(${LIBGTAFORMATS_SOURCE_DIR}/src/gtaformats/gf_config.h.cmake ${LIBGTAFORMATS_SOURCE_DIR}/src/gtaformats/gf_config.h)
