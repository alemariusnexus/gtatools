SET(FILES StringComparator util File Exception)

FOREACH(FILE ${FILES})
    SET(SRCS ${SRCS} "gtaformats/util/${FILE}")
ENDFOREACH(FILE)

ADD_SUBDIRECTORY(stream)

SET(SRCS ${SRCS} PARENT_SCOPE)
