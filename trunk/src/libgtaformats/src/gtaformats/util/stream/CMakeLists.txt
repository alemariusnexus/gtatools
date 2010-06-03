SET(FILES FileInputStream InputStream IOException STLInputStream)

FOREACH(FILE ${FILES})
    SET(SRCS ${SRCS} "gtaformats/util/stream/${FILE}")
ENDFOREACH(FILE)

SET(SRCS ${SRCS} PARENT_SCOPE)
