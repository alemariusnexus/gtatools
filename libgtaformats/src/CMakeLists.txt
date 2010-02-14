SET(SRC_FILES
	DFFFrame DFFGeometry DFFGeometryPart DffLoader DFFMaterial DffMesh DFFTexture gf_filetype gta
	IDEEntity IDEReader IDEStaticObject IMGArchive IMGException IMGVisitor TXDArchive TXDException TXDTexture TXDVisitor
	PARENT_SCOPE
	)
SET(INC_FILES
	gf_config DFFException DFFFrame DFFGeometry DFFGeometryPart DffLoader DFFMaterial DffMesh DFFTexture gf_filetype gta gtadff gtaimg
	gtatxd IDEEntity IDEReader IDEStaticObject IMGArchive IMGException IMGVisitor TXDArchive
	TXDException TXDTexture TXDVisitor
	PARENT_SCOPE
	)

IF(MSVC)
	SET(INC_FILES ${INC_FILES} msvcstdint PARENT_SCOPE)
ENDIF(MSVC)
