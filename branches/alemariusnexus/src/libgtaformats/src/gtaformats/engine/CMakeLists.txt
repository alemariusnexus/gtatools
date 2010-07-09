SET(SRC_FILES	DefaultResourceManager DFFOpenGLRenderer EngineException OpenGLResourceManager
                ResourceIndex )

FOREACH(FILE ${SRC_FILES})
	SET(SRCS ${SRCS} "gtaformats/engine/${FILE}")
ENDFOREACH(FILE)

SET(SRCS ${SRCS} PARENT_SCOPE)
