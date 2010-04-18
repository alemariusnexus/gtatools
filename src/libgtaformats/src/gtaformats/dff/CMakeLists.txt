SET(FILES   DFFException DFFFrame DFFGeometry DFFGeometryPart DFFLoader DFFMaterial DFFMesh DFFTexture )

FOREACH(FILE ${FILES})
    SET(SRCS ${SRCS} "gtaformats/dff/${FILE}")
ENDFOREACH(FILE)

SET(SRCS ${SRCS} PARENT_SCOPE)
