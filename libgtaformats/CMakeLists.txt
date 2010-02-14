CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

OPTION(GF_STATIC_BUILD "Do you want to build the library as a static library?" ON)


SET(SRC_FILES
	DFFFrame DFFGeometry DFFGeometryPart DffLoader DFFMaterial DffMesh DFFTexture gf_filetype gta
	IDEEntity IDEReader IDEStaticObject IMGArchive IMGException IMGVisitor TXDArchive TXDException TXDTexture TXDVisitor
	)
SET(INC_FILES
	gf_config DFFException DFFFrame DFFGeometry DFFGeometryPart DffLoader DFFMaterial DffMesh DFFTexture gf_filetype gta gtadff gtaimg
	gtatxd IDEEntity IDEReader IDEStaticObject IMGArchive IMGException IMGVisitor TXDArchive
	TXDException TXDTexture TXDVisitor
	)

IF(MSVC)
	SET(INC_FILES ${INC_FILES} msvcstdint)
ENDIF(MSVC)


ADD_SUBDIRECTORY(src)

FOREACH(SRC_FILE ${SRC_FILES})
	SET(SRC_PATHS ${SRC_PATHS} src/${SRC_FILE})
ENDFOREACH(SRC_FILE)
FOREACH(INC_FILE ${INC_FILES})
	SET(INC_PATHS ${INC_PATHS} src/${INC_FILE}.h)
ENDFOREACH(INC_FILE)

IF(GF_STATIC_BUILD)
	ADD_LIBRARY(gtaformats STATIC ${SRC_PATHS})
ELSE(GF_STATIC_BUILD)
	ADD_LIBRARY(gtaformats SHARED ${SRC_PATHS})
ENDIF(GF_STATIC_BUILD)

INSTALL(	TARGETS gtaformats
		DESTINATION lib
		PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
		)
INSTALL(	FILES ${INC_PATHS}
		DESTINATION include/gtaformats
		PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
		)

SET(GF_INCLUDE_DIR ${GTATOOLS_SOURCE_DIR}/libgtaformats/src PARENT_SCOPE)
