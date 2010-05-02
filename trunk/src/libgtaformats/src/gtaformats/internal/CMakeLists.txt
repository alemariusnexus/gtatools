SET(FILES StringComparator util)

FOREACH(FILE ${FILES})
    SET(SRCS ${SRCS} "gtaformats/internal/${FILE}")
ENDFOREACH(FILE)

SET(SRCS ${SRCS} PARENT_SCOPE)
